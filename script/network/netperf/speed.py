if __name__ == '__main__':
    total = 0
    with open('out.txt', 'r') as f:
        for line in f.readlines():
            if '30.' in line:
                d = line.split()
                print(d)
                s = float(d[-1])
                total += s
    print('%f' % total)
