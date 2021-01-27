/**
 * Name: Project 1, Pass 1 through Assembly
 * Author: Hailey Francis, n01402670
 * Last Edited: 9/24/20
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 7 //including null token, MAXNAME - 1 ACCOUNTED FOR DURING CHECKS
#define MAX_LOCCTR 32768 //8000 hex
#define MAX_WORD 8388607 //2.3 Data Formats SIC Assembly guide -- 24 bits assumed signed with 2's complement -- MAX
#define MIN_WORD -8388608 //2.3 Data Formats SIC Assembly guide -- 24 bits assumed signed with 2's complement -- MIN
#define SYMTAB_SIZE 26

// OPCODE TABLE //
static const char *OPTAB[] = {
    "ADD","ADDF","ADDR","AND","CLEAR","COMP","COMPF",
    "COMPR","DIV","DIVF","DIVR","FIX","FLOAT","HIO",
    "J","JEQ","JGT","JLT","JSUB","LDA","LDB","STB",
    "LDF","LDL","LDS","LDT","LDX","LPS","MUL","MULF",
    "MULR","NORM","OR","RD","RMO","RSUB","SHIFTL",
    "SHIFTR","SIO","SSK","STA","STCH","STF","STI",
    "STL","STS","STSW","STT","STX","SUB","SUBF",
    "SUBR","SVC","TD","TIO","TIX","TIXR","WD","LDCH"
};

// SYMBOL STRUCTURE //
typedef struct Symbol{
    int address;
	int SourceLineDef;
    char Name[MAX_NAME];
	struct Symbol *next;
}Symbol;

// FUNCTION DECLARATIONS //
int letterHash(char symName[]);
int insert_toTable(Symbol *sym);
int validOpcode(char *check);
int isDirective(char *check);
int validSymbol(char *check);
int validHex(char *check);


Symbol *SYMTAB[26]; //SYMBOL TABLE (HASH TABLE)

// MAIN //
int main(int argc, char *argv[]){

	char line[1024]; //representing each line of file during read
    int LocCtr = 0; //location counter
	int sourceLine = 0; //line number in file
    FILE *inputFile; //name of file inputted
	FILE *symTabFile; //intermediate file
	char temp_label[MAX_NAME]; //temporary label
	char temp_opcode[MAX_NAME]; //temporary opcode
	char *temp_operand; // temporary operand
	int start; //used in calculating program length
	int programLength; // length of SIC program
	

	if(argc != 2) {
		printf("USAGE: %s <filename>\n", argv[0] );
		return 1;
	}

	inputFile = fopen( argv[1], "r"); //FILE OPEN TO READ
	
    if(!inputFile){
	    printf("ERROR: Could not open %s for reading\n", argv[1]);
	    return 1;
	}	

	symTabFile = fopen("SymTab.txt", "w"); //INTERMEDIATE FILE OPEN TO WRITE

	// FINDING START //
	while(fgets(line, 1024, inputFile)){ 
		sourceLine++; //I believe the comments before the code count as line numbers

		if(line[0] == 35){ //comment handling
			continue;
		}
		
		if ((line[0] >= 65) && (line[0] <= 90)){ //THIS LINE DEFINES a NEW SYMBOL!(with label)
			
			char *tokenTemp = strtok( line, " \t\n\r");

			if(validSymbol(tokenTemp) == 1){
				printf("ERROR: <line #%d> %s (label) is more than 6 characters.\n", sourceLine, tokenTemp);
				fclose(inputFile); //FILE CLOSE
				fclose(symTabFile);
				return 1;
			}else if(validSymbol(tokenTemp) == 2){
				printf("ERROR: <line #%d> Symbol %s has at least 1 invalid character.\n", sourceLine, temp_label);
				fclose(inputFile); //FILE CLOSE
				fclose(symTabFile);
				return 1;
			}else{
				strcpy(temp_label, tokenTemp);
				tokenTemp = strtok(NULL, " \t\n\r"); //next token
			}

			if(isDirective(temp_label) == 1){
				printf("ERROR: <line #%d> Cannot have label name be directive name! (%s)\n", sourceLine, temp_label);
				fclose(inputFile); //FILE CLOSE
				fclose(symTabFile);
				return 1;
			}
			
			if(strcmp(tokenTemp, "START") != 0){
				printf("ERROR: <line%d> no START directive.\n", sourceLine);
				fclose(inputFile); //FILE CLOSE
				fclose(symTabFile);
				return 1;
			}else{
				strcpy(temp_opcode, tokenTemp);
				tokenTemp = strtok(NULL, " \t\n\r"); //next token
			}

			temp_operand = malloc(sizeof(tokenTemp));
			strcpy(temp_operand, tokenTemp);
			tokenTemp = strtok(NULL, " \t\n\r"); //clear token

			int addressValid = validHex(temp_operand);
			if(addressValid == 0){
				printf("ERROR: <line #%d> Address at START is invalid hex. (%s)\n", sourceLine, temp_operand);
				fclose(inputFile);
				fclose(symTabFile);
				return 1;
			}

			LocCtr = strtol(temp_operand, NULL, 16);
			start = LocCtr; //for calculating length of program at end
			if(LocCtr > MAX_LOCCTR){
				printf("ERROR: <line #%d> Program exceeds capacity of SIC machine.\n", sourceLine);
				fclose(inputFile); //FILE CLOSE
				fclose(symTabFile);
			return 1;

			}

			// CREATING NEW SYMBOL FOR LABEL OF START //
			Symbol *tempSym;
			tempSym = malloc(sizeof(Symbol));
			strcpy(tempSym->Name, temp_label);
			tempSym->address = LocCtr;
			tempSym->SourceLineDef = sourceLine;
			tempSym->next = NULL;
			insert_toTable(tempSym); 

			printf("%s\t%x\n", tempSym->Name, tempSym->address);
			fprintf(symTabFile, "%s\t%x\n", tempSym->Name, tempSym->address); //print Symbol at START to intermediate file

			break; // get out of finding start

		} //end if statement defining if new symbol
	} //end while for FINDING START

	// CYCLE THROUGH REST OF FILE OR UNTIL TEMP OPCODE IS END //
    while(fgets(line, 1024, inputFile) && strcmp(temp_opcode, "END") != 0){ //start going line by line
		sourceLine++;

		if(line[0] == 35){ //comment handling
			continue;
		}

		else if ((line[0] >= 65) && (line[0] <= 90)){ // THIS DEFINES a NEW SYMBOL (label)! //
						
			char *tokenTemp = strtok( line, " \t\n\r");

			if(validSymbol(tokenTemp) == 1){
				printf("ERROR: <line #%d> Symbol (%s) is more than 6 characters.\n", sourceLine, tokenTemp);
				fclose(inputFile); //FILE CLOSE
				fclose(symTabFile);
				return 1;
			}else if(validSymbol(tokenTemp) == 2){
				printf("ERROR: <line #%d> At least 1 invalid character in Symbol (%s).\n", sourceLine, tokenTemp);
				fclose(inputFile);
				fclose(symTabFile);
				return 1;
			}else{
				strcpy(temp_label, tokenTemp); //printf("temp_label = %s\n", temp_label);
				tokenTemp = strtok(NULL, " \t\n\r"); //next token
			} 

			if(validOpcode(tokenTemp) == 0){
				if(isDirective(tokenTemp) == 0){
					printf("ERROR: <line #%d> Invalid opcode or directive (%s).", sourceLine, tokenTemp);
					fclose(inputFile);
					fclose(symTabFile);
					return 1;
				}
			}
			strcpy(temp_opcode, tokenTemp);
			tokenTemp = strtok(NULL, " \t\n\r"); //next token

			temp_operand = malloc(sizeof(tokenTemp));
			strcpy(temp_operand, tokenTemp); //printf("temp_operand is %s\n", temp_operand);
			tokenTemp = strtok(NULL, " \t\n\r"); //next token	

			if(isDirective(temp_label) == 1){
				printf("ERROR: <line #%d> Cannot have label name be directive name! (%s)\n", sourceLine, temp_label);
				fclose(inputFile); //FILE CLOSE
				fclose(symTabFile);
				return 1;
			}
			if(strcmp(temp_opcode, "START") == 0){
				printf("ERROR: <line #%d> Cannot repeat START directive.", sourceLine);
				fclose(inputFile);
				fclose(symTabFile);
				return 1;
			}
				
			// MAKING SYMBOL TO INSERT TO TABLE //
			Symbol *temp = malloc(sizeof(Symbol));
			temp->address = LocCtr;
			temp->SourceLineDef = sourceLine;
			strcpy(temp->Name, temp_label);
			temp->next = NULL;
			int insertCheck = insert_toTable(temp); //will still attemp insertion regardless. Will not insert if is duplicate
			if(insertCheck == 0){
				printf("ERROR: <line #%d> DUPLICATE SYMBOL DETECTED (%s)\n", sourceLine, temp->Name); //terminate if duplicate
				fclose(inputFile);
				fclose(symTabFile);
				return 1;
			}

			
			printf("%s\t%x\n", temp->Name,temp->address);
			fprintf(symTabFile, "%s\t%x\n", temp->Name,temp->address); //print Symbol to intermediate file 

		} //end of line[0] check A-Z

		else if(line[0] == 9){ //TAB, no symbol
			char *tokenTemp = strtok( line, " \t\n\r");

			if(validOpcode(tokenTemp) == 0){
				if(isDirective(tokenTemp) == 0){
					printf("ERROR: <line #%d> Invalid Opcode or directive (%s)\n", sourceLine, tokenTemp);
					fclose(inputFile); //FILE CLOSE
					fclose(symTabFile);
					return 1;
				}
			}
			strcpy(temp_opcode, tokenTemp);
			tokenTemp = strtok(NULL, " \t\n\r"); //next token
			
			if(tokenTemp != NULL){
				temp_operand = malloc(sizeof(tokenTemp));
				strcpy(temp_operand, tokenTemp);
				tokenTemp = strtok(NULL, " \t\n\r"); //next token	
			}	
		}

		else if(line[0] == 10 || line[0] == 13){ //Line Feed (10) on Windows, Carriage Return 13 on Linux
			printf("ERROR: <line #%d> Line empty.\n", sourceLine);
			fclose(inputFile);
			fclose(symTabFile);
			return 1;
		}

		else{
			printf("ERROR: <line #%d> Symbol starts with invalid character. (%c)\n", sourceLine, line[0]);
			fclose(inputFile);
			fclose(symTabFile);
			return 1;
		}

		if(LocCtr >= MAX_LOCCTR){
			printf("ERROR: <line #%d> Program exceeds capacity of SIC machine.\n", sourceLine);
			fclose(inputFile); //FILE CLOSE
			fclose(symTabFile);
			return 1;
		}

		// Incrementing Location Counter //
		if(validOpcode(temp_opcode) == 1){
			LocCtr += 3;
		}else if(strcmp(temp_opcode, "WORD") == 0){
			int operand = atoi(temp_operand); //no error checks for if this is invalid
			if(operand > MAX_WORD){
				printf("ERROR: <line #%d> WORD constant exceeds 24 bits (signed).[TOO LARGE]\n", sourceLine);
				fclose(inputFile);
				fclose(symTabFile);
				return 1;
			}else if(operand < MIN_WORD){
				printf("ERROR: <line #%d> WORD constant exceeds 24 bits (signed).[TOO SMALL]\n", sourceLine);
				fclose(inputFile);
				fclose(symTabFile);
				return 1;
			}else{
				LocCtr += 3;
			}
		}else if(strcmp(temp_opcode, "RESW") == 0){
			int operand = atoi(temp_operand); //no error checks for if this is invalid
			LocCtr =  LocCtr + (3 * operand);	
		}else if(strcmp(temp_opcode, "RESB") == 0){
			int operand = atoi(temp_operand); //no error checks for if this is invalid
			LocCtr += operand;
		}else if(strcmp(temp_opcode, "BYTE") == 0){
			if(temp_operand[0] == 67){ //Character constant (C)
				char *tempTokC = strtok(temp_operand, "C'");
				int length = strlen(tempTokC);
				LocCtr += length;
			}else if(temp_operand[0] == 88){ //Byte constant (X)
				char *tempTokX = strtok(temp_operand, "X'");
				int tokX_valid = validHex(tempTokX);
				if(tokX_valid == 0){
					printf("ERROR: <line #%d> Invalid byte constant. (%s)\n", sourceLine, tempTokX);
					fclose(inputFile);
					fclose(symTabFile);
					return 1;
				}else{
					int length = strlen(tempTokX);
					LocCtr += (length / 2);
				}
				
			}else{
				printf("ERROR: <line #%d> Improper operand syntax for BYTE. (%s)\n", sourceLine, temp_operand);
				fclose(inputFile);
				fclose(symTabFile);
				return 1;
			}
		}
	} //end going line by line
	if(strcmp(temp_opcode, "END") != 0){
		printf("ERROR: <line #%d> Program did not have END directive.\n", sourceLine);
		fclose(inputFile);
		fclose(symTabFile);
		return 1;
	}

	fclose(inputFile); //FILE CLOSE
	fclose(symTabFile);

	programLength = LocCtr - start;
	printf("The length of this program is %x\n", programLength);
	return 0;

} //end main	

/**
 * Function: letterHash
 * Parameters: char []
 * return: int
 * 
 * Creates a hash key based on the first char of symName[].
 * 
 */
