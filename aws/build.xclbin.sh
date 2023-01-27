# assume running in root
# ditty
conf=$1
# 2
x=$2
# 2
y=$3
# hw/hw_emu
m=$4


PLATFORM=/home/centos/src/project_data/aws-fpga/Vitis/aws_platform/xilinx_aws-vu9p-f1_shell-v04261818_201920_3/xilinx_aws-vu9p-f1_shell-v04261818_201920_3.xpfm

name="${conf}_${x}_${y}"
VPP_FLAGS="--save-temps --temp_dir temp --log_dir log --report_dir report"

pushd ${name} > /dev/null
v++ ${VPP_FLAGS} --report_level 2 --target ${m} --platform ${PLATFORM} --link -o kernel_${m}.xclbin \
  --config connect.cfg \
  --debug.chipscope sinker_1:TO_CORE \
  --debug.chipscope sinker_1:REQ \
  --debug.chipscope sinker_1:M_AXI_GMEM  \
  --debug.chipscope system_kernel_1:from_core_0 \
  --debug.chipscope system_kernel_1:to_core_0  \
  --debug.chipscope dram_module_1:DRAM_REQ \
  --debug.chipscope dram_module_1:DRAM_RESP \
  --debug.chipscope dram_module_1:M_AXI_GMEM \
  -g \
  ../sinker_${m}.xo ../dram_module_${m}.xo \
  desc.xo

popd > /dev/null
