#include <stdio.h>    /* standard input/output library */
#include <stdlib.h>    /* Standard C Library */
#include <string.h>    /* String operations library */
#include <ctype.h>    /* Library for useful character operations */
#include <limits.h>    /* Library for definitions of common variable type characteristics */

#define MAX_LINE_LENGTH 255
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255

enum {DONE, OK, EMPTY_LINE};
FILE* infile = NULL;
FILE* outfile = NULL;

typedef struct {
    int address;
    char label[MAX_LABEL_LEN + 1];    /* Question for the reader: Why do we need to add 1? */
} TableEntry;

int symbol_counter = 0;
TableEntry symbolTable[MAX_SYMBOLS];

int init_addr;

/*determines whether a string of characters is a valid opcode*/
/* ADD, AND, BR (all 8 variations), HALT, JMP, JSR, JSRR, LDB, LDW, 
    LEA, NOP, NOT, RET, LSHF, RSHFL, RSHFA, RTI, STB, STW, TRAP, XOR */
int isOpcode(char* pt){
	if(strcmp(pt, "add") == 0){return 1;}
	if(strcmp(pt, "and") == 0){return 1;}
	if(strcmp(pt, "br") == 0){return 1;}
	if(strcmp(pt, "brn") == 0){return 1;}
	if(strcmp(pt, "brz") == 0){return 1;}
	if(strcmp(pt, "brp") == 0){return 1;}
	if(strcmp(pt, "brnz") == 0){return 1;}
	if(strcmp(pt, "brnp") == 0){return 1;}
	if(strcmp(pt, "brzp") == 0){return 1;}
	if(strcmp(pt, "brnzp") == 0){return 1;}
	if(strcmp(pt, "halt") == 0){return 1;}
	if(strcmp(pt, "jmp") == 0){return 1;}
	if(strcmp(pt, "jsr") == 0){return 1;}
	if(strcmp(pt, "jsrr") == 0){return 1;}
	if(strcmp(pt, "ldb") == 0){return 1;}
	if(strcmp(pt, "ldw") == 0){return 1;}
	if(strcmp(pt, "lea") == 0){return 1;}
	if(strcmp(pt, "nop") == 0){return 1;}
	if(strcmp(pt, "not") == 0){return 1;}
	if(strcmp(pt, "ret") == 0){return 1;}
	if(strcmp(pt, "lshf") == 0){return 1;}
	if(strcmp(pt, "rshfl") == 0){return 1;}
	if(strcmp(pt, "rshfa") == 0){return 1;}
	if(strcmp(pt, "rti") == 0){return 1;}
	if(strcmp(pt, "stb") == 0){return 1;}
	if(strcmp(pt, "stw") == 0){return 1;}
	if(strcmp(pt, "trap") == 0){return 1;}
	if(strcmp(pt, "xor") == 0){return 1;}
  if(strcmp(pt, ".orig") == 0){return 1;}
  if(strcmp(pt, ".fill") == 0){return 1;}
  if(strcmp(pt, ".end") == 0){return 1;}
	return -1;
}

