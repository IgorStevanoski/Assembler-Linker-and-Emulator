#ifndef _as_constants_hpp_
#define _as_constants_hpp_
#include <stdio.h>
#include <iostream>

using namespace std;

const int DIR_ERROR = -100;
const int INSTR_ERROR = -200;
const int LABEL_ERROR = -300;
const int LABEL_DEFINED_ERROR = -301;

const int DIR_GLOBAL_ERROR = -110;
const int DIR_GLOBAL_UNDEFINED_ERROR = -111;
const int DIR_EXTERN_ERROR = -120;
const int DIR_EXTERN_DEFINED_ERROR = -121;
const int DIR_SECTION_ERROR = -130;
const int DIR_WORD_ERROR = -140;
const int DIR_SKIP_ERROR = -150;
const int DIR_END_ERROR = -160;

const int END_ASSEMBLING = 160;

const int INSTR_HALT_ERROR = -201;
const int INSTR_INT_ERROR = -202;
const int INSTR_IRET_ERROR = -203;
const int INSTR_CALL_ERROR = -204;
const int INSTR_RET_ERROR = -205;
const int INSTR_JMP_ERROR = -206;
const int INSTR_BEQ_ERROR = -207;
const int INSTR_BNE_ERROR = -208;
const int INSTR_BGT_ERROR = -209;
const int INSTR_PUSH_ERROR = -210;
const int INSTR_POP_ERROR = -211;
const int INSTR_XCHG_ERROR = -212;
const int INSTR_ADD_ERROR = -213;
const int INSTR_SUB_ERROR = -214;
const int INSTR_MUL_ERROR = -215;
const int INSTR_DIV_ERROR = -216;
const int INSTR_NOT_ERROR = -217;
const int INSTR_AND_ERROR = -218;
const int INSTR_OR_ERROR = -219;
const int INSTR_XOR_ERROR = -220;
const int INSTR_SHL_ERROR = -221;
const int INSTR_SHR_ERROR = -222;
const int INSTR_LD_ERROR = -223;
const int INSTR_ST_ERROR = -224;
const int INSTR_CSRRD_ERROR = -225;
const int INSTR_CSRWR_ERROR = -226;

const int INSTR_HALT_OPCODE          = 0x00;
const int INSTR_INT_OPCODE           = 0x10;
const int INSTR_IRET_OPCODE          = 0x11; // Svoja const
const int INSTR_RET_OPCODE           = 0x93;
const int INSTR_CALL_OPCODE          = 0x20;
const int INSTR_JMP_OPCODE           = 0x30;
const int INSTR_BEQ_OPCODE           = 0x31;
const int INSTR_BNE_OPCODE           = 0x32;
const int INSTR_BGT_OPCODE           = 0x33;
const int INSTR_PUSH_OPCODE          = 0x81;
const int INSTR_POP_OPCODE           = 0x93;
const int INSTR_POP_CSR_OPCODE       = 0x97;
const int INSTR_XCHG_OPCODE          = 0x40;
const int INSTR_ADD_OPCODE           = 0x50;
const int INSTR_SUB_OPCODE           = 0x51;
const int INSTR_MUL_OPCODE           = 0x52;
const int INSTR_DIV_OPCODE           = 0x53;
const int INSTR_NOT_OPCODE           = 0x60;
const int INSTR_AND_OPCODE           = 0x61;
const int INSTR_OR_OPCODE            = 0x62;
const int INSTR_XOR_OPCODE           = 0x63;
const int INSTR_SHL_OPCODE           = 0x70;
const int INSTR_SHR_OPCODE           = 0x71;
const int INSTR_LD_OPCODE            = 0x91;
const int INSTR_LD_INDIRECT_OPCODE   = 0x92;
const int INSTR_ST_OPCODE            = 0x80; 
const int INSTR_ST_INDIRECT_OPCODE   = 0x82;
const int INSTR_CSRRD_OPCODE         = 0x90;
const int INSTR_CSRWR_OPCODE         = 0x94;

