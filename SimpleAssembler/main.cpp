#include <bits/stdc++.h>
#include "Hardcode.cpp"
using namespace std;

#ifdef ONPC
#include "debug_utils.h"
#else
#define dbg(...)
#endif

//#define int long long
bool testcases = false;

void solve(int testcase) {
  string line;
  
  vector<string> lines;
  int line_no = 0;
  while(getline(cin, line)) {
    lines.push_back(line);
    line_no += 1;
    string instr = "", rd = "", rs1 = "", rs2 = "", imm12 = "";
    int itr = 0;
    while(itr < line.size()){
      auto E = line[itr];
      if(E != ' '){
        instr += E;
      }
      if(instr != "" and (E == ' ' or E == ':')){
        break;
      }
      itr += 1;
    }

    bool otherflag = false;
    int otheritr = itr;
    while(otheritr < line.size()){
      auto E = line[otheritr];

      if(E == ':'){
        labels[instr] = line_no;
        otherflag = true;
      }
      
      if(E != ' '){
        break;
      }
      otheritr += 1;
    }
  }
  
  line_no = 0;
  for(auto line : lines){
    line_no += 1;
    cout << line << endl;
    string binary = "";

    string instr = "", rd = "", rs1 = "", rs2 = "", imm12 = "";
    int itr = 0;
    while(itr < line.size()){
      auto E = line[itr];
      if(E != ' '){
        instr += E;
      }
      if(instr != "" and (E == ' ' or E == ':')){
        break;
      }
      itr += 1;
    }

    bool otherflag = false;
    int otheritr = itr;
    while(otheritr < line.size()){
      auto E = line[otheritr];

      if(E == ':'){
        labels[instr] = line_no;
        otherflag = true;
      }
      
      if(E != ' '){
        break;
      }
      otheritr += 1;
    }

      

    if(otherflag) continue;

    string itype = instr_type[instr];
    if(itype == ""){
      cout << "Invalid Instruction type at line : " << line_no << endl;
      break;
    }

    if(itype == "R"){

      while(itr < line.size()){
        auto E = line[itr];
        if(E != ' '){
          rd += E;
        }
        itr += 1;
        if(rd != "" and line[itr] == ','){
          itr += 1;
          break;
        }
      }

      while(itr < line.size()){
        auto E = line[itr];
        if(E != ' '){
          rs1 += E;
        }
        itr += 1;
        if(rs1 != "" and line[itr] == ','){
          itr += 1;
          break;
        }
      }

      while(itr < line.size()){
        auto E = line[itr];
        if(E != ' '){
          rs2 += E;
        }
        itr += 1;
        if(rs2 != "" and line[itr] == ','){
          itr += 1;
          break;
        }
      }
      string opcode = OPCODE[instr];
      string RS1 = REG[rs1];
      string RS2 = REG[rs2];
      string RD  = REG[rd];
      string funct3 = FUNCT3[instr];
      string funct7 = FUNCT7[instr];
      if(RS1 == "" or RS2 == "" or RD == ""){
        cout << "Invalid Register name in line : " << line_no << endl;
        break;
      }

      string Answer = funct7 + RS2 + RS1 + funct3 + RD + opcode;
      cout << Answer << endl;
      continue;
    }

    if(itype == "I" or itype == "S" or itype == "B"){
      while(itr < line.size()){
        auto E = line[itr];
        if(E != ' '){
          rd += E;
        }
        itr += 1;
        if(rd != "" and line[itr] == ','){
          itr += 1;
          break;
        }
      }

      if(instr == "lw" or instr == "sw"){
        while(itr < line.size()){
          auto E = line[itr];
          if(E != ' '){
            imm12 += E;
          }
          itr += 1;
          if(imm12 != "" and line[itr] == '('){
            itr += 1;
            break;
          }
        }

        while(itr < line.size()){
          auto E = line[itr];
          if(E != ' '){
            rs1 += E;
          }
          itr += 1;
          if(rs1 != "" and line[itr] == ')'){
            itr += 1;
            break;
          }
        }

      }


      else{
        while(itr < line.size()){
          auto E = line[itr];
          if(E != ' '){
            rs1 += E;
          }
          itr += 1;
          if(rs1 != "" and line[itr] == ','){
            itr += 1;
            break;
          }
        }

        while(itr < line.size()){
          auto E = line[itr];
          if(E != ' '){
            imm12 += E;
          }
          itr += 1;
          if(imm12 != "" and line[itr] == ','){
            itr += 1;
            break;
          }
        }

      }


      imm12 = Binary(imm12, 12, line_no);
      if(imm12 == "-1"){
        cout << "The Immediate is above the range in line : " << line_no << endl;
        break;
      }
      if(imm12 == "-2"){
        cout << "The Label is not present in the file" << endl;
        break;
      }

      string RS1 = REG[rs1];
      string RD  = REG[rd];
      string funct3 = FUNCT3[instr];
      string opcode = OPCODE[instr];
      if(RS1 == "" or RD == ""){
        cout << "Invalid Register name in line : " << line_no << endl;
        break;
      }

      string Answer = imm12 + RS1 + funct3 + RD + opcode;

      if(instr == "sw"){
        Answer = "";
        for(int a=0 ; a<7 ; a+=1){
          Answer += imm12[a];
        }
        
        Answer += RD + RS1 + funct3;

        for(int a=7 ; a<12 ; a+=1){
          Answer += imm12[a];
        }
        Answer += opcode;
        cout << Answer << endl;

        continue;
      }


      if(itype == "B"){
        Answer = "";

        Answer = imm12[0];
        for(int i = 2 ; i < 8 ; i += 1){
          Answer += imm12[i];
        }

        Answer += RS1 + RD + funct3;

        for(int i = 8 ; i < 12 ; i += 1){
          Answer += imm12[i];
        }

        Answer += imm12[1];
        Answer += opcode;
        cout << Answer << endl;
        continue;
      }
      cout << Answer << endl;
      continue;
    }
    
    if(itype == "U"){
      while(itr < line.size()){
        auto E = line[itr];
        if(E != ' '){
          rs1 += E;
        }
        itr += 1;
        if(rs1 != "" and line[itr] == ','){
          itr += 1;
          break;
        }
      }
      while(itr < line.size()){
        auto E = line[itr];
        if(E != ' '){
          imm12 += E;
        }
        itr += 1;
        if(imm12 != "" and line[itr] == ','){
          itr += 1;
          break;
        }
      }

      string RD = REG[rs1];
      imm12 = Binary(imm12, 20, line_no);

      string opcode = OPCODE[instr];

      if(RD == ""){
        cout << "Incorrect register name at line : " << line_no << endl;
        break;
      }

      if(imm12 == "-1"){
        cout << "Immediate out of range at line : " << line_no << endl;
        break;
      }

      string Answer = imm12 + RD + opcode;

      cout << Answer << endl;

    }


    if(itype == "J"){
      while(itr < line.size()){
        auto E = line[itr];
        if(E != ' '){
          rs1 += E;
        }
        itr += 1;
        if(rs1 != "" and line[itr] == ','){
          itr += 1;
          break;
        }
      }
      while(itr < line.size()){
        auto E = line[itr];
        if(E != ' '){
          imm12 += E;
        }
        itr += 1;
        if(imm12 != "" and line[itr] == ','){
          itr += 1;
          break;
        }
      }

      string RD = REG[rs1];
      string opcode = OPCODE[instr];
      string Answer = "";
      imm12 = Binary(imm12, 20, line_no);

      if(RD == ""){
        cout << "Incorrect register name at line : " << line_no << endl;
        break;
      }
      if(imm12 == "-1"){
        cout << "Immediate out of range at line : " << line_no << endl;
        break;
      }

      Answer = imm12[0];
      for(int a = 10; a < 20 ; a+=1){
        Answer += imm12[a];
      }

      Answer += imm12[9];
      for(int a = 1 ; a < 9 ; a += 1){ 
        Answer += imm12[a];
      }

      Answer += RD + opcode;
      cout << Answer << endl;
    }
  }

}


signed main() {
  ios::sync_with_stdio(0);
  cin.tie(0);

  int t = 1, testcase = 1;
  if(testcases) cin >> t;
  while (t--) {
    solve(testcase); testcase++;
  }
  return 0;
}
