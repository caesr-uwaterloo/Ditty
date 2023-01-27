import itertools as it
import math

def hoplite_conf(n):
    return it.product([2], range(1, n+1))

for n in [1, 2, 3, 4, 5]:
    l = list(hoplite_conf(n))
    T_f = -1

    for u, v in l:
        for w, x in l:
            dx = abs(u - w)
            dy = abs(v - x)
            T_f = max(dx + dy + (dy * 2) + 2, T_f)
    # Note that we do not have information about the flow, and only knows that each flow is 1 / 2*n
    # 1/rho
    rho = 1 / (2 * n)
    rho_reciprocal = 2 * n
    pre_term = rho_reciprocal  - 1

    # we assume the bucket is only one for simplisity
    # Available injection rate for one core is simply 1 - 1/(2*n)
    # The total bucket of other cores is 2 * n - 1
    # we could optimize by analyzing the stream direction
    T_s = math.ceil( (2 * n - 1) / (1 - (1 - rho)) )
    print(f"---- {n} ----")
    print(f"T_f: ", T_f)
    print(f"L_Nmsg: ", (2 * n) * (2 * n - 1) + (pre_term + T_s) + T_f)  # waiting time + in flight
