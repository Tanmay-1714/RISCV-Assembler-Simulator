import sys


WORD_MASK = 0xFFFFFFFF
PROGRAM_BASE = 0x00000000
PROGRAM_LIMIT = 0x000000FF
STACK_BASE = 0x00000100
STACK_LIMIT = 0x0000017F
DATA_BASE = 0x00010000
DATA_LIMIT = 0x0001007F
STACK_POINTER_INIT = 0x0000017C


def fail(message):
    print(message)
    sys.exit(1)


def sign_extend(value, bits):
    sign_bit = 1 << (bits - 1)
    return (value ^ sign_bit) - sign_bit


def to_signed(value):
    return sign_extend(value & WORD_MASK, 32)


def to_unsigned(value):
    return value & WORD_MASK


def bits_to_int(bit_string):
    return int(bit_string, 2)


def format_binary32(value):
    return "0b" + format(value & WORD_MASK, "032b")


def decode_b_imm(instruction):
    imm12 = (instruction >> 31) & 0x1
    imm10_5 = (instruction >> 25) & 0x3F
    imm4_1 = (instruction >> 8) & 0xF
    imm11 = (instruction >> 7) & 0x1
    imm = (imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1)
    return sign_extend(imm, 13)


def decode_j_imm(instruction):
    imm20 = (instruction >> 31) & 0x1
    imm10_1 = (instruction >> 21) & 0x3FF
    imm11 = (instruction >> 20) & 0x1
    imm19_12 = (instruction >> 12) & 0xFF
    imm = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1)
    return sign_extend(imm, 21)


def build_machine_state():
    registers = [0] * 32
    registers[2] = STACK_POINTER_INIT

    stack_memory = {address: 0 for address in range(STACK_BASE, STACK_LIMIT + 1)}
    data_memory = {address: 0 for address in range(DATA_BASE, DATA_LIMIT + 1)}
    return registers, stack_memory, data_memory


def load_program(input_path):
    try:
        with open(input_path, "r", encoding="utf-8") as source:
            lines = [line.strip() for line in source if line.strip()]
    except FileNotFoundError:
        fail(f"ERROR: Input file not found: {input_path}")

    program = {}
    for index, line in enumerate(lines, start=1):
        if len(line) != 32 or any(bit not in "01" for bit in line):
            fail(f"ERROR: Invalid binary instruction at line {index}")
        address = PROGRAM_BASE + (index - 1) * 4
        if address > PROGRAM_LIMIT:
            fail(f"ERROR: Program memory overflow at line {index}")
        program[address] = bits_to_int(line)
    return program


def read_word(address, stack_memory, data_memory):
    if address in stack_memory:
        return stack_memory[address]
    if address in data_memory:
        return data_memory[address]
    return 0


def write_word(address, value, stack_memory, data_memory):
    value &= WORD_MASK
    if address in stack_memory:
        stack_memory[address] = value
        return
    if address in data_memory:
        data_memory[address] = value
        return
    fail(f"ERROR: Invalid memory write at address {hex(address)}")


def write_register(registers, index, value):
    if index != 0:
        registers[index] = value & WORD_MASK
    registers[0] = 0


