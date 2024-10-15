#ifndef _emulator_hpp_
#define _emulator_hpp_
#include "../inc/as_constants.hpp"
//#include "../inc/assembler.hpp"
#include "../inc/util.hpp"
#include <list>
#include <map>

using namespace::std;

class Emulator {
public:
  Emulator(){
    for (int i = 0; i < 16; i++)
    {
      gpr[i] = 0;
    }
    for (int i = 0; i < 3; i++)
    {
      csr[i] = 0;
    }
  }
  ~Emulator(){}

  void fillMemory(string);
  void fill_symbols_literals();
  int emulate();

  void print_end_emulation();
  void print_memory(); // za testiranje

  void push(unsigned);
  unsigned pop();

  unsigned getMemValue(unsigned);
  void setMemValue(unsigned, unsigned);

private:
  list<unsigned> symbols;
  list<unsigned> literals;
  unsigned symtabStart; // adresa od koje krecu simboli
  unsigned lpoolStart;  // adresa od koje krecu literali
  unsigned lastAddr; //poslednja adresa do koje idu literali

  unsigned const startAddress =  0x40000000; // odredjeno tekstom projekta 

  // gpr[0] = 0; gpr[14] = sp; gpr[15] = pc
  unsigned gpr[16];
  unsigned csr[3]; // status, handler, cause
  unsigned& sp = gpr[14];
  unsigned& pc = gpr[15];
  unsigned& status  = csr[0];
  unsigned& handler = csr[1];
  unsigned& cause   = csr[2];
  map<unsigned, string> memory; // unsigned = adresa, string = val
};

#endif