%{
  #include <cstdio>
  #include <iostream>
  #include <fstream>
  using namespace std;

  ofstream output_file("parser_output.txt");

  // Declare stuff from Flex that Bison needs to know about:
  extern int yylex();
  extern int yyparse();
  extern FILE *yyin;
  extern int line_num;
 
  void yyerror(const char *s);
%}

%union {
  int ival;
  float fval;
  char *sval;
}

// Constant-string tokens:
%token ENDL DOLLAR COLLON COMMA LBRACKET RBRACKET PLUS SP PC
%token GLOBAL_DIR EXTERN_DIR SECTION_DIR WORD_DIR SKIP_DIR ASCII_DIR EQU_DIR END_DIR
%token HALT_I INT_I IRET_I CALL_I RET_I JMP_I BEQ_I BNE_I BGT_I PUSH_I POP_I XCHG_I ADD_I
%token SUB_I MUL_I DIV_I NOT_I AND_I OR_I XOR_I SHL_I SHR_I LD_I ST_I CSRRD_I CSRWR_I
%token STATUS_REG HANDLER_REG CAUSE_REG

// Terminali
%token <ival> INT
%token <ival> LITERAL
%token <fval> FLOAT
%token <sval> LITERAL_HEX
%token <sval> STRING
%token <sval> REG
%token <sval> SYMBOL

%%
// Neterminali
start:
  lines ENDL{ output_file << "Done!!!" << endl; cout << "Done with file!" << endl; }
  |
  endls lines ENDL{ output_file << "Done!!!" << endl; cout << "Done with file!" << endl; }
  ;
lines:
  lines endls line
  |
  line
  ;
endls: 
  endls ENDL
  |
  ENDL
  ;
line:
  SYMBOL COLLON { output_file << "Label: "<< $1 << endl; free($1); } dir_instr
  |
  SYMBOL COLLON { output_file << "Label: "<< $1 << endl; free($1); }
  |
  dir_instr
  ;
dir_instr:
  directive
  |
  instruction
  ;
instruction:
  HALT_I { output_file << "Instruction: halt" << endl; }
  |
  INT_I { output_file << "Instruction: int" << endl; }
  |
  IRET_I { output_file << "Instruction: iret" << endl; }
  |
  CALL_I { output_file << "Instruction: call Operand: "; } operand { output_file << endl; }
  |
  RET_I { output_file << "Instruction: ret" << endl; }
  |
  JMP_I { output_file << "Instruction: jmp Operand: "; } operand { output_file << endl; }
  |
  BEQ_I { output_file << "Instruction: beq Operand: "; } 
  gpr { output_file << " "; } COMMA gpr { output_file << " "; } COMMA operand { output_file << endl; }
  |
  BNE_I { output_file << "Instruction: bne Operand: "; }
  gpr { output_file << " "; } COMMA gpr { output_file << " "; } COMMA operand { output_file << endl; }
  |
  BGT_I { output_file << "Instruction: bgt Operand: "; }
  gpr { output_file << " "; } COMMA gpr { output_file << " "; } COMMA operand { output_file << endl; }
  |
  PUSH_I { output_file << "Instruction: push Operand: "; } gpr { output_file << endl; }
  |
  POP_I { output_file << "Instruction: pop Operand: "; } gpr { output_file << endl; }
  |
  XCHG_I { output_file << "Instruction: xchg Operand: "; }
  gpr COMMA { output_file << " "; } gpr { output_file << endl; }
  |
  ADD_I { output_file << "Instruction: add Operand: "; } 
  gpr COMMA { output_file << " "; } gpr { output_file << endl; }
  |
  SUB_I { output_file << "Instruction: sub Operand: "; } 
  gpr COMMA { output_file << " "; } gpr { output_file << endl; }
  |
  MUL_I { output_file << "Instruction: mul Operand: "; } 
  gpr COMMA { output_file << " "; } gpr { output_file << endl; }
  |
  DIV_I { output_file << "Instruction: div Operand: "; } 
  gpr COMMA { output_file << " "; } gpr { output_file << endl; }
  |
  NOT_I { output_file << "Instruction: not Operand: "; } operand { output_file << endl; }
  |
  AND_I { output_file << "Instruction: and Operand: "; } 
  gpr COMMA { output_file << " "; } gpr { output_file << endl; }
  |
  OR_I { output_file << "Instruction: or Operand: "; } 
  gpr COMMA { output_file << " "; } gpr { output_file << endl; }
  |
  XOR_I { output_file << "Instruction: xor Operand: "; } 
  gpr COMMA { output_file << " "; } gpr { output_file << endl; }
  |
  SHL_I { output_file << "Instruction: shl Operand: "; } 
  gpr COMMA { output_file << " "; } gpr { output_file << endl; }
  |
  SHR_I { output_file << "Instruction: shr Operand: "; } 
  gpr COMMA { output_file << " "; } gpr { output_file << endl; }
  |
  LD_I { output_file << "Instruction: ld Operand: "; } 
  operand COMMA { output_file << " "; } gpr { output_file << endl; }
  |
  ST_I { output_file << "Instruction: st Operand: "; } 
  gpr COMMA { output_file << " "; } operand { output_file << endl; }
  |
  CSRRD_I { output_file << "Instruction: csrrd Operand: "; } 
  csr COMMA { output_file << " "; } gpr { output_file << endl; }
  |
  CSRWR_I { output_file << "Instruction: csrwr Operand: "; } 
  gpr COMMA { output_file << " "; } csr { output_file << endl; }
  ;
