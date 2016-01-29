#include <stdio.h>    /* standard input/output library */
#include <stdlib.h>    /* Standard C Library */
#include <string.h>    /* String operations library */
#include <ctype.h>    /* Library for useful character operations */
#include <limits.h>    /* Library for definitions of common variable type characteristics */

#define MAX_LINE_LENGTH 255

enum {DONE, OK, EMPTY_LINE};
FILE* infile = NULL;
FILE* outfile = NULL;

typedef struct Symbol{
	char* name;
	int memAddr;
} Symbol;


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
}

    /* Note: MAX_LINE_LENGTH, OK, EMPTY_LINE, and DONE are defined values */

int main(int argc, char* argv[]) {

	/* open the source file */
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

     /* Do stuff with files */
  struct Symbol symbol_table[10000];
  int i;
  for(i = 0; i < 10000; i++){
    symbol_table[i].name = NULL;
    symbol_table[i].memAddr = -1;
  }
  int symbol_counter = 0;
  /*
    Figure out how to manipulate array of structs, implement symbol table
  */
	char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
	int lRet;

	do{
		lRet = readAndParse(infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);

		if( lRet != DONE && lRet != EMPTY_LINE ){

/*
printf("parsed: %d\n", lRet);
printf("%s %s %s %s %s %s\n", lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
*/
printf("OUTSIDE: %s\n", lLabel);
		    /*fprintf(outfile, "0x%.4X\n", 128 );*/
      if(lLabel != NULL || *lLabel != '\n' || lLabel != "\0"){
printf("ENTERED: %s\n", lLabel);
fprintf(outfile, "%c\n", *lLabel);
        symbol_table[symbol_counter].name = (char*)malloc(strlen(lLabel) * sizeof(char));
        strcpy(symbol_table[symbol_counter].name, lLabel);
        symbol_counter++;
      }
    }
	} while( lRet != DONE );

/*
printf("%s\n", symbol_table[0].name);
printf("%s\n", symbol_table[1].name);
printf("%s\n", symbol_table[2].name);
*/

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

int main1(int argc, char const *argv[])
{
  char str1[50] = "";
  char str2[10] = "\0";
  char str3[10] = "  ";
  printf("%s\n", str1);
  printf("%s\n", str2);
  printf("%s\n", str3);

  return 0;
}
