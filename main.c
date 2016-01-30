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
TableEntry symbolTable[MAX_SYMBOLS];

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
  if(str[0] != 'r'){exit(4);} 
  if(!(str[1] >= '0' && str[1] <= '7')){exit(4);} /*between 0 and 7 (inclusive)*/
  if(str[2] != '\0'){exit(4);}
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
  else{return 0;}
}

int isimm4(char* str){
  int num;
  if(str[0] == '#' || str[0] == 'x'){
    num = toNum(str);
    if(num < 0 || num > 15){
      printf("INVALID CONSTANT\n");
      exit(3);
    }
    return 1;
  }
  else{return 0;}
}

/*Opcode Functions*/

void add(char* lOpcode, char* lArg1, char* lArg2, char* lArg3){
  int code = 0x1000;
  if(!strcmp(lOpcode, "add")){                          /*check opcode name*/
    isValidRegister(lArg1);                             /*append 1st register*/
    code += atoi(&lArg1[1]) * 512;
    isValidRegister(lArg2);                             /*append 2nd register*/
    code += atoi(&lArg2[1]) * 64;
    if(isimm5(lArg3)){                                  /*check if imm5 or register*/
      if(toNum(lArg3) >= 0){code += 32 + toNum(lArg3);} /*check if imm5 val is positive or negative and append accordingly*/
      else{
        code += 32 + 32 + toNum(lArg3);}
    } else{
      isValidRegister(lArg3);                           /*if not imm5 append 3rd register*/
      code += atoi(&lArg3[1]);
    }
    fprintf(outfile, "0x%.4X\n", code);                 /*output hexcode to file*/
  }
}