int toNum( char * pStr )
{
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   long int lNumLong;

   orig_pStr = pStr;
   if( *pStr == '#' )                /* decimal */
   { 
     pStr++;
     if( *pStr == '-' )            /* dec is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isdigit(*t_ptr))
       {
     printf("Error: invalid decimal operand, %s\n",orig_pStr);
     exit(4);
       }
       t_ptr++;
     }
     lNum = atoi(pStr);
     if (lNeg)
       lNum = -lNum;
 
     return lNum;
   }
   else if( *pStr == 'x' )    /* hex */
   {
     pStr++;
     if( *pStr == '-' )       /* hex is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isxdigit(*t_ptr))
       {
     printf("Error: invalid hex operand, %s\n",orig_pStr);
     exit(4);
       }
       t_ptr++;
     }
     lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
     lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
     if( lNeg )
       lNum = -lNum;
     return lNum;
   }
   else
   {
    printf( "Error: invalid operand, %s\n", orig_pStr);
    exit(4); 
    /*This has been changed from error code 3 to error code 4, see clarification 12 */
   }
}

int readAndParse(FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4){
  char * lRet, * lPtr;
  int i;
  if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) ){
    return( DONE );
  }

  for( i = 0; i < strlen( pLine ); i++ ){
    pLine[i] = tolower( pLine[i] );
  }
  /* convert entire line to lowercase */
  
  *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

  /* ignore the comments */
  lPtr = pLine;
  while( *lPtr != ';' && *lPtr != '\0' && *lPtr != '\n' ){
    lPtr++;
  }

  *lPtr = '\0';
  if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) { /*returns !NULL if EMPTY_LINE*/
    return( EMPTY_LINE );
  }

  if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ){ /* found a label */
    *pLabel = lPtr;
    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ){
      return( OK );
    }
  }

  *pOpcode = lPtr;
  if( !( lPtr = strtok( NULL, "\t\n ," ) ) ){
    return( OK );
  }

  *pArg1 = lPtr;
  if( !( lPtr = strtok( NULL, "\t\n ," ) ) ){
    return( OK );
  }

  *pArg2 = lPtr;
  if( !( lPtr = strtok( NULL, "\t\n ," ) ) ){
    return( OK );
  }

  *pArg3 = lPtr;
  if( !( lPtr = strtok( NULL, "\t\n ," ) ) ){
    return( OK );
  }

  *pArg4 = lPtr;
  return( OK );
} /* Note: MAX_LINE_LENGTH, OK, EMPTY_LINE, and DONE are defined values */

/*DEBUG FUNCTIONS*/
void printSymbolTable(TableEntry* symbolTable, int size){
  int i;
  for(i = 0; i < size; i++){
    printf("%s: %x\n", symbolTable[i].label, symbolTable[i].address);
  }
}

/*ERROR CHECKER HELPER FUNCTIONS*/
int containsNonAlnum(char* str){
  int i;
  for(i = 0; i < strlen(str); i++){
    if(isalnum(str[i]) == 0){
      return 1;
    }
  }
  return 0;
}

int isValidRegister(char* str){
  if(str[0] != 'r'){printf("INVALID ARGUMENT\n");exit(4);} 
  if(!(str[1] >= '0' && str[1] <= '7')){printf("INVALID REGISTER\n");exit(4);} /*between 0 and 7 (inclusive)*/
  if(str[2] != '\0'){printf("INVALID REGISTER\n");exit(4);}
}

int isimm6(char* str){
  int num;
  if(str[0] == '#' || str[0] == 'x'){
    num = toNum(str);
    if(num < -32 || num > 31){
      printf("INVALID CONSTANT\n");
      exit(3);
    }
    return 1;
  }
  else{
    printf("INVALID ARGUMENT\n");
    exit(3);
  }
}

int isimm5(char* str){
  int num;
  if(str[0] == '#' || str[0] == 'x'){
    num = toNum(str);
    if(num < -16 || num > 15){
      printf("INVALID CONSTANT\n");
      exit(3);
    }
    return 1;
  }
  else{
    return 0;
  }
}

int isamount4(char* str){
  int num;
  if(str[0] == '#' || str[0] == 'x'){
    num = toNum(str);
    if(num < 0 || num > 15){
      printf("INVALID CONSTANT\n");
      exit(3);
    }
    return 1;
  }
  else{
    printf("INVALID ARGUMENT\n");
    exit(3);
  }
}

int labelExists(char* str){
  int i;
/*printf("str: %s\n", str);*/
  for(i = 0; i < symbol_counter; i++){
/*printf("current label: %s\n", symbolTable[i].label);*/
    if(!strcmp(str, symbolTable[i].label)){
      return i;
    }
  }
  /*label does not exist*/
/*printf("i: %d, symbol_counter: %d\n", i, symbol_counter);*/
  if(i == symbol_counter){
    printf("UNDEFINED LABEL\n");
    exit(1);
  }
}

int isInRange(int offset, int range){
  int upperBound = range -1;
  int lowerBound = range * -1;
  if(offset < lowerBound || offset > upperBound){
    printf("OUT OF RANGE\n");
    exit(4);
  }
}

