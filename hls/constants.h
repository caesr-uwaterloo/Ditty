#pragma once
#ifndef VITIS_SYSTEM_CONSTANTS_H
#define VITIS_SYSTEM_CONSTANTS_H
#include <ap_int.h>

constexpr unsigned mlog2(unsigned x) {
  return x < 2 ? x : 1+mlog2(x >> 1);
}
constexpr unsigned cilog2(unsigned x) {
  return x < 2 ? x : mlog2(x - 1);
}

// defining different flavors of protocols
constexpr unsigned int prot_MSI = 0;
constexpr unsigned int prot_DirMSIUnordered = 1;
constexpr unsigned int prot_DirMSIOrdered = 2;
constexpr unsigned int prot_MESI = 3;
constexpr unsigned int prot_DirMESIUnordered = 4;
constexpr unsigned int prot_DirMESIOrdered = 5;
// PROTOCOL is the protocol used for the current synthesis and simulation
#ifndef PROTOCOL
#define PROTOCOL prot_MSI
#endif

// Params
#define N_	 16

#define NSET 16
#define NWAY 2

#define DIR_NSET NSET
#define DIR_NWAY (NWAY * N_)

#define LLC_NSET 512
#define LLC_NWAY 4

#define CL_BITS 128
#define CL_SIZE (CL_BITS / 8)
#define ADDR_W  32
constexpr unsigned int log2c_NWAY = cilog2(NWAY); // 2
constexpr unsigned int log2c_NSET = cilog2(NSET); // 4
constexpr unsigned int log2c_DIR_NWAY = cilog2(DIR_NWAY); // 2
constexpr unsigned int log2c_DIR_NSET = cilog2(DIR_NSET); // 4
constexpr unsigned int log2c_LLC_NWAY = cilog2(LLC_NWAY); // 2
constexpr unsigned int log2c_LLC_NSET = cilog2(LLC_NSET); // 4
constexpr unsigned int log2c_N    = cilog2(N_); // 4
constexpr unsigned int log2c_2N1    = cilog2(2*N_+1); // 4
constexpr unsigned int log2c_CL_SIZE = cilog2(CL_SIZE); // 16
#define MSHR_ENTRIES 1
#define log2c_MSHR_ENTRIES 1
#define REPLAY_BUFFER_ENTRIES 1
#define log2c_REPLAY_BUFFER_ENTRIES 1
#define OUTSTANDING_REQUESTS 1
#define log2c_OUTSTANDING_REQUESTS 1
#define MAX_INST 16

typedef ap_uint<cilog2(LLC_NSET)> llc_set_t;
typedef ap_uint<CL_BITS> cacheline_t;

#warning "ack_t is hard coded, scaling up requires a different approach"
typedef ap_uint<16> ack_t;
typedef ap_uint<cilog2(CL_SIZE)> llc_offset_t;
typedef ap_uint<cilog2(CL_SIZE)> offset_t;
// DRAM size in terms of the number of cache lines
// 64 KB
const unsigned N_CACHELINE_DRAM = 1024 * 1024 / CL_SIZE;

#endif //VITIS_SYSTEM_CONSTANTS_H
