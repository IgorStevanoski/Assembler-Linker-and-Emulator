#include "../inc/emulator.hpp"
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>

using namespace::std;

int main(int argc, char* argv[]){

  string hex_filename;

  if (argc < 2){
    cout << "Error! Not enough arguments.";
    return -1;
  }
  hex_filename = argv[1];
  //hex_filename = "./program.hex";
  Emulator emulator;

  emulator.fillMemory(hex_filename);

  emulator.emulate();

  //emulator.print_memory();

  return 0;
}

void Emulator::fillMemory(string filename){
  ifstream input_stream(filename);

  string line;
  string addr;
  unsigned addrVal;
  string str[9]; // adresa: val1 val2 val3 val4 val5 val6 val7 val8
  int ii;
  while (getline(input_stream, line)) {
    stringstream ss(line);
    ii = 0;
    while (ss >> str[ii++]);
    addr = "";
    for (int i = 0; i < str[0].length() - 1; i++) // oduzima se ':'
    {
      addr += str[0].at(i);
    }
    addrVal = Util::parseStringHex(addr);
    
    ii = 1; // preskace se adresa
    for (int i = 0; i < 8; i++) // 8 bajtova po liniji
    {
      this->memory[addrVal++] = str[ii++];
    }
  }

  this->symtabStart = Util::parseStringHex(
    str[1] + str[2] + str[3] + str[4]
  );
  this->lpoolStart = Util::parseStringHex(
    str[5] + str[6] + str[7] + str[8]
  );

  lastAddr = addrVal - 8;
  fill_symbols_literals();

  input_stream.close();
}

void Emulator::fill_symbols_literals(){
  map<unsigned, string>::iterator it = memory.begin();
  string temp;

  while (it != memory.end())
  {
    if (it->first >= symtabStart && it->first < lpoolStart){
      temp = "";
      temp = it->second + temp; ++it;    
      temp = it->second + temp; ++it;    
      temp = it->second + temp; ++it;    
      temp = it->second + temp;
      symbols.push_back(Util::parseStringHex(temp));    
    } else if (it->first >= lpoolStart && it->first < lastAddr){
      temp = "";
      temp = it->second + temp; ++it;    
      temp = it->second + temp; ++it;    
      temp = it->second + temp; ++it;    
      temp = it->second + temp;
      literals.push_back(Util::parseStringHex(temp));   
    }
    ++it;
  }
  
}

