#include <bits/stdc++.h>
#include "Hardcode.cpp"
using namespace std;

bool testcases = false;

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

void solve(int testcase) {
    string line;
    vector<string> lines;
    int pc = 0;
    
    while(getline(cin, line)) {
        line = trim(line);
        if (line.empty()) continue;

        string instr = "";
        int itr = 0;
        while(itr < line.size()){
            if(line[itr] == ':' || line[itr] == ' ' || line[itr] == '\t'){
                if(instr != "") break;
            } else {
                instr += line[itr];
            }
            itr++;
        }
        if(instr == "") continue;

        bool is_label = false;
        int otheritr = itr;
        while(otheritr < line.size()){
            if(line[otheritr] == ':'){
                labels[instr] = pc;
                is_label = true;
                break;
            }
            if(line[otheritr] != ' ' && line[otheritr] != '\t') break;
            otheritr++;
        }

        string code_part = is_label ? line.substr(otheritr + 1) : line;
        code_part = trim(code_part);

        if (!code_part.empty()) {
            lines.push_back(code_part);
            pc++;
        }
    }

    if (lines.empty()) return;

    bool has_virtual_halt = false;
    for (const string& l : lines) {
        string compact = "";
        for (char c : l) if (c != ' ' && c != '\t' && c != '\r') compact += c;
        if (compact == "beqzero,zero,0" || compact == "beqzero,zero,0x00000000") {
            has_virtual_halt = true;
        }
    }
    if (!has_virtual_halt) {
        cout << "Virtual Halt missing\n";
        return;
    }

    int current_pc = 0;
    for(auto line_content : lines){
        current_pc += 1;
        string instr = "", rd = "", rs1 = "", rs2 = "", imm12 = "";
        int itr = 0;
        
        while(itr < line_content.size()){
            auto E = line_content[itr];
            if(E == ':' || E == ' ' || E == '\t'){
                if(instr != "") break;
            } else {
                instr += E;
            }
            itr += 1;
        }

        string itype = instr_type[instr];
        if(itype == ""){
            cout << "Invalid Instruction type at line : " << current_pc << "\n";
            break;
        }

        if(itype == "R"){
            while(itr < line_content.size()){
                auto E = line_content[itr];
                if(E != ' ') rd += E;
                itr += 1;
                if(rd != "" and line_content[itr] == ','){ itr += 1; break; }
            }
            while(itr < line_content.size()){
                auto E = line_content[itr];
                if(E != ' ') rs1 += E;
                itr += 1;
                if(rs1 != "" and line_content[itr] == ','){ itr += 1; break; }
            }
            while(itr < line_content.size()){
                auto E = line_content[itr];
                if(E != ' ') rs2 += E;
                itr += 1;
                if(rs2 != "" and line_content[itr] == ','){ itr += 1; break; }
            }
            
            rd = trim(rd); rs1 = trim(rs1); rs2 = trim(rs2);
            string opcode = OPCODE[instr];
            string RS1 = REG[rs1], RS2 = REG[rs2], RD = REG[rd];
            
            if(RS1 == "" or RS2 == "" or RD == ""){
                cout << "Invalid Register name in line : " << current_pc << "\n";
                break;
            }
            cout << FUNCT7[instr] + RS2 + RS1 + FUNCT3[instr] + RD + opcode << "\n";
            continue;
        }

        if(itype == "I" or itype == "S" or itype == "B"){
            while(itr < line_content.size()){
                auto E = line_content[itr];
                if(E != ' ') rd += E;
                itr += 1;
                if(rd != "" and line_content[itr] == ','){ itr += 1; break; }
            }

            if(instr == "lw" or instr == "sw"){
                while(itr < line_content.size()){
                    auto E = line_content[itr];
                    if(E != ' ') imm12 += E;
                    itr += 1;
                    if(imm12 != "" and line_content[itr] == '('){ itr += 1; break; }
                }
                while(itr < line_content.size()){
                    auto E = line_content[itr];
                    if(E != ' ') rs1 += E;
                    itr += 1;
                    if(rs1 != "" and line_content[itr] == ')'){ itr += 1; break; }
                }
            } else {
                while(itr < line_content.size()){
                    auto E = line_content[itr];
                    if(E != ' ') rs1 += E;
                    itr += 1;
                    if(rs1 != "" and line_content[itr] == ','){ itr += 1; break; }
                }
                while(itr < line_content.size()){
                    auto E = line_content[itr];
                    if(E != ' ') imm12 += E;
                    itr += 1;
                    if(imm12 != "" and (itr == line_content.size() || line_content[itr] == ',')){
                        if(itr < line_content.size()) itr += 1; 
                        break; 
                    }
                }
            }

            rd = trim(rd); rs1 = trim(rs1); imm12 = trim(imm12);

            int size_val = (itype == "B") ? 13 : 12;
            imm12 = Binary(imm12, size_val, current_pc - 1);
            
            if(imm12 == "-1"){
                cout << "The Immediate is above the range in line : " << current_pc << "\n";
                break;
            }
            if(imm12 == "-2"){
                cout << "The Label is not present in the file\n";
                break;
            }

            string RS1 = REG[rs1], RD = REG[rd];
            if(RS1 == "" or RD == ""){
                cout << "Invalid Register name in line : " << current_pc << "\n";
                break;
            }

            if(instr == "sw"){
                string Answer = "";
                for(int a=0 ; a<7 ; a+=1) Answer += imm12[a];
                Answer += RD + RS1 + FUNCT3[instr];
                for(int a=7 ; a<12 ; a+=1) Answer += imm12[a];
                Answer += OPCODE[instr];
                cout << Answer << "\n";
                continue;
            }

            if(itype == "B"){
                string Answer = "";
                Answer += imm12[0];
                for(int i = 2 ; i < 8 ; i += 1) Answer += imm12[i];
                Answer += RS1 + RD + FUNCT3[instr];
                for(int i = 8 ; i < 12 ; i += 1) Answer += imm12[i];
                Answer += imm12[1];
                Answer += OPCODE[instr];
                cout << Answer << "\n";
                continue;
            }
            
            cout << imm12 + RS1 + FUNCT3[instr] + RD + OPCODE[instr] << "\n";
            continue;
        }
        
        if(itype == "U" || itype == "J"){
            while(itr < line_content.size()){
                auto E = line_content[itr];
                if(E != ' ') rd += E; 
                itr += 1;
                if(rd != "" and line_content[itr] == ','){ itr += 1; break; }
            }
            while(itr < line_content.size()){
                auto E = line_content[itr];
                if(E != ' ') imm12 += E;
                itr += 1;
                if(imm12 != "" and (itr == line_content.size() || line_content[itr] == ',')){
                    if(itr < line_content.size()) itr += 1; 
                    break; 
                }
            }

            rd = trim(rd); imm12 = trim(imm12);
            string RD = REG[rd]; 
            
            int size_val = (itype == "J") ? 21 : 20;
            imm12 = Binary(imm12, size_val, current_pc - 1);

            if(RD == ""){
                cout << "Incorrect register name at line : " << current_pc << "\n";
                break;
            }
            if(imm12 == "-1"){
                cout << "Immediate out of range at line : " << current_pc << "\n";
                break;
            }

            if(itype == "J"){
                string Answer = "";
                Answer += imm12[0];
                for(int a = 10; a < 20 ; a+=1) Answer += imm12[a];
                Answer += imm12[9];
                for(int a = 1 ; a < 9 ; a += 1) Answer += imm12[a];
                cout << Answer + RD + OPCODE[instr] << "\n";
            } else {
                cout << imm12 + RD + OPCODE[instr] << "\n";
            }
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