def execute(program, output_path, readable_path=None):
    registers, stack_memory, data_memory = build_machine_state()
    pc = PROGRAM_BASE
    traces = []

    while True:
        if pc not in program:
            fail(f"ERROR: No instruction found at PC {hex(pc)}")

        instruction = program[pc]
        opcode = instruction & 0x7F
        rd = (instruction >> 7) & 0x1F
        funct3 = (instruction >> 12) & 0x7
        rs1 = (instruction >> 15) & 0x1F
        rs2 = (instruction >> 20) & 0x1F
        funct7 = (instruction >> 25) & 0x7F
        next_pc = (pc + 4) & WORD_MASK
        halt_reached = False

        if opcode == 0x33:
            lhs = registers[rs1]
            rhs = registers[rs2]
            if funct3 == 0x0 and funct7 == 0x00:
                result = lhs + rhs
            elif funct3 == 0x0 and funct7 == 0x20:
                result = lhs - rhs
            elif funct3 == 0x1 and funct7 == 0x00:
                result = lhs << (rhs & 0x1F)
            elif funct3 == 0x2 and funct7 == 0x00:
                result = 1 if to_signed(lhs) < to_signed(rhs) else 0
            elif funct3 == 0x3 and funct7 == 0x00:
                result = 1 if lhs < rhs else 0
            elif funct3 == 0x4 and funct7 == 0x00:
                result = lhs ^ rhs
            elif funct3 == 0x5 and funct7 == 0x00:
                result = lhs >> (rhs & 0x1F)
            elif funct3 == 0x6 and funct7 == 0x00:
                result = lhs | rhs
            elif funct3 == 0x7 and funct7 == 0x00:
                result = lhs & rhs
            else:
                fail(f"ERROR: Unsupported R-type instruction at PC {hex(pc)}")
            write_register(registers, rd, result)

        elif opcode == 0x03:
            imm = sign_extend((instruction >> 20) & 0xFFF, 12)
            address = (registers[rs1] + imm) & WORD_MASK
            if funct3 != 0x2:
                fail(f"ERROR: Unsupported load instruction at PC {hex(pc)}")
            write_register(registers, rd, read_word(address, stack_memory, data_memory))

        elif opcode == 0x13:
            imm = sign_extend((instruction >> 20) & 0xFFF, 12)
            if funct3 == 0x0:
                write_register(registers, rd, registers[rs1] + imm)
            elif funct3 == 0x3:
                write_register(registers, rd, 1 if registers[rs1] < to_unsigned(imm) else 0)
            else:
                fail(f"ERROR: Unsupported I-type instruction at PC {hex(pc)}")

        elif opcode == 0x67:
            imm = sign_extend((instruction >> 20) & 0xFFF, 12)
            if funct3 != 0x0:
                fail(f"ERROR: Unsupported jalr instruction at PC {hex(pc)}")
            target = (registers[rs1] + imm) & WORD_MASK
            target &= ~1
            write_register(registers, rd, next_pc)
            next_pc = target

        elif opcode == 0x23:
            imm = ((instruction >> 25) << 5) | ((instruction >> 7) & 0x1F)
            imm = sign_extend(imm, 12)
            address = (registers[rs1] + imm) & WORD_MASK
            if funct3 != 0x2:
                fail(f"ERROR: Unsupported store instruction at PC {hex(pc)}")
            write_word(address, registers[rs2], stack_memory, data_memory)

        elif opcode == 0x63:
            imm = decode_b_imm(instruction)
            lhs = registers[rs1]
            rhs = registers[rs2]
            take_branch = False

            if funct3 == 0x0:
                take_branch = lhs == rhs
            elif funct3 == 0x1:
                take_branch = lhs != rhs
            elif funct3 == 0x4:
                take_branch = to_signed(lhs) < to_signed(rhs)
            elif funct3 == 0x5:
                take_branch = to_signed(lhs) >= to_signed(rhs)
            elif funct3 == 0x6:
                take_branch = lhs < rhs
            elif funct3 == 0x7:
                take_branch = lhs >= rhs
            else:
                fail(f"ERROR: Unsupported branch instruction at PC {hex(pc)}")

            if take_branch:
                next_pc = (pc + imm) & WORD_MASK

            if instruction == 0x00000063:
                halt_reached = True

        elif opcode == 0x17:
            imm = instruction & 0xFFFFF000
            write_register(registers, rd, pc + imm)

        elif opcode == 0x37:
            imm = instruction & 0xFFFFF000
            write_register(registers, rd, imm)

        elif opcode == 0x6F:
            imm = decode_j_imm(instruction)
            write_register(registers, rd, next_pc)
            next_pc = (pc + imm) & WORD_MASK

        else:
            fail(f"ERROR: Unsupported opcode at PC {hex(pc)}")

        registers[0] = 0
        pc = next_pc
        traces.append(" ".join([format_binary32(pc)] + [format_binary32(value) for value in registers]) + " ")

        if halt_reached:
            break

    memory_dump = [
        f"0x{address:08X}:{format_binary32(data_memory[address])}"
        for address in range(DATA_BASE, DATA_LIMIT + 1, 4)
    ]
    output_lines = traces + memory_dump

    with open(output_path, "w", encoding="utf-8") as output_file:
        output_file.write("\n".join(output_lines))

    if readable_path:
        with open(readable_path, "w", encoding="utf-8") as readable_file:
            readable_file.write("\n".join(output_lines))


def main():
    if len(sys.argv) < 3:
        fail("ERROR: Usage: python3 Simulator.py <input_machine_code_path> <output_trace_path> [output_readable_path]")

    input_path = sys.argv[1]
    output_path = sys.argv[2]
    readable_path = sys.argv[3] if len(sys.argv) > 3 else None

    program = load_program(input_path)
    execute(program, output_path, readable_path)


if __name__ == "__main__":
    main()