int Emulator::emulate(){
  string opcode;
  int reg1;
  int reg2;
  pc = startAddress;
  unsigned currPc = pc;
  
  while(true){ // emulacija se zavrsava tek kad se naidje na HALT
    map<unsigned, string>::iterator it = memory.begin();
    
    while (it != memory.end())
    {
      if (it->first != pc) {
        it++;
        continue;
      }
      opcode = it->second;

      if (opcode == INSTR_HALT_OPCODE_string){
        print_end_emulation();
        return 0;
      } else if (opcode == INSTR_INT_OPCODE_string){
        push(status);
        push(pc + 4);
        cause = 4;
        status = status & (~0x1);
        pc = handler;
        ++it; ++it; ++it;
      } else if (opcode == INSTR_IRET_OPCODE_string){
        // nema opcode; iret = pop pc, pop status
      //} else if (opcode == INSTR_RET_OPCODE_string){
        // nema opcode; ret = pop pc
      } else if (opcode == INSTR_CALL_OPCODE_string){
        ++it; // AAAABBBB
        string temp = "";
        temp += it->second.at(0);
        reg1 = Util::parseStringHex(temp);
        ++it; // CCCCDDDD
        temp = "";
        temp += it->second.at(1);
        ++it; // DDDDDDDD
        temp += it->second.at(0);
        temp += it->second.at(1);
        int index = Util::parseStringHex(temp);
        int j;
        unsigned value = 0;
        if (reg1 == 0){ // literal
          j = 0;
          for (auto l : this->literals){
            if (j == index) {
              auto itt = literals.begin();
              advance(itt, j);

              value = *itt;
              break;
            }
            j++;
          }
        } else { // simbol
          j = 0;
          for (auto s : this->symbols){
            if (j == index) {
              auto itt = symbols.begin();
              advance(itt, j);

              value = *itt;
              break;
            }
            j++;
          }
        }
        push(pc + 4);
        pc = value;
        // it = memory.begin(); // krece se od nove adrese
        // while (it != memory.end())
        // {
        //   if (it->first == pc) break;
        // }
      } else if (opcode == INSTR_JMP_OPCODE_string){
        ++it; // AAAABBBB
        string temp = "";
        temp += it->second.at(0);
        int regA = Util::parseStringHex(temp);
        ++it; // CCCCDDDD
        temp = "";
        temp += it->second.at(1);
        ++it; // DDDDDDDD
        temp += it->second.at(0);
        temp += it->second.at(1);
        int index = Util::parseStringDecimal(temp);
        int j;
        unsigned value = 0;
        if (regA == 0){ // literal
          j = 0;
          for (auto l : this->literals){
            if (j == index) {
              auto itt = literals.begin();
              advance(itt, j);

              value = *itt;
              break;
            }
            j++;
          }
        } else { // simbol
          j = 0;
          for (auto s : this->symbols){
            if (j == index) {
              auto itt = symbols.begin();
              advance(itt, j);

              value = *itt;
              break;
            }
            j++;
          }
        }
        pc = value;
        // it = memory.begin(); // krece se od nove adrese
        // while (it != memory.end())
        // {
        //   if (it->first == pc) break;
        // }
      } else if (opcode == INSTR_BEQ_OPCODE_string){
        ++it; // AAAABBBB
        string temp = "";
        temp += it->second.at(0);
        int regA = Util::parseStringHex(temp);
        temp = "";
        temp += it->second.at(1);
        reg1 = Util::parseStringDecimal(temp);
        ++it; // CCCCDDDD
        temp = "";
        temp += it->second.at(0);
        reg2 = Util::parseStringDecimal(temp);
        temp = "";
        temp += it->second.at(1);
        ++it; // DDDDDDDD
        temp += it->second.at(0);
        temp += it->second.at(1);
        int index = Util::parseStringDecimal(temp);
        int j;
        unsigned value = 0;
        if (gpr[reg1] == gpr[reg2]){
          if (regA == 0){ // literal
            j = 0;
            for (auto l : this->literals){
              if (j == index) {
                auto itt = literals.begin();
                advance(itt, j);

                value = *itt;
                break;
              }
              j++;
            }
          } else { // simbol
            j = 0;
            for (auto s : this->symbols){
              if (j == index) {
                auto itt = symbols.begin();
                advance(itt, j);

                value = *itt;
                break;
              }
              j++;
            }
          }
          pc = value;
          // it = memory.begin(); // krece se od nove adrese
          // while (it != memory.end())
          // {
          //   if (it->first == pc) break;
          // }
        }
      } else if (opcode == INSTR_BNE_OPCODE_string){
        ++it; // AAAABBBB
        string temp = "";
        temp += it->second.at(0);
        int regA = Util::parseStringHex(temp);
        temp = "";
        temp += it->second.at(1);
        reg1 = Util::parseStringDecimal(temp);
        ++it; // CCCCDDDD
        temp = "";
        temp += it->second.at(0);
        reg2 = Util::parseStringDecimal(temp);
        temp = "";
        temp += it->second.at(1);
        ++it; // DDDDDDDD
        temp += it->second.at(0);
        temp += it->second.at(1);
        int index = Util::parseStringDecimal(temp);
        int j;
        unsigned value = 0;
        if (gpr[reg1] != gpr[reg2]){
          if (regA == 0){ // literal
            j = 0;
            for (auto l : this->literals){
              if (j == index) {
                auto itt = literals.begin();
                advance(itt, j);

                value = *itt;
                break;
              }
              j++;
            }
          } else { // simbol
            j = 0;
            for (auto s : this->symbols){
              if (j == index) {
                auto itt = symbols.begin();
                advance(itt, j);

                value = *itt;
                break;
              }
              j++;
            }
          }
          pc = value;
          // it = memory.begin(); // krece se od nove adrese
          // while (it != memory.end())
          // {
          //   if (it->first == pc) break;
          // }
        }
      } else if (opcode == INSTR_BGT_OPCODE_string){
        ++it; // AAAABBBB
        string temp = "";
        temp += it->second.at(0);
        int regA = Util::parseStringHex(temp);
        temp = "";
        temp += it->second.at(1);
        reg1 = Util::parseStringDecimal(temp);
        ++it; // CCCCDDDD
        temp = "";
        temp += it->second.at(0);
        reg2 = Util::parseStringDecimal(temp);
        temp = "";
        temp += it->second.at(1);
        ++it; // DDDDDDDD
        temp += it->second.at(0);
        temp += it->second.at(1);
        int index = Util::parseStringDecimal(temp);
        int j;
        unsigned value = 0;
        if (gpr[reg1] > gpr[reg2]){
          if (regA == 0){ // literal
            j = 0;
            for (auto l : this->literals){
              if (j == index) {
                auto itt = literals.begin();
                advance(itt, j);

                value = *itt;
                break;
              }
              j++;
            }
          } else { // simbol
            j = 0;
            for (auto s : this->symbols){
              if (j == index) {
                auto itt = symbols.begin();
                advance(itt, j);

                value = *itt;
                break;
              }
              j++;
            }
          }
          pc = value;
          // it = memory.begin(); // krece se od nove adrese
          // while (it != memory.end())
          // {
          //   if (it->first == pc) break;
          // }
        }
      } else if (opcode == INSTR_PUSH_OPCODE_string){
        ++it; // AAAABBBB
        ++it; // CCCCDDDD
        string temp = "";
        temp += it->second.at(0);
        reg1 = Util::parseStringHex(temp);
        push(gpr[reg1]);
        ++it; // DDDDDDDD
      } else if (opcode == INSTR_POP_OPCODE_string){
        ++it; // AAAABBBB
        string temp = "";
        temp += it->second.at(0);
        reg1 = Util::parseStringHex(temp);
        gpr[reg1] = pop();
        ++it; // CCCCDDDD
        char c = it->second.at(0);
        if (c == '1'){ // u pitanju je iret, treba jos pop status
          ++it; // DDDDDDDD
          ++it; // OPCODE
          status = pop();
          ++it; // AAAABBBB
          ++it; // CCCCDDDD
        }
        ++it; // DDDDDDDD
      } else if (opcode == INSTR_POP_CSR_OPCODE_string){
        // uvek je csr = status
        status = pop();
        ++it; // AAAABBBB
        ++it; // CCCCDDDD
        ++it; // DDDDDDDD
      } else if (opcode == INSTR_XCHG_OPCODE_string){
        ++it; // AAAABBBB
        string tempstr1 = "";
        tempstr1 += it->second.at(1); //tempstr1 = BBBB
        reg1 = Util::parseStringHex(tempstr1);
        ++it; // CCCCDDDD
        string tempstr2 = "";
        tempstr2 += it->second.at(0); //tempstr2 = CCCC
        reg2 = Util::parseStringHex(tempstr2);

        unsigned temp = gpr[reg1];
        gpr[reg1] = gpr[reg2];
        gpr[reg2] = temp;

        ++it; // DDDDDDDD
      } else if (opcode == INSTR_ADD_OPCODE_string){
        ++it; // AAAABBBB
        string tempstr1 = "";
        tempstr1 += it->second.at(1); //tempstr1 = BBBB
        reg1 = Util::parseStringHex(tempstr1);
        ++it; // CCCCDDDD
        string tempstr2 = "";
        tempstr2 += it->second.at(0); //tempstr2 = CCCC
        reg2 = Util::parseStringHex(tempstr2);

        gpr[reg1] = gpr[reg1] + gpr[reg2];

        ++it; // DDDDDDDD
      } else if (opcode == INSTR_SUB_OPCODE_string){
        ++it; // AAAABBBB
        string tempstr1 = "";
        tempstr1 += it->second.at(1); //tempstr1 = BBBB
        reg1 = Util::parseStringHex(tempstr1);
        ++it; // CCCCDDDD
        string tempstr2 = "";
        tempstr2 += it->second.at(0); //tempstr2 = CCCC
        reg2 = Util::parseStringHex(tempstr2);

        gpr[reg1] = gpr[reg1] - gpr[reg2];

        ++it; // DDDDDDDD
      } else if (opcode == INSTR_MUL_OPCODE_string){
        ++it; // AAAABBBB
        string tempstr1 = "";
        tempstr1 += it->second.at(1); //tempstr1 = BBBB
        reg1 = Util::parseStringHex(tempstr1);
        ++it; // CCCCDDDD
        string tempstr2 = "";
        tempstr2 += it->second.at(0); //tempstr2 = CCCC
        reg2 = Util::parseStringHex(tempstr2);

        gpr[reg1] = gpr[reg1] * gpr[reg2];

        ++it; // DDDDDDDD
      } else if (opcode == INSTR_DIV_OPCODE_string){
        ++it; // AAAABBBB
        string tempstr1 = "";
        tempstr1 += it->second.at(1); //tempstr1 = BBBB
        reg1 = Util::parseStringHex(tempstr1);
        ++it; // CCCCDDDD
        string tempstr2 = "";
        tempstr2 += it->second.at(0); //tempstr2 = CCCC
        reg2 = Util::parseStringHex(tempstr2);

        gpr[reg1] = gpr[reg1] / gpr[reg2];

        ++it; // DDDDDDDD
      } else if (opcode == INSTR_NOT_OPCODE_string){
        ++it; // AAAABBBB
        string tempstr1 = "";
        tempstr1 += it->second.at(0); //tempstr1 = AAAA
        reg1 = Util::parseStringHex(tempstr1);
        string tempstr2 = "";
        tempstr2 += it->second.at(1); //tempstr2 = BBBB
        reg2 = Util::parseStringHex(tempstr2);

        gpr[reg1] = ~gpr[reg2];

        ++it; // CCCCDDDD
        ++it; // DDDDDDDD
      } else if (opcode == INSTR_AND_OPCODE_string){
        ++it; // AAAABBBB
        string tempstr1 = "";
        tempstr1 += it->second.at(1); //tempstr1 = BBBB
        reg1 = Util::parseStringHex(tempstr1);
        ++it; // CCCCDDDD
        string tempstr2 = "";
        tempstr2 += it->second.at(0); //tempstr2 = CCCC
        reg2 = Util::parseStringHex(tempstr2);

        gpr[reg1] = gpr[reg1] & gpr[reg2];

        ++it; // DDDDDDDD
      } else if (opcode == INSTR_OR_OPCODE_string){
        ++it; // AAAABBBB
        string tempstr1 = "";
        tempstr1 += it->second.at(1); //tempstr1 = BBBB
        reg1 = Util::parseStringHex(tempstr1);
        ++it; // CCCCDDDD
        string tempstr2 = "";
        tempstr2 += it->second.at(0); //tempstr2 = CCCC
        reg2 = Util::parseStringHex(tempstr2);

        gpr[reg1] = gpr[reg1] | gpr[reg2];

        ++it; // DDDDDDDD
      } else if (opcode == INSTR_XOR_OPCODE_string){
        ++it; // AAAABBBB
        string tempstr1 = "";
        tempstr1 += it->second.at(1); //tempstr1 = BBBB
        reg1 = Util::parseStringHex(tempstr1);
        ++it; // CCCCDDDD
        string tempstr2 = "";
        tempstr2 += it->second.at(0); //tempstr2 = CCCC
        reg2 = Util::parseStringHex(tempstr2);

        gpr[reg1] = gpr[reg1] ^ gpr[reg2];

        ++it; // DDDDDDDD
      } else if (opcode == INSTR_SHL_OPCODE_string){
        ++it; // AAAABBBB
        string tempstr1 = "";
        tempstr1 += it->second.at(1); //tempstr1 = BBBB
        reg1 = Util::parseStringHex(tempstr1);
        ++it; // CCCCDDDD
        string tempstr2 = "";
        tempstr2 += it->second.at(0); //tempstr2 = CCCC
        reg2 = Util::parseStringHex(tempstr2);

        gpr[reg1] = gpr[reg1] << gpr[reg2];

        ++it; // DDDDDDDD
      } else if (opcode == INSTR_SHR_OPCODE_string){
        ++it; // AAAABBBB
        string tempstr1 = "";
        tempstr1 += it->second.at(1); //tempstr1 = BBBB
        reg1 = Util::parseStringHex(tempstr1);
        ++it; // CCCCDDDD
        string tempstr2 = "";
        tempstr2 += it->second.at(0); //tempstr2 = CCCC
        reg2 = Util::parseStringHex(tempstr2);

        gpr[reg1] = gpr[reg1] >> gpr[reg2];

        ++it; // DDDDDDDD
      } else if (opcode == INSTR_LD_OPCODE_string){
        ++it; // AAAABBBB
        string temp = "";
        temp += it->second.at(0); //temp = AAAA
        reg1 = Util::parseStringHex(temp);
        temp = "";
        temp += it->second.at(1); //temp = BBBB
        reg2 = Util::parseStringHex(temp);
        ++it; // CCCCDDDD
        temp = "";
        temp += it->second.at(0); //temp = CCCC
        int flag = Util::parseStringHex(temp);
        temp = "";
        temp += it->second.at(1); //temp = DDDD
        ++it; // DDDDDDDD
        temp += it->second.at(0); //temp = DDDD
        temp += it->second.at(1); //temp = DDDD
        int index = Util::parseStringHex(temp);
        if (index == 0 && flag == 0){ // registar
          gpr[reg1] = gpr[reg2];
        } else {
          unsigned value;
          int j;
          if (flag % 2 == 0) { // literal
            j = 0;
            for (auto l : this->literals){
              if (j == index) {
                auto itt = literals.begin();
                advance(itt, j);

                value = *itt;
                break;
              }
              j++;
            }
          } else { // simbol
            j = 0;
            for (auto s : this->symbols){
              if (j == index) {
                auto itt = symbols.begin();
                advance(itt, j);

                value = *itt;
                break;
              }
              j++;
            }
          }
          if(flag < 2){ //neposredno
            gpr[reg1] = value;
          } else { //memorijsko 
            gpr[reg1] = getMemValue(value);
          }
        }
      } else if (opcode == INSTR_LD_INDIRECT_OPCODE_string){
        ++it; // AAAABBBB
        string temp = "";
        temp += it->second.at(0); //temp = AAAA
        reg1 = Util::parseStringHex(temp);
        temp = "";
        temp += it->second.at(1); //temp = BBBB
        reg2 = Util::parseStringHex(temp);
        ++it; // CCCCDDDD
        temp = "";
        temp += it->second.at(1); //temp = DDDD
        ++it; // DDDDDDDD
        temp += it->second.at(0); //temp = DDDD
        temp += it->second.at(1); //temp = DDDD
        int offset = Util::parseStringHex(temp);
        gpr[reg1] = getMemValue(gpr[reg2] + offset);
      } else if (opcode == INSTR_ST_OPCODE_string){
        ++it; // AAAABBBB
        string temp = "";
        temp += it->second.at(0); //temp = AAAA
        reg1 = Util::parseStringHex(temp);
        temp = "";
        temp += it->second.at(1); //temp = BBBB
        int flag = Util::parseStringHex(temp);
        ++it; // CCCCDDDD
        temp = "";
        temp += it->second.at(0); //temp = CCCC
        reg2 = Util::parseStringHex(temp);
        temp = "";
        temp += it->second.at(1); //temp = DDDD
        ++it; // DDDDDDDD
        temp += it->second.at(0); //temp = DDDD
        temp += it->second.at(1); //temp = DDDD
        int index = Util::parseStringHex(temp);
        if (index == 0 && flag == 0){ // registar
          gpr[reg1] = gpr[reg2];
        } else {
          unsigned value;
          int j;
          if (flag % 2 == 0) { // literal
            j = 0;
            for (auto l : this->literals){
              if (j == index) {
                auto itt = literals.begin();
                advance(itt, j);

                value = *itt;
                break;
              }
              j++;
            }
          } else { // simbol
            j = 0;
            for (auto s : this->symbols){
              if (j == index) {
                auto itt = symbols.begin();
                advance(itt, j);

                value = *itt;
                break;
              }
              j++;
            }
          }
          setMemValue(value, gpr[reg2]);
          // if(flag < 2){ //neposredno
          //   gpr[reg1] = value;
          // } else { //memorijsko 
          //   gpr[reg1] = getMemValue(value);
          // }
        }
      } else if (opcode == INSTR_ST_INDIRECT_OPCODE_string){
        ++it; // AAAABBBB
        string temp = "";
        temp += it->second.at(0); //temp = AAAA
        reg1 = Util::parseStringHex(temp);
        ++it; // CCCCDDDD
        temp = "";
        temp += it->second.at(0); //temp = CCCC
        reg2 = Util::parseStringHex(temp);
        temp = "";
        temp += it->second.at(1); //temp = DDDD
        ++it; // DDDDDDDD
        temp += it->second.at(0); //temp = DDDD
        temp += it->second.at(1); //temp = DDDD
        int offset = Util::parseStringHex(temp);
        setMemValue(gpr[reg1] + offset, gpr[reg2]);
      } else if (opcode == INSTR_CSRRD_OPCODE_string){
        ++it; // AAAABBBB
        string tempstr1 = "";
        tempstr1 += it->second.at(0); //tempstr1 = AAAA
        reg1 = Util::parseStringHex(tempstr1);
        string tempstr2 = "";
        tempstr2 += it->second.at(1); //tempstr2 = BBBB
        reg2 = Util::parseStringHex(tempstr2);

        gpr[reg1] = csr[reg2];

        ++it; // CCCCDDDD
        ++it; // DDDDDDDD
      } else if (opcode == INSTR_CSRWR_OPCODE_string){
        ++it; // AAAABBBB
        string tempstr1 = "";
        tempstr1 += it->second.at(0); //tempstr1 = AAAA
        reg1 = Util::parseStringHex(tempstr1);
        string tempstr2 = "";
        tempstr2 += it->second.at(1); //tempstr2 = BBBB
        reg2 = Util::parseStringHex(tempstr2);

        csr[reg1] = gpr[reg2];

        ++it; // CCCCDDDD
        ++it; // DDDDDDDD
      } else {
        cout << "Greska pri emulaciji!" << endl;
        print_end_emulation();
        return -1;
      }

      ++it;
      currPc += 4;
      if (currPc == pc + 4){
        pc = currPc;
      } else {
        currPc = pc;
      }
    }  
  }

  return 0;
}
// val = 01234567
// sp  = f0000000
//  f0000004: 67      f0000001: 01 
//  f0000003: 45  ->  f0000002: 23 
//  f0000002: 23      f0000003: 45 
//  f0000001: 01      f0000004: 67 
void Emulator::push(unsigned val) {
  string temp;
  string v = Util::decToHex(val);

  stringstream ss;
  ss << setw(8) << setfill('0') << v;
  v = ss.str();

  temp = "";
  temp += v.at(6);
  temp += v.at(7);
  memory[--sp] = temp; 
  temp = "";
  temp += v.at(4);
  temp += v.at(5);
  memory[--sp] = temp; 
  temp = "";
  temp += v.at(2);
  temp += v.at(3);
  memory[--sp] = temp; 
  temp = "";
  temp += v.at(0);
  temp += v.at(1);
  memory[--sp] = temp; 
}
unsigned Emulator::pop() {
  string temp = "";
  
  temp  = memory[sp++];
  temp += memory[sp++];
  temp += memory[sp++];
  temp += memory[sp++];

  return Util::parseStringHex(temp);
}

