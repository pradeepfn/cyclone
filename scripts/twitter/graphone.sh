#!/bin/bash
declare -a wl=("graphone")
# declare -a bf=(1)
declare -a bf=(1 2 4 6 8 10 12 14)

declare -a mt=("nvram")

declare -r rl=(1)

for replicas in "${rl[@]}"
do
for w in "${wl[@]}"
do
  for m in "${mt[@]}"
  do
        for b in "${bf[@]}"
        do
    ./twitter_bench.py -c
    ./twitter_bench.py -g -rep "$replicas" -m "$m" -w "$w" -b "$b"
    ./twitter_bench.py -dc -m "$m" -w "$w"
    ./twitter_bench.py -db -m "$m" -w "$w"

    ./twitter_bench.py -startsrv -m "$m" -w "$w"
    sleep 10
    ./twitter_bench.py -startclnt -m "$m" -w "$w"
        # wait for sometime
        sleep 80
    ./twitter_bench.py -stop -m "$m" -w "$w"

        # gather output
    ./twitter_bench.py -collect -rep "$replicas" -m "$m" -w "$w" -b "$b"
        done
  done
done
done
exit
