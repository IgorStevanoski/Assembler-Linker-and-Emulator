#include "../inc/linker.hpp"
#include "../inc/assembler.hpp"
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// DODATI:
// pocetna adresa sekcije iz argumenata cmd
// preklapanje sekcija

int main(int argc, char* argv[]){

  string obj_file_names [argc];
  //string obj_file_names [6];
  int obj_file_cnt = 0;
  SymbolTable symtab; // SymbolTable symtab() [argc];
  Linker linker;
  string section_name;
  string section_start_addr;
  string output_filename = "";
  bool hexOption = false;

  for (int i = 0; i < argc; i++) // uzimaju se .o fajlovi
  {
    string arg = argv[i];
    if (arg.find("-place=") != std::string::npos
      && arg.find("@") != std::string::npos) {
      
      section_name = "";
      section_start_addr = "";
      int j;
      for (j = 7; arg.at(j) != '@'; j++)
      {
        section_name += arg.at(j);
      }
      
      j++; // preskace '@'

      for (; j < arg.length(); j++)
      {
        section_start_addr += arg.at(j);
      }

      linker.section_map[section_name] = Util::parseString(section_start_addr);
      //cout << section_name << " " << section_start_addr << endl;

    } else if (arg.at(arg.length() - 1) == 'o' && arg.at(arg.length() - 2) == '.'){
      //cout << arg << endl;
      obj_file_names[obj_file_cnt++] = arg;
    } else if (arg == "-o") {
      output_filename = argv[++i];
    } else if (arg == "-hex") {
      hexOption = true;
    }
  }

  if (!hexOption) {
    cout << "Error! -hex or -relocatable required." << endl;
    return -1;
  }

  // obrisati ------------------------------
  /*auto it =  linker.section_map.begin();
 
  // Iterate through the map and print the elements
  while (it != linker.section_map.end())
  {
    std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
    ++it;
  }*/
  // ---------------------------------------

  // za testiranje -------------------------
  // linker.section_map["my_code"] = Util::parseString("0x40000000");
  // linker.section_map["math"] = Util::parseString("0xF0000000");

  // output_filename = "program.hex";

  // obj_file_names[0] = "../handler.o";
  // obj_file_names[1] = "../math.o";
  // obj_file_names[2] = "../main.o";
  // obj_file_names[3] = "../isr_terminal.o";
  // obj_file_names[4] = "../isr_timer.o";
  // obj_file_names[5] = "../isr_software.o";
  // obj_file_cnt = 6;
  // ---------------------------------------

  for (int i = 0; i < obj_file_cnt; i++)
  {
    // obrada ulaznog fajla
    if ( linker.parseObjFile(obj_file_names[i]) < 0 ) {
      cout << "Error! There are undefined symbols!" << endl;
      return -1;
    }
  }
  
  if (linker.UNDexists()){
    cout << "Error! There are undefined symbols!" << endl;
  }

  linker.mapSections();
  linker.relocateData();

  linker.print_table();
  linker.print_lpool();
  linker.print_sections();

  linker.of = ofstream(output_filename);
  linker.hex();

  cout << "Linkovanje uspesno!" << endl;
  return 0;
}