//------------------------------------------

void Emulator::print_memory(){
  map<unsigned, string>::iterator it = memory.begin();
 
  while (it != memory.end())
  {
    cout << Util::decToHex(it->first) << ": " 
    << it->second << endl;
    ++it;
  }

  cout << "SYMBOLS" << endl;
  int ii = 0;
  for (auto s : symbols)
  {
    cout << ' ' << ii++ << ". " << Util::decToHex(s) << endl; 
  }
  cout << "LITERALS" << endl;
  ii = 0;
  for (auto l : literals)
  {
    cout << ' ' << ii++ << ". " << Util::decToHex(l) << endl; 
  }

  cout << "Symtab start: " << Util::decToHex(symtabStart) << endl;
  cout << "Lpool start: " << Util::decToHex(lpoolStart) << endl;
  cout << "Last address: " << Util::decToHex(lastAddr) << endl;
}

void Emulator::print_end_emulation(){
  cout << "Emulated processor executed halt instruction" << endl
    << "Emulated processor state:" << endl;

  for (int i = 0; i < 16; i++)
  {
    if (i % 4 == 0) cout << (i < 10 ? " " : "");
    cout << "r" << i << "=0x";
    if(i < 10){
      stringstream ss;
      string temp = Util::decToHex(gpr[i]);
      ss << setw(8) << setfill('0') << temp;
      temp = ss.str();
      cout << temp.at(6) << temp.at(7)
        << temp.at(4) << temp.at(5)
        << temp.at(2) << temp.at(3)
        << temp.at(0) << temp.at(1);
    } else {
      cout << setw(8) << setfill('0') << Util::decToHex(gpr[i]);
    }
    if ((i + 1) % 4 == 0) {
      cout << endl;
    } else {
      cout << (i < 9 ? "    " : "   ");
    }
  }
  
}

