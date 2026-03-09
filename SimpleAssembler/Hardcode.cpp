#include <unordered_map>
#include <string>
#define int long long

std::unordered_map<std::string,std::string> REG = {

{"zero","00000"},
{"ra","00001"},
{"sp","00010"},
{"gp","00011"},
{"tp","00100"},

{"t0","00101"},
{"t1","00110"},
{"t2","00111"},

{"s0","01000"},
{"s1","01001"},

{"a0","01010"},
{"a1","01011"},
{"a2","01100"},
{"a3","01101"},
{"a4","01110"},
{"a5","01111"},
{"a6","10000"},
{"a7","10001"},

{"s2","10010"},
{"s3","10011"},
{"s4","10100"},
{"s5","10101"},
{"s6","10110"},
{"s7","10111"},
{"s8","11000"},
{"s9","11001"},
{"s10","11010"},
{"s11","11011"},

{"t3","11100"},
{"t4","11101"},
{"t5","11110"},
{"t6","11111"}

};



std::unordered_map<std::string,std::string> OPCODE = {

{"add","0110011"},
{"sub","0110011"},
{"sll","0110011"},
{"slt","0110011"},
{"sltu","0110011"},
{"xor","0110011"},
{"srl","0110011"},
{"or","0110011"},
{"and","0110011"},

{"lw","0000011"},
{"addi","0010011"},
{"sltiu","0010011"},
{"jalr","1100111"},

{"sw","0100011"},

{"beq","1100011"},
{"bne","1100011"},
{"blt","1100011"},
{"bge","1100011"},
{"bltu","1100011"},
{"bgeu","1100011"},

{"lui","0110111"},
{"auipc","0010111"},

{"jal","1101111"}

};


std::unordered_map<std::string,std::string> FUNCT3 = {

{"add","000"},
{"sub","000"},
{"sll","001"},
{"slt","010"},
{"sltu","011"},
{"xor","100"},
{"srl","101"},
{"or","110"},
{"and","111"},

{"lw","010"},
{"addi","000"},
{"sltiu","011"},
{"jalr","000"},

{"sw","010"},

{"beq","000"},
{"bne","001"},
{"blt","100"},
{"bge","101"},
{"bltu","110"},
{"bgeu","111"}

};


std::unordered_map<std::string,std::string> FUNCT7 = {

{"add","0000000"},
{"sub","0100000"},
{"sll","0000000"},
{"slt","0000000"},
{"sltu","0000000"},
{"xor","0000000"},
{"srl","0000000"},
{"or","0000000"},
{"and","0000000"}

};


int INSTRUCTION_MEMORY_SIZE = 256;
int STACK_MEMORY_SIZE = 128;
int DATA_MEMORY_SIZE = 128;

int MAX_INSTRUCTIONS = 64;

int STACK_START = 0x00000100;
int STACK_END = 0x0000017F;

int DATA_START = 0x00010000;

int INITIAL_SP = 0x0000017C;

std::string VIRTUAL_HALT = "beq zero,zero,0";

std::unordered_map<std::string,int> LABEL;
int REG_FILE[32] = {0};
int DATA_MEMORY[32] = {0};

std::unordered_map<std::string,std::string> instr_type = {
    {"add", "R"},
    {"sub", "R"},
    {"sll", "R"},
    {"slt", "R"},
    {"sltu", "R"},
    {"xor", "R"},
    {"srl", "R"},
    {"or", "R"},
    {"and", "R"},

    {"addi", "I"},
    {"sltiu", "I"},
    {"lw", "I"},
    {"jalr", "I"},

    {"sw", "S"},

    {"beq", "B"},
    {"bne", "B"},
    {"blt", "B"},
    {"bge", "B"},
    {"bltu", "B"},
    {"bgeu", "B"},

    {"lui", "U"},
    {"auipc", "U"},

    {"jal", "J"}
};

int HexToDecimal(std::string number);

std::unordered_map<std::string, int> labels;

std::string Binary(std::string number, int size, int line_no) {
    if (number.empty()) return "-1";

    long long num = 0;
    if (labels.find(number) != labels.end()) {
        num = (labels[number] - line_no) * 4;
    } else {
        try {
            num = std::stoll(number, nullptr, 0);
        } catch (...) {
            return "-2";
        }
    }

    long long max_signed = (1LL << (size - 1)) - 1;
    long long min_signed = -(1LL << (size - 1));
    long long max_unsigned = (1LL << size) - 1;

    if (num < min_signed || num > max_unsigned) {
        return "-1";
    }

    std::string bin = "";
    unsigned long long unum = (unsigned long long)num;

    for (int i = 0; i < size; ++i) {
        bin = std::to_string((unum >> i) & 1) + bin;
    }

    return bin;
}
