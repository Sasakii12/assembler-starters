//////////////////////////////////////////////////////////////////////////////
//
// File:         simpleCopy.c
// Date:         Tue 17 Feb 2026  16:34
// Author:       Ken Basye (from an earlier version by John Magee)
// Description:  Illustration of line-oriented I/O in C
//
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // for isspace()
#include <assert.h>
#include "map.h"

#define MAX_LINE 256

// create symbol table
typedef struct {
    Map symbolMap;
    int nextAddr;
} SymbolTable;

SymbolTable* createTable() {
    SymbolTable* table = (SymbolTable*) malloc(sizeof(SymbolTable));
    table->symbolMap = createMap(1000);
    table->nextAddr = 16; // since vars start at RAM[16]

    // initialize pre-defined symbols
    insertKey(table->symbolMap, "SP", "0");
    insertKey(table->symbolMap, "LCL", "1");
    insertKey(table->symbolMap, "ARG", "2");
    insertKey(table->symbolMap, "THIS", "3");
    insertKey(table->symbolMap, "THAT", "4");
    insertKey(table->symbolMap, "R0", "0");
    insertKey(table->symbolMap, "R1", "1");
    insertKey(table->symbolMap, "R2", "2");
    insertKey(table->symbolMap, "R3", "3");
    insertKey(table->symbolMap, "R4", "4");
    insertKey(table->symbolMap, "R5", "5");
    insertKey(table->symbolMap, "R6", "6");
    insertKey(table->symbolMap, "R7", "7");
    insertKey(table->symbolMap, "R8", "8");
    insertKey(table->symbolMap, "R9", "9");
    insertKey(table->symbolMap, "R10", "10");
    insertKey(table->symbolMap, "R11", "11");
    insertKey(table->symbolMap, "R12", "12");
    insertKey(table->symbolMap, "R13", "13");
    insertKey(table->symbolMap, "R14", "14");
    insertKey(table->symbolMap, "R15", "15");
    insertKey(table->symbolMap, "SCREEN", "16384");
    insertKey(table->symbolMap, "KBD", "24576");
    
    return table;
}

void freeSymbolTable(SymbolTable* table) {
    if (table) {
        freeMap(table->symbolMap);
        free(table);
    }
}

// Strip spaces and // comments from a string
void removeSpacesAndComments(char *dest, const char *src)
{
  unsigned i, j;
  i = 0;
  j = 0;
  while(src[i]!= '\0' && src[i] != '/')
  {
    if(isspace(src[i]))
    {
        i++;
    } else {
        // not a space, copy it
        dest[j] = src[i];
        i++;
        j++;
    }
  }
  // null terminate the string
  dest[j] = '\0';
}

// I saw in the canvas post that you asked for the output .hack file
// to be generated on its own with the assembler just needing the input.
// while trying to submit for the autograder I thought this was a requirement
// and ended up making this and refactoring some of the CLI code.
void outputFileName(const char *inp, char *out, size_t maxlen) {
    size_t len = strlen(inp);
    if (len > 4 && strcmp(inp + len - 4, ".asm") == 0) {
        snprintf(out, maxlen, "%.*s.hack",(int) (len - 4), inp);
    } else {
        snprintf(out, maxlen, "%s.hack", inp);
    }
}



int isNum(const char* str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }

    if (*str == '-') {
        str++;
    }

    while (*str) {
        if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}

int isLabel(const char* line) {
    return (line[0] == '(' && line[strlen(line) - 1] == ')');
}

void getLabel(const char* line, char* label) {
    int len = strlen(line);
    strncpy(label, line + 1, len - 2);
    label[len - 2] = '\0';
}

// Store first pass in function for cleaner code
void firstPass(FILE* file, SymbolTable* table, char line[], char editedLine[]) {
    int romAddr = 0;

    rewind(file);

    while (fgets(line, MAX_LINE, file) != NULL) {
        removeSpacesAndComments(editedLine, line);

        if (editedLine[0] != '\0') {
            if (isLabel(editedLine)) {
                char label[MAX_LINE];
                getLabel(editedLine, label);

                char addrStr[10];
                sprintf(addrStr, "%d", romAddr);
                insertKey(table->symbolMap, label, addrStr);
            } else {
                romAddr++;
            }
        }
    }
}


