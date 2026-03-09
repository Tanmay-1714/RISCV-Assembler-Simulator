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

std::string Binary(std::string number, int size, int line_no){
  if(number.size() == 0) return "-1";
  int num = 0;
  int pow = 1;
  bool flag = false;

  if(number[0] - '0' > 9 or number[0] - '0' < 0){
    if(labels[number] == 0){
      return "-2";
    }
    else{
      num = (labels[number] - line_no) * 4;
      flag = true;
    }
  }

  if(number.size() > 2 && number[1] == 'x'){
    num = HexToDecimal(number);
    flag = true;
  }

  int Max = (1ll << (size - 1)) - 1;
  int Min = -Max - 1;

  for(int a=number.size() - 1 ; a >= 0 and !flag ; a -= 1){
    if(number[a] == '-'){
      num = -num;
    }
    else {
      num += pow * (number[a] - '0');
    }
    if(num > Max) return "-1";
    if(num < Min) return "-1";
    pow *= 10;
  }

  printf("%lld ", num);

  std::string bin = "";
  bool complement = false;

  if(num < 0){
    num = - num;
    complement = true;
    // we nned to convert it into 2's compliment for this now
  }

  while(num > 0){
    bin = ((num & 1) ? "1" : "0") + bin ;
    num /= 2;
  }

  while(bin.size() != size){
    bin = "0" + bin;
  }

  if(complement){
    std::string Nbin = "";
    int n = bin.size();
    bool first = false;
    for(int a = n - 1; a >= 0 ; a-=1){
      if(first) Nbin = (bin[a] == '1' ? '0' : '1') + Nbin;
      else Nbin = bin[a] + Nbin;
      if(bin[a] == '1') first = true;
    }
    bin = Nbin;
  }
  return bin;
}


int HexToDecimal(std::string hex) {
    unsigned int result = 0;
    int start = 0;

    if (hex.length() >= 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        start = 2;
    }

    for (int i = start; i < hex.length(); i++) {
        char c = hex[i];
        int value = 0;

        if (c >= '0' && c <= '9') {
            value = c - '0';
        } 
        else if (c >= 'A' && c <= 'F') {
            value = c - 'A' + 10;
        } 
        else if (c >= 'a' && c <= 'f') {
            value = c - 'a' + 10;
        } 
        else {
            break; 
        }
        result = (result << 4) | value;
    }

    return result;
}