int Linker::parseObjFile(string obj_file_name){
  ifstream input_stream(obj_file_name);

  string line;
  string str[7]; // 7 ulaza u tabeli simbola
  string data;   // section data
  int ii = 0;
  bool isSymTab = true;  // symtab = true, section/lpool = false;
  //bool isSection = false; // section data = true, lpool = false
  int sec_lpool_rel = 2; // 0 = sec, lpool = 1, rel = 2
  int relSize = 0;

  int oldSec;
  int sect;
  getline (input_stream, line); // #.symtab
  getline (input_stream, line); // Num Value    Size Type  Bind Ndx Name
  getline (input_stream, line); //  0. 00000000    0 NOTYP  LOC UND
  while (getline (input_stream, line)) { // symtab
    stringstream ss(line);    
    if (line == "") continue;
    if (line.at(0) == '#'){
      isSymTab = false; //tabela simbola samo na pocetku
      sec_lpool_rel = ( sec_lpool_rel + 1 ) % 3; // inicijalno 2 da bi prvi
        // prvi rpolaz bio 0 (uvek je prvo sec_data, pa se smenjuje sa lpool i rel)
      //isSection = !isSection; // isSection je inicijalno false, 
        // pa ce ovde biti true prvi put, smenjuju se sec_data i lpool
      if (sec_lpool_rel == 0){ // ako je sec_data
        curr_section_name = "";
        for (int i = 1; i < line.length(); i++)
        {
          curr_section_name += line.at(i);
        }
        relSize = getCurrentSectionSize();
        if (relSize < 0) relSize = 0;
      } else if (sec_lpool_rel == 1){ // ako je lpool
        getline (input_stream, line); // 0. 00000000
      }
      if(!sectionExists(curr_section_name) && sec_lpool_rel == 0){
        addSection(ELF_Section(curr_section_name));
      }  
      continue;
    }
    if (isSymTab){ //symtab
      ii = 0;
      while (ss >> str[ii++]);
      oldSec = str[5] == "UND" ? 0 : Util::parseString(str[5]);
      if (str[3] == "SCTN" && !sectionExists(str[6])) {
        current_section = symtab.getCnt();
        curr_section_name = str[6];
      }
      if (str[5] != "UND"){
        sect = current_section;
      } else {
        sect = UND;
      }
      int add = 0;
      if (str[3] != "SCTN" && str[5] != "UND") add = getCurrentSectionSize(); 
      if (add < 0) add = 0;
      int tmp = symtab.checkSymbolLinker(str[6], Util::parseString(str[1]), sect, add);
      if (tmp < 0) {
        cout << "Error! Double definition of symbol!" << endl;
        return -1;
      } else if (tmp == 1) { // treba dodati ulaz
        symtab.addSymbol(
          ELF_Symbol(
            symtab.getCnt(),
            Util::parseString(str[1]),
            0,
            str[3] == "NOTYP" ? ELF_Type::NOTYP : ELF_Type::SCTN,
            str[4] == "LOC" ? ELF_Bind::LOC : ELF_Bind::GLOB,
            //str[5] == "UND" ? 0 : Util::parseString(str[5]),
            sect,
            str[6],
            oldSec
        ));
      }
    } else if (sec_lpool_rel == 0){ // section data
      string temp;
      data = "";
      while (ss >> temp) data += temp;
      updateCurrentSection(data);
    } else if (sec_lpool_rel == 1) { // lpool
      string temp = "";
      for (int i = 4; i < line.length(); i++)
      {
        temp += line.at(i);
      }
      getLiteralPoolIndex(temp);
      getLPIndex(temp);
    } else { // relocation
      string temp;
      string s[5];
      int i = 0;
      while (ss >> temp){ s[i++] = temp; }
      string name = "";
      if(s[3].at(0) == '$'){
        for (int i = 1; i < s[3].length(); i++)
        {
          name += s[3].at(i);
        }
      } else {
        name = s[3];
      }

      addRelocationCurrentSection(
        ELF_Relocation(
          Util::parseStringDecimal(s[1]) + relSize,
          s[2] == "LIT" ? true : false,
          //Util::parseStringDecimal(s[3]),
          name,
          s[4] == "1" ? true : false
        )
      );
    }
  }
  
  return 0;
}

bool Linker::sectionExists(string name){
  for (auto s : this->sections){
    if (s.getName() == name) {
      return true;
    }
  }
  return false;
}

