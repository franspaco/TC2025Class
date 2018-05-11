
import sys, random

X = 3500
Y = 3500

live_per = 15

def gen():
    val = random.randint(1, 100)
    return 1 if val < live_per else 0

with open(sys.argv[1], 'w') as f:
    f.write(f"{Y} {X}\n")
    for y in range(Y):
        for x in range(X):
            f.write(f"{gen()} ")
        f.write("\n")
