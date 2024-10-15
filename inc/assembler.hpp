#ifndef _asembler_hpp_
#define _asembler_hpp_
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <iterator>
#include <cmath>

using namespace std;

const int UND = 0;      // za nedefinisane ( korisceni ovde, definisani negde druge)
// const int ABS    = ; // za apsolutne simbole kojima ne treba realokacija 
// const int COMMON = ; // za neinicijalizovane podatke koji jos nisu alocirani

enum ELF_Type
{
  NOTYP,
  SCTN
};

enum ELF_Bind
{
  LOC,
  GLOB
};

class Assembler;
class Linker;

class ELF_Relocation {
public:
  ELF_Relocation(unsigned pos, bool l, string n) : 
    position(pos), isLiteral(l), name(n), word_dir(false) {}

  ELF_Relocation(unsigned pos, bool l, string n, bool w) : 
    position(pos), isLiteral(l), name(n), word_dir(w) {} 

  unsigned getPosition() { return position; }
  bool getIsLiteral() { return isLiteral; }
  bool getWord_dir() { return word_dir; }
  //int getNum() { return num; }
  string getName() { return name;}

private:
  unsigned position; // bajt u kodu koji treba da se promeni
  bool isLiteral; // literal = true / symbol = false
  //int num;  // index u bazenu literala / tabeli simbola
  // kada se pravi relokacija u .word onda ne treba da se 
  // upisuje pomeraj, vec vrednost simbola iz tabele  
  string name; // ime simbola/literala
  bool word_dir; 
};

class ELF_Symbol {
public:
  ELF_Symbol(int n, int v, int s, ELF_Type t, ELF_Bind b, int se, string na) :
    num(n), value(v), size(s), type(t), bind(b), section(se), name(na), oldSection(0) {};
  ELF_Symbol(int n, int v, int s, ELF_Type t, ELF_Bind b, int se, string na, int old) :
    num(n), value(v), size(s), type(t), bind(b), section(se), name(na), oldSection(old) {};
  virtual ~ELF_Symbol() {};  
  
  string getName() { return name; }
  int getNum() { return num; }
  unsigned getValue() { return value; }
  void setValue(unsigned v) { value = v; }
  int getSection() { return section; }
  void setSection(int s) { section = s; }
  void setSymbolGlobal() { this->bind = ELF_Bind::GLOB; }
  ELF_Type getType() { return type; }
  void setSymbolUND() { this->section = UND; }

  void print_symbol(Assembler*);
  void print_symbol(); // u cout

protected:
  int num;
  unsigned value;
  int size;
  ELF_Type type;
  ELF_Bind bind;
  int section;
  string name;
  int oldSection; // za linker
};

class SymbolTable{
public:
  SymbolTable() : cnt(0){
    addSymbol(ELF_Symbol(cnt, 0, 0, NOTYP, LOC, UND, " "));
  };
  virtual ~SymbolTable(){};

  void addSymbol(ELF_Symbol symbol) { symbols.push_back(symbol); cnt++;};
  bool checkSymbol(string symbol_name);
  bool UNDExists(); // za linker
  int checkSymbolLinker(string symbol_name, int val, int section, int add);
  int setSymbolGlobal(string symbol_name);
  int setSymbolUND(string symbol_name);
  int getSymbolValue(string symbol_name);
  int getSymbolValue(int index);
  string getSymbolName(int index); // za Linker
  int setSymbolValue(string symbol_name, unsigned value);
  int getSymbolIndex(string symbol_name);
  int getCurrSection(string symbol_name);
  int getCnt() { return cnt; }

  void updateSymbols();

  void print_table(); // u cout
  void print_table(Assembler*);
  void hex_table(Linker*);

protected:
  int cnt;
  list<ELF_Symbol> symbols;
};

class LP_Entry {
public:
  LP_Entry(int val, int ind) : value(val), index(ind) {}

  int getValue() { return value; }
  int getIndex() { return index; }

private:
  int index;
  int value;
};

class Literal_Pool {
public:
  friend class ELF_Section;
  friend class Linker;

  Literal_Pool() : cnt(0) { addLiteral(0); };
  virtual ~Literal_Pool(){};

  int addLiteral(int val) { literals.push_back(LP_Entry(val, cnt)); return cnt++;}
  void print_pool();
  void print_pool(Assembler*);
  void hex_pool(Linker*);

private:
  int cnt;
  list<LP_Entry> literals;
};

class ELF_Section {
public:
  ELF_Section(string s) : name(s), data(""), size(0), start(0) {};
  virtual ~ELF_Section() {};

  string getName() { return name; }
  int getSize() { return size; }
  int getStart() { return start; }
  void setStart(unsigned s) { start = s; }
  void addData(string s) { if (data != "") data += " "; data += s; }
  void modifyData(string, int); // za linker
  void addRelocation(ELF_Relocation rel) { relocations.push_back(rel); }
  void addDataNoSpace(string s) { data += s; size += s.length() / 2; } // za linker
  void print_section();
  void print_section(Assembler*);
  void hex_section(Linker*); // ispis u hex file
  int getLPIndex(string);

  friend class Linker; // zbog relocateData() u linkeru
  
private:
  string name;
  string data;
  int size;
  unsigned start;
  Literal_Pool lpool;
  list<ELF_Relocation> relocations;
};

class Assembler {
public:
  Assembler(string s) : str(s), location_counter(0), isFirstPass(true), curr_section(UND) {};
  Assembler(string s, string ofname) : str(s), location_counter(0), isFirstPass(true), curr_section(UND) 
  { of = ofstream(ofname); };
  virtual ~Assembler() { of.close(); }; // treba da dealocira

  int firstPass();
  int secondPass();

  void addSection(ELF_Section section) { sections.push_back(section); };
  int updateCurrentSection(string);
  int fillCurrentSection(string);
  int addRelocationCurrentSection(ELF_Relocation rel);
  int getLiteralPoolIndex(string);
  string getIndirectOperandValue(string);
  string getOperandValue(string);

  static void error_message(int);

  void print_table() { symtab.print_table(this); }
  void print_sections();

  ofstream of;
private:

  // prosledjuje se pozicija do koje se obradio ulazni fajl
  int process_directive(int*);
  int process_instruction(int*);
  int process_label(int*);

  // direktive
  int process_dir_global(int*);
  int process_dir_extern(int*);
  int process_dir_section(int*);
  int process_dir_word(int*);
  int process_dir_skip(int*);
  int process_dir_end(int*);

  //instrukcije
  int process_instr_halt(int*);
  int process_instr_int(int*);
  int process_instr_iret(int*);
  int process_instr_call(int*);
  int process_instr_ret(int*);
  int process_instr_jmp(int*);
  int process_instr_beq(int*);
  int process_instr_bne(int*);
  int process_instr_bgt(int*);
  int process_instr_push(int*);
  int process_instr_pop(int*);
  int process_instr_xchg(int*);
  int process_instr_add(int*);
  int process_instr_sub(int*);
  int process_instr_mul(int*);
  int process_instr_div(int*);
  int process_instr_not(int*);
  int process_instr_and(int*);
  int process_instr_or(int*);
  int process_instr_xor(int*);
  int process_instr_shl(int*);
  int process_instr_shr(int*);
  int process_instr_ld(int*);
  int process_instr_st(int*);
  int process_instr_csrrd(int*);
  int process_instr_csrwr(int*);

  bool isFirstPass;
  int location_counter;
  SymbolTable symtab;
  list<ELF_Section> sections;
  string str; //parser output
  int curr_section;
  string curr_section_name;
};

#endif