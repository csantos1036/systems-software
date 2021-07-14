//
//  lex.c
//
//  Authors: Carolina Santos and Jacob Campbell
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


// Constants
#define MAX_LENGTH 11
#define DIGITS_LENGTH 5
#define RESERVED_WORDS 15
#define RESERVED_LENGTH 10
#define CHAR_ARRAY_LENGTH 500

// Struct for tokens
typedef struct token {
    int val;
    char * identifier;
    char * digit;
} token;

// Declaration of token types: 34 symbols
typedef enum { modsym = 1, identsym, numbersym, plussym, minussym, multsym, slashsym, oddsym, eqsym, neqsym, lessym, leqsym,gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym, periodsym, becomessym, beginsym, endsym, ifsym, thensym, whilesym, dosym, callsym, constsym, varsym, procsym, writesym, readsym, elsesym, returnsym } token_type;

// Declaration of reserved words: 15 words
char reservedWords[RESERVED_WORDS][RESERVED_LENGTH] = {"const", "var", "procedure", "call", "if", "then", "else", "while", "do", "read", "write", "odd", "return", "begin", "end"};

// Beginning of main function
int main(int argc, const char * argv[]) {
    // Initial values
    int end = 0;
    int index = 0;
    int length = 0;
    int tokenListLength = 0;
    
    // Declaration of a char arrray to copy input
    char * ch;
    // Declaration of a struct array of tokens to store tokens
    token * tokenList;
    
    // Allocate memory for ch array and tokenList array
    ch = (char*)malloc(CHAR_ARRAY_LENGTH * sizeof(char));
    tokenList =(token*)malloc(CHAR_ARRAY_LENGTH * sizeof(token));
    
    // Read input file
    FILE *f = fopen(argv[1], "r");
    
    // Output heading
    fprintf(stdout, "Lexeme Table:");
    fprintf(stdout, "\n\tlexeme   token type\n");
    
    // Copy contents of input file into char array
    while (!feof(f)) {
        ch[length] = fgetc(f);
        length++;
    }
    
    // Marks the end of the char array
    end = length - 1;
    
    // Loops until the end of the char array
    while(index < end) {
        // Initial values
        int error = 0;
        int comment = 0;
        int value = -1;
        // Declarations
        char * lexeme;
        token token;
        
        // Allocate memory for lexeme, token.identifier, and token.digit (char arrays)
        lexeme  = (char*)malloc(MAX_LENGTH * sizeof(char));
        token.identifier = (char*)malloc(MAX_LENGTH * sizeof(char));
        token.digit = (char*)malloc(MAX_LENGTH * sizeof(char));

        // If a character is a letter
        if (isalpha(ch[index]) != 0) {
            // Initialize buffer
            int buffer = 0;
            lexeme[buffer] = ch[index];
            buffer++;
            // Loops through until character is neither a letter or digit
            while(isalpha(ch[index + buffer]) != 0 ||
                isdigit(ch[index + buffer]) != 0) {
                // Store characters in lexeme and increment buffer
                lexeme[buffer] = ch[index + buffer];
                buffer++;
            }
                    
            // If buffer > 11, then invalid identifier results in error
            if (buffer > 11) {
                fprintf(stdout, "Error : Identifier names cannot exceed 11 characters\n");
            }
            else {
                for(int i = 0 ; i < RESERVED_WORDS; i++) {
                    // If lexeme matches a reserved word,
                    if (strcmp(lexeme, reservedWords[i]) == 0) {
                        // Assign reserved word's associated index to value
                        value = i;
                    }
                }
                // Switch statement depending on value
                switch(value) {
                    case 0: {
                        strcpy(lexeme, "const");
                        token.val = constsym;
                        break;
                    }
                    case 1: {
                        strcpy(lexeme, "var");
                        token.val = varsym;
                        break;
                    }
                    case 2: {
                        strcpy(lexeme, "procedure");
                        token.val = procsym;
                        break;
                    }
                    case 3: {
                        strcpy(lexeme, "call");
                        token.val = callsym;
                        break;
                    }
                    case 4: {
                        strcpy(lexeme, "if");
                        token.val = ifsym;
                        break;
                    }
                    case 5: {
                        strcpy(lexeme, "then");
                        token.val = thensym;
                        break;
                    }
                    case 6: {
                        strcpy(lexeme, "else");
                        token.val = elsesym;
                        break;
                    }
                    case 7: {
                        strcpy(lexeme, "while");
                        token.val = whilesym;
                        break;
                    }
                    case 8: {
                        strcpy(lexeme, "do");
                        token.val = dosym;
                        break;
                    }
                    case 9: {
                        strcpy(lexeme, "read");
                        token.val = readsym;
                        break;
                    }
                    case 10: {
                        strcpy(lexeme, "write");
                        token.val = writesym;
                        break;
                    }
                    case 11: {
                        strcpy(lexeme, "odd");
                        token.val = oddsym;
                        break;
                    }
                    case 12: {
                        strcpy(lexeme, "return");
                        token.val = returnsym;
                        break;
                    }
                    case 13: {
                        strcpy(lexeme, "begin");
                        token.val = beginsym;
                        break;
                        }
                    case 14: {
                        strcpy(lexeme, "end");
                        token.val = endsym;
                        break;
                    }
                    default: {
                        // If does not match a reserved word, it must be an identifier
                        token.val = identsym;
                        token.identifier = lexeme;
                    }
                }
                
                // Print lexeme and token
                fprintf(stdout, "\t%s ", lexeme);
                fprintf(stdout, "\t\t%d\n", token.val);
                
                // Add token to struct array tokenList
                tokenList[tokenListLength] = token;
                tokenListLength++;
            }
            // Increment index by buffer to skip over characters
            index += buffer;
        }
        // If a charcter is a digit
        else if (isdigit(ch[index]) != 0) {
            // Initialize buffer
            int buffer = 0;
            lexeme[buffer] = ch[index];
            buffer++;
            // Loops until character is not a digit
            while(isdigit(ch[index + buffer]) != 0) {
                lexeme[buffer] = ch[index + buffer];
                buffer++;
            }
            // If buffer > 5, then invalid digit results in error
            if (buffer > 5) {
                fprintf(stdout, "Error : Numbers cannot exceed 5 digits\n");
                // Checks if any remaining letters or digits exist attached to digits
                while(isalpha(ch[index + buffer]) != 0 || isdigit(ch[index + buffer]) != 0) {
                    // Store characters in lexeme and increment buffer
                    lexeme[buffer] = ch[index + buffer];
                    buffer++;
                }
            }
            else if (isalpha(ch[index + buffer]) != 0) {
                // Results in error
                fprintf(stdout, "Error : Identifiers cannot begin with a digit\n");
                // Checks if any remaining letters or digits exist attached to digits
                while(isalpha(ch[index + buffer]) != 0 || isdigit(ch[index + buffer]) != 0) {
                    // Store characters in lexeme and increment buffer
                    lexeme[buffer] = ch[index + buffer];
                    buffer++;
                }
            }
            else {
                // Must be a valid number
                token.val = numbersym;
                token.digit = lexeme;
                // Print lexeme and token.val
                fprintf(stdout, "\t%s ", lexeme);
                fprintf(stdout, "\t\t%d\n", token.val);
                
                // Add token to struct array tokenList
                tokenList[tokenListLength] = token;
                tokenListLength++;
            }
            // Increment index by buffer to skip over characters
            index += buffer;
        }
        // If character is a control character or whitespace
        else if (iscntrl(ch[index]) != 0 || isspace(ch[index]) != 0) {
            index++;
        }
        else {
            // Switch statement depending on symbol
            switch(ch[index]) {
                case '%': {
                    strcpy(lexeme, "%");
                    token.val = modsym;
                    break;
                }
                case '+': {
                    strcpy(lexeme, "+");
                    token.val = plussym;
                    break;
                }
                case '-': {
                    strcpy(lexeme, "-");
                    token.val = minussym;
                    break;
                }
                case '*': {
                    strcpy(lexeme, "*");
                    token.val = multsym;
                    break;
                }
                case '=': {
                    strcpy(lexeme, "=");
                    token.val = eqsym;
                    break;
                }
                case '(': {
                    strcpy(lexeme, "(");
                    token.val = lparentsym;
                    break;
                }
                case ')': {
                    strcpy(lexeme, ")");
                    token.val = rparentsym;
                    break;
                }
                case ',': {
                    strcpy(lexeme, ",");
                    token.val = commasym;
                    break;
                }
                case '.': {
                    strcpy(lexeme, ".");
                    token.val = periodsym;
                    break;
                }
                case ';': {
                    strcpy(lexeme, ";");
                    token.val = semicolonsym;
                    break;
                }
                case '/': {
                    // Checks if it's the beginning of a comment
                    if(ch[index + 1] == '*') {
                        index++;
                        while(ch[index + 1] != '*' && ch[index + 2] != '/'){
                            index++;
                        }
                        index += 2;
                        // Indicates that it is a comment
                        comment = 1;
                    }
                    // Otherwise, character is just '/'
                    else {
                        strcpy(lexeme, "/");
                        token.val = slashsym;
                    }
                    break;
                }
                case '<': {
                    if(ch[index + 1] == '>') {
                        strcpy(lexeme, "<>");
                        token.val = neqsym;
                        index++;
                    }
                    else if (ch[index + 1] == '=') {
                        strcpy(lexeme, "<=");
                        token.val = leqsym;
                        index++;
                    }
                    else {
                        strcpy(lexeme, "<");
                        token.val = lessym;
                    }
                    index++;
                    break;
                }
                case '>': {
                    if(ch[index + 1] == '=') {
                        strcpy(lexeme, ">=");
                        token.val = geqsym;
                        index++;
                    }
                    else {
                        strcpy(lexeme, ">");
                        token.val = gtrsym;
                    }
                    index++;
                    break;
                }
                case ':': {
                    // Must have an '=' sign after
                    if(ch[index + 1] == '=') {
                        strcpy(lexeme, ":=");
                        token.val = becomessym;
                        index++;
                    }
                    // Otherwise invalid symbol
                    else {
                        // Indicates an error exists
                        error = 1;
                        fprintf(stdout, "Error : Invalid Symbol\n");
                    }
                    break;
                }
                default: {
                    // Indicates an error exists
                    error = 1;
                    fprintf(stdout, "Error : Invalid Symbol\n");
                }
            }
            // If no errors nor comments exist
            if (error == 0 && comment == 0) {
                // Print lexeme and token
                fprintf(stdout, "\t%s ", lexeme);
                fprintf(stdout, "\t\t%d\n", token.val);
                // Add token to struct array tokenList
                tokenList[tokenListLength] = token;
                tokenListLength++;
            }
            // Increment index
            index++;
        }
    }
    
    // Prints final tokenList array
    fprintf(stdout, "\nToken List:\n");
    for (int i = 0; i < tokenListLength; i++) {
        fprintf(stdout, "%d ", tokenList[i].val);
        fprintf(stdout, "%s ", tokenList[i].identifier);
        fprintf(stdout, "%s ", tokenList[i].digit);
    }
}
