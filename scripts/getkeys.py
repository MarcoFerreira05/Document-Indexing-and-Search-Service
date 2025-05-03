# Script that prints every file path in the dataset directory.
# Used to test the search component without interaction with the rest of the server.

import os

path = ""

for i in range(0, 2230):
    path = "dataset/" + str(i) + ".txt"
    if(os.path.exists(path)):
        print(i)