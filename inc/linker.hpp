#ifndef _linker_hpp_
#define _linker_hpp_
#include "../inc/as_constants.hpp"
#include "../inc/assembler.hpp"
#include "../inc/util.hpp"
#include <list>
#include <map>

//  Uloga linkera:
//  -Razresavanje simbola (jedan simbol -> jedna definicija)
//      - ako je simbol nedefinisan - greska
//      - ako je simbol visestruko definisan - greska
//  -Relokacija - asembler sve postavlja od 0, linker dodeljuje mem. lokaciju simbolima
//        zatim azurira reference na te simbole
//      - relokacija sekcija i definicija simbola
//      - relokacija referenci simbola

// Rad linkera:
// 1. Uzima tabelu simbola iz prvog fajla i postavlja je kao pocetnu ()
// 2. Uzima sekcije iz fajla i popunjava svoje (to ukljucuje i bazen literala i relokacije)
// 3. Otvara naredni fajl:
//      - dopunjuje tabelu simbola
//      - proverava duple definicije i azurira UND simbole ako su sad definisani
//      - popunjava sekcije (bazen literala, relokacije)
// 4. Kada zavrsi sa fajlovima:
//      - provera da li ima nerazresenih simbola
//      - provera da li se preklapaju sekcije ( sekcije imaju velicinu i pocetnu adr. koja moze biti dodeljena sa -place=...)
//      - relokacije 
//      - pravi se zavrsni fajl koji se salje emulatoru 
// * vrednosti simbola u tabeli simbola treba sabrati sa pocetnom adresom sekcije
// * #shdr na pocetku 

//linker:
//	- ucita sve sekcije iz svih fajlova, zatim popunjava memoriju, pa popunjava tabelu simbola
//	- value od simbola u tabeli simbola je jednak njegovoj virtuelnoj adresi (ovo vazi i za sekcije)
//	- shdr = section headers

// Nije uradjeno za linker:
// - nisu podrzane relokacije za .word (kada je 4. argument = 1), treba umesto 
//   upisivanja indexa na mestu relokacije da se upise vrednost

// Ostalo jos:
// - veliki bazen literala
// - izvrsavanje relokacija
// - hex izlaz

class Linker {
public:
  Linker() : current_section(0) {
    hex_current_addr = 0;
    hex_cnt = 0;
  };
  virtual ~Linker(){};

  int parseObjFile(string);

  void addSection(ELF_Section section) { sections.push_back(section); }
  int addRelocationCurrentSection(ELF_Relocation rel);
  int updateCurrentSection(string);
  int updateCurrentSectionAtPosition(string, int);
  bool sectionExists(string);
  bool UNDexists();

  int mapSections();
  void relocateData();
  void updateSymbols();

  int getLiteralPoolIndex(string str);
  int getLPIndex(string str);
  int getCurrentSectionSize();

  void print_table() { symtab.print_table(); }
  void print_lpool() { lpool.print_pool(); }
  void print_sections();

  void hex();
  void hex_string(string);

  map<string, unsigned> section_map;

  ofstream of;
  unsigned hex_current_addr;
  unsigned hex_cnt;
private:

  list<ELF_Section> sections;
  SymbolTable symtab;
  string curr_section_name;
  int current_section;
  Literal_Pool lpool;
};

#endif