int letterHash(char symName[]) {
	return symName[0] - 65;
} //end letterHash

/**
 * Function: insert_toTable
 * Parameters: Symbol*
 * return type: int
 * 
 * Adds Symbol from paramater into SYMTAB.
 * Returns 1 if successful. Returns 0 if error.
 * This code references the code shown in the 
 * Zoom meeting on Friday, 9/11/20.
 * 
 */
int insert_toTable(Symbol *sym){
	if (sym == NULL){
		printf("The symbol attempted to be inserted was invalid (NULL). Was not inserted.\n");
		return 0;
	}
	int key = letterHash(sym->Name);
	int found = 0;
	
	Symbol *curr = SYMTAB[key];

	if(SYMTAB[key] == NULL){
		SYMTAB[key] = sym;
		return 1;
	}

	while(curr != NULL && found == 0){
		if(strcmp(curr->Name, sym->Name) == 0){
			found = 1;
		}else{
			if(curr->next == NULL){
				curr->next = sym;
				break;
			}else{
				curr = curr->next;
			}
			
		}
	}

	if(found == 1){
		return 0;
	}
	return 1;
} //end insert_toTable

/**
 * Function: validOpcode
 * Parameters: char*
 * return type: int
 * 
 * Checks if opcode given in parameter is valid using 
 * the Opcode Table.
 * Returns 1 if valid. Returns 0 if invalid
 * 
 */