void and(char* lOpcode, char* lArg1, char* lArg2, char* lArg3){
  int code = 0x5000;
  if(!strcmp(lOpcode, "and")){
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

void xor(char* lOpcode, char* lArg1, char* lArg2, char* lArg3){
  int code = 0x9000;
  if(!strcmp(lOpcode, "xor")){
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

void nop(char* lOpcode){
  int code = 0x0000;
  if(!strcmp(lOpcode, "nop")){
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void not(char* lOpcode, char* lArg1, char* lArg2){
  int code = 0x903F;
  if(!strcmp(lOpcode, "not")){
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 512;
    isValidRegister(lArg2);
    code += atoi(&lArg2[1]) * 64;
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void lshf(char* lOpcode, char* lArg1, char* lArg2, char* lArg3){
  int code = 0xD000;
  if(!strcmp(lOpcode, "lshf")){
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 512;
    isValidRegister(lArg2);
    code += atoi(&lArg2[1]) * 64;
    if(isimm4(lArg3)){
      code += toNum(lArg3);
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void rshfl(char* lOpcode, char* lArg1, char* lArg2, char* lArg3){
  int code = 0xD000;
  if(!strcmp(lOpcode, "rshfl")){
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 512;
    isValidRegister(lArg2);
    code += atoi(&lArg2[1]) * 64;
    if(isimm4(lArg3)){
      code += 16 + toNum(lArg3);
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void rshfa(char* lOpcode, char* lArg1, char* lArg2, char* lArg3){
  int code = 0xD000;
  if(!strcmp(lOpcode, "rshfa")){
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 512;
    isValidRegister(lArg2);
    code += atoi(&lArg2[1]) * 64;
    if(isimm4(lArg3)){
      code += 32 + 16 + toNum(lArg3);
    }
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void trap(char* lOpcode, char* lArg1){                      /*??Trap vector should be hex number??*/
  int code = 0xF000;
  int num;
  if(!strcmp(lOpcode, "trap")){
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

void halt(char* lOpcode){         /*HALT equivalent to TRAP x25*/
  int code = 0xF025;
  if(!strcmp(lOpcode, "halt")){
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void jmp(char* lOpcode, char* lArg1){
  int code = 0xC000;
  if(!strcmp(lOpcode, "jmp")){
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 64;
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void ret(char* lOpcode){
  int code = 0xC1C0;
  if(!strcmp(lOpcode, "ret")){
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void rti(char* lOpcode){
  int code = 0x8000;
  if(!strcmp(lOpcode, "rti")){
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void jsrr(char* lOpcode, char* lArg1){
  int code = 0x4000;
  if(!strcmp(lOpcode, "jmp")){
    isValidRegister(lArg1);
    code += atoi(&lArg1[1]) * 64;
    fprintf(outfile, "0x%.4X\n", code);
  }
}

void br(char* lOpcode, char* lArg1){
  int code = 0x0000;

  if((!strcmp(lOpcode, "br")) || (!strcmp(lOpcode, "brnzp"))){
    code += 2048 + 1024 + 512;
    fprintf(outfile, "0x%.4X\n", code);
  }
  if(!strcmp(lOpcode, "brn")){
    code += 2048;
    fprintf(outfile, "0x%.4X\n", code);
  }
  if(!strcmp(lOpcode, "brz")){
    code += 1024;
    fprintf(outfile, "0x%.4X\n", code);
  }
  if(!strcmp(lOpcode, "brp")){
    code += 512;
    fprintf(outfile, "0x%.4X\n", code);
  }
  if(!strcmp(lOpcode, "brnz")){
    code += 2048 + 1024;
    fprintf(outfile, "0x%.4X\n", code);
  }
  if(!strcmp(lOpcode, "brnp")){
    code += 2048 + 512;
    fprintf(outfile, "0x%.4X\n", code);
  }
  if(!strcmp(lOpcode, "brzp")){
    code += 1024 + 512;
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
  int symbol_counter = 0, errCounter = 0;
  int init_addr;
  int PC;

	do{
		lRet = readAndParse(infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);

		if( lRet != DONE && lRet != EMPTY_LINE ){
/*
printf("parse code#: %d\n", lRet);
printf("%s %s %s %s %s %s\n", lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);*/
/*printf("INITIAL ADDR: %d\n", init_addr);*/
		    /*fprintf(outfile, "0x%.4X\n", 128 );*/

      /*find starting address*/
      if(!strcmp(lOpcode, ".orig")){
        init_addr = toNum(lArg1);
        /*ERROR: check if orig mem is odd*/
        if(init_addr % 2 == 1){printf("STARTING MEM ODD\n");exit(3);}
        PC = init_addr - 2;         /*increments when reading .orig line so -2 to correct*/
      }

      /*fill label names and addresses in symbol table*/
      if(isalnum(*lLabel) != 0){
        /*ERROR: check if label already exists*/
        for(errCounter = 0; errCounter < symbol_counter; errCounter++){
          if(!strcmp(lLabel, symbolTable[errCounter].label)){
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
    if(strcmp(symbolTable[errCounter].label, "in") == 0){exit(4);}
    if(strcmp(symbolTable[errCounter].label, "out") == 0){exit(4);}
    if(strcmp(symbolTable[errCounter].label, "getc") == 0){exit(4);}
    if(strcmp(symbolTable[errCounter].label, "puts") == 0){exit(4);}
  }

/*
printf("%s, 0x%.4X\n", symbolTable[0].label, symbolTable[0].address);
printf("%s, 0x%.4X\n", symbolTable[1].label, symbolTable[1].address);
printf("%s, 0x%.4X\n", symbolTable[2].label, symbolTable[2].address);
printf("%s, 0x%.4X\n", symbolTable[3].label, symbolTable[3].address);
*/

/*******************************SECOND PASS********************************/
printf("ENTERING SECOND PASS...\n");
rewind(infile);

do{
  lRet = readAndParse(infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);

    if( lRet != DONE && lRet != EMPTY_LINE ){
/*printf("%s\n", lOpcode);*/
      /*ERROR: check valid opcode*/
      if(isOpcode(lOpcode) == -1){printf("INVALID OPCODE\n");exit(2);}
      
      add(lOpcode, lArg1, lArg2, lArg3);
      and(lOpcode, lArg1, lArg2, lArg3);
      xor(lOpcode, lArg1, lArg2, lArg3);
      not(lOpcode, lArg1, lArg2);
      nop(lOpcode);
      lshf(lOpcode, lArg1, lArg2, lArg3);
      rshfl(lOpcode, lArg1, lArg2, lArg3);
      rshfa(lOpcode, lArg1, lArg2, lArg3);
      trap(lOpcode, lArg1);
      halt(lOpcode);
      jmp(lOpcode, lArg1);
      ret(lOpcode);
      rti(lOpcode);
      jsrr(lOpcode, lArg1);
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
}
