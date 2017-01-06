perf record -e 'block:block_bio_queue' -a dd if=/dev/zero of=/dev/sdb bs=4k count=10
# perf report
perf script -g python
perf script -s perf-script.py