directive:
  GLOBAL_DIR { output_file << "Directive: .global Symbols: "; } symbol_list { output_file << endl; }
  |
  EXTERN_DIR { output_file << "Directive: .extern Symbols: "; } symbol_list { output_file << endl; }
  | 
  SECTION_DIR SYMBOL 
  { output_file << "Directive: .section Section: " << $2 << endl; free($2); }
  |
  WORD_DIR { output_file << "Directive: .word Symbols/Literals: "; } symbol_literal_list { output_file << endl; }
  |
  SKIP_DIR { output_file << "Directive: .skip Literal: "; } literal { output_file << endl; }
  |
  ASCII_DIR { output_file << "Directive: .ascii" << endl; }
  |
  EQU_DIR { output_file << "Directive: .equ" << endl; }
  |
  END_DIR { output_file << "Directive: .end" << endl; }
  ;
symbol_literal_list:
  symbol_literal_list { output_file << " "; }  COMMA symbol_literal 
  |
  symbol_literal
  ;
symbol_literal:
  SYMBOL { output_file << $1; free($1); }
  |
  literal
  ;
literal:
  LITERAL { output_file << $1; }
  |
  LITERAL_HEX { output_file << $1; free($1); }
  ;
symbol_list:
  symbol_list COMMA SYMBOL { output_file << " "<< $3; free($3); }
  |
  SYMBOL { output_file << $1; free($1); }
  ;
operand:
  DOLLAR { output_file << "$"; } literal
  |
  DOLLAR SYMBOL { output_file << "$"<< $2 ; free($2); }
  |
  literal
  |
  SYMBOL { output_file << $1; free($1); }
  |
  gpr
  |
  lbracket gpr rbracket
  |
  lbracket gpr PLUS { output_file << " + "; } literal rbracket
  |
  lbracket gpr PLUS SYMBOL 
  { output_file << " + " << $4; free($4); } rbracket
  ;
gpr:
  PC { output_file << "%pc"; }
  |
  SP { output_file << "%sp"; }
  |
  REG { output_file << $1; free($1); }
  ;
csr:
  STATUS_REG { output_file << "%status"; }
  |
  HANDLER_REG { output_file << "%handler"; }
  |
  CAUSE_REG { output_file << "%cause"; }
  ;
lbracket:
  LBRACKET { output_file << "[ ";}
  ;
rbracket:
  RBRACKET { output_file << " ]";}
  ;
// snazzle:
//   header template body_section footer {
//       cout << "done with a snazzle file!" << endl;
//     }
//   ;
// header:
//   SNAZZLE FLOAT ENDL{
//       cout << "reading a snazzle file version " << $2 << endl;
//     }
//   ;
// template:
//   typelines
//   ;
// typelines:
//   typelines typeline
//   | typeline
//   ;
// typeline:
//   TYPE STRING ENDL{
//       cout << "new defined snazzle type: " << $2 << endl;
//       free($2);
//     }
//   ;
// body_section:
//   body_lines
//   ;
// body_lines:
//   body_lines body_line
//   | body_line
//   ;
// body_line:
//   INT INT INT INT STRING ENDL{
//       cout << "new snazzle: " << $1 << $2 << $3 << $4 << $5 << endl;
//       free($5);
//     }
//   ;
// footer:
//   END ENDL
//   ;

%%

int main(int argc, char* argv[]) {

  string fname;
  if (argc > 1) fname = argv[1]; 

  FILE *myfile = fopen(argv[1], "r");
  //FILE *myfile = fopen("../tests/test.s", "r");

  if (!myfile) {
    cout << "Cant open file." << endl;
    return -1;
  }

  yyin = myfile;
  
  // Parse through the input:
  yyparse();
  
  output_file.close();
}

void yyerror(const char *s) {
  cout << "Parse error at line: " << line_num << "!  Message: " << s << endl;
  exit(-1);
}