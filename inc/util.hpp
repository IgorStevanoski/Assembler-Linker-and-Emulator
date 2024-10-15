#ifndef _util_hpp_
#define _util_hpp_
#include "../inc/as_constants.hpp"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <iterator>
#include <math.h>

using namespace std;

class Util{
public:

  static string get_word(string str, int *j);
  static string get_word_with_endl(string str, int *j, int* flag);
  static string get_operand_hex(string str, int *j);
  static string get_operand_gpr(string str, int *j);
  static string get_operand_csr(string str, int *j);
  static string getOperandValue(string);
  static string fillHex(string);
  static int parseString(string str);
  static int parseStringDecimal(string str);
  static int parseStringHex(string str);
  
  static bool isLiteral(string str);
  static bool isHex(string str);
  
  static string decToHex(unsigned); 
  static string hexLittleEndian(string);

};


string Util::get_word(string str, int* j){
  string curr_word;
  stringstream ss;
  char c;
  int i = *j;

  if (str.at(i) == ' ') i++;
  while ( str.at(i++) != ' ' && i < str.length()) {
      c = str.at(i-1);
      if ( c == '\n'){
          break;
        }

      ss << str.at(i - 1);
    }
    *j = i;

    return curr_word = ss.str();
} 

// flag = 1 ako se posle reci nadje '\n'
// koristi se kada ima proizvoljan broj operanada
string Util::get_word_with_endl(string str, int* j, int* flag){
  string curr_word;
  stringstream ss;
  char c;
  int i = *j;
  *flag = 0;

  if (str.at(i) == ' ') i++;
  while ( str.at(i++) != ' ' && i < str.length()) {
      c = str.at(i-1);
      if ( c == '\n'){
          *flag = 1;
          break;
        }

      ss << str.at(i - 1);
    }
    *j = i;

    return curr_word = ss.str();
}

string Util::get_operand_hex(string str, int *j){
  string curr_word;
  string hex;
  int flag = 0;

  curr_word = get_word(str, j);
  char c = curr_word.at(0);

  if (c == '%'){ // registar
    if ( curr_word.at(1) == 'r'){ // %rxx

    }
  } else if (c == '$'){ // neposredna vrednost    

  } else if (c == '['){ // registarsko indirektno sa ofsetom    
    
  } else { // memorijsko direktno    
    
  } 

  while (flag == 0){
    curr_word = Util::get_word_with_endl(str, j, &flag);
    // treba zavrsiti
  }

  return "xx xx xx";
}

// str je: %rxx, %sp ili %pc
string Util::get_operand_gpr(string str, int *j){
  string curr_word;

  curr_word = get_word(str, j);
  char c = curr_word.at(2);

  if (curr_word == "\%sp") return REG_SP_string;
  if (curr_word == "\%pc") return REG_PC_string;
  
  if ( curr_word.length() == 3 ) {
    switch (c)
    {
    case '0': return "0";
    case '1': return "1";
    case '2': return "2";
    case '3': return "3";
    case '4': return "4";
    case '5': return "5";
    case '6': return "6";
    case '7': return "7";
    case '8': return "8";
    case '9': return "9";
    default: return "x";
    }
  }

  c = curr_word.at(3);

  switch (c)
  {
  case '0': return "a";
  case '1': return "b";
  case '2': return "c";
  case '3': return "d";
  case '4': return "e";
  case '5': return "f";
  default: return "x";
  }
}
// str je: %status", %handler ili %cause
string Util::get_operand_csr(string str, int *j){
  string curr_word = get_word(str, j);

  if ( curr_word == "\%status" ){
    return "0";
  }
  if ( curr_word == "\%handler" ){
    return "1";
  }
  if ( curr_word == "\%cause" ){
    return "2";
  }
  return "x";
}

// Uvek vraca decimalnu vrednost (i za str kao dec i hex)
int Util::parseString(string str){
  int val = 0, cur = 0;
  int start = 0; // ako je hex vr koja pocinje sa 0x preskacu se prva 2 slova
  char c = str.at(0);

  if ( str.length() == 1 ) return c - '0';

  if ( c == '0' ) {// hex
    if ( str.at(1) == 'x') start = 2;
    for (int i = start; i < str.length(); i++)
    {
      c = str.at(i);
      switch (c)
      {
      case '0': cur = 0; break;
      case '1': cur = 1; break;
      case '2': cur = 2; break;
      case '3': cur = 3; break;
      case '4': cur = 4; break;
      case '5': cur = 5; break;
      case '6': cur = 6; break;
      case '7': cur = 7; break;
      case '8': cur = 8; break;
      case '9': cur = 9; break;
      case 'a': case 'A': cur = 10; break;
      case 'b': case 'B': cur = 11; break;
      case 'c': case 'C': cur = 12; break;
      case 'd': case 'D': cur = 13; break;
      case 'e': case 'E': cur = 14; break;
      case 'f': case 'F': cur = 15; break;
      default:
        break;
      }
      val = val * 16 + cur;
    
    }
  } else { // decimal
      for (int i = 0; i < str.length(); i++)
        {
          c = str.at(i);
          cur = c - '0';
          val = val * 10 + cur;
        }
  }

  return val;
}

