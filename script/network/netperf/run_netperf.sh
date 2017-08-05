rm -f out.txt
for i in `seq 20`
do
  # netperf -H 192.168.122.16 -l 30 -t TCP_RR 1>>out.txt&
  # netperf -H 192.168.122.16 -l 30 -t TCP_STREAM -f M 1>>out.txt&
  netperf -H 192.168.122.16 -l 30 -t TCP_STREAM -f M -P 0 1>>out.txt&
done
