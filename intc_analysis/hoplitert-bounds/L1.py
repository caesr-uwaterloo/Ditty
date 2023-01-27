import sys
b = []
for line in sys.stdin.readlines():
    a = map(int, line.strip().split(' '))
    a = list(a)
    b.append(a[0] + max(a[1], a[2]))

print(max(b))