int isValidNumArg(int num, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  switch(num){
    case 0:
      if(*lArg1 != '\0'){printf("INVALID NUMBER OF ARGUMENTS\n");exit(4);}
    case 1:
      if(*lArg2 != '\0'){printf("INVALID NUMBER OF ARGUMENTS\n");exit(4);}
    case 2:
      if(*lArg3 != '\0'){printf("INVALID NUMBER OF ARGUMENTS\n");exit(4);}
    case 3:
      if(*lArg4 != '\0'){printf("INVALID NUMBER OF ARGUMENTS\n");exit(4);}
  }
  return 1;
}

/*Opcode Functions*/

void orig(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code;
  if(!strcmp(lOpcode, ".orig")){
    isValidNumArg(1, lArg1, lArg2, lArg3, lArg4);
    code = toNum(lArg1);
    fprintf(outfile, "0x%.4X\n", code);
  } 
}

/*TODO: test fill*/
void fill(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code;
  if(!strcmp(lOpcode, ".fill")){
    isValidNumArg(1, lArg1, lArg2, lArg3, lArg4);
    code = toNum(lArg1);
    if(code > 0xFFFF){printf("INVALID CONSTANT\n");exit(3);}
    fprintf(outfile, "0x%.4X\n", code);
  } 
}

/*TODO: arg after end?*/
int end(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  if(!strcmp(lOpcode, ".end")){
    return 1;
  }
  return 0;
}

void add(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code = 0x1000;
  if(!strcmp(lOpcode, "add")){                          /*check opcode name*/
    isValidNumArg(3, lArg1, lArg2, lArg3, lArg4);
    isValidRegister(lArg1);                             /*append 1st register*/
    code += atoi(&lArg1[1]) * 512;
    isValidRegister(lArg2);                             /*append 2nd register*/
    code += atoi(&lArg2[1]) * 64;
    if(isimm5(lArg3)){                                  /*check if imm5 or register*/
      if(toNum(lArg3) >= 0){code += 32 + toNum(lArg3);} /*check if imm5 val is positive or negative and append accordingly*/
      else{
        code += 32 + 32 + toNum(lArg3);}                /*+32 : 5-bit two's complement conversion*/
    } else{
      isValidRegister(lArg3);                           /*if not imm5 append 3rd register*/
      code += atoi(&lArg3[1]);
    }
    fprintf(outfile, "0x%.4X\n", code);                 /*output hexcode to file*/
  }
}

