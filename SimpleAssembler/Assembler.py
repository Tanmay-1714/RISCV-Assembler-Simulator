# this is not the main code this is just a python wrapper that  calls for the object file created through cpp code 
# and returns the output that is given by that file for main code logic you need to see main.cpp and for all the hardcoded 
# libraries that are used in mina.cpp you can find all of that in Hardcode.cpp
#thanks for reading


import sys
import os
import shutil

input_path = sys.argv[1]
output_path = sys.argv[2]
readable_path = sys.argv[3] if len(sys.argv) > 3 else None

if not os.path.exists("./bin"):
    os.makedirs("./bin")

executable = "./bin/Done" if os.name != 'nt' else ".\\bin\\Done.exe"

os.system(f"g++ main.cpp -o {executable} 2> compile_log.txt")

command = f"{executable} < {input_path} > {output_path}"
os.system(command)

with open("assembler_debug.log", "a") as log:
    log.write(f"--- TEST: {input_path} ---\n")
    if os.path.exists(output_path):
        with open(output_path, "r") as out_f:
            log.write(out_f.read() + "\n")
    else:
        log.write("[CRASH] Output file was not generated.\n")

if readable_path:
    shutil.copy(output_path, readable_path)
else:
    with open(output_path, 'r') as f:
        print(f.read(), end="")
