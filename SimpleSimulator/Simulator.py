import sys
import os
import shutil

if len(sys.argv) < 3:
    sys.exit(1)

input_path = sys.argv[1]
output_path = sys.argv[2]
readable_path = sys.argv[3] if len(sys.argv) > 3 else None

# Get the exact folder where Simulator.py is located
script_dir = os.path.dirname(os.path.abspath(__file__))
bin_dir = os.path.join(script_dir, "bin")

if not os.path.exists(bin_dir):
    os.makedirs(bin_dir)

exe_name = "SimExec.exe" if os.name == 'nt' else "SimExec"
executable = os.path.join(bin_dir, exe_name)

cpp_source = os.path.join(script_dir, "sim_main.cpp")
compile_log = os.path.join(script_dir, "compile_log.txt")

# Compile using absolute paths
os.system(f'g++ "{cpp_source}" -o "{executable}" 2> "{compile_log}"')

# Run using absolute paths
command = f'"{executable}" < "{input_path}" > "{output_path}"'
os.system(command)

if readable_path:
    if os.path.exists(output_path):
        shutil.copy(output_path, readable_path)
else:
    if os.path.exists(output_path):
        with open(output_path, 'r') as f:
            print(f.read(), end="")
