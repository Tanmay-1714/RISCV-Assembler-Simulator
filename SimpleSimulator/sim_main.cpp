#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <bitset>
#include <iomanip>

using namespace std;

const uint32_t WORD_MASK = 0xFFFFFFFF;
const uint32_t PROGRAM_BASE = 0x00000000;
const uint32_t STACK_BASE = 0x00000100;
const uint32_t STACK_LIMIT = 0x0000017F;
const uint32_t DATA_BASE = 0x00010000;
const uint32_t DATA_LIMIT = 0x0001007F;

int32_t sign_extend(uint32_t value, int bits) {
    uint32_t sign_bit = 1 << (bits - 1);
    return (value ^ sign_bit) - sign_bit;
}

string format_binary32(uint32_t value) {
    return "0b" + bitset<32>(value).to_string();
}

int32_t decode_b_imm(uint32_t inst) {
    uint32_t imm12 = (inst >> 31) & 0x1;
    uint32_t imm10_5 = (inst >> 25) & 0x3F;
    uint32_t imm4_1 = (inst >> 8) & 0xF;
    uint32_t imm11 = (inst >> 7) & 0x1;
    uint32_t imm = (imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1);
    return sign_extend(imm, 13);
}

int32_t decode_j_imm(uint32_t inst) {
    uint32_t imm20 = (inst >> 31) & 0x1;
    uint32_t imm10_1 = (inst >> 21) & 0x3FF;
    uint32_t imm11 = (inst >> 20) & 0x1;
    uint32_t imm19_12 = (inst >> 12) & 0xFF;
    uint32_t imm = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1);
    return sign_extend(imm, 21);
}

