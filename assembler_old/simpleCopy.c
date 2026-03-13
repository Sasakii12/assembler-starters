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


// Usage: simpleCopy inFile outFile
int main(int argc, char ** argv) {
    // Just fail if usage not correct
    assert(argc == 3);
    Map compMap = createMap(200);
    Map destMap = createMap(200);
    Map jumpMap = createMap(200);

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


    // format: prefix + value + dest + jump


    // FILE is a system structure
    // Open the input file in read mode
    FILE * fin = fopen(argv[1], "r");
    if(fin == NULL) {
        // File must exist and be readable or fopen() will return NULL
        // Let system print a useful error message and exit the program
        perror("");
        exit(1);
    }

    FILE *fout;
    // Open second file for writing; this will replace the file if it exists
    fout = fopen(argv[2], "w");
    if(fout == 0) {
        perror("");
        exit(1);
    }

    // Just use fixed-size char arrays
    char line[MAX_LINE];
    char editedLine[MAX_LINE];

    // read the first line
    fgets(line, MAX_LINE,  fin);  
    // feof() returns 1 (true) if we have reached the end of file
    while(!feof(fin)) {

        removeSpacesAndComments(editedLine, line);

        // ONLY print lines that have text in them
        if(editedLine[0] != '\0') {
            // Checks if its an A instruction
            if (editedLine[0] == '@') {

                
                char *numStr = editedLine + 1;   
                int value = atoi(numStr);        
                char binary[17];                  
                binary[0] = '0';                  

                
                // Generates 15 bits
                for (int i = 0; i < 15; i++) {
                    binary[i] = (value & 1) ? '1' : '0';
                    value >>= 1;
                }


                binary[16] = '\0';
                fputs(binary, fout);
                fputs("\n", fout);

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
                    } else {
                        strcpy(compStr, restOfEq);
                    }
                } else {
                    if (semi) {
                        int len = semi - editedLine;
                        strncpy(compStr, editedLine, len);
                        compStr[len] = '\0';
                        strcpy(jumpStr, semi + 1);
                    } else {
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
            // fputs(editedLine, fout);  // output
            // fputs("\n", fout);
        }
        fgets(line, MAX_LINE,  fin); // read the next one
    }





    












    

    // Make sure to close open files before exiting
    fclose(fin);
    fclose(fout);
    freeMap(compMap);
    freeMap(destMap);
    freeMap(jumpMap);


    printf("Copied %s to %s\n", argv[1], argv[2]);
    // A 0 indicates success
    return 0;
}