const string INSTR_HALT_OPCODE_string          = "00";
const string INSTR_INT_OPCODE_string           = "10";
const string INSTR_IRET_OPCODE_string          = "11"; // Svoja const
const string INSTR_RET_OPCODE_string           = "93";
const string INSTR_CALL_OPCODE_string          = "20";
const string INSTR_JMP_OPCODE_string           = "30";
const string INSTR_BEQ_OPCODE_string           = "31";
const string INSTR_BNE_OPCODE_string           = "32";
const string INSTR_BGT_OPCODE_string           = "33";
const string INSTR_PUSH_OPCODE_string          = "81";
const string INSTR_POP_OPCODE_string           = "93";
const string INSTR_POP_CSR_OPCODE_string       = "97";
const string INSTR_XCHG_OPCODE_string          = "40";
const string INSTR_ADD_OPCODE_string           = "50";
const string INSTR_SUB_OPCODE_string           = "51";
const string INSTR_MUL_OPCODE_string           = "52";
const string INSTR_DIV_OPCODE_string           = "53";
const string INSTR_NOT_OPCODE_string           = "60";
const string INSTR_AND_OPCODE_string           = "61";
const string INSTR_OR_OPCODE_string            = "62";
const string INSTR_XOR_OPCODE_string           = "63";
const string INSTR_SHL_OPCODE_string           = "70";
const string INSTR_SHR_OPCODE_string           = "71";
const string INSTR_LD_OPCODE_string            = "91";
const string INSTR_LD_INDIRECT_OPCODE_string   = "92";
const string INSTR_ST_OPCODE_string            = "80"; 
const string INSTR_ST_INDIRECT_OPCODE_string   = "82";
const string INSTR_CSRRD_OPCODE_string         = "90";
const string INSTR_CSRWR_OPCODE_string         = "94";

const string REG_PC_string = "f";
const string REG_SP_string = "e";

class Error_Message {
public:
  static void error_message(int error_code);
};

void Error_Message::error_message(int error_code){

  cout << "Assembler_error: ";

  switch (error_code)
  {
  case DIR_ERROR: cout << "Directive error!" << endl; break;
  case INSTR_ERROR: cout << "Instruction error!" << endl; break;
  case LABEL_ERROR: cout << "Label error!" << endl; break;
  case LABEL_DEFINED_ERROR: cout << "Label already defined!" << endl; break;
  case DIR_GLOBAL_ERROR: cout << "Directive .global error!" << endl; break;
  case DIR_GLOBAL_UNDEFINED_ERROR: cout << ".global symbol already undefined!" << endl; break;
  case DIR_EXTERN_ERROR: cout << "Directive .extern error!" << endl; break;
  case DIR_EXTERN_DEFINED_ERROR: cout << ".extern symbol defined!" << endl; break;
  case DIR_SECTION_ERROR: cout << "Directive .section error!" << endl; break;
  case DIR_WORD_ERROR: cout << "Directive .word error!" << endl; break;
  case DIR_SKIP_ERROR: cout << "Directive .skip error!" << endl; break;
  case DIR_END_ERROR: cout << "Directive .end error!" << endl; break;
  case INSTR_HALT_ERROR: cout << "Instruction halt error!" << endl; break;
  case INSTR_INT_ERROR: cout << "Instruction int error!" << endl; break;
  case INSTR_IRET_ERROR: cout << "Instruction iret error!" << endl; break;
  case INSTR_CALL_ERROR: cout << "Instruction call error!" << endl; break;
  case INSTR_RET_ERROR: cout << "Instruction ret error!" << endl; break;
  case INSTR_JMP_ERROR: cout << "Instruction jmp error!" << endl; break;
  case INSTR_BEQ_ERROR: cout << "Instruction beq error!" << endl; break;
  case INSTR_BNE_ERROR: cout << "Instruction bne error!" << endl; break;
  case INSTR_BGT_ERROR: cout << "Instruction bgt error!" << endl; break;
  case INSTR_PUSH_ERROR: cout << "Instruction push error!" << endl; break;
  case INSTR_POP_ERROR: cout << "Instruction pop error!" << endl; break;
  case INSTR_XCHG_ERROR: cout << "Instruction xchg error!" << endl; break;
  case INSTR_ADD_ERROR: cout << "Instruction add error!" << endl; break;
  case INSTR_SUB_ERROR: cout << "Instruction sub error!" << endl; break;
  case INSTR_MUL_ERROR: cout << "Instruction mul error!" << endl; break;
  case INSTR_DIV_ERROR: cout << "Instruction div error!" << endl; break;
  case INSTR_NOT_ERROR: cout << "Instruction not error!" << endl; break;
  case INSTR_AND_ERROR: cout << "Instruction and error!" << endl; break;
  case INSTR_OR_ERROR: cout << "Instruction or error!" << endl; break;
  case INSTR_XOR_ERROR: cout << "Instruction xor error!" << endl; break;
  case INSTR_SHL_ERROR: cout << "Instruction shl error!" << endl; break;
  case INSTR_SHR_ERROR: cout << "Instruction shr error!" << endl; break;
  case INSTR_LD_ERROR: cout << "Instruction ld error!" << endl; break;
  case INSTR_ST_ERROR: cout << "Instruction st error!" << endl; break;
  case INSTR_CSRRD_ERROR: cout << "Instruction csrrd error!" << endl; break;
  case INSTR_CSRWR_ERROR: cout << "Instruction csrwr error!" << endl; break;
  default:
    break;
  }

}

#endif