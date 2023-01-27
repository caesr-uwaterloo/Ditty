# calculating L1
for n in 1 2 3 4 5 ; do
  echo "N = $n"
  # ./runnoc.py -o result hn_${n}_all_to_one.dat | tail -n 3 | head -n 2 | cut -d ":" -f 2 | sort -rn | head -n 1
  ./runnoc.py -o result hn_${n}_all_to_one.dat > /dev/null
  cat result | cut -d ',' -f 8,13,14 | tr , " "  | tr -s " " | python L1.py
  echo ""
done
