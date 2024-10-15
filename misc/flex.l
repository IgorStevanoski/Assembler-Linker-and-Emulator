%{
  #include <cstdio>
  #include <iostream>
  using namespace std;
  extern int yylex();
  int line_num = 1;
  #include "bison.tab.h"  // to get the token types from Bison
%}
%option noyywrap

%x COMMENT

%%

\.global       { return GLOBAL_DIR; }
\.extern       { return EXTERN_DIR; }
\.section      { return SECTION_DIR; }
\.word         { return WORD_DIR; }
\.skip         { return SKIP_DIR; }
\.ascii        { return ASCII_DIR; }
\.equ          { return EQU_DIR; }
\.end          { return END_DIR; }

halt           { return HALT_I; }
int            { return INT_I; }
iret           { return IRET_I; }
call           { return CALL_I; }
ret            { return RET_I; }
jmp            { return JMP_I; }
beq            { return BEQ_I; }
bne            { return BNE_I; }
bgt            { return BGT_I; }
push           { return PUSH_I; }
pop            { return POP_I; }
xchg           { return XCHG_I; }
add            { return ADD_I; }
sub            { return SUB_I; }
mul            { return MUL_I; }
div            { return DIV_I; }
not            { return NOT_I; }
and            { return AND_I; }
or             { return OR_I; }
xor            { return XOR_I; }
shl            { return SHL_I; }
shr            { return SHR_I; }
ld             { return LD_I; }
st             { return ST_I; }
csrrd          { return CSRRD_I; }
csrwr          { return CSRWR_I; }

%status        { return STATUS_REG; }
%handler       { return HANDLER_REG; }
%cause         { return CAUSE_REG; }
%sp            { return SP; }
%pc            { return PC; }
%r[0-9]+       { yylval.sval = strdup(yytext); return REG; }
[a-zA-Z][a-zA-Z0-9_]* { yylval.sval = strdup(yytext); return SYMBOL; } 

[ \t]          ;
:              { return COLLON; }
,              { return COMMA; }
\[             { return LBRACKET; }
\]             { return RBRACKET; }
\$             { return DOLLAR; }
\+             { return PLUS; }
#              { BEGIN(COMMENT); }

0x[0-9A-F]+       { yylval.sval = strdup(yytext); return LITERAL_HEX;}
[0-9]+         { yylval.ival = atoi(yytext); return LITERAL;}

[0-9]+\.[0-9]+ { yylval.fval = atof(yytext); return FLOAT; }
[a-zA-Z0-9_]+   {
  yylval.sval = strdup(yytext);
  return STRING;
}
<COMMENT>[\r\n] { line_num++; BEGIN(INITIAL); }
<COMMENT>.      { BEGIN(COMMENT); }
\n              { line_num++; return ENDL; }
.              ;
%%