// Usage: simpleCopy inFile outFile
int main(int argc, char ** argv) {
    // Just fail if usage not correct
    assert(argc >= 2);

    const char *inFileName = argv[1];
    char outFileName[256];
        
    if (argc == 3) {
        strncpy(outFileName, argv[2], sizeof(outFileName));
        outFileName[sizeof(outFileName)-1] = '\0';
    } else {
        outputFileName(inFileName, outFileName, sizeof(outFileName));
    }
    Map compMap = createMap(200);
    Map destMap = createMap(200);
    Map jumpMap = createMap(200);

    // format: prefix + value + dest + jump

    // Comp instructions
    insertKey(compMap, "0", "0101010");
    insertKey(compMap, "1", "0111111");
    insertKey(compMap, "-1", "0111010");
    insertKey(compMap, "D", "0001100");
    insertKey(compMap, "A", "0110000");
    insertKey(compMap, "!D", "0001101");
    insertKey(compMap, "!A", "0110001");
    insertKey(compMap, "-D", "0001111");
    insertKey(compMap, "-A", "0110011");
    insertKey(compMap, "D+1", "0011111");
    insertKey(compMap, "1+D", "0011111");
    insertKey(compMap, "A+1", "0110111");
    insertKey(compMap, "1+A", "0110111");
    insertKey(compMap, "D-1", "0001110");
    insertKey(compMap, "A-1", "0110010");
    insertKey(compMap, "D+A", "0000010");
    insertKey(compMap, "D-A", "0010011");
    insertKey(compMap, "A-D", "0000111");
    insertKey(compMap, "D&A", "0000000");
    insertKey(compMap, "D|A", "0010101");

    // C instructions when a = 1
    insertKey(compMap,"M", "1110000");
    insertKey(compMap,"!M", "1110001");
    insertKey(compMap,"-M", "1110011");
    insertKey(compMap,"M+1", "1110111");
    insertKey(compMap,"1+M", "1110111");
    insertKey(compMap,"M-1", "1110010");
    insertKey(compMap,"D+M", "1000010");
    insertKey(compMap,"M+D", "1000010");
    insertKey(compMap,"D-M", "1010011");
    insertKey(compMap,"M-D", "1000111");
    insertKey(compMap,"D&M", "1000000");
    insertKey(compMap,"M&D", "1000000");
    insertKey(compMap,"D|M", "1010101");
    insertKey(compMap,"M|D", "1010101");

    // Dest instructions
    insertKey(destMap, "null", "000");
    insertKey(destMap, "M", "001");
    insertKey(destMap, "D", "010");
    insertKey(destMap, "MD", "011");
    insertKey(destMap, "A", "100");
    insertKey(destMap, "AM", "101");
    insertKey(destMap, "AD", "110");
    insertKey(destMap, "AMD", "111");

    // Jump instructions
    insertKey(jumpMap, "null", "000");
    insertKey(jumpMap, "JGT", "001");
    insertKey(jumpMap, "JEQ", "010");
    insertKey(jumpMap, "JGE", "011");
    insertKey(jumpMap, "JLT", "100");
    insertKey(jumpMap, "JNE", "101");
    insertKey(jumpMap, "JLE", "110");
    insertKey(jumpMap, "JMP", "111");

    // Just use fixed-size char arrays
    char line[MAX_LINE];
    char editedLine[MAX_LINE];

    // FILE is a system structure
    // Open the input file in read mode
    FILE * fin = fopen(inFileName, "r");
    if(fin == NULL) {
        // File must exist and be readable or fopen() will return NULL
        // Let system print a useful error message and exit the program
        perror("");
        exit(1);
    }

    SymbolTable* symTable = createTable();
    
    // First pass to collect all the labels
    firstPass(fin, symTable, line, editedLine);

    // Rewind to go back to the start for the second pass
    rewind(fin);

    FILE *fout;
    // Open second file for writing; this will replace the file if it exists
    fout = fopen(outFileName, "w");
    if(fout == 0) {
        perror("");
        exit(1);
    }

    // read the first line
    fgets(line, MAX_LINE,  fin);  
    // feof() returns 1 (true) if we have reached the end of file
    // feof() had to get changed to the following since it would never read the last line of the .asm files
    while(fgets(line, MAX_LINE, fin) != NULL) {
        removeSpacesAndComments(editedLine, line);

        // ONLY print lines that have text in them
        if(editedLine[0] != '\0' && !isLabel(editedLine)) {
            // Checks if the command is an A instruction
            if (editedLine[0] == '@') {
                char *numStr = editedLine + 1;

                if (isNum(numStr)) {
                    int value = atoi(numStr);        
                    char binary[17];                  
                    binary[0] = '0';                  

                    // Generates 15 bits
                    for (int i = 15; i >= 0; i--) {
                        binary[i] = (value & 1) ? '1' : '0';
                        value >>= 1;
                    }

                    binary[16] = '\0';
                    fputs(binary, fout);
                    fputs("\n", fout);
                } else { // Else it has to be a C instruction

                char* addrStr = lookupKey(symTable->symbolMap, numStr);

                // Allocate address for variable
                if (addrStr == NULL) {
                    char newAddr[10];

                    sprintf(newAddr, "%d", symTable->nextAddr);
                    insertKey(symTable->symbolMap, numStr, newAddr);

                    addrStr = newAddr;
                    symTable->nextAddr++;
                }
                
                int value = atoi(addrStr);
                char binary[17];

                for (int i = 15; i >= 0; i--) {
                    binary[15 - i] = ((value >> i) & 1) ? '1' : '0';
                }

                binary[16] = '\0';
                fputs(binary, fout);
                fputs("\n", fout);
                }
            } else {
                char destStr[10] = "null";
                char compStr[10] = "";
                char jumpStr[10] = "null";

                char *eq = strchr(editedLine, '=');
                char *semi = strchr(editedLine, ';');

                if (eq) {

                    int len = eq - editedLine;
                    strncpy(destStr, editedLine, len);
                    destStr[len] = '\0';
                    char * restOfEq = eq + 1;
                    if (semi && semi > eq) {
                        len = semi - restOfEq;
                        strncpy(compStr, restOfEq, len);
                        compStr[len] = '\0';
                        strcpy(jumpStr, semi + 1);
                    } 
                    else {
                        strcpy(compStr, restOfEq);
                    }
                } 
                else {
                    if (semi) {
                        int len = semi - editedLine;
                        strncpy(compStr, editedLine, len);
                        compStr[len] = '\0';
                        strcpy(jumpStr, semi + 1);
                    } 
                    else {
                        strcpy(compStr, editedLine);
                    }
                }

                // Look up the binary strings in the maps
                char *compBin = lookupKey(compMap, compStr);
                char *destBin = lookupKey(destMap, destStr);
                char *jumpBin = lookupKey(jumpMap, jumpStr);

                if (!compBin || !destBin || !jumpBin) {
                    fprintf(stderr, "Error: invalid C-instruction: %s\n", editedLine);
                    exit(1);
                }

                // Builds the 16-bit C-instruction
                char binary[17];
                sprintf(binary, "111%s%s%s", compBin, destBin, jumpBin);
                fputs(binary, fout);
                fputs("\n", fout);
                
            }
        }
    }

    

    // Make sure to close open files before exiting
    fclose(fin);
    fclose(fout);
    freeMap(compMap);
    freeMap(destMap);
    freeMap(jumpMap);
    freeSymbolTable(symTable);


    printf("Copied %s to %s\n", inFileName, outFileName);
    // A 0 indicates success
    return 0;
}
