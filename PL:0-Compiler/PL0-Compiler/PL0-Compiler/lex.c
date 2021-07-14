//
//  lex.c
//
//  Authors: Carolina Santos and Jacob Campbell
//


#include "compiler.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

// Constants
#define CHAR_ARRAY_LENGTH 1000
#define MAX_LENGTH 11
#define RESERVED_WORDS 15

char reservedWords[15][10] = {"const", "var", "procedure", "call", "if", "then", "else", "while", "do", "read", "write", "odd", "return", "begin", "end"};

lexeme* lex_analyze(char *inputfile, int print)
{
	lexeme *list = malloc(CHAR_ARRAY_LENGTH * sizeof(lexeme));
    
    // Initial values
    int index = 0;
    int tokenListLength = 0;
    
    // Output heading
    if (print == 1) {
        printf("Lexeme Table:");
        printf("\n\tlexeme          token type\n");
    }
    
    // Loops until the end of the char array
    while(index < CHAR_ARRAY_LENGTH) {
        // Initial values
        int error = 0;
        int comment = 0;
        int value = -1;
        // Declarations
        char * name;
        lexeme lexeme;
        
        // Allocate memory for lexeme and token.identifier
        name  = (char*)malloc(MAX_LENGTH * sizeof(char));
        lexeme.name = (char*)malloc(MAX_LENGTH * sizeof(char));

        // If a character is a letter
        if (isalpha(inputfile[index]) != 0) {
            // Initialize buffer
            int buffer = 0;
            name[buffer] = inputfile[index];
            buffer++;
            // Loops through until character is neither a letter or digit
            while(isalpha(inputfile[index + buffer]) != 0 ||
                isdigit(inputfile[index + buffer]) != 0) {
                // Store characters in lexeme and increment buffer
                name[buffer] = inputfile[index + buffer];
                buffer++;
            }
                    
            // If buffer > 11, then invalid identifier results in error
            if (buffer > 11) {
                printf("Error : Identifier names cannot exceed 11 characters\n");
            }
            else {
                for(int i = 0 ; i < RESERVED_WORDS; i++) {
                    // If lexeme matches a reserved word,
                    if (strcmp(name, reservedWords[i]) == 0) {
                        // Assign reserved word's associated index to value
                        value = i;
                    }
                }
                // Switch statement depending on value
                switch(value) {
                    case 0: {
                        strcpy(name, "const");
                        lexeme.type = constsym;
                        break;
                    }
                    case 1: {
                        strcpy(name, "var");
                        lexeme.type = varsym;
                        break;
                    }
                    case 2: {
                        strcpy(name, "procedure");
                        lexeme.type = procsym;
                        break;
                    }
                    case 3: {
                        strcpy(name, "call");
                        lexeme.type = callsym;
                        break;
                    }
                    case 4: {
                        strcpy(name, "if");
                        lexeme.type = ifsym;
                        break;
                    }
                    case 5: {
                        strcpy(name, "then");
                        lexeme.type = thensym;
                        break;
                    }
                    case 6: {
                        strcpy(name, "else");
                        lexeme.type = elsesym;
                        break;
                    }
                    case 7: {
                        strcpy(name, "while");
                        lexeme.type = whilesym;
                        break;
                    }
                    case 8: {
                        strcpy(name, "do");
                        lexeme.type = dosym;
                        break;
                    }
                    case 9: {
                        strcpy(name, "read");
                        lexeme.type = readsym;
                        break;
                    }
                    case 10: {
                        strcpy(name, "write");
                        lexeme.type = writesym;
                        break;
                    }
                    case 11: {
                        strcpy(name, "odd");
                        lexeme.type = oddsym;
                        break;
                    }
                    case 12: {
                        strcpy(name, "return");
                        lexeme.type = returnsym;
                        break;
                    }
                    case 13: {
                        strcpy(name, "begin");
                        lexeme.type = beginsym;
                        break;
                        }
                    case 14: {
                        strcpy(name, "end");
                        lexeme.type = endsym;
                        break;
                    }
                    default: {
                        // If does not match a reserved word, it must be an identifier
                        lexeme.type = identsym;
                        lexeme.name = name;
                    }
                }
                
                // Print lexeme name and type
                if(print == 1) {
                    printf("\t%s ", name);
                    printf("\t\t%d\n", lexeme.type);
                }
                
                // Add token to struct array tokenList
                list[tokenListLength] = lexeme;
                tokenListLength++;
            }
            // Increment index by buffer to skip over characters
            index += buffer;
        }
        // If a charcter is a digit
        else if (isdigit(inputfile[index]) != 0) {
            // Initialize buffer
            int buffer = 0;
            name[buffer] = inputfile[index];
            buffer++;
            // Loops until character is not a digit
            while(isdigit(inputfile[index + buffer]) != 0) {
                name[buffer] = inputfile[index + buffer];
                buffer++;
            }
            // If buffer > 5, then invalid digit results in error
            if (buffer > 5) {
                printf("Error : Numbers cannot exceed 5 digits\n");
                // Checks if any remaining letters or digits exist attached to digits
                while(isalpha(inputfile[index + buffer]) != 0 || isdigit(inputfile[index + buffer]) != 0) {
                    // Store characters in lexeme and increment buffer
                    name[buffer] = inputfile[index + buffer];
                    buffer++;
                }
            }
            else if (isalpha(inputfile[index + buffer]) != 0) {
                // Results in error
                printf("Error : Identifiers cannot begin with a digit\n");
                // Checks if any remaining letters or digits exist attached to digits
                while(isalpha(inputfile[index + buffer]) != 0 || isdigit(inputfile[index + buffer]) != 0) {
                    // Store characters in lexeme and increment buffer
                    name[buffer] = inputfile[index + buffer];
                    buffer++;
                }
            }
            else {
                // Must be a valid number
                lexeme.type = numbersym;
                lexeme.value = atoi(name);
                // Print lexeme and token.val
                if(print == 1) {
                    printf("\t%d ", lexeme.value);
                    printf("\t\t%d\n", lexeme.type);
                }
                
                // Add token to struct array tokenList
                list[tokenListLength] = lexeme;
                tokenListLength++;
            }
            // Increment index by buffer to skip over characters
            index += buffer;
        }
        // If character is a control character or whitespace
        else if (iscntrl(inputfile[index]) != 0 || isspace(inputfile[index]) != 0) {
            index++;
        }
        else {
            // Switch statement depending on symbol
            switch(inputfile[index]) {
                case '%': {
                    strcpy(name, "%");
                    lexeme.type = modsym;
                    break;
                }
                case '+': {
                    strcpy(name, "+");
                    lexeme.type = plussym;
                    break;
                }
                case '-': {
                    strcpy(name, "-");
                    lexeme.type = minussym;
                    break;
                }
                case '*': {
                    strcpy(name, "*");
                    lexeme.type = multsym;
                    break;
                }
                case '=': {
                    strcpy(name, "=");
                    lexeme.type = eqlsym;
                    break;
                }
                case '(': {
                    strcpy(name, "(");
                    lexeme.type = lparentsym;
                    break;
                }
                case ')': {
                    strcpy(name, ")");
                    lexeme.type = rparentsym;
                    break;
                }
                case ',': {
                    strcpy(name, ",");
                    lexeme.type = commasym;
                    break;
                }
                case '.': {
                    strcpy(name, ".");
                    lexeme.type = periodsym;
                    break;
                }
                case ';': {
                    strcpy(name, ";");
                    lexeme.type = semicolonsym;
                    break;
                }
                case '/': {
                    // Checks if it's the beginning of a comment
                    if(inputfile[index + 1] == '*') {
                        index++;
                        while(inputfile[index + 1] != '*' && inputfile[index + 2] != '/'){
                            index++;
                        }
                        index += 2;
                        // Indicates that it is a comment
                        comment = 1;
                    }
                    // Otherwise, character is just '/'
                    else {
                        strcpy(name, "/");
                        lexeme.type = slashsym;
                    }
                    break;
                }
                case '<': {
                    if(inputfile[index + 1] == '>') {
                        strcpy(name, "<>");
                        lexeme.type = neqsym;
                        index++;
                    }
                    else if (inputfile[index + 1] == '=') {
                        strcpy(name, "<=");
                        lexeme.type = leqsym;
                        index++;
                    }
                    else {
                        strcpy(name, "<");
                        lexeme.type = lessym;
                    }
                    index++;
                    break;
                }
                case '>': {
                    if(inputfile[index + 1] == '=') {
                        strcpy(name, ">=");
                        lexeme.type = geqsym;
                        index++;
                    }
                    else {
                        strcpy(name, ">");
                        lexeme.type = gtrsym;
                    }
                    index++;
                    break;
                }
                case ':': {
                    // Must have an '=' sign after
                    if(inputfile[index + 1] == '=') {
                        strcpy(name, ":=");
                        lexeme.type = becomessym;
                        index++;
                    }
                    // Otherwise invalid symbol
                    else {
                        // Indicates an error exists
                        error = 1;
                        printf("Error : Invalid Symbol\n");
                    }
                    break;
                }
                default: {
                    // Indicates an error exists
                    error = 1;
                    printf("Error : Invalid Symbol\n");
                }
            }
            // If no errors nor comments exist
            if (error == 0 && comment == 0) {
                // Print lexeme and token
                if(print == 1) {
                    printf("\t%s ", name);
                    printf("\t\t%d\n", lexeme.type);
                }
                // Add token to struct array tokenList
                list[tokenListLength] = lexeme;
                tokenListLength++;
            }
            // Increment index
            index++;
        }
    }
    
    // Prints final tokenList array
    if(print == 1) {
        printf("\nToken List:\n");
        for (int i = 0; i < tokenListLength; i++) {
            printf("%d ", list[i].type);
            if(list[i].type == identsym ){
            printf("%s ", list[i].name);
            }
            if(list[i].type == numbersym){
                printf("%d ", list[i].value);
            }
        }
        printf("\n\n");
    }
	return list;
}