// Samo za decimalne vrednosti
int Util::parseStringDecimal(string str){
  int val = 0, cur = 0;
  int start = 0; 
  char c = str.at(0);

  for (int i = 0; i < str.length(); i++)
    {
      c = str.at(i);
      cur = c - '0';
      val = val * 10 + cur;
    }
  
  return val;
}

// Samo za hex vrednosti bez 0x na pocetku
int Util::parseStringHex(string str){
  int val = 0, cur = 0;
  char c;

  for (int i = 0; i < str.length(); i++)
  {
    c = str.at(i);
    switch (c)
    {
    case '0': cur = 0; break;
    case '1': cur = 1; break;
    case '2': cur = 2; break;
    case '3': cur = 3; break;
    case '4': cur = 4; break;
    case '5': cur = 5; break;
    case '6': cur = 6; break;
    case '7': cur = 7; break;
    case '8': cur = 8; break;
    case '9': cur = 9; break;
    case 'a': case 'A': cur = 10; break;
    case 'b': case 'B': cur = 11; break;
    case 'c': case 'C': cur = 12; break;
    case 'd': case 'D': cur = 13; break;
    case 'e': case 'E': cur = 14; break;
    case 'f': case 'F': cur = 15; break;
    default:
      break;
    }
    val = val * 16 + cur;
  
  }
  
  return val;
}

// da li je dati string literal (broj u hex ili decimalnom formatu)
bool Util::isLiteral(string str){
  char c;
  
  if ( str.length() > 1 )
    c = str.at(1);

  if ( c == 'x' ) {
    for (int i = 0; i < str.length(); i++){
      if ( i == 1 ) continue; 
      c = str.at(i);
      if ( c < '0' || c > '9' ){
        if ( c < 'a' || c > 'f' ){
          if ( c < 'A' || c > 'F' )
            return false;
        }
      }
    }
  } 
  else {
    for (int i = 0; i < str.length(); i++)
    {
      c = str.at(i);
      if ( c < '0' || c > '9' ) return false;
    }
  }
  
  return true;
}

// Treba da se koristi posle isLiteral() jer nema
// smisla proveravati nesto sto nije broj
bool Util::isHex(string str){
  if ( str.length() > 1 ) {
    return str.at(1) == 'x';
  }
  return false;
}

string Util::decToHex(unsigned dec){
  int val = 0;
  char c;
  string hex = "";

  while ( dec != 0 ){
    val = dec % 16;
    switch (val){
      case 0: c = '0'; break;
      case 1: c = '1'; break;
      case 2: c = '2'; break;
      case 3: c = '3'; break;
      case 4: c = '4'; break;
      case 5: c = '5'; break;
      case 6: c = '6'; break;
      case 7: c = '7'; break;
      case 8: c = '8'; break;
      case 9: c = '9'; break;
      case 10: c = 'a'; break;
      case 11: c = 'b'; break;
      case 12: c = 'c'; break;
      case 13: c = 'd'; break;
      case 14: c = 'e'; break;
      case 15: c = 'f'; break;
      default: c = '0';break;
    }
    hex = c + hex;
    dec /= 16;
  }

  if ( hex == "" ) return "0";
  return hex;
}

//12345
// out = 05
// 45 23 01

string Util::hexLittleEndian(string str){
  string out = "";
  int length = str.length();

  if (length % 2 == 1){
    str = '0' + str;
    length++;
  }

  for (int i = length - 1; i >= 0; i -= 2)
  {
    out += str.at(i - 1);
    out += str.at(i);
    if ( i > 1 ) out += ' ';
  }
  
  return out;
}

// Popunjava str sa nulama do 4B
// treba koristiti posle hexLittleEndian(string)
string Util::fillHex(string str){
  string out = "";
  int len = str.length();

  switch (len){ // aa vv bb ss
    case 2:  out = "00 00 00 " + str; break;
    case 5:  out = "00 00 " + str; break;
    case 8:  out = "00 " + str; break;
    case 11: out = str; break;
    default: out = "00 00 00 00"; break;
  }

  return out;
}

string Util::getOperandValue(string str){
  string val = "";
  int dec_val = 0;
  if (str.at(0) == '$'){ // neposredna vrednost 
    for (int i = 1; i < str.length(); i++) //uklanjanje $
    {
      val += str.at(i);
    }
    if (Util::isLiteral(val)){
      dec_val = Util::parseString(val); // za proveru decimalne vr od val
      val = Util::decToHex(dec_val);

      if (dec_val >= pow(2,12)){
        return "error";
      } else { // vraca se neposredna vrednost
        return Util::hexLittleEndian(val);
      }
    }
  }

  return val;
}

#endif