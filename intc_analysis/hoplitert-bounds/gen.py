def to_xy(i, n):
    return (i // n, i % n)
x = 2
for y in [1, 2, 3, 4, 5]:
    with open(f"hn_{y}_all_to_one.dat", "w+") as f:
        n = 2 * y
        for i in range(n):
            (u, v) = to_xy(i, y)
            f.writelines(f"{u}, {v}, 1, {y-1}, 1, {1 / n}\n")
        f.writelines(f"{1}, {y-1}, 0, 0, 1, 0")