void and(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code = 0x5000;
  if(!strcmp(lOpcode, "and")){
    isValidNumArg(3, lArg1, lArg2, lArg3, lArg4);
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 512;
    isValidRegister(lArg2);
    code += atoi(&lArg2[1]) * 64;
    if(isimm5(lArg3)){
      if(toNum(lArg3) >= 0){code += 32 + toNum(lArg3);}
      else{
        code += 32 + 32 + toNum(lArg3);}
    } else{
      isValidRegister(lArg3);
      code += atoi(&lArg3[1]);
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void xor(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code = 0x9000;
  if(!strcmp(lOpcode, "xor")){
    isValidNumArg(3, lArg1, lArg2, lArg3, lArg4);
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 512;
    isValidRegister(lArg2);
    code += atoi(&lArg2[1]) * 64;
    if(isimm5(lArg3)){
      if(toNum(lArg3) >= 0){code += 32 + toNum(lArg3);}
      else{
        code += 32 + 32 + toNum(lArg3);
      }
    } else{
      isValidRegister(lArg3);
      code += atoi(&lArg3[1]);
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void nop(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code = 0x0000;
  if(!strcmp(lOpcode, "nop")){
    isValidNumArg(0, lArg1, lArg2, lArg3, lArg4);
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void not(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code = 0x903F;
  if(!strcmp(lOpcode, "not")){
    isValidNumArg(2, lArg1, lArg2, lArg3, lArg4);
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 512;
    isValidRegister(lArg2);
    code += atoi(&lArg2[1]) * 64;
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void lshf(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code = 0xD000;
  if(!strcmp(lOpcode, "lshf")){
    isValidNumArg(3, lArg1, lArg2, lArg3, lArg4);
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 512;
    isValidRegister(lArg2);
    code += atoi(&lArg2[1]) * 64;
    if(isamount4(lArg3)){
      code += toNum(lArg3);
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void rshfl(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code = 0xD000;
  if(!strcmp(lOpcode, "rshfl")){
    isValidNumArg(3, lArg1, lArg2, lArg3, lArg4);
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 512;
    isValidRegister(lArg2);
    code += atoi(&lArg2[1]) * 64;
    if(isamount4(lArg3)){
      code += 16 + toNum(lArg3);
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void rshfa(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code = 0xD000;
  if(!strcmp(lOpcode, "rshfa")){
    isValidNumArg(3, lArg1, lArg2, lArg3, lArg4);
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 512;
    isValidRegister(lArg2);
    code += atoi(&lArg2[1]) * 64;
    if(isamount4(lArg3)){
      code += 32 + 16 + toNum(lArg3);
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
}

/*TODO: test ldb*/
void ldb(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code = 0x2000;
  if(!strcmp(lOpcode, "ldb")){
    isValidNumArg(3, lArg1, lArg2, lArg3, lArg4);
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 512;
    isValidRegister(lArg2);
    code += atoi(&lArg2[1]) * 64;
    if(isimm6(lArg3)){
      if(toNum(lArg3) >= 0){
        code += toNum(lArg3);
      }
      else{
        code += 64 + toNum(lArg3);
      }
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
}

/*TODO: test ldw*/
void ldw(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code = 0x6000;
  int offset;
  if(!strcmp(lOpcode, "ldw")){
    isValidNumArg(3, lArg1, lArg2, lArg3, lArg4);
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 512;
    isValidRegister(lArg2);
    code += atoi(&lArg2[1]) * 64;
    if(isimm6(lArg3)){
      if(toNum(lArg3) >= 0){
        code += toNum(lArg3);
      }
      else{
        code += 64 + toNum(lArg3);
      }
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
}

/*TODO: test STB*/
void stb(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code = 0x3000;
  int offset;
  if(!strcmp(lOpcode, "stb")){
    isValidNumArg(3, lArg1, lArg2, lArg3, lArg4);
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 512;
    isValidRegister(lArg2);
    code += atoi(&lArg2[1]) * 64;
    if(isimm6(lArg3)){
      if(toNum(lArg3) >= 0){
        code += toNum(lArg3);
      }
      else{
        code += 64 + toNum(lArg3);
      }
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
}

/*TODO: test STW*/
void stw(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code = 0x7000;
  int offset;
  if(!strcmp(lOpcode, "stw")){
    isValidNumArg(3, lArg1, lArg2, lArg3, lArg4);
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 512;
    isValidRegister(lArg2);
    code += atoi(&lArg2[1]) * 64;
    if(isimm6(lArg3)){
      if(toNum(lArg3) >= 0){
        code += toNum(lArg3);
      }
      else{
        code += 64 + toNum(lArg3);
      }
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
}

/*test trap*/
void trap(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){                     
  int code = 0xF000;
  int num;
  if(!strcmp(lOpcode, "trap")){
    isValidNumArg(1, lArg1, lArg2, lArg3, lArg4);
    if(lArg1[0] != 'x'){printf("INVALID CONSTANT");exit(3);}
    num = toNum(lArg1);
    if(num < 0 || num > 255){
      printf("INVALID CONSTANT\n");
      exit(3);
    } else{
      code += num;
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void halt(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){         /*HALT equivalent to TRAP x25*/
  int code = 0xF025;
  if(!strcmp(lOpcode, "halt")){
    isValidNumArg(0, lArg1, lArg2, lArg3, lArg4);
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void jmp(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code = 0xC000;
  if(!strcmp(lOpcode, "jmp")){
    isValidNumArg(1, lArg1, lArg2, lArg3, lArg4);
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 64;
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void ret(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code = 0xC1C0;
  if(!strcmp(lOpcode, "ret")){
    isValidNumArg(0, lArg1, lArg2, lArg3, lArg4);
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void rti(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code = 0x8000;
  if(!strcmp(lOpcode, "rti")){
    isValidNumArg(0, lArg1, lArg2, lArg3, lArg4);
    fprintf(outfile, "0x%.4X\n", code);
  }
}

/*TODO: test jsr*/
void jsr(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4, int PC){
  int code = 0x4000;
  int label_index;
  int PCmod = PC + 2;
  int offset;

  if(!strcmp(lOpcode, "jsr")){
    isValidNumArg(1, lArg1, lArg2, lArg3, lArg4);
    code += 0x0800;                                                
    label_index = labelExists(lArg1);
    offset = (symbolTable[label_index].address - PCmod)/2;
    /*check in range*/
    isInRange(offset, 1024);
    if(offset >= 0){
      code += offset;
    } else{
      code += offset + 0x0800;    /*11-bit two's complement conversion*/
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
}

/*TODO: test jsrr*/
void jsrr(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4){
  int code = 0x4000;
  if(!strcmp(lOpcode, "jsrr")){
    isValidNumArg(1, lArg1, lArg2, lArg3, lArg4);
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 64;
    fprintf(outfile, "0x%.4X\n", code);
  }
}

/*TODO: test LEA*/
void lea(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4, int PC){
  int code = 0xE000;
  int label_index;
  int PCmod = PC + 2;
  int offset;

  if(!strcmp(lOpcode, "lea")){
    isValidNumArg(2, lArg1, lArg2, lArg3, lArg4);
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 512;
    label_index = labelExists(lArg2);
    offset = (symbolTable[label_index].address - PCmod)/2;
    isInRange(offset, 256);
    if(offset >= 0){
      code += offset;
    } else{
      code += offset + 0x0200;    /*0x0200 : 9-bit two's complement conversion*/
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
}

/*TODO: test statements*/
void br(char* lOpcode, char* lArg1, char* lArg2, char* lArg3, char* lArg4, int PC){
  int code = 0x0000;
  int label_index;
  int PCmod = PC + 2;
  int offset;

  if((!strcmp(lOpcode, "br")) || (!strcmp(lOpcode, "brnzp"))){
    isValidNumArg(1, lArg1, lArg2, lArg3, lArg4);
    code += 0x0E00;                                                 /*set nzp*/
    label_index = labelExists(lArg1);
/*printf("%x | %x\n", PCmod, symbolTable[label_index].address);*/
    offset = (symbolTable[label_index].address - PCmod)/2;
    /*check in range*/
    isInRange(offset, 256);
    if(offset >= 0){
      code += offset;
    } else{
      code += offset + 0x0200;    /*0x0200 : 9-bit two's complement conversion*/
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
  if(!strcmp(lOpcode, "brn")){
    isValidNumArg(1, lArg1, lArg2, lArg3, lArg4);
    code += 2048;
    label_index = labelExists(lArg1);
    offset = (symbolTable[label_index].address - PCmod)/2;
    /*check in range*/
    isInRange(offset, 256);
    if(offset >= 0){
      code += offset;
    } else{
      code += offset + 0x0200;    /*0x0200 : 9-bit two's complement conversion*/
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
  if(!strcmp(lOpcode, "brz")){
    isValidNumArg(1, lArg1, lArg2, lArg3, lArg4);
    code += 1024;
    label_index = labelExists(lArg1);
    offset = (symbolTable[label_index].address - PCmod)/2;
    /*check in range*/
    isInRange(offset, 256);
    if(offset >= 0){
      code += offset;
    } else{
      code += offset + 0x0200;    /*0x0200 : 9-bit two's complement conversion*/
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
  if(!strcmp(lOpcode, "brp")){
    isValidNumArg(1, lArg1, lArg2, lArg3, lArg4);
    code += 512;
    label_index = labelExists(lArg1);
    offset = (symbolTable[label_index].address - PCmod)/2;
    /*check in range*/
    isInRange(offset, 256);
    if(offset >= 0){
      code += offset;
    } else{
      code += offset + 0x0200;    /*0x0200 : 9-bit two's complement conversion*/
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
  if(!strcmp(lOpcode, "brnz")){
    isValidNumArg(1, lArg1, lArg2, lArg3, lArg4);
    code += 2048 + 1024;
    label_index = labelExists(lArg1);
    offset = (symbolTable[label_index].address - PCmod)/2;
    /*check in range*/
    isInRange(offset, 256);
    if(offset >= 0){
      code += offset;
    } else{
      code += offset + 0x0200;    /*0x0200 : 9-bit two's complement conversion*/
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
  if(!strcmp(lOpcode, "brnp")){
    isValidNumArg(1, lArg1, lArg2, lArg3, lArg4);
    code += 2048 + 512;
    label_index = labelExists(lArg1);
    offset = (symbolTable[label_index].address - PCmod)/2;
    /*check in range*/
    isInRange(offset, 256);
    if(offset >= 0){
      code += offset;
    } else{
      code += offset + 0x0200;    /*0x0200 : 9-bit two's complement conversion*/
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
  if(!strcmp(lOpcode, "brzp")){
    isValidNumArg(1, lArg1, lArg2, lArg3, lArg4);
    code += 1024 + 512;
    label_index = labelExists(lArg1);
    offset = (symbolTable[label_index].address - PCmod)/2;
    /*check in range*/
    isInRange(offset, 256);
    if(offset >= 0){
      code += offset;
    } else{
      code += offset + 0x0200;    /*0x0200 : 9-bit two's complement conversion*/
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
}

int main(int argc, char* argv[]) {

/*******************************OPEN FILES********************************/
printf("OPENING FILES...\n");
  infile = fopen(argv[1], "r");
  outfile = fopen(argv[2], "w");
     
  if (!infile) {
   printf("Error: Cannot open file %s\n", argv[1]);
   exit(4);
     }
  if (!outfile) {
   printf("Error: Cannot open file %s\n", argv[2]);
   exit(4);
  }

/*******************************FIRST PASS********************************/
printf("ENTERING FIRST PASS...\n");
  char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
	int lRet;
  int errCounter = 0;
  int PC;

	do{
		lRet = readAndParse(infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);

		if( lRet != DONE && lRet != EMPTY_LINE ){

/*printf("parse code#: %d\n", lRet);*/
/*printf("%s %s %s %s %s %s\n", lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);*/
/*printf("INITIAL ADDR: %d\n", init_addr);*/
		    /*fprintf(outfile, "0x%.4X\n", 128 );*/

      if(end(lOpcode, lArg1, lArg2, lArg3, lArg4)){
        break;
      }

      /*find starting address*/
      if(!strcmp(lOpcode, ".orig")){
        init_addr = toNum(lArg1);
        /*ERROR: check if orig mem is odd*/
        if(init_addr % 2 == 1){printf("STARTING MEM ODD\n");exit(3);}
        /*ERROR: check if orig mem is too large*/
        if(init_addr > 0xFFFF){printf("STARTING MEM TOO LARGE\n");exit(4);}
        PC = init_addr - 2;         /*increments when reading .orig line so -2 to correct*/
      }

      /*fill label names and addresses in symbol table*/
      if(isalnum(*lLabel) != 0){
        /*ERROR: check if label already exists*/
        for(errCounter = 0; errCounter < symbol_counter; errCounter++){
          if(!strcmp(lLabel, symbolTable[errCounter].label)){
            printf("LABEL ALREADY EXISTS\n");
            exit(4);
          }
        }
        strcpy(symbolTable[symbol_counter].label, lLabel);
        symbolTable[symbol_counter].address = PC;
        symbol_counter++;
      }

      /*increment PC*/
      PC = PC + 2;
    }
	} while( lRet != DONE );

  if(PC > 0xFFFF){printf("PROGRAM OVER MEMORY RANGE\n");exit(4);}

/*ERROR: check for invalid label*/
  for(errCounter = 0; errCounter < symbol_counter; errCounter++){
    if(isdigit(symbolTable[errCounter].label[0]) || symbolTable[errCounter].label[0] =='x'){
      printf("ERROR BEGINS WITH 'x' OR NUMBER\n");
      exit(4);
    }
    if(containsNonAlnum(symbolTable[errCounter].label)){
      printf("ERROR CONTAINS NON-ALPHANUMERIC CHARACTER\n");
      exit(4);
    }
    if(strcmp(symbolTable[errCounter].label, "in") == 0){printf("INVALID LABEL\n");exit(4);}
    if(strcmp(symbolTable[errCounter].label, "out") == 0){printf("INVALID LABEL\n");exit(4);}
    if(strcmp(symbolTable[errCounter].label, "getc") == 0){printf("INVALID LABEL\n");exit(4);}
    if(strcmp(symbolTable[errCounter].label, "puts") == 0){printf("INVALID LABEL\n");exit(4);}
  }

/*printSymbolTable(symbolTable, symbol_counter);*/

/*******************************SECOND PASS********************************/
printf("ENTERING SECOND PASS...\n");
rewind(infile);

PC = init_addr - 2; /*correction for counting orig*/

do{
  lRet = readAndParse(infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);

    if( lRet != DONE && lRet != EMPTY_LINE ){
/*printf("%x: %s\n", PC, lOpcode);
      /*ERROR: check valid opcode*/
      if(isOpcode(lOpcode) == -1){printf("INVALID OPCODE\n");exit(2);}

      /*ops*/
      add(lOpcode, lArg1, lArg2, lArg3, lArg4);
      and(lOpcode, lArg1, lArg2, lArg3, lArg4);
      br(lOpcode, lArg1, lArg2, lArg3, lArg4, PC);
      jmp(lOpcode, lArg1, lArg2, lArg3, lArg4);
      jsr(lOpcode, lArg1, lArg2, lArg3, lArg4, PC);
      jsrr(lOpcode, lArg1, lArg2, lArg3, lArg4);
      ldb(lOpcode, lArg1, lArg2, lArg3, lArg4);
      ldw(lOpcode, lArg1, lArg2, lArg3, lArg4);
      lea(lOpcode, lArg1, lArg2, lArg3, lArg4, PC);
      not(lOpcode, lArg1, lArg2, lArg3, lArg4);
      nop(lOpcode, lArg1, lArg2, lArg3, lArg4);
      ret(lOpcode, lArg1, lArg2, lArg3, lArg4);
      rti(lOpcode, lArg1, lArg2, lArg3, lArg4);
      lshf(lOpcode, lArg1, lArg2, lArg3, lArg4);
      rshfl(lOpcode, lArg1, lArg2, lArg3, lArg4);
      rshfa(lOpcode, lArg1, lArg2, lArg3, lArg4);
      stb(lOpcode, lArg1, lArg2, lArg3, lArg4);
      stw(lOpcode, lArg1, lArg2, lArg3, lArg4); 
      trap(lOpcode, lArg1, lArg2, lArg3, lArg4);
      xor(lOpcode, lArg1, lArg2, lArg3, lArg4);
      
      /*pseudo-ops*/
      orig(lOpcode, lArg1, lArg2, lArg3, lArg4);
      halt(lOpcode, lArg1, lArg2, lArg3, lArg4);
      fill(lOpcode, lArg1, lArg2, lArg3, lArg4);

      if(end(lOpcode, lArg1, lArg2, lArg3, lArg4)){
        break;
      }

      /*increment PC*/
      PC = PC + 2;
    }
  } while( lRet != DONE );


/*******************************FINISHING********************************/
printf("CLOSING FILES...\n");
     fclose(infile);
     fclose(outfile);

     char *prgName   = NULL;
     char *iFileName = NULL;
     char *oFileName = NULL;

     prgName   = argv[0];
     iFileName = argv[1];
     oFileName = argv[2];

     printf("program name = '%s'\n", prgName);
     printf("input file name = '%s'\n", iFileName);
     printf("output file name = '%s'\n", oFileName);

printf("EXITING...\n");
     exit(0);
}


