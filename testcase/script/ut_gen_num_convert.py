# !/usr/bin/python3
from random import Random

rand = Random()

arr_format = "{\n%s\n}"
item_str = ""
for idx in range(30):
    preserve = rand.randint(1, 4)
    preserve_val = 10 ** preserve
    val = rand.random() * preserve_val
    item_str += "  {" + str(val) + ", \"" + str(round(val, preserve)) + "\", " + str(preserve) + "},\n"

arr = arr_format % item_str

print(arr)