// za lpool sekcija
int Linker::getLiteralPoolIndex(string str){
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

// za lpool linkera
int Linker::getLPIndex(string str){
  int j = 0;
  int ind;
  if (str.length() > 2){    // moze da se desi da primi hex vr bez 0x
    if ( str.at(1) != 'x'){ // sto pravi problem kod parseString
      str = "0x" + str; 
    }
  }
  unsigned val = Util::parseString(str);

  for (auto s : this->lpool.literals){
    if (s.getValue() == val) {
      return s.getIndex();
    }
    j++;
  }
  
  return lpool.addLiteral(val);
}

// dodaje sadrzaj na kraj sekcije
int Linker::updateCurrentSection(string str){
  int j = 0;
  for (auto s : this->sections){
    if (s.getName() == curr_section_name) {
      auto it = sections.begin();
      
      advance(it, j);

      it->addDataNoSpace(str);

      return 0;
    }
    j++;
  }

  return -1;
}

// menja sadrzaj na odredjenom mestu
int Linker::updateCurrentSectionAtPosition(string str, int position){
  int j = 0;
  for (auto s : this->sections){
    if (s.getName() == curr_section_name) {
      auto it = sections.begin();
      
      advance(it, j);

      it->modifyData(str, position);

      return 0;
    }
    j++;
  }

  return -1;
}

// postavlja sekcijama pocetne adrese i azurira val u symtab
int Linker::mapSections(){
  int j = 0;
  unsigned start = 0;
  unsigned far_start = 0;
  // prvo upisivanje vrednosti iz mape
  auto itMap = this->section_map.begin();
  while (itMap != this->section_map.end()){
    auto it = sections.begin();
    while (it != sections.end()){
      if (it->getName() == itMap->first){
        it->setStart(itMap->second);
        far_start = itMap->second + it->getSize() > far_start ? 
          itMap->second + it->getSize() : far_start;
      }
      ++it;
    } 
    ++itMap;
  }

  // zatim popunjavanje sekcija redom
  for (auto s : this->sections){
    auto it = sections.begin();

    advance(it, j);

    unsigned tmp = it->getStart();
    if (tmp == 0){
      start = far_start;
      //it->setStart(start);
      far_start += it->getSize();
    } else {
      start = it->getStart();
    }
    it->setStart(start);

    this->symtab.setSymbolValue(it->getName(), start);
    
    start += it->getSize();

    j++;
  }

  updateSymbols();

  return 0;
}

void Linker::relocateData(){
  auto it = sections.begin();
  while (it != sections.end()){
    for (auto r : it->relocations)
    {
      if (r.getIsLiteral()){ // literal
        it->modifyData(Util::decToHex(getLPIndex(r.getName())), r.getPosition() * 2 + 1);
      } else { // simbol
        it->modifyData(Util::decToHex(symtab.getSymbolIndex(r.getName())), r.getPosition() * 2 + 1);
      }
    }
    ++it;
  }
}

int Linker::getCurrentSectionSize(){
  int j = 0;
  
  for (auto s : this->sections){
    if (s.getName() == curr_section_name) {
      auto it = sections.begin();
      
      advance(it, j);

      return it->getSize();

    }
    j++;
  }

  return -1;
}

// dodaje relokaciju na kraj sekcije
int Linker::addRelocationCurrentSection(ELF_Relocation rel){
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

void Linker::updateSymbols(){
  symtab.updateSymbols();
}

// vraca true ako postoji nedefinisan simbol -> greska!
bool Linker::UNDexists(){
  return symtab.UNDExists();
}
//------------------------------------------
// sabira vrednosti simbola sa sekcijama
void SymbolTable::updateSymbols(){
  int j = 0;
  int start = 0;
  for (auto s : this->symbols){
    auto it = symbols.begin();

    advance(it, j);

    int sec = it->getSection();
    int symval = getSymbolValue(sec);
    start = symval;
    ELF_Type type = it->getType();
    if ( type != ELF_Type::SCTN){
      int val = it->getValue();
      val += start;
      val = (unsigned) val;
      it->setValue(val);
    }

    // start = getSymbolValue(it->getSection());
    // if (it->getType() != ELF_Type::SCTN){
    //   it->setValue(it->getValue() + start);
    // }

    j++;
  }
}

int SymbolTable::getSymbolValue(int index){
  for (auto s : this->symbols){
    if (s.getNum() == index)
      return s.getValue();
  }
  return -1;
}

bool SymbolTable::UNDExists(){
  for (auto s : this->symbols){
    if (s.getNum() == 0) continue;
    if (s.getSection() == UND) {
      return true; 
    }
  }
  return false;
}

int ELF_Section::getLPIndex(string str){
  int j = 0;
  int ind;
  if (str.length() > 2){    // moze da se desi da primi hex vr bez 0x
    if ( str.at(1) != 'x'){ // sto pravi problem kod parseString
      str = "0x" + str; 
    }
  }
  unsigned val = Util::parseString(str);

  for (auto s : this->lpool.literals){
    if (s.getValue() == val) {
      return s.getIndex();
    }
    j++;
  }
  
  return lpool.addLiteral(val);
}

void ELF_Symbol::print_symbol(){
    cout << setw(2) << setfill(' ') << num << ". "
      << setw(8) << setfill('0') << Util::decToHex(value) << " "
      << setw(4) << setfill(' ') << size << " "
      << setw(5) << setfill(' ') << ((type == ELF_Type::NOTYP) ? "NOTYP" : "SCTN") << " "
      << setw(4) << setfill(' ') << ((bind == ELF_Bind::LOC) ? "LOC" : "GLOB") << " ";
    
    if (section == UND) {
      cout << setw(3) << setfill(' ') << "UND ";
    } else 
      cout << setw(3) << setfill(' ') << section << " ";
      
    cout << name << endl;
}

void SymbolTable::print_table(){
  // umesto cout -> ofstream
  cout << "#.symtab" << endl;
  cout << "Num Value    Size Type  Bind Ndx Name" << endl;

  for (auto s : this->symbols) {
    s.print_symbol();
  }
}

void Linker::print_sections(){
    for (auto s : this->sections) {  
      s.print_section();
    }
}

void ELF_Section::print_section(){
  /*cout << "#" << this->name << endl;
  int n = 0;
  for (int i = 0, j = 0; i < this->data.length(); i++, j++)
  {
    cout << data.at(i);
    if ( j % 10 == 0 && j > 0){
      i++;
      j-=11;
      if ( ++n == 2 ) { cout << endl; n = 0; }
      else cout << "  ";
    }
  }
  cout << endl;*/
  cout << "#" << this->name << " Start: " << start << " Size: " << size << endl;
  int n = 0;
  for (int i = 0, j = 0; i < this->data.length(); i++, j++)
  {
    cout << data.at(i);
    if ( j % 7== 0 && j > 0){
      //i++;
      j-=8;
      if ( ++n == 2 ) { cout << endl; n = 0; }
      else cout << "  ";
    }
  }
  cout << endl;

  //cout << "#" + name;
  //lpool.print_pool();
  
  cout << "#" + name + ".relocations" << endl;
  int i = 0;
  for (auto r : this->relocations)
  {
    cout << setw(2) << setfill(' ') << i << ". "
      << setw(4) << setfill('0') << r.getPosition()
      << (r.getIsLiteral() == true ? " LIT " : " SYM ")
      << setw(4) << r.getName()
      << " " << (r.getWord_dir() == true ? "1" : "0")
      << endl;
    i++;  
  }
}

void Literal_Pool::print_pool(){
  cout << ".literal_pool" << endl;
    for (auto l : this->literals) {     
    cout << setw(2) << setfill(' ') << l.getIndex() << ". "
      << setw(8) << setfill('0') << Util::decToHex(l.getValue())
      << endl;
    }
  
}

int SymbolTable::checkSymbolLinker(string symbol_name, int val, int section, int add){
    int j = 0;
  for (auto s : this->symbols){
    if (s.getName() == symbol_name) {
      if (section == UND || s.getType() == ELF_Type::SCTN) return 0; // ne radi nista
      if (s.getSection() == UND && section != UND){ // azuriraj ulaz
        auto it = symbols.begin();
        
        advance(it, j);

        it->setSection(section);
        it->setValue(val + add);

        return 0;
      }
      // section != "UND" && s.getSection() != UND -> dupla definicija
      return -1;
    }
    j++;
  }
  return 1;
}

int SymbolTable::setSymbolValue(string symbol_name, unsigned value){
  int j = 0;
  for (auto s : this->symbols){
    if (s.getName() == symbol_name) {
      auto it = symbols.begin();
      
      advance(it, j);

      it->setValue(value);

      return 0;
    }
    j++;
  }

  return -1;
}

// primer za modify data: 666777888999
// str = 123, pos = 5
// 666777888999 => 666123888999
void ELF_Section::modifyData(string str, int pos){
  this->data[pos] = '0';
  this->data[pos - 1] = '0';
  this->data[pos - 2] = '0';
  for (int i = 0; i < str.length(); i++)
  {
    this->data[pos - i] = str.at( str.length() - 1 - i);
  }
}

string SymbolTable::getSymbolName(int index){
  for (auto s : symbols){
    if ( s.getNum() == index ) return s.getName();
  }

  return "";
}

int SymbolTable::getSymbolIndex(string symbol_name){
  for (auto s : this->symbols){
    if (s.getName() == symbol_name)
      return s.getNum();
  }
  return -1;
}

// -----------------------------------------

void Linker::hex(){
  list<unsigned> start_addr_list;
  
  for (auto s : this->sections) {     
    start_addr_list.push_back(s.getStart());
  }
  
  start_addr_list.sort();

  for (auto s : start_addr_list) {     
   for (auto ss : this->sections) {     
      if (s == ss.getStart()) {
        hex_current_addr = ss.getStart();
        ss.hex_section(this);
      }
    } 
  }

  unsigned symtab_start = hex_current_addr;
  symtab.hex_table(this);
  unsigned pool_start = hex_current_addr;
  lpool.hex_pool(this);
  hex_string(Util::decToHex(symtab_start)); 
  hex_string(Util::decToHex(pool_start)); 
}

void ELF_Section::hex_section(Linker* ld){
  ld->hex_string(data); 
}

void SymbolTable::hex_table(Linker* ld){
  for (auto s : symbols)
  {
    string val = Util::decToHex(s.getValue());
    string temp = "";
    if ( val.length() < 8){
      for (int i = 0; i < 8 - val.length(); i++)
      {
        temp += "0";
      }
      temp += val;
      val = temp;
    }
    temp = "";
    temp += val.at(6);
    temp += val.at(7);
    temp += val.at(4);
    temp += val.at(5);
    temp += val.at(2);
    temp += val.at(3);
    temp += val.at(0);
    temp += val.at(1);
    val = temp;
    ld->hex_string(val);
  }
}

void Literal_Pool::hex_pool(Linker* ld){
  for (auto s : literals)
  {
    string val = Util::decToHex(s.getValue());
    string temp = "";
    if ( val.length() < 8){
      for (int i = 0; i < 8 - val.length(); i++)
      {
        temp += "0";
      }
      temp += val;
      val = temp;
    }
    temp = "";
    temp += val.at(6);
    temp += val.at(7);
    temp += val.at(4);
    temp += val.at(5);
    temp += val.at(2);
    temp += val.at(3);
    temp += val.at(0);
    temp += val.at(1);
    val = temp;
    ld->hex_string(val);
  }
}

void Linker::hex_string(string str) {
  for (int i = 0; i < str.length(); i++)
  {
    if (hex_cnt % 16 == 0){
      if (hex_cnt > 0) of << endl;
      of << Util::decToHex(hex_current_addr) << ":";
    }
    if (i % 2 == 0){
      of << " ";
      hex_current_addr++;
    }
    of << str.at(i);
    hex_cnt++;
  }
}