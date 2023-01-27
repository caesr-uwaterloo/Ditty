

#define OCL_CHECK(error, call)                                                                   \
    call;                                                                                        \
    if (error != CL_SUCCESS) {                                                                   \
        printf("%s:%d Error calling " #call ", error code is: %d\n", __FILE__, __LINE__, error); \
        exit(EXIT_FAILURE);                                                                      \
    }

#include <experimental/xrt_ip.h>
#include "main.h"
#include "common.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <chrono>
#include <sys/stat.h>



static const int DATA_SIZE = 4096;

void wait_for_enter(const std::string &msg) {
    std::cout << msg << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}


static const std::string error_message =
    "Error: Result mismatch:\n"
    "i = %d CPU result = %d Device result = %d\n";

// addresses
uint64_t* trace = nullptr;
uint64_t  trace_size;
int fd;

void wait_reset() {
  while(true) {
    std::ifstream ifs("reset", std::fstream::in);
    int a  = 0;
    ifs >> a;
    if(a == 2) break;
  }
}

void load_trace(const char* filename) {
  fd = open(filename, O_RDONLY);
  if (fd < 0) {
    printf("Error opening file %s", filename);
    exit(1);
  }
  std::cout << "File loaded" << std::endl;
  // TODO: determine by systemcalls
  struct stat stat_buf;
  int rc = fstat(fd, &stat_buf);
  if(rc < 0) {
    printf("Error getting file size");
    exit(1);
  }
  trace_size = stat_buf.st_size;
  trace = (uint64_t*)mmap(NULL, trace_size, PROT_READ, MAP_PRIVATE, fd, 0);
  std::cout << "mapped" << std::endl;
}

void unload_trace() {
  munmap(trace, trace_size);
}

int main(int argc, char* argv[]) {
  // TARGET_DEVICE macro needs to be passed from gcc command line
  if (argc != 5) {
    std::cout << "Usage: " << argv[0] << " <xclbin> <N> <trace> <is_debug>" << std::endl;
    return EXIT_FAILURE;
  }

  wait_reset();

  std::string xclbinFilename = argv[1];
  int N = atoi(argv[2]);
  std::string trace_fn = argv[3];
  int is_debug = atoi(argv[4]);
  std::cout << "N = " << N << std::endl;
  std::cout << "trace = " << trace_fn << std::endl;

  load_trace(trace_fn.c_str());


  // Compute the size of array in bytes
  size_t size_in_bytes = DATA_SIZE * sizeof(int);

  // Creates a vector of DATA_SIZE elements with an initial value of 10 and 32
  // using customized allocator for getting buffer alignment to 4k boundary

  std::vector<cl::Device> devices;
  cl_int err;
  cl::Context context, contextd;
  cl::CommandQueue q, qd;
  cl::Kernel krnl_vector_add, krnl_sinker[N], krnl_dram;
  cl::Program program;
  std::vector<cl::Platform> platforms;
  bool found_device = false;

  // traversing all Platforms To find Xilinx Platform and targeted
  // Device in Xilinx Platform
  cl::Platform::get(&platforms);
  for (size_t i = 0; (i < platforms.size()) & (found_device == false); i++) {
    cl::Platform platform = platforms[i];
    std::string platformName = platform.getInfo<CL_PLATFORM_NAME>();
    if (platformName == "Xilinx") {
      devices.clear();
      platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);
      if (devices.size()) {
        found_device = true;
        break;
      }
    }
  }
  if (found_device == false) {
    std::cout << "Error: Unable to find Target Device " << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "INFO: Reading " << xclbinFilename << std::endl;
  FILE* fp;
  if ((fp = fopen(xclbinFilename.c_str(), "r")) == nullptr) {
    printf("ERROR: %s xclbin not available please build\n", xclbinFilename.c_str());
    exit(EXIT_FAILURE);
  }
  // Load xclbin
  std::cout << "Loading: '" << xclbinFilename << "'\n";
  std::ifstream bin_file(xclbinFilename, std::ifstream::binary);
  bin_file.seekg(0, bin_file.end);
  unsigned nb = bin_file.tellg();
  bin_file.seekg(0, bin_file.beg);
  char* buf = new char[nb];
  bin_file.read(buf, nb);

  // Creating Program from Binary File
  cl::Program::Binaries bins;
  bins.push_back({buf, nb});
  bool valid_device = false;
  for (unsigned int i = 0; i < devices.size(); i++) {
    auto device = devices[i];
    // Creating Context and Command Queue for selected Device
    OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
    OCL_CHECK(err, q = cl::CommandQueue(context, device, 0, &err));
    OCL_CHECK(err, qd = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    cl::Program program(context, {device}, bins, nullptr, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
    } else {
      std::cout << "Device[" << i << "]: program successful!\n";
      //OCL_CHECK(err, krnl_vector_add = cl::Kernel(program, "krnl_vadd", &err));
      // for(int i = 0; i < N; i++) {
      //   std::stringstream ss;
      //   ss << "private_cache:{private_cache_" << i + 1 << "}";
      //   OCL_CHECK(err, krnl_private_cache[i] = cl::Kernel(program, ss.str().c_str(), &err));
      // }
      for(int i = 0; i < N; i++) {
        std::stringstream ss;
        ss << "sinker:{sinker_" << i + 1 << "}";
        OCL_CHECK(err, krnl_sinker[i] = cl::Kernel(program, ss.str().c_str(), &err));
      }
      OCL_CHECK(err, krnl_dram = cl::Kernel(program, "dram_module:{dram_module_1}", &err));
      valid_device = true;
      break; // we break because we found a valid device
    }
  }

  if (!valid_device) {
    std::cout << "Failed to program any device found, exit!\n";
    exit(EXIT_FAILURE);
  }

  if(is_debug) {
    wait_for_enter("\nPress ENTER to continue");
  }

  // Request r[1024];
  // r[0].addr = 0x12345600;
  // r[0].type_ = Load;
  unsigned NReq = trace_size / sizeof(uint64_t);
  std::cout << "Number of request: " << NReq << std::endl;
  auto size_of_buffer = sizeof(Request) * NReq;
  auto size_of_buffer_resp = sizeof(Response) * NReq;

  std::cout << "Starting DRAM" << std::endl;


  // On the board, we do nothing about it
  OCL_CHECK(err, cl::Buffer dram_mod(context, CL_MEM_READ_WRITE, 1024*1024, NULL, &err));
  OCL_CHECK(err, err = krnl_dram.setArg(0, dram_mod));
  OCL_CHECK(err, err = qd.enqueueMigrateMemObjects({dram_mod}, 0 /* 0 means from host*/));
  OCL_CHECK(err, err = qd.enqueueTask(krnl_dram));


  std::cout << "creating clbuffer" << std::endl;
  cl::Buffer req_buffer[N];
  cl::Buffer req_buffer_out[N];

  // auto xclbin_uuid = device.load_xclbin(xclbinFilename);
  // auto ip = xrt::ip(device, xclbin_uuid, "private_cache:{private_cache_1}");

  for(int i = 0; i < N; i++) {
    OCL_CHECK(err, req_buffer[i] = cl::Buffer(context, CL_MEM_READ_ONLY, size_of_buffer, NULL, &err));
    OCL_CHECK(err, req_buffer_out[i] = cl::Buffer(context, CL_MEM_WRITE_ONLY, size_of_buffer_resp, NULL, &err));
  }
  std::cout << "setting kernel arg" << std::endl;

  Request* ptr_req[N];
  Response* ptr_read[N];
  for(int i = 0; i < N; i++) {
    // OCL_CHECK(err, err = krnl_private_cache[i].setArg(3, i));

    OCL_CHECK(err, err = krnl_sinker[i].setArg(0, req_buffer[i]));
    OCL_CHECK(err, err = krnl_sinker[i].setArg(1, req_buffer_out[i]));
    OCL_CHECK(err, err = krnl_sinker[i].setArg(2, NReq));

    std::cout << "enqueue map buffer of " << i << std::endl;
    OCL_CHECK(err,
              ptr_req[i] = (Request*)q.enqueueMapBuffer(req_buffer[i], CL_TRUE, CL_MAP_WRITE, 0, size_of_buffer, NULL, NULL, &err));
    OCL_CHECK(err,
              ptr_read[i] = (Response*)q.enqueueMapBuffer(req_buffer_out[i], CL_TRUE, CL_MAP_READ, 0, size_of_buffer_resp, NULL, NULL, &err));


    for(int r = 0; r < NReq; r++) {
      ptr_req[i][r].addr = trace[r] & (1024 * 1024 - 1);
      set_type(ptr_req[i][r],  r % 2 == 0 ? LD : ST);
      // ptr_req[i][r].sz = double_word;
      if(i == 1 && r == 0) {
        std::cout << ptr_req[i][r].addr.to_string(16) << std::endl;
      }
    }
    std::cout << "enqueue map buffer done" << std::endl;
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({req_buffer[i]}, 0 /* 0 means from host*/));

  }

  OCL_CHECK(err, q.finish());


  std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
  for(int i = 0; i < N; i++) {
    std::cout << "running kernel" << std::endl;
    OCL_CHECK(err, err = q.enqueueTask(krnl_sinker[i]));
  }
  // transferring back...
  // std::cout << "transferring result back" << std::endl;
  // OCL_CHECK(err, q.enqueueMigrateMemObjects({req_buffer_out}, CL_MIGRATE_MEM_OBJECT_HOST));
  // for(int i = 0; i < 1; i++) {
  //   std::cout << ptr_read[i].addr.to_string(16) << ", " << ptr_read[i].data << std::endl;
  // }
  OCL_CHECK(err, q.finish());
  std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
  auto x = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
  std::cout << "Time difference = " << x << "[µs]" << std::endl;

  std::cout << "Done executing the krnl_sinker kernel, checking content" << std::endl;
  for(int i = 0; i < N; i++) {
    OCL_CHECK(err, q.enqueueMigrateMemObjects({req_buffer_out[i]}, CL_MIGRATE_MEM_OBJECT_HOST));
  }
  OCL_CHECK(err, q.finish());
  // std::cout << "printing response for core 1" << std::endl;
  // for (int i = 0; i < NReq; i++) {
  //   auto resp = ptr_read[1][i];
  //   std::cout << resp.addr.to_string(16) << ", " << resp.data << std::endl;
  // }

  // self clearing -- not really good
  std::ofstream  ofs("reset", std::fstream::trunc | std::fstream::out);
  ofs << "1" << std::endl;
  ofs.close();

  std::ofstream res_ofs("results.csv", std::fstream::out);
  // 10 ns -> 100 MHz
  res_ofs << xclbinFilename << ", " << trace_fn << "," << x << "," << (x * 1000 / NReq / 10) <<std::endl;
  res_ofs.close();
  system("echo Resetting the FPGA");
  // system("sudo fpga-clear-local-image -S 0");

  return 0;

  // // These commands will allocate memory on the Device. The cl::Buffer objects can
  // // be used to reference the memory locations on the device.
  // OCL_CHECK(err, cl::Buffer buffer_a(context, CL_MEM_READ_ONLY, size_in_bytes, NULL, &err));
  // OCL_CHECK(err, cl::Buffer buffer_b(context, CL_MEM_READ_ONLY, size_in_bytes, NULL, &err));
  // OCL_CHECK(err, cl::Buffer buffer_result(context, CL_MEM_WRITE_ONLY, size_in_bytes, NULL, &err));

  // // set the kernel Arguments
  // int narg = 0;
  // OCL_CHECK(err, err = krnl_vector_add.setArg(narg++, buffer_a));
  // OCL_CHECK(err, err = krnl_vector_add.setArg(narg++, buffer_b));
  // OCL_CHECK(err, err = krnl_vector_add.setArg(narg++, buffer_result));
  // OCL_CHECK(err, err = krnl_vector_add.setArg(narg++, DATA_SIZE));

  // // We then need to map our OpenCL buffers to get the pointers
  // int* ptr_a;
  // int* ptr_b;
  // int* ptr_result;
  // OCL_CHECK(err,
  //           ptr_a = (int*)q.enqueueMapBuffer(buffer_a, CL_TRUE, CL_MAP_WRITE, 0, size_in_bytes, NULL, NULL, &err));
  // OCL_CHECK(err,
  //           ptr_b = (int*)q.enqueueMapBuffer(buffer_b, CL_TRUE, CL_MAP_WRITE, 0, size_in_bytes, NULL, NULL, &err));
  // OCL_CHECK(err, ptr_result = (int*)q.enqueueMapBuffer(buffer_result, CL_TRUE, CL_MAP_READ, 0, size_in_bytes, NULL,
  //                                                      NULL, &err));

  // // Data will be migrated to kernel space
  // OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_a, buffer_b}, 0 /* 0 means from host*/));

  // // Launch the Kernel
  // OCL_CHECK(err, err = q.enqueueTask(krnl_vector_add));

  // // The result of the previous kernel execution will need to be retrieved in
  // // order to view the results. This call will transfer the data from FPGA to
  // // source_results vector
  // OCL_CHECK(err, q.enqueueMigrateMemObjects({buffer_result}, CL_MIGRATE_MEM_OBJECT_HOST));

  // OCL_CHECK(err, q.finish());

  // // Verify the result
  // int match = 0;
  // for (int i = 0; i < DATA_SIZE; i++) {
  //   int host_result = ptr_a[i] + ptr_b[i];
  //   if (ptr_result[i] != host_result) {
  //     printf(error_message.c_str(), i, host_result, ptr_result[i]);
  //     match = 1;
  //     break;
  //   }
  // }

  // OCL_CHECK(err, err = q.enqueueUnmapMemObject(buffer_a, ptr_a));
  // OCL_CHECK(err, err = q.enqueueUnmapMemObject(buffer_b, ptr_b));
  // OCL_CHECK(err, err = q.enqueueUnmapMemObject(buffer_result, ptr_result));
  // OCL_CHECK(err, err = q.finish());

  // std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
  // return (match ? EXIT_FAILURE : EXIT_SUCCESS);
}
