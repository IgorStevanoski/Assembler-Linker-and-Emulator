#include "../inc/assembler.hpp"
#include "../inc/as_constants.hpp"
#include "../inc/util.hpp"
#include <iostream>
#include <iomanip>

// DODATI:
// relokacije
// assembler sam poziva parser

using namespace std;

int main(int argc, char* argv[]){

  string input_filename = "";
  string output_filename = "";
  string flag = "";

  if (argc == 2){ // samo ulazni fajl
    input_filename = argv[1];
  } else if (argc == 4){ 
    flag = argv[1];
    if (flag != "-o"){
      std::cout << "Error, invalid arguments1!" << endl;
      std::cout << flag << "0" << endl;
      return -1;
    }
    output_filename = argv[2];
    input_filename = argv[3];
  } else {
    std::cout << "Error, invalid arguments2!" << endl;
    return -1;
  } 
  
  // ako nema output_filename onda je isti kao input,
  // samo umesto .s ide .o
  if (output_filename == ""){ 
    for (int i = 0; i < input_filename.length() - 1; i++)
    {
      output_filename += input_filename.at(i);
    }
    output_filename += "o";
  }

  ifstream input_stream(input_filename);
  //ofstream output_stream(output_filename);
  ifstream parser_output("../misc/parser_output.txt");
  stringstream ss;
  string str;
  int ret = 0;

  string line;

  while (getline (input_stream, line)) {
    ss << line << endl;
  }
  //while (getline (parser_output, line)) {
  //  ss << line << endl;
  //}

  str = ss.str();

  Assembler assembler(str, output_filename);

  // prvi prolaz
  ret = assembler.firstPass();
  if (ret < 0) {
    Assembler::error_message(ret);
    return ret;
  }
  //assembler.print_table();

  // drugi prolaz
  ret = assembler.secondPass();
  if (ret < 0) {
    Assembler::error_message(ret);
    return ret;
  }
  assembler.print_table();
  assembler.print_sections();

  std::cout << "Asembliranje uspesno!" << endl;

  parser_output.close();
  input_stream.close();
  //output_stream.close();
}

//------------------------------------------

int Assembler::firstPass(){
  string curr_word;
  stringstream ss;
  int ret = 0;
  this->location_counter = 0;

  this->symtab = SymbolTable();

  // Obrada svih reci iz ulaznog fajla
  int i = 0;
  char c;
  while (i < str.length()){
    curr_word = Util::get_word(str, &i);

    if (curr_word == "Directive:"){
      ret = process_directive(&i);
    } else if (curr_word == "Instruction:"){
      ret = process_instruction(&i);
    } else if (curr_word == "Label:"){
      ret = process_label(&i);
    } else if (curr_word == "Done!!!"){
      ret = END_ASSEMBLING;
    } else {
      //cout << "Greska!" << endl;
    }
    
    if (ret < 0) return ret;
    else if (ret == END_ASSEMBLING) break;
  }

  //cout << "Prvi prolaz." << endl;
  return 0;
}

int Assembler::secondPass(){
  this->isFirstPass = false;
  string curr_word;
  stringstream ss;
  int ret = 0;
  this->location_counter = 0;

  // Obrada svih reci iz ulaznog fajla
  int i = 0;
  char c;
  while (i < str.length()){
    curr_word = Util::get_word(str, &i);

    if (curr_word == "Directive:"){
      ret = process_directive(&i);
    } else if (curr_word == "Instruction:"){
      ret = process_instruction(&i);
    } else if (curr_word == "Label:"){
      ret = process_label(&i);
    } else if (curr_word == "Done!!!"){
      ret = END_ASSEMBLING;
    } else {
      //cout << "Greska!" << endl;
    }
    
    if (ret < 0) return ret;
    else if (ret == END_ASSEMBLING) break;
  }
  //cout << "Drugi prolaz." << endl;
  return 0;
}

//------------------------------------------

int Assembler::process_directive(int *j){
  //cout << "Process Directive." << endl;

  string curr_word;
  
  curr_word = Util::get_word(str, j);

  if (curr_word == ".global"){
    return process_dir_global(j);
  } else if (curr_word == ".extern"){
    return process_dir_extern(j);
  } else if (curr_word == ".section"){
    return process_dir_section(j);
  } else if (curr_word == ".word"){
    return process_dir_word(j);
  } else if (curr_word == ".skip"){
    return process_dir_skip(j);
  } else if (curr_word == ".end"){
    return process_dir_end(j);
  } else {
    //cout << "Greska!" << endl;
    return DIR_ERROR;
  }

  return 0;
}

int Assembler::process_instruction(int *j){
  //cout << "Process Instruction." << endl;
  string curr_word;
  curr_word = Util::get_word(str, j);

  if (curr_word == "halt"){
    return process_instr_halt(j);
  } else if (curr_word == "int"){
    return process_instr_int(j);
  } else if (curr_word == "iret"){
    return process_instr_iret(j);
  } else if (curr_word == "call"){
    return process_instr_call(j);
  } else if (curr_word == "ret"){
    return process_instr_ret(j);
  } else if (curr_word == "jmp"){
    return process_instr_jmp(j);
  } else if (curr_word == "beq"){
    return process_instr_beq(j);
  } else if (curr_word == "bne"){
    return process_instr_bne(j);
  } else if (curr_word == "bgt"){
    return process_instr_bgt(j);
  } else if (curr_word == "push"){
    return process_instr_push(j);
  } else if (curr_word == "pop"){
    return process_instr_pop(j);
  } else if (curr_word == "xchg"){
    return process_instr_xchg(j);
  } else if (curr_word == "add"){
    return process_instr_add(j);
  } else if (curr_word == "sub"){
    return process_instr_sub(j);
  } else if (curr_word == "mul"){
    return process_instr_mul(j);
  } else if (curr_word == "div"){
    return process_instr_div(j);
  } else if (curr_word == "not"){
    return process_instr_not(j);
  } else if (curr_word == "and"){
    return process_instr_and(j);
  } else if (curr_word == "or"){
    return process_instr_or(j);
  } else if (curr_word == "xor"){
    return process_instr_xor(j);
  } else if (curr_word == "shl"){
    return process_instr_shl(j);
  } else if (curr_word == "shr"){
    return process_instr_shr(j);
  } else if (curr_word == "ld"){
    return process_instr_ld(j);
  } else if (curr_word == "st"){
    return process_instr_st(j);
  } else if (curr_word == "csrrd"){
    return process_instr_csrrd(j);
  } else if (curr_word == "csrwr"){
    return process_instr_csrwr(j);
  } else {
    return INSTR_ERROR;
  }

  return 0;
}

