for c in gp  ; do
  for y in 1; do
    name="${c}_2_${y}"
    # cp ./*.dat  ${name}/
    python generate_yaml.py -c $c -x 2 -y $y
    python generate_config.py -c $c -x 2 -y $y
    bash patch.dat.sh $c 2 $y
    pushd ${name} > /dev/null
      python ../generate_package_script.py ./desc.yml -o package.tcl
    popd > /dev/null
  done
done

for c in gp  ; do
  for y in 1 ; do
    name="${c}_2_${y}"
    pushd ${name} > /dev/null
      vivado -mode batch -nojournal -nolog -source package.tcl
    popd > /dev/null
  done
done
