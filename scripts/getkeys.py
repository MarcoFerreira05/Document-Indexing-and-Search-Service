import os

path = ""

for i in range(0, 2230):
    path = "dataset/" + str(i) + ".txt"
    if(os.path.exists(path)):
        print(i)