int validOpcode(char *check){
	for(int i = 0; i < 59; i++){
		if(strcmp(check, OPTAB[i]) == 0){
			return 1;
		}
	}
	return 0;
}

/**
 * Function: isDirective
 * Parameters: char*
 * return type: int
 * 
 * Checks whether or not given char is a directive.
 * This is based off code from lecture Thursday 9/10/20.
 * It wasn't directly gone over, but it was in the
 * file shown. This also includes EXPORTS and RESR, although
 * these were not included in the function shown in class.
 * Returns 1 if valid. Returns 0 if invalid.
 * 
 */
int isDirective(char *check){
	if(strcmp(check, "START") == 0){
		return 1;
	}else if(strcmp(check, "WORD") == 0){
		return 1;
	}else if(strcmp(check, "BYTE") == 0){
		return 1;
	}else if(strcmp(check, "RESW") == 0){
		return 1;
	}else if(strcmp(check, "RESB") == 0){
		return 1;
	}else if(strcmp(check, "RESR") == 0){
		return 1;
	}else if(strcmp(check, "EXPORTS") == 0){
		return 1;	
	}else if(strcmp(check, "END") == 0){
		return 1;
	}else{
		return 0;
	}
}

/**
 * Function: validSymbol
 * Parameters: char*
 * return type: int
 * 
 * Checks if given string is valid under SIC rules.
 * Returns 1 if length is too long. Returns 2 if contains
 * invalid symbol. Returns 0 if valid.
 * 
 */