int main() {
    vector<string> lines;
    string line;
    while (cin >> line) {
        if (!line.empty()) lines.push_back(line);
    }

    map<uint32_t, uint32_t> program;
    for (size_t i = 0; i < lines.size(); ++i) {
        program[PROGRAM_BASE + i * 4] = stoul(lines[i], nullptr, 2);
    }

    uint32_t registers[32] = {0};
    registers[2] = 0x0000017C;
    map<uint32_t, uint32_t> stack_memory;
    map<uint32_t, uint32_t> data_memory;
    for (uint32_t addr = STACK_BASE; addr <= STACK_LIMIT; ++addr) stack_memory[addr] = 0;
    for (uint32_t addr = DATA_BASE; addr <= DATA_LIMIT; ++addr) data_memory[addr] = 0;

    uint32_t pc = PROGRAM_BASE;
    vector<string> traces;
    bool crashed = false;

    while (true) {
        if (program.find(pc) == program.end()) {
            crashed = true;
            break;
        }

        uint32_t instruction = program[pc];
        uint32_t opcode = instruction & 0x7F;
        uint32_t rd = (instruction >> 7) & 0x1F;
        uint32_t funct3 = (instruction >> 12) & 0x7;
        uint32_t rs1 = (instruction >> 15) & 0x1F;
        uint32_t rs2 = (instruction >> 20) & 0x1F;
        uint32_t funct7 = (instruction >> 25) & 0x7F;
        uint32_t next_pc = pc + 4;
        bool halt_reached = false;

        if (opcode == 0x33) {
            uint32_t lhs = registers[rs1];
            uint32_t rhs = registers[rs2];
            uint32_t res = 0;
            if (funct3 == 0x0 && funct7 == 0x00) res = lhs + rhs;
            else if (funct3 == 0x0 && funct7 == 0x20) res = lhs - rhs;
            else if (funct3 == 0x1 && funct7 == 0x00) res = lhs << (rhs & 0x1F);
            else if (funct3 == 0x2 && funct7 == 0x00) res = ((int32_t)lhs < (int32_t)rhs) ? 1 : 0;
            else if (funct3 == 0x3 && funct7 == 0x00) res = (lhs < rhs) ? 1 : 0;
            else if (funct3 == 0x4 && funct7 == 0x00) res = lhs ^ rhs;
            else if (funct3 == 0x5 && funct7 == 0x00) res = lhs >> (rhs & 0x1F);
            else if (funct3 == 0x6 && funct7 == 0x00) res = lhs | rhs;
            else if (funct3 == 0x7 && funct7 == 0x00) res = lhs & rhs;
            else { crashed = true; break; }
            if (rd != 0) registers[rd] = res;
        }
        else if (opcode == 0x03) {
            int32_t imm = sign_extend((instruction >> 20) & 0xFFF, 12);
            uint32_t addr = registers[rs1] + imm;
            if (funct3 != 0x2) { crashed = true; break; }
            if (stack_memory.count(addr)) registers[rd] = stack_memory[addr];
            else if (data_memory.count(addr)) registers[rd] = data_memory[addr];
            else { crashed = true; break; } 
            if (rd == 0) registers[0] = 0;
        }
        else if (opcode == 0x13) {
            int32_t imm = sign_extend((instruction >> 20) & 0xFFF, 12);
            uint32_t res = 0;
            if (funct3 == 0x0) res = registers[rs1] + imm;
            else if (funct3 == 0x3) res = (registers[rs1] < (uint32_t)imm) ? 1 : 0;
            else { crashed = true; break; }
            if (rd != 0) registers[rd] = res;
        }
        else if (opcode == 0x67) {
            int32_t imm = sign_extend((instruction >> 20) & 0xFFF, 12);
            if (funct3 != 0x0) { crashed = true; break; }
            uint32_t target = (registers[rs1] + imm) & ~1;
            if (rd != 0) registers[rd] = next_pc;
            next_pc = target;
        }
        else if (opcode == 0x23) {
            int32_t imm = ((instruction >> 25) << 5) | ((instruction >> 7) & 0x1F);
            imm = sign_extend(imm, 12);
            uint32_t addr = registers[rs1] + imm;
            if (funct3 != 0x2) { crashed = true; break; }
            if (stack_memory.count(addr)) stack_memory[addr] = registers[rs2];
            else if (data_memory.count(addr)) data_memory[addr] = registers[rs2];
            else { crashed = true; break; } 
        }
        else if (opcode == 0x63) {
            int32_t imm = decode_b_imm(instruction);
            uint32_t lhs = registers[rs1];
            uint32_t rhs = registers[rs2];
            bool take_branch = false;
            if (funct3 == 0x0) take_branch = (lhs == rhs);
            else if (funct3 == 0x1) take_branch = (lhs != rhs);
            else if (funct3 == 0x4) take_branch = ((int32_t)lhs < (int32_t)rhs);
            else if (funct3 == 0x5) take_branch = ((int32_t)lhs >= (int32_t)rhs);
            else if (funct3 == 0x6) take_branch = (lhs < rhs);
            else if (funct3 == 0x7) take_branch = (lhs >= rhs);
            else { crashed = true; break; }
            if (take_branch) next_pc = pc + imm;
            if (instruction == 0x00000063) halt_reached = true;
        }
        else if (opcode == 0x17) {
            uint32_t imm = instruction & 0xFFFFF000;
            if (rd != 0) registers[rd] = pc + imm;
        }
        else if (opcode == 0x37) {
            uint32_t imm = instruction & 0xFFFFF000;
            if (rd != 0) registers[rd] = imm;
        }
        else if (opcode == 0x6F) {
            int32_t imm = decode_j_imm(instruction);
            if (rd != 0) registers[rd] = next_pc;
            next_pc = pc + imm;
        }
        else {
            crashed = true; break;
        }

        registers[0] = 0; 
        
        string trace_line = format_binary32(pc);
        for (int i = 0; i < 32; ++i) {
            trace_line += " " + format_binary32(registers[i]);
        }
        traces.push_back(trace_line);
        
        pc = next_pc;
        if (halt_reached) break;
    }

    for (const string& t : traces) {
        cout << t << "\n";
    }

    if (!crashed) {
        for (uint32_t addr = DATA_BASE; addr <= DATA_LIMIT; addr += 4) {
            cout << "0x" << uppercase << hex << setfill('0') << setw(8) << addr << ":" 
                 << format_binary32(data_memory[addr]) << "\n";
        }
    }

    return 0;
}