int Assembler::process_label(int *j){
  //cout << "Process Label." << endl;
  string curr_word;
  
  curr_word = Util::get_word(str, j);
  if (isFirstPass) {
    if (!symtab.checkSymbol(curr_word))
      this->symtab.addSymbol(ELF_Symbol(symtab.getCnt(), location_counter, 0, 
        ELF_Type::NOTYP, ELF_Bind::LOC, curr_section, curr_word));
    else {
      return LABEL_DEFINED_ERROR;
    }
  } 
  // u drugom prolazu ne radi nista

  return 0;
}

// DIREKTIVE--------------------------------
int Assembler::process_dir_global(int *j){
  //cout << "Process .global." << endl;
  string curr_word;
  int flag = 0;
  
  curr_word = Util::get_word(str, j);
  if (curr_word != "Symbols:") return DIR_GLOBAL_ERROR;
  
  while (flag == 0){
    curr_word = Util::get_word_with_endl(str, j, &flag);
    if (isFirstPass) continue; // asembler ne radi nista u prvom prolazu
    else { // drugi prolaz
      if (symtab.setSymbolGlobal(curr_word) < 0) {
        return DIR_GLOBAL_UNDEFINED_ERROR;
      }
    }
  }

  return 0;
}

int Assembler::process_dir_extern(int *j){
  //cout << "Process .extern." << endl;
  string curr_word;
  int flag = 0;
  
  curr_word = Util::get_word(str, j);
  if (curr_word != "Symbols:") return DIR_EXTERN_ERROR;
  
  while (flag == 0){
    curr_word = Util::get_word_with_endl(str, j, &flag);
    if (isFirstPass) continue; // asembler ne radi nista u prvom prolazu
    else { // drugi prolaz
      if (!symtab.checkSymbol(curr_word))
        this->symtab.addSymbol(ELF_Symbol(symtab.getCnt(), 0, 0, 
          ELF_Type::NOTYP, ELF_Bind::GLOB, UND, curr_word));
      else {
        return DIR_EXTERN_DEFINED_ERROR;
      }
    }
  }

  return 0;
}

int Assembler::process_dir_section(int *j){
  //cout << "Process .section." << endl;
  string curr_word;
  
  curr_word = Util::get_word(str, j);
  if (curr_word != "Section:") return DIR_SECTION_ERROR;

  curr_word = Util::get_word(str, j);
  if (isFirstPass) {
    int index = symtab.getCnt();

    this->symtab.addSymbol(ELF_Symbol(index, 0, 0, 
      ELF_Type::SCTN, ELF_Bind::LOC, index, curr_word));
    
    addSection(ELF_Section(curr_word));

    this->curr_section_name = curr_word;
    this->curr_section = index;
    this->location_counter = 0;
  } else { // drugi prolaz
    int index = symtab.getCurrSection(curr_word);
    if ( index < 0 ) return DIR_SKIP_ERROR;

    this->curr_section_name = curr_word;
    this->curr_section = index;
    this->location_counter = 0;
  }

  return 0;
}

int Assembler::process_dir_word(int *j){
  //cout << "Process .word." << endl;
  string curr_word;
  int flag = 0;
  
  curr_word = Util::get_word(str, j);
  if (curr_word != "Symbols/Literals:") return DIR_WORD_ERROR;
  
  while (flag == 0){
    curr_word = Util::get_word_with_endl(str, j, &flag);
    if (!isFirstPass) {
      if ( Util::isLiteral(curr_word)) { // Ako je literal
        curr_word = Util::decToHex(Util::parseString(curr_word)); 
      } else { // Ako je simbol
        int val = symtab.getSymbolValue(curr_word);
        addRelocationCurrentSection(
          ELF_Relocation(location_counter + 3, 
          false, 
          //symtab.getSymbolIndex(curr_word),
          curr_word,
          true)
        );
        if ( val < 0 ) return DIR_WORD_ERROR;
        curr_word = Util::decToHex(val);
      }
      curr_word = Util::fillHex(Util::hexLittleEndian(curr_word));
      updateCurrentSection(curr_word);
    }
    location_counter += 4;
  }

  return 0;
}

int Assembler::process_dir_skip(int *j){
  //cout << "Process .skip." << endl;
  string curr_word;
  
  curr_word = Util::get_word(str, j);
  if (curr_word != "Literal:") return DIR_SECTION_ERROR;

  curr_word = Util::get_word(str, j);
  int cnt = Util::parseString(curr_word);
  if (isFirstPass) { // u prvom prolazu povecava location_counter
    location_counter += cnt;
  } else { // u drugom prolazu povecava location_counter i alocira
    location_counter += cnt;
    for (int i = 0; i < cnt; i++) {
      updateCurrentSection("00");
    }
  }
   
  return 0;
}

int Assembler::process_dir_end(int *j){
  //cout << "Process .end." << endl;

  return END_ASSEMBLING;
}