int validSymbol(char *check){
	int length = strlen(check);

	// return 1 if longer than symbol max length //
	if(length > MAX_NAME - 1){
		return 1;
	}

	// return 2 if contains invalid symbol //
	// $ ! = (space) + - @ ( ) //
	for(int i = 0; i < length; i++){
		if(check[i] == 36 || check[i] == 33 || check[i] == 61 
			|| check[i] == 32 || check[i] == 43 || check[i] == 45
				|| check[i] == 64 || check[i] == 40 || check[i] == 41){
					return 2;
				}
	}
	
	return 0; //valid
}

/**
 * Function: validHex
 * Parameters: char*
 * return type: int
 * 
 * Checks whether or not the given string a valid hex.
 * (Does not calculate the value of the hex if valid).
 * Returns 1 if valid. Returns 0 if invalid.
 * 
 */
int validHex(char *check){
	int length = strlen(check);
	int valid;
	//48-57 is 1-9
	//65-70 is A-F
	//97-102 is a-f
	for(int i = 0; i < length; i++){
		if( check[i] == 48 || check[i] == 49 || check[i] == 50 
			|| check[i] == 51 || check[i] == 52 || check[i] == 53
				|| check[i] == 54 || check[i] == 55 || check[i] == 56 
					|| check[i] == 57 || check[i] == 65 || check[i] == 66 
						|| check[i] == 67 || check[i] == 68 || check[i] == 69 
							|| check[i] == 70 || check[i] == 97 || check[i] == 98
								|| check[i] == 99 || check[i] == 100 || check[i] == 101 || check[i] == 102){
									valid = 1;
		}else{
			valid = 0;
			return valid;
		}
	}
	return valid; //valid
}