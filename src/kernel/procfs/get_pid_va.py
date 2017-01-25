import sys
import re

def get_x86_pte(va):
    va_mask = (1 << 48) - 1
    mask9 = (1 << 9) - 1
    mask12 = (1 << 12) - 1
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
    return (pml4, dir_ptr, dir, table, offset)

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage:python %s <pid> <Hex address>" % sys.argv[0])
        sys.exit(1)
    pid_proc = "/proc/zproc/mminfo"
    phys_proc = "/proc/zproc/physvalue"
    pid = int(sys.argv[1], 10)
    va = int(sys.argv[2], 16)
    print("pid:%d va:0x%x" % (pid, va))
    pml4, dir_ptr, dir, table, offset = get_x86_pte(va)
    cr3 = 0
    with open(pid_proc, "r+") as f:
        f.write(str(pid))
        out = f.read()
        pgd_pa = re.search("pgd pa:0x(\w+)", out)
        if pgd_pa is not None:
            cr3 = pgd_pa.group().split(':')[1]
            cr3 = int(cr3, 16)
    print("cr3:0x%x" % cr3)

    clear_mask = ~((1 << 12)-1)
    with open(phys_proc, 'r+') as f:
        # 1. cr3 + pml4
        pa = (cr3 & clear_mask) | (pml4 << 3)
        print("1 pa:0x%x" % pa)
        f.write(hex(pa))
        out = f.read()
        value = int(out.split(':')[1], 16)
        print("0x%x value:0x%x" % (pa, value))

        # 2. + dir_ptr
        f.seek(0)
        pa = (value & clear_mask) | (dir_ptr << 3)
        print("2 pa:0x%x" % pa)
        f.write(hex(pa))
        out = f.read()
        value = int(out.split(':')[1], 16)
        print("0x%x value:0x%x" % (pa, value))

        # 3. + dir
        f.seek(0)
        pa = (value & clear_mask) | (dir << 3)
        print("3 pa:0x%x" % pa)
        f.write(hex(pa))
        out = f.read()
        value = int(out.split(':')[1], 16)
        print("0x%x value:0x%x" % (pa, value))

        # 4. + table
        f.seek(0)
        pa = (value & clear_mask) | (table << 3)
        print("4 pa:0x%x" % pa)
        f.write(hex(pa))
        out = f.read()
        value = int(out.split(':')[1], 16)
        print("0x%x value:0x%x" % (pa, value))

        # 5. + offset
        f.seek(0)
        pa = (value & clear_mask) | offset
        pa &= 0xffffffff
        print("5 pa:0x%x" % pa)
        f.write("0x{0:x}".format(pa))
        out = f.read()
        value = int(out.split(':')[1], 16)
        print("0x%x value:0x%x" % (pa, value))
