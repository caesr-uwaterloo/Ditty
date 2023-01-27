# assume running in root
# ditty
conf=$1
# 2
x=$2
# 2
y=$3
# hw/hw_emu


name="${conf}_${x}_${y}"

pushd ${name} > /dev/null
rm -rf to_aws
$VITIS_DIR/tools/create_vitis_afi.sh -xclbin=kernel_hw.xclbin \
		-o=kernel \
		-s3_bucket=<bucket name> -s3_dcp_key=${name}_dcp -s3_logs_key=${name}_log
popd > /dev/null

