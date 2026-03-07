# this is just a python wrapper for the project to accompany the testing setup because i have used cpp language for the assignment
#thankyou

import sys
import os

input_path = sys.argv[1]
output_path = sys.argv[2]

readable_path = None
if len(sys.argv) > 3:
    readable_path = sys.argv[3]

# path to compiled C++ assembler this is in the binary folder for readibility

executable = "./bin/Done"

# run assembler and redirect input/output

command = executable + " < " + input_path + " > " + output_path
os.system(command)

# optional readable output file

if readable_path:
    os.system("cp " + output_path + " " + readable_path)
