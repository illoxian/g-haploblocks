import random
import sys

dir = sys.argv[1]
m = int(sys.argv[2])
n = int(sys.argv[3])
error = float(sys.argv[4])

# m = 5000
# n = 5380000
# error = 0.05
if (dir[len(dir)-1] != "/"):
    dir += "/"

name = dir+"hap_gen_"+str(m)+"_"+str(n)+".txt"

f = open(name, "w")

lmot = []
for y in range(n):
    lmot.append(random.choice(["0","1"]))


for x in range(m):
    print("\r"+str(x))
    for y in range(n):
        if (random.random() <= error):
            lmot[y] = random.choice(["0","1"])
    f.write("".join(lmot)+"\n")
f.close()


print("creation of the file: "+name)
