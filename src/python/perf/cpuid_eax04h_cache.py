#!/usr/bin/env python
import sys

def get_bits(d, start, end):
    s = bin(d)[2:][::-1]
    return int(s[start:end][::-1], 2)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage %s <ebx> <ecx>" % sys.argv[0])
        sys.exit(1)
    ebx = int(sys.argv[1], 16)
    ecx = int(sys.argv[2], 16)
    ways = get_bits(ebx, 22, 31)
    partition = get_bits(ebx, 12, 21)
    line_size = get_bits(ebx, 0, 11)
    sets = ecx
    print("ebx:0x%x ecx:0x%x ways:0x%x partition:0x%x line_size:0x%x" %
    (ebx, ecx, ways, partition, line_size))
    cache = (ways + 1)*(partition + 1)*(line_size + 1)*(sets + 1)
    print("cache:%d" % cache)