unsigned Emulator::getMemValue(unsigned val){
  string temp = "";
  map<unsigned, string>::iterator it = memory.begin();
 
  while (it != memory.end())
  {
    if (it->first == val){
      // temp = it->second + temp; // 1. B
      temp += it->second;
      ++it;
      // temp = it->second + temp; // 2. B
      temp += it->second;
      ++it;
      // temp = it->second + temp; // 3. B
      temp += it->second;
      ++it; 
      // temp = it->second + temp; // 4. B
      temp += it->second;
      ++it; 
      return Util::parseStringHex(temp);
    }
    ++it;
  }
  return 0;
}
// addr = 40000000
// val  = 01234567
// 4000000: 67 45 23 01
void Emulator::setMemValue(unsigned addr, unsigned val){
  string temp;
  string v = Util::decToHex(val);

  stringstream ss;
  ss << setw(8) << setfill('0') << v;
  v = ss.str();

  temp = "";
  temp += v.at(6);
  temp += v.at(7);
  memory[addr++] = temp; 
  temp = "";
  temp += v.at(4);
  temp += v.at(5);
  memory[addr++] = temp; 
  temp = "";
  temp += v.at(2);
  temp += v.at(3);
  memory[addr++] = temp; 
  temp = "";
  temp += v.at(0);
  temp += v.at(1);
  memory[addr] = temp; 
}