//INSTRUKCIJE-------------------------------
int Assembler::process_instr_halt(int *j){
  //cout << "Process halt." << endl;
  if(!isFirstPass){
    updateCurrentSection(INSTR_HALT_OPCODE_string);
    updateCurrentSection("00 00 00");
  }
  location_counter += 4;
  return 0;
}
int Assembler::process_instr_int(int *j){
  //cout << "Process int." << endl;
  if(!isFirstPass){
    updateCurrentSection(INSTR_INT_OPCODE_string);
    updateCurrentSection("00 00 00");
  }
  location_counter += 4;
  return 0;
}
int Assembler::process_instr_iret(int *j){
  //cout << "Process iret." << endl;
  if(!isFirstPass){
    updateCurrentSection(INSTR_POP_OPCODE_string);
    // razlika u odnosu na obican pop je to sto je CCCC = 1
    // da bi mogao emulator da prepozna i izvrsi jos jedan pop
    // jer bi inace odmah zamenio vrednost pc i time skocio na 
    // drugu instrukciju
    updateCurrentSection(REG_PC_string + REG_SP_string + " 10 04");
    updateCurrentSection(INSTR_POP_CSR_OPCODE_string);
    updateCurrentSection("0" + REG_SP_string + " 00 04");
  }
  location_counter += 4;
  location_counter += 4; // iret cine 2 instrukcije
  return 0;
}
int Assembler::process_instr_ret(int *j){
  //cout << "Process ret." << endl;

  if(!isFirstPass){
    updateCurrentSection(INSTR_RET_OPCODE_string);
    updateCurrentSection(REG_PC_string + REG_SP_string + " 00 04");
  }
  location_counter += 4;
  return 0;
}
int Assembler::process_instr_call(int *j){
  string curr_word;
  //cout << "Process call." << endl;
    
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_CALL_ERROR;

  curr_word = Util::get_word(str, j);
  
  // samo se postavlja D da pokazuje na ulaz u 
  // tabelu simbola / bazen literala

  string regA; // AAAA = 0 ako literal / 1 ako symbol

  if(!isFirstPass){ 
    updateCurrentSection(INSTR_CALL_OPCODE_string);
    if (Util::isLiteral(curr_word)){ // literal
      curr_word = Util::decToHex(Util::parseString(curr_word));
      stringstream ss;
      ss << setw(8) << setfill('0') << curr_word;
      addRelocationCurrentSection(
        ELF_Relocation(location_counter + 3, 
        true, 
        //getLiteralPoolIndex(curr_word))
        ss.str())
      );
      curr_word = to_string(getLiteralPoolIndex(curr_word));
      regA = "0";
    } else { // simbol
      addRelocationCurrentSection(
        ELF_Relocation(location_counter + 3, 
        false, 
        //symtab.getSymbolIndex(curr_word))
        curr_word)
      );
      curr_word = to_string(symtab.getSymbolIndex(curr_word));
      regA = "1";
    }
    if (curr_word.length() == 1) {
      updateCurrentSection(regA + "0 00 0" + curr_word);
    } else if (curr_word.length() == 2) {
      updateCurrentSection(regA + "0 00 " + curr_word);
    } else {
      string temp = regA + "0 0";
      temp += curr_word.at(0);
      temp += " ";
      temp += curr_word.at(1);
      temp += curr_word.at(2);
      updateCurrentSection(temp);
    }
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_jmp(int *j){
  string curr_word; 
  string regA; // AAAA = 0 ako literal / 1 ako symbol
  //cout << "Process jmp." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_JMP_ERROR;
  curr_word = Util::get_word(str, j);

  if(!isFirstPass){
    updateCurrentSection(INSTR_JMP_OPCODE_string);
    if (Util::isLiteral(curr_word)){ // literal
      curr_word = Util::decToHex(Util::parseString(curr_word));
      stringstream ss;
      ss << setw(8) << setfill('0') << curr_word;
      addRelocationCurrentSection(
        ELF_Relocation(location_counter + 3, 
        true, 
        //getLiteralPoolIndex(curr_word))
        ss.str())
      );
      curr_word = to_string(getLiteralPoolIndex(curr_word));
      regA = "0";
    } else { // simbol
      addRelocationCurrentSection(
        ELF_Relocation(location_counter + 3, 
        false, 
        //symtab.getSymbolIndex(curr_word))
        curr_word)
      );
      curr_word = to_string(symtab.getSymbolIndex(curr_word));
      regA = "1";
    }
    if (curr_word.length() == 1) {
      updateCurrentSection(regA + "0 00 0" + curr_word);
    } else if (curr_word.length() == 2) {
      updateCurrentSection(regA + "0 00 " + curr_word);
    } else {
      string temp = regA + "0 0";
      temp += curr_word.at(0);
      temp += " ";
      temp += curr_word.at(1);
      temp += curr_word.at(2);
      updateCurrentSection(temp);
    }
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_beq(int *j){
  string curr_word;
  string reg1, reg2;
  string regA;
  //cout << "Process beq." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_BEQ_ERROR;
  reg1 = Util::get_operand_gpr(str, j);
  reg2 = Util::get_operand_gpr(str, j);
  curr_word = Util::get_word(str, j);

  if(!isFirstPass){
    updateCurrentSection(INSTR_BEQ_OPCODE_string);
    if (Util::isLiteral(curr_word)){ // literal
      curr_word = Util::decToHex(Util::parseString(curr_word));
      stringstream ss;
      ss << setw(8) << setfill('0') << curr_word;
      addRelocationCurrentSection(
        ELF_Relocation(location_counter + 3, 
        true, 
        //getLiteralPoolIndex(curr_word))
        ss.str())
      );
      curr_word = to_string(getLiteralPoolIndex(curr_word));
      regA = "0";
    } else { // simbol
      addRelocationCurrentSection(
        ELF_Relocation(location_counter + 3, 
        false, 
        //symtab.getSymbolIndex(curr_word))
        curr_word)
      );
      curr_word = to_string(symtab.getSymbolIndex(curr_word));
      regA = "1";
    }
    string temp = "";
    int len = curr_word.length();
    switch (len){
    case 1: temp += "0 0"; temp += curr_word; break;
    case 2: temp += "0 ";  temp += curr_word; break;
    default: temp += curr_word.at(0);  
      temp += " ";  
      temp += curr_word.at(1);  
      temp += curr_word.at(2);  
      break;
    }
    
    updateCurrentSection(regA + reg1 + " " + reg2 + temp);
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_bne(int *j){
  string curr_word;
  string reg1, reg2, operand;
  string regA;
  //cout << "Process bne." << endl;
    
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_BNE_ERROR;
  reg1 = Util::get_operand_gpr(str, j);
  reg2 = Util::get_operand_gpr(str, j);
  curr_word = Util::get_word(str, j);
  if(!isFirstPass){
    updateCurrentSection(INSTR_BNE_OPCODE_string);
    if (Util::isLiteral(curr_word)){ // literal
      curr_word = Util::decToHex(Util::parseString(curr_word));
      stringstream ss;
      ss << setw(8) << setfill('0') << curr_word;
      addRelocationCurrentSection(
        ELF_Relocation(location_counter + 3, 
        true, 
        //getLiteralPoolIndex(curr_word))
        ss.str())
      );
      curr_word = to_string(getLiteralPoolIndex(curr_word));
      regA = "0";
    } else { // simbol
      addRelocationCurrentSection(
        ELF_Relocation(location_counter + 3, 
        false, 
        //symtab.getSymbolIndex(curr_word))
        curr_word)
      );
      curr_word = to_string(symtab.getSymbolIndex(curr_word));
      regA = "1";
    }
    string temp = "";
    int len = curr_word.length();
    switch (len){
    case 1: temp += "0 0"; temp += curr_word; break;
    case 2: temp += "0 ";  temp += curr_word; break;
    default: temp += curr_word.at(0);  
      temp += " ";  
      temp += curr_word.at(1);  
      temp += curr_word.at(2);  
      break;
    }
    
    updateCurrentSection(regA + reg1 + " " + reg2 + temp);
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_bgt(int *j){
  string curr_word;
  string reg1, reg2, operand;
  string regA;
  //cout << "Process bgt." << endl;
    
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_BGT_ERROR;
  reg1 = Util::get_operand_gpr(str, j);
  reg2 = Util::get_operand_gpr(str, j);
  curr_word = Util::get_word(str, j);

  if(!isFirstPass){
    updateCurrentSection(INSTR_BNE_OPCODE_string);
    if (Util::isLiteral(curr_word)){ // literal
      curr_word = Util::decToHex(Util::parseString(curr_word));
      stringstream ss;
      ss << setw(8) << setfill('0') << curr_word;
      addRelocationCurrentSection(
        ELF_Relocation(location_counter + 3, 
        true, 
        //getLiteralPoolIndex(curr_word))
        ss.str())
      );
      curr_word = to_string(getLiteralPoolIndex(curr_word));
      regA = "0";
    } else { // simbol
      addRelocationCurrentSection(
        ELF_Relocation(location_counter + 3, 
        false, 
        //symtab.getSymbolIndex(curr_word))
        curr_word)
      );
      curr_word = to_string(symtab.getSymbolIndex(curr_word));
      regA = "1";
    }
    string temp = "";
    int len = curr_word.length();
    switch (len){
    case 1: temp += "0 0"; temp += curr_word; break;
    case 2: temp += "0 ";  temp += curr_word; break;
    default: temp += curr_word.at(0);  
      temp += " ";  
      temp += curr_word.at(1);  
      temp += curr_word.at(2);  
      break;
    }
    
    updateCurrentSection(regA + reg1 + " " + reg2 + temp);
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_push(int *j){
  string curr_word, reg1 = "x";
  //cout << "Process push." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_PUSH_ERROR;
  reg1 = Util::get_operand_gpr(str, j);

  if(!isFirstPass){
    updateCurrentSection(INSTR_PUSH_OPCODE_string);

    //reg1 = gpr
    //reg2 = sp
    curr_word = "";
    curr_word += REG_SP_string; // AAAA = sp
    curr_word += "0 ";          // BBBB = 0
    curr_word += reg1;          // CCCC = reg1
    curr_word += "F FC";        // DDDD DDDD DDDD = -4

    updateCurrentSection(curr_word);
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_pop(int *j){
  string curr_word, reg1 = "x";
  //cout << "Process pop." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_POP_ERROR;
  reg1 = Util::get_operand_gpr(str, j);
  if ( reg1 == "x" ) return INSTR_POP_ERROR;

  if(!isFirstPass){
    updateCurrentSection(INSTR_POP_OPCODE_string);

    //reg1 = gpr
    //reg2 = sp
    curr_word = "";
    curr_word += reg1;          // AAAA = reg1
    curr_word += REG_SP_string; // BBBB = sp
    curr_word += " 0";          // CCCC = 0
    curr_word += "0 04";        // DDDD DDDD DDDD = 4

    updateCurrentSection(curr_word);
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_xchg(int *j){
  string curr_word, reg1 = "x", reg2 = "x";
  //cout << "Process xchg." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_XCHG_ERROR;
  reg1 = Util::get_operand_gpr(str, j);
  reg2 = Util::get_operand_gpr(str, j);
  if ( reg1 == "x" || reg2 == "x" ) return INSTR_XCHG_ERROR;

  if(!isFirstPass){
    updateCurrentSection(INSTR_XCHG_OPCODE_string);

    //reg1 = gprS
    //reg2 = gprD
    curr_word = "";
    curr_word += "0";           // AAAA = 0
    curr_word += reg2;          // BBBB = reg2
    curr_word += " " + reg1;    // CCCC = reg1
    curr_word += "0 00";        // DDDD DDDD DDDD = 0

    updateCurrentSection(curr_word);
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_add(int *j){
  string curr_word, reg1 = "x", reg2 = "x";
  //cout << "Process add." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_ADD_ERROR;
  reg1 = Util::get_operand_gpr(str, j);
  reg2 = Util::get_operand_gpr(str, j);
  if ( reg1 == "x" || reg2 == "x" ) return INSTR_ADD_ERROR;
  
  if(!isFirstPass){
    updateCurrentSection(INSTR_ADD_OPCODE_string);

    //reg1 = Util::get_operand_hex(str, j);
    //reg2 = Util::get_operand_hex(str, j);
    curr_word = "";
    curr_word += "0";           // AAAA = 0
    curr_word += reg2;          // BBBB = reg2
    curr_word += " " + reg1;    // CCCC = reg1
    curr_word += "0 00";        // DDDD DDDD DDDD = 0

    updateCurrentSection(curr_word);
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_sub(int *j){
  string curr_word, reg1 = "x", reg2 = "x";
  //cout << "Process sub." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_SUB_ERROR;
  reg1 = Util::get_operand_gpr(str, j);
  reg2 = Util::get_operand_gpr(str, j);
  if ( reg1 == "x" || reg2 == "x" ) return INSTR_SUB_ERROR;

  if(!isFirstPass){
    updateCurrentSection(INSTR_SUB_OPCODE_string);

    //reg1 = Util::get_operand_hex(str, j);
    //reg2 = Util::get_operand_hex(str, j);
    curr_word = "";
    curr_word += "0";           // AAAA = 0
    curr_word += reg2;          // BBBB = reg2
    curr_word += " " + reg1;    // CCCC = reg1
    curr_word += "0 00";        // DDDD DDDD DDDD = 0

    updateCurrentSection(curr_word);
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_mul(int *j){
  string curr_word, reg1 = "x", reg2 = "x";
  //cout << "Process mul." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_MUL_ERROR;
  reg1 = Util::get_operand_gpr(str, j);
  reg2 = Util::get_operand_gpr(str, j);
  if ( reg1 == "x" || reg2 == "x" ) return INSTR_MUL_ERROR;

  if(!isFirstPass){
    updateCurrentSection(INSTR_MUL_OPCODE_string);

    //reg1 = Util::get_operand_hex(str, j);
    //reg2 = Util::get_operand_hex(str, j);
    curr_word = "";
    curr_word += "0";           // AAAA = 0
    curr_word += reg2;          // BBBB = reg2
    curr_word += " " + reg1;    // CCCC = reg1
    curr_word += "0 00";        // DDDD DDDD DDDD = 0

    updateCurrentSection(curr_word);
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_div(int *j){
  string curr_word, reg1 = "x", reg2 = "x";
  //cout << "Process div." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_DIV_ERROR;
  reg1 = Util::get_operand_gpr(str, j);
  reg2 = Util::get_operand_gpr(str, j);
  if ( reg1 == "x" || reg2 == "x" ) return INSTR_MUL_ERROR;

  if(!isFirstPass){
    updateCurrentSection(INSTR_DIV_OPCODE_string);

    //reg1 = Util::get_operand_hex(str, j);
    //reg2 = Util::get_operand_hex(str, j);
    curr_word = "";
    curr_word += "0";           // AAAA = 0
    curr_word += reg2;          // BBBB = reg2
    curr_word += " " + reg1;    // CCCC = reg1
    curr_word += "0 00";        // DDDD DDDD DDDD = 0

    updateCurrentSection(curr_word);
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_not(int *j){
  string curr_word, reg1 = "x";
  //cout << "Process not." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_NOT_ERROR;
  reg1 = Util::get_operand_gpr(str, j);
  if ( reg1 == "x" ) return INSTR_NOT_ERROR;

  if(!isFirstPass){
    updateCurrentSection(INSTR_NOT_OPCODE_string);

    //reg1 = Util::get_operand_hex(str, j);
    curr_word = "";
    curr_word += reg1;          // AAAA = reg1
    curr_word += reg1;          // BBBB = reg1
    curr_word += " 0";          // CCCC = 0
    curr_word += "0 00";        // DDDD DDDD DDDD = 0

    updateCurrentSection(curr_word);
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_and(int *j){
  string curr_word, reg1 = "x", reg2 = "x";
  //cout << "Process and." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_AND_ERROR;
  reg1 = Util::get_operand_gpr(str, j);
  reg2 = Util::get_operand_gpr(str, j);
  if ( reg1 == "x" || reg2 == "x" ) return INSTR_AND_ERROR;
  
  if(!isFirstPass){
    updateCurrentSection(INSTR_AND_OPCODE_string);

    //reg1 = Util::get_operand_hex(str, j);
    //reg2 = Util::get_operand_hex(str, j);
    curr_word = "";
    curr_word += reg2;           // AAAA = 0
    curr_word += reg2;          // BBBB = reg2
    curr_word += " " + reg1;    // CCCC = reg1
    curr_word += "0 00";        // DDDD DDDD DDDD = 0

    updateCurrentSection(curr_word);
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_or(int *j){
  string curr_word, reg1 = "x", reg2 = "x";
  //cout << "Process or." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_OR_ERROR;
  reg1 = Util::get_operand_gpr(str, j);
  reg2 = Util::get_operand_gpr(str, j);
  if ( reg1 == "x" || reg2 == "x" ) return INSTR_OR_ERROR;

  if(!isFirstPass){
    updateCurrentSection(INSTR_OR_OPCODE_string);

    //reg1 = Util::get_operand_hex(str, j);
    //reg2 = Util::get_operand_hex(str, j);
    curr_word = "";
    curr_word += reg2;          // AAAA = 0
    curr_word += reg2;          // BBBB = reg2
    curr_word += " " + reg1;    // CCCC = reg1
    curr_word += "0 00";        // DDDD DDDD DDDD = 0

    updateCurrentSection(curr_word);
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_xor(int *j){
  string curr_word, reg1 = "x", reg2 = "x";
  //cout << "Process xor." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_XOR_ERROR;
  reg1 = Util::get_operand_gpr(str, j);
  reg2 = Util::get_operand_gpr(str, j);
  if ( reg1 == "x" || reg2 == "x" ) return INSTR_XOR_ERROR;

  if(!isFirstPass){
    updateCurrentSection(INSTR_XOR_OPCODE_string);

    //reg1 = Util::get_operand_hex(str, j);
    //reg2 = Util::get_operand_hex(str, j);
    curr_word = "";
    curr_word += reg2;          // AAAA = 0
    curr_word += reg2;          // BBBB = reg2
    curr_word += " " + reg1;    // CCCC = reg1
    curr_word += "0 00";        // DDDD DDDD DDDD = 0

    updateCurrentSection(curr_word);
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_shl(int *j){
  string curr_word, reg1 = "x", reg2 = "x";
  //cout << "Process shl." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_SHL_ERROR;
  reg1 = Util::get_operand_gpr(str, j);
  reg2 = Util::get_operand_gpr(str, j);
  if ( reg1 == "x" || reg2 == "x" ) return INSTR_SHL_ERROR;

  if(!isFirstPass){
    updateCurrentSection(INSTR_SHL_OPCODE_string);

    //reg1 = Util::get_operand_hex(str, j);
    //reg2 = Util::get_operand_hex(str, j);
    curr_word = "";
    curr_word += reg2;          // AAAA = 0
    curr_word += reg2;          // BBBB = reg2
    curr_word += " " + reg1;    // CCCC = reg1
    curr_word += "0 00";        // DDDD DDDD DDDD = 0

    updateCurrentSection(curr_word);
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_shr(int *j){
  string curr_word, reg1 = "x", reg2 = "x";
  //cout << "Process shr." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_SHR_ERROR;
  reg1 = Util::get_operand_gpr(str, j);
  reg2 = Util::get_operand_gpr(str, j);
  if ( reg1 == "x" || reg2 == "x" ) return INSTR_SHR_ERROR;
  
  if(!isFirstPass){
    updateCurrentSection(INSTR_SHR_OPCODE_string);

    //reg1 = Util::get_operand_hex(str, j);
    //reg2 = Util::get_operand_hex(str, j);
    curr_word = "";
    curr_word += reg2;          // AAAA = 0
    curr_word += reg2;          // BBBB = reg2
    curr_word += " " + reg1;    // CCCC = reg1
    curr_word += "0 00";        // DDDD DDDD DDDD = 0

    updateCurrentSection(curr_word);
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_ld(int *j){
string curr_word, reg1 = "x", reg2 = "x", offset = "x";
  //cout << "Process ld." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_LD_ERROR;

  curr_word = Util::get_word(str, j);
  if (curr_word == "["){ // indirektno adresiranje
    reg2 = Util::get_operand_gpr(str, j);
    curr_word = Util::get_word(str, j); // curr_word = "+" ili "]"
    offset = "0 00";
    if (curr_word == "+"){
      offset = Util::get_word(str, j);
      offset = getIndirectOperandValue(offset);
      if (offset == "error" && !isFirstPass){
        return INSTR_LD_ERROR;
      }
      curr_word = Util::get_word(str, j); // curr_word = "]"
    }
  }
  reg1 = Util::get_operand_gpr(str, j);

  if(!isFirstPass){
    if (offset != "x"){ // A = reg1, B = reg2, C = 0, D = offset
      updateCurrentSection(INSTR_LD_INDIRECT_OPCODE_string);
      updateCurrentSection(reg1 + reg2);
      if (offset.length() == 5){
        string temp = "0"; // ovo sam morao ovako jer nije radilo u jednoj liniji
        temp += offset.at(4);
        temp += " ";
        temp += offset.at(0);
        temp += offset.at(1);
        updateCurrentSection(temp);
      } else if (offset.length() == 2){
        updateCurrentSection("00 " + offset);
      } else if (offset.length() == 4){
        updateCurrentSection("0" + offset);
      } else {
        return INSTR_LD_ERROR;
      } 
    } else {
      if (curr_word.at(0) == '%'){ // operand je registar
        int ii = 0;
        reg2 = Util::get_operand_gpr(curr_word + " ", &ii);
        updateCurrentSection(INSTR_LD_OPCODE_string);
        updateCurrentSection(reg1 + reg2 + " 00 00");
      } else { 
        // DDDD DDDD DDDD je index u tabeli simbola ili bazenu literala
        // reg2 = CCCC, koje se ne koristi
        // 0. reg2 = 00 -> neposredno, literal 
        // 1. reg2 = 01 -> neposredno, simbol 
        // 2. reg2 = 10 -> memorijsko, literal 
        // 3. reg2 = 11 -> memorijsko, simbol 
        string operand = "";
        if (curr_word.at(0) == '$'){ // operand je neposredna vr
          reg2 = "0"; //neposredno
          for (int i = 1; i < curr_word.length(); i++)
          {
            operand += curr_word.at(i);
          }
        } else {
          reg2 = "1"; // memorijsko
          operand = curr_word;
        }
        updateCurrentSection(INSTR_LD_OPCODE_string);
        if (Util::isLiteral(operand)){ // literal
          if (reg2 == "0") reg2 = "0";
          else reg2 = "2";  
          operand = Util::decToHex(Util::parseString(operand));
          offset = to_string(getLiteralPoolIndex(operand));
          stringstream ss;
          ss << setw(8) << setfill('0') << operand;
          addRelocationCurrentSection(
            ELF_Relocation(location_counter + 3, 
            true, 
            //getLiteralPoolIndex(operand))
            ss.str())
          );
        } else { // simbol
          if (reg2 == "0") reg2 = "1"; // ako je CCCC = 1 onda je simbol
          else reg2 = "3";
          offset = to_string(symtab.getSymbolIndex(operand));
          addRelocationCurrentSection(
            ELF_Relocation(location_counter + 3, 
            false, 
            //symtab.getSymbolIndex(operand))
            curr_word)
          );
        }
        if (offset.length() == 1){
          updateCurrentSection(reg1 + "0 " + reg2 + "0 0" + offset);
        } else if (offset.length() == 2){
          updateCurrentSection(reg1 + "0 " + reg2 + "0 " + offset);
        } else {
          updateCurrentSection(reg1 + "0 " + reg2 + offset.at(0) + 
          " " + offset.at(1) + offset.at(2));
        }
      } 
      //else updateCurrentSection("bb 00 00 00");
    }
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_st(int *j){
  string curr_word, reg1 = "x", reg2 = "x", offset = "x";
  //cout << "Process st." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_ST_ERROR;
  reg1 = Util::get_operand_gpr(str, j);

  curr_word = Util::get_word(str, j);
  if (curr_word == "["){ // indirektno adresiranje
    reg2 = Util::get_operand_gpr(str, j);
    curr_word = Util::get_word(str, j); // curr_word = "+" ili "]"
    offset = "0 00";
    if (curr_word == "+"){
      offset = Util::get_word(str, j);
      offset = getIndirectOperandValue(offset);
      if (offset == "error" && !isFirstPass){
        return INSTR_ST_ERROR;
      }
      curr_word = Util::get_word(str, j); // curr_word = "]"
    }
  }

  if(!isFirstPass){
    if (offset != "x"){
      updateCurrentSection(INSTR_ST_INDIRECT_OPCODE_string);
      updateCurrentSection(reg2 + "0");
      if (offset.length() == 5){
        updateCurrentSection(reg1 + offset.at(4) + " " + 
          offset.at(0) + offset.at(1));
      } else if (offset.length() == 2){
        updateCurrentSection(reg1 + "0 " + offset);
      } else if (offset.length() == 4){
        updateCurrentSection(reg1 + offset);
      } else {
        return INSTR_ST_ERROR;
      } 

    } else {
      if (curr_word.at(0) == '%'){ // operand je registar
        int ii = 0;
        reg2 = Util::get_operand_gpr(curr_word + " ", &ii);
        updateCurrentSection(INSTR_ST_OPCODE_string);
        updateCurrentSection(reg2 + "0");
        updateCurrentSection(reg1 + "0 00");
      } else { 
        // reg2 = BBBB, koje se ne koristi
        // reg2 = 00 -> neposredno, literal 
        // reg2 = 01 -> neposredno, simbol 
        // reg2 = 10 -> memorijsko, literal 
        // reg2 = 11 -> memorijsko, simbol 
        string operand = "";
        if (curr_word.at(0) == '$'){ // operand je neposerdna vr
          reg2 = "0"; //neposredno
          for (int i = 1; i < curr_word.length(); i++)
          {
            operand += curr_word.at(i);
          }
        } else {
          reg2 = "1"; // memorijsko
          operand = curr_word;
        }
        updateCurrentSection(INSTR_ST_OPCODE_string);
        if (Util::isLiteral(operand)){ // literal
          if (reg2 == "0") reg2 = "0";
          else reg2 = "2";  
          operand = Util::decToHex(Util::parseString(operand));
          offset = to_string(getLiteralPoolIndex(operand));
          stringstream ss;
          ss << setw(8) << setfill('0') << operand;
          addRelocationCurrentSection(
            ELF_Relocation(location_counter + 3, 
            true, 
            //getLiteralPoolIndex(operand))
            ss.str())
          );
        } else { // simbol
          if (reg2 == "0") reg2 = "1"; // ako je BBBB = 1 onda je simbol
          else reg2 = "3";
          offset = to_string(symtab.getSymbolIndex(operand));
          addRelocationCurrentSection(
            ELF_Relocation(location_counter + 3, 
            false, 
            //symtab.getSymbolIndex(operand))
            curr_word)
          );
        }
        if (offset.length() == 1){
          updateCurrentSection("0" + reg2 + " " + reg1 + "0 0" + offset);
        } else if (offset.length() == 2){
          updateCurrentSection("0" + reg2 + " " + reg1 + "0 " + offset);
        } else {
          updateCurrentSection("0" + reg2 + " " + reg1 + offset.at(0) + 
          " " + offset.at(1) + offset.at(2));
        } 
      } 
      //else updateCurrentSection("bb 00 00 00");
    }
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_csrrd(int *j){
  string curr_word, reg = "x", csr = "x";
  //cout << "Process csrrd." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_CSRRD_ERROR;
  csr = Util::get_operand_csr(str, j);
  reg = Util::get_operand_gpr(str, j);
  if ( csr == "x" || reg == "x" ) return INSTR_CSRRD_ERROR;

  if(!isFirstPass){
    updateCurrentSection(INSTR_CSRRD_OPCODE_string);

    curr_word = "";
    curr_word += reg;           // AAAA = reg
    curr_word += csr;           // BBBB = csr
    curr_word += " 0";          // CCCC = 0
    curr_word += "0 00";        // DDDD DDDD DDDD = 0

    updateCurrentSection(curr_word);
  }

  location_counter += 4;
  return 0;
}
int Assembler::process_instr_csrwr(int *j){
  string curr_word, reg = "x", csr = "x";
  //cout << "Process csrwr." << endl;
      
  curr_word = Util::get_word(str, j);
  if (curr_word != "Operand:") return INSTR_CSRWR_ERROR;
  reg = Util::get_operand_gpr(str, j);
  csr = Util::get_operand_csr(str, j);
  if ( csr == "x" || reg == "x" ) return INSTR_CSRWR_ERROR;

  if(!isFirstPass){
    updateCurrentSection(INSTR_CSRWR_OPCODE_string);
    curr_word = "";
    curr_word += csr;           // AAAA = csr
    curr_word += reg;           // BBBB = reg
    curr_word += " 0";          // CCCC = 0
    curr_word += "0 00";        // DDDD DDDD DDDD = 0

    updateCurrentSection(curr_word);
  }

  location_counter += 4;
  return 0;
}

//------------------------------------------

// vraca false ako ne postoji simbol u tabeli simbola sa imenom symbol
bool SymbolTable::checkSymbol(string symbol_name){
  for (auto s : this->symbols){
    if (s.getName() == symbol_name)
      return true;
  }
  return false;
}

int SymbolTable::getSymbolValue(string symbol_name){
  for (auto s : this->symbols){
    if (s.getName() == symbol_name)
      return s.getValue();
  }
  return -1;
}

int SymbolTable::getSymbolIndex(string symbol_name){
  for (auto s : this->symbols){
    if (s.getName() == symbol_name)
      return s.getNum();
  }
  return -1;
}

void SymbolTable::print_table(Assembler* as){
  // umesto cout -> ofstream
  as->of << "#.symtab" << endl;
  as->of << "Num Value    Size Type  Bind Ndx Name" << endl;

  for (auto s : this->symbols) {
    s.print_symbol(as);
  }
}

int SymbolTable::setSymbolGlobal(string symbol_name){
  int j = 0;
  for (auto s : this->symbols){
    if (s.getName() == symbol_name) {
      auto it = symbols.begin();
      
      advance(it, j);

      it->setSymbolGlobal();

      return 0;
    }
    j++;
  }

  return -1;
}

int SymbolTable::setSymbolUND(string symbol_name){
  int j = 0;
  for (auto s : this->symbols){
    if (s.getName() == symbol_name) {
      auto it = symbols.begin();
      
      advance(it, j);

      it->setSymbolUND();

      return 0;
    }
    j++;
  }

  return -1;
}

int SymbolTable::getCurrSection(string symbol_name){
  for (auto s : this->symbols) {
    if (s.getName() == symbol_name) {
      return s.getNum();
    }
  }
  return -1;
}

void Literal_Pool::print_pool(Assembler* as){
  as->of << ".literal_pool" << endl;
    for (auto l : this->literals) {     
    as->of << setw(2) << setfill(' ') << l.getIndex() << ". "
      << setw(8) << setfill('0') << Util::decToHex(l.getValue())
      << endl;
    }
  
}

void ELF_Symbol::print_symbol(Assembler* as){
    as->of << setw(2) << setfill(' ') << num << ". "
      << setw(8) << setfill('0') << Util::decToHex(value) << " "
      << setw(4) << setfill(' ') << size << " "
      << setw(5) << setfill(' ') << ((type == ELF_Type::NOTYP) ? "NOTYP" : "SCTN") << " "
      << setw(4) << setfill(' ') << ((bind == ELF_Bind::LOC) ? "LOC" : "GLOB") << " ";
    
    if (section == UND) {
      as->of << setw(3) << setfill(' ') << "UND ";
    } else 
      as->of << setw(3) << setfill(' ') << section << " ";
      
    as->of << name << endl;
}

void ELF_Section::print_section(Assembler* as){
  as->of << "#" << this->name << endl;
  int n = 0;
  for (int i = 0, j = 0; i < this->data.length(); i++, j++)
  {
    as->of << data.at(i);
    if ( j % 10 == 0 && j > 0){
      i++;
      j-=11;
      if ( ++n == 2 ) { as->of << endl; n = 0; }
      else as->of << "  ";
    }
  }
  as->of << endl;
  as->of << "#" + name;
  lpool.print_pool(as);

  as->of << "#" + name + ".relocations" << endl;
  int i = 0;
  for (auto r : this->relocations)
  {
    as->of << setw(2) << setfill(' ') << i << ". "
      << setw(4) << setfill('0') << r.getPosition()
      << (r.getIsLiteral() == true ? " LIT " : " SYM ")
      << r.getName()
      << " " << (r.getWord_dir() == true ? "1" : "0")
      << endl;
    i++;  
  }
  
}

void Assembler::print_sections(){
    for (auto s : this->sections) {     
      s.print_section(this);
    }
}

// -----------------------------------------

// dodaje sadrzaj na kraj sekcije
int Assembler::updateCurrentSection(string str){
  int j = 0;
  for (auto s : this->sections){
    if (s.getName() == curr_section_name) {
      auto it = sections.begin();
      
      advance(it, j);

      it->addData(str);

      return 0;
    }
    j++;
  }

  return -1;
}

// dodaje relokaciju na kraj sekcije
int Assembler::addRelocationCurrentSection(ELF_Relocation rel){
  int j = 0;
  for (auto s : this->sections){
    if (s.getName() == curr_section_name) {
      auto it = sections.begin();
      
      advance(it, j);

      it->addRelocation(rel);

      return 0;
    }
    j++;
  }

  return -1;
}

// vraca index literala u bazenu literala
// str je literal
int Assembler::getLiteralPoolIndex(string str){
  int j = 0;
  int ind;
  for (auto s : this->sections){
    if (s.getName() == curr_section_name) {
      auto it = sections.begin();
      
      advance(it, j);

      ind = it->getLPIndex(str);

      return ind;
    }
    j++;
  }

  return -1;
}

int ELF_Section::getLPIndex(string str){
  int j = 0;
  int ind;
  if (str.length() > 2){    // moze da se desi da primi hex vr bez 0x
    if ( str.at(1) != 'x'){ // sto pravi problem kod parseString
      str = "0x" + str; 
    }
  }
  int val = Util::parseString(str);

  for (auto s : this->lpool.literals){
    if (s.getValue() == val) {
      return s.getIndex();
    }
    j++;
  }
  
  return lpool.addLiteral(val);
}

string Assembler::getIndirectOperandValue(string str){
  string val = "";
  int dec_val = 0;
  const int limit =  pow(2,12); 

  if (Util::isLiteral(str)){ // literal
    dec_val = Util::parseString(str); // za proveru decimalne vr od val
    val = Util::decToHex(dec_val);

    if (dec_val >= limit){
      return "error";
    } else { // vraca se neposredna vrednost
      return Util::hexLittleEndian(val);
    }
  } else { // simbol
    dec_val = symtab.getSymbolValue(str);
    if (dec_val < 0) return "error";
    if (dec_val < limit){
      val = Util::decToHex(dec_val);
      return Util::hexLittleEndian(val);
    }
  }

  return "error"; 
}

// vraca "error" ako je doslo do greske
string Assembler::getOperandValue(string str){
  string val = "";
  int dec_val = 0;
  const int limit =  pow(2,12); 

  if (str.at(0) == '$'){ // neposredna vrednost
    for (int i = 1; i < str.length(); i++) //uklanjanje $
    {
      val += str.at(i);
    }
    if (Util::isLiteral(val)){ // $literal
      dec_val = Util::parseString(val); // za proveru decimalne vr od val
      val = Util::decToHex(dec_val);

      if (dec_val >= limit){
        return "error";
      } else { // vraca se neposredna vrednost
        return Util::hexLittleEndian(val);
      }
    } else { // $simbol
      if (dec_val = symtab.getSymbolValue(val) >= limit){
        return "error";
      }
      return Util::hexLittleEndian(val);
    }
  } else { // vrednost iz memorije
    if (Util::isLiteral(val)){
      
    }
  }

  return val;
}


void Assembler::error_message(int ret){
  Error_Message::error_message(ret);
}