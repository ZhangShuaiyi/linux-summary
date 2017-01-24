import sys

if len(sys.argv) < 2:
    print("usage: %s <hex>" % (sys.argv[0]))
    sys.exit(1)

va_mask = (1 << 48) - 1
mask9 = (1 << 9) - 1
mask12 = (1 << 12) - 1
va = int(sys.argv[1], 16)
va = va & va_mask
# binva = bin(va)
# binva = "{0:b}".format(va)
binva = "{0:048b}".format(va)
print("va:0x%x" % va)
print(binva)
pml4 = (va >> 39) & mask9
dir_ptr = (va >> 30) & mask9
dir = (va >> 21) & mask9
table = (va >> 12) & mask9
offset = va & mask12
print("pml4\t:%s \t:0x%x \t*8:0x%x" % (binva[48-48:48-39], pml4, pml4 << 3))
print("dir_ptr\t:%s \t:0x%x \t*8:0x%x" % (binva[48-39:48-30], dir_ptr, dir_ptr << 3))
print("dir\t:%s \t:0x%x \t*8:0x%x" % (binva[48-30:48-21], dir, dir << 3))
print("table\t:%s \t:0x%x \t*8:0x%x" % (binva[48-21:48-12], table, table << 3))
print("offset\t:%s \t:0x%x" % (binva[48-12:], offset))
