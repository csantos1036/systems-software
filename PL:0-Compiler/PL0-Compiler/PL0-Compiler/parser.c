//
//  parser.c
//
//  Authors: Carolina Santos and Jacob Campbell
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

// Constants
#define UNMARKED 0
#define MARKED 1
#define MAX_LENGTH 1000

// Global variable storing current lexeme/token
lexeme TOKEN;
// Global instruction stuct array to use as input for emit
instruction *code;
// Global symbol struct array to use as a symbol table
symbol *table;

// Global initial variables
int codeidx = 0;
int tokenListIdx = 0;
int symbolIdx = 0; //rename
int procedureCount = 0;
int symIdx = 0;
int numVars = 0;

// Error messages
char *errormsg[] = {
	"",
	"Error : program must end with period\n",
	"Error : const, var, procedure, call, and read keywords must be followed by identifier\n",
	"Error : competing symbol declarations at the same level\n",
	"Error : constants must be assigned with =\n",
	"Error : constants must be assigned an integer value\n",
	"Error : symbol declarations must be followed by a semicolon\n",
	"Error : undeclared variable or constant in equation\n",
	"Error : only variable values may be altered\n",
	"Error : assignment statements must use :=\n",
	"Error : begin must be followed by end\n",
	"Error : if must be followed by then\n",
	"Error : while must be followed by do\n",
	"Error : condition must contain comparison operator\n",
	"Error : right parenthesis must follow left parenthesis\n",
	"Error : arithmetic equations must contain operands, parentheses, numbers, or symbols\n",
    "Error : undeclared procedure for call\n",
    "Error : parameters may only be specified by an identifier",
    "Error : parameters must be declared",
    "Error : cannot return from main"
};

// Prototypes
void program(lexeme *tokenList);
void block(lexeme *tokenList, int lexLevel, int param, int procedureIndex);
int const_declaration(lexeme *tokenList, int lexLevel);
int var_declaration(lexeme *tokenList, int lexLevel, int param);
int proc_declaration(lexeme *tokenList, int lexLevel);
void statement(lexeme *tokenList, int lexLevel);
void condition(lexeme *tokenList, int lexLevel);
void expression(lexeme *tokenList, int lexLevel);
void term(lexeme *tokenList, int lexLevel);
void factor(lexeme *tokenList, int lexLevel);

/*
 * Parse list of tokens and fill out the symbol table
 * If the program does not follow the grammar, a message
 * indicating the type  of  error  present.
 */
instruction *parse(lexeme *tokenList, int print)
{
    // Initialize arrays
	code = malloc(MAX_LENGTH * sizeof(instruction));
    table = malloc(MAX_LENGTH * sizeof(symbol));
    // Begin parsing
    TOKEN = tokenList[0];
    program(tokenList);
    
    // Output heading
    if(print == 1) {
        printf("\nGenerated Assembly:\n");
        printf("Line\tOP\tL\tM\n");
        for(int i = 0; i < codeidx; i++) {
            printf("%d\t", i);
            printf("%s\t", code[i].op);
            printf("%d\t", code[i].l);
            printf("%d\t", code[i].m);
            printf("\n");
        }
    }
	return code;
}

// Code generation procedure to use as input for VM
void emit(int opcode, char op[], int l, int m)
{
	code[codeidx].opcode = opcode;
	strcpy(code[codeidx].op, op);
	code[codeidx].l = l;
	code[codeidx].m = m;
	codeidx++;
}

// Return next token
void getNextToken(lexeme *tokenList) {
    tokenListIdx++;
    TOKEN = tokenList[tokenListIdx];
}

// Returns index of the desired procedure in the symbol table
int findProcedure(int index) {
    int matchedIndex = -1;
    // Linear search through symbol table
    for (int i = 0; i < symbolIdx; i++) {
        // Return the index of the value that matches of symbols with kind = 3
        if(table[i].val == index && table[i].kind == 3) {
            matchedIndex = i;
        }
    }
    return matchedIndex;
}

// Marks unmarked entries
void mark(int count){
    // Start from the end of the symbol table, looping backwards
    for (int i = symbolIdx - 1; i >= 0; i--)  {
        // If entry is unmarked, mark it
        if (count != 0) {
            if(table[symbolIdx - count].mark == UNMARKED) {
                table[symbolIdx - count].mark = MARKED;
                count--;
            }
            else {
                continue;
            }
        }
    }
}

// Checks to make sure identifier exists in the table
int symbolTableCheck(char *name, int level){
    // Return index if it exists in the table
    for (int i = 0; i < symbolIdx; i++) {
        if(strcmp(name, table[i].name) == 0 && level == table[i].level) {
            if (table[i].mark == UNMARKED) {
                return i;
            }
        }
    }
    // Return -1 if does not exist
    return -1;
}

// Return index of exact match of name and kind, unmarked with nearest lexlevel
int symbolTableSearch(char *name, int lexLevel, int kind) {
    // Loop backwards
    for (int i = symbolIdx - 1; i >= 0; i--)  {
        if(strcmp(name, table[i].name) == 0 && kind == table[i].kind && lexLevel >= table[i].level) {
            if (table[i].mark == UNMARKED) {
                return i;
            }
        }
    }
    // Return -1 if does not exist
    return -1;
}

// Add symbol to table 
void addToSymbolTable(int kind, char *name, int val, int level, int addr, int mark, int param) {
    table[symbolIdx].kind = kind;
    strcpy(table[symbolIdx].name, name);
    table[symbolIdx].val = val;
    table[symbolIdx].level = level;
    table[symbolIdx].addr = addr;
    table[symbolIdx].mark =  mark;
    table[symbolIdx].param = param;
    symbolIdx++;
}

// Procedure to handle program
void program(lexeme *tokenList) {
    int numProc = 1;
    emit(7, "JMP", 0, 0); // Need to figure out M
    for (int i = 0; i < MAX_LENGTH; i++) {
        if (tokenList[i].type == procsym) {
            numProc++;
            emit(7, "JMP", 0, 0); // Need to figure out M
        }
    }
    addToSymbolTable(3, "main", 0, 0, 0, UNMARKED, 0);
    procedureCount++;
    block(tokenList, 0, 0, 0);
    if (TOKEN.type != periodsym) {
        printf("\n%s", errormsg[1]);
        exit(0);
    }
    for(int i = 0; i < numProc; i++) {
        code[i].m = table[findProcedure(i)].addr;
    }
    for (int i = 0; i < codeidx; i++) {
        // Checks if opcode is "CAL"
        if(code[i].opcode == 5) {
            code[i].m = table[findProcedure(code[i].m)].addr;
        }
    }
    emit(9, "SYS", 0, 3);
}

// Procedure to handle block
void block(lexeme *tokenList, int lexLevel, int param, int procedureIndex) {
    int c = const_declaration(tokenList, lexLevel);
    int v = var_declaration(tokenList, lexLevel, param);
    int p = proc_declaration(tokenList, lexLevel);
    table[procedureIndex].addr = codeidx;
    emit(6, "INC", 0, 4 + v);
    statement(tokenList, lexLevel);
    mark(c + v + p);
}

// Procedure to handle const declaration
int const_declaration(lexeme *tokenList, int lexLevel){
    int numConst = 0;
    if (TOKEN.type == constsym) {
        do {
            numConst++;
            getNextToken(tokenList);
            if(TOKEN.type != identsym){
                printf("\n%s", errormsg[2]);
                exit(0);
            }
            if(symbolTableCheck(TOKEN.name, lexLevel) != -1) {
                printf("\n%s", errormsg[3]);
                exit(0);
            }
            char * identName = malloc(10 * sizeof(char));
            strcpy(identName,TOKEN.name);
            getNextToken(tokenList);
            if(TOKEN.type != eqlsym){
                printf("\n%s", errormsg[4]);
                exit(0);
            }
            getNextToken(tokenList);
            if(TOKEN.type != numbersym){
                printf("\n%s", errormsg[5]);
                exit(0);
            }
            addToSymbolTable(1, identName, TOKEN.value, lexLevel, 0, UNMARKED, 0);
            getNextToken(tokenList);
        }
        while(TOKEN.type == commasym);
        if (TOKEN.type != semicolonsym) {
            printf("\n%s", errormsg[6]);
            exit(0);
        }
        getNextToken(tokenList);
    }
    return numConst;
}

// Procedure to handle var declaration
int var_declaration(lexeme *tokenList, int lexLevel, int param) {
    if (param == 1){
        numVars = 1;
    } else  {
        numVars = 0;
    }
    if(TOKEN.type == varsym) {
        do {
            numVars++;
            getNextToken(tokenList);
            if(TOKEN.type != identsym) {
                printf("\n%s", errormsg[2]);
                exit(0);
            }
            if(symbolTableCheck(TOKEN.name, lexLevel) != -1) {
                printf("\n%s", errormsg[3]);
                exit(0);
            }
            addToSymbolTable(2, TOKEN.name, 0, lexLevel, numVars + 3, UNMARKED, 0);
            getNextToken(tokenList);
        }
        while(TOKEN.type == commasym);
        if (TOKEN.type != semicolonsym) {
            printf("\n%s", errormsg[6]);
            exit(0);
        }
        getNextToken(tokenList);
    }
    return numVars;
}

// Procedure to handle procedure declaration
int proc_declaration(lexeme *tokenList, int lexLevel) {
    int numProc = 0;
    if (TOKEN.type == procsym) {
        do {
            numProc++;
            getNextToken(tokenList);
            if(TOKEN.type != identsym) {
                printf("\n%s", errormsg[2]);
                exit(0);
            }
            if(symbolTableCheck(TOKEN.name, lexLevel) != -1) {
                printf("\n%s", errormsg[3]);
                exit(0);
            }
            int procIdx = symbolIdx;
            addToSymbolTable(3, TOKEN.name, procedureCount, lexLevel, 0, UNMARKED, 0);
            procedureCount++;
            getNextToken(tokenList);
            if(TOKEN.type == lparentsym) {
                getNextToken(tokenList);
                if(TOKEN.type != identsym) {
                    printf("\n%s", errormsg[17]);
                    exit(0);
                }
                addToSymbolTable(2, TOKEN.name, 0, lexLevel + 1, 3, UNMARKED, 0);
                table[procIdx].param = 1;
                getNextToken(tokenList);
                if(TOKEN.type != rparentsym) {
                    printf("\n%s", errormsg[14]);
                    exit(0);
                }
                getNextToken(tokenList);
                if(TOKEN.type != semicolonsym) {
                    printf("\n%s", errormsg[6]);
                    exit(0);
                }
                getNextToken(tokenList);
                block(tokenList, lexLevel + 1, 1, procIdx);
            }
            else {
                if(TOKEN.type != semicolonsym) {
                    printf("\n%s", errormsg[6]);
                    exit(0);
                }
                getNextToken(tokenList);
                block(tokenList, lexLevel + 1, 0, procIdx);
            }
            if(code[codeidx - 1].opcode != 2 && code[codeidx - 1].m != 0) {
                emit(1, "LIT", 0, 0);
                // RTN
                emit(2, "OPR", 0, 0);
            }
            if(TOKEN.type != semicolonsym) {
                printf("\n%s", errormsg[6]);
                exit(0);
            }
            getNextToken(tokenList);
        }
        while(TOKEN.type == procsym);
    }
    return numProc;
}

// Procedure to handle statement
void statement(lexeme *tokenList, int lexLevel) {
    if(TOKEN.type == identsym) {
        int symIdx = symbolTableSearch(TOKEN.name, lexLevel, 2);
        if(symIdx == -1) {
            printf("\n%s", errormsg[7]);
            exit(0);
        }
        getNextToken(tokenList);
        if(TOKEN.type != becomessym){
            printf("\n%s", errormsg[9]);
            exit(0);
        }
        getNextToken(tokenList);
        expression(tokenList, lexLevel);
        emit(4, "STO", lexLevel - table[symIdx].level, table[symIdx].addr);
        return;
    }
    if(TOKEN.type == callsym) {
        getNextToken(tokenList);
        if(TOKEN.type != identsym){
            printf("\n%s", errormsg[2]);
            exit(0);
        }
        symIdx = symbolTableSearch(TOKEN.name, lexLevel, 3);
        if (symIdx == -1) {
            printf("\n%s", errormsg[16]);
            exit(0);
        }
        getNextToken(tokenList);
        if (TOKEN.type == lparentsym) {
            getNextToken(tokenList);
            if(table[symIdx].param != 1) {
                printf("\n%s", errormsg[18]);
                exit(0);
            }
            expression(tokenList, lexLevel);
            if (TOKEN.type != rparentsym) {
                printf("\n%s", errormsg[14]);
                exit(0);
            }
            getNextToken(tokenList);
        }
        else {
            emit(1, "LIT", 0, 0);
        }
        emit(5, "CAL", lexLevel - table[symIdx].level, table[symIdx].val);
        return;
    }
    if(TOKEN.type == returnsym) {
        if (lexLevel == 0) {
            printf("\n%s", errormsg[19]);
            exit(0);
        }
        getNextToken(tokenList);
        if (TOKEN.type == lparentsym) {
            getNextToken(tokenList);
            expression(tokenList, lexLevel);
            emit(2, "OPR", 0, 0);
            if(TOKEN.type != rparentsym){
                printf("\n%s", errormsg[14]);
                exit(0);
            }
            getNextToken(tokenList);
        }
        else {
            emit(1, "LIT", 0, 0);
            emit(2, "OPR", 0, 0);
        }
        return;
    }
    if(TOKEN.type == beginsym) {
        do {
            getNextToken(tokenList);
            statement(tokenList, lexLevel);
        }
        while(TOKEN.type == semicolonsym);
        if(TOKEN.type != endsym) {
            printf("\n%s", errormsg[10]);
            exit(0);
        }
        getNextToken(tokenList);
        return;
    }
    if(TOKEN.type == ifsym) {
        getNextToken(tokenList);
        condition(tokenList, lexLevel);
        int jpcIdx = codeidx;
        emit(8, "JPC", 0, jpcIdx);
        if (TOKEN.type != thensym) {
            printf("\n%s", errormsg[11]);
            exit(0);
        }
        getNextToken(tokenList);
        statement(tokenList, lexLevel);
        if (TOKEN.type == elsesym) {
            getNextToken(tokenList);
            int jmpIdx = codeidx;
            emit(7, "JMP", lexLevel, jmpIdx);
            code[jpcIdx].m = codeidx;
            statement(tokenList, lexLevel);
            code[jmpIdx].m = codeidx;
        }
        else {
            code[jpcIdx].m = codeidx;
        }
        return;
    }
    if(TOKEN.type == whilesym) {
        getNextToken(tokenList);
        int loopIdx = codeidx;
        condition(tokenList, lexLevel);
        if(TOKEN.type != dosym) {
            printf("\n%s", errormsg[12]);
            exit(0);
        }
        getNextToken(tokenList);
        int jpcIdx = codeidx;
        emit(8, "JPC", lexLevel, jpcIdx);
        statement(tokenList, lexLevel);
        emit(7, "JMP", lexLevel, loopIdx);
        code[jpcIdx].m = codeidx;
        return;
    }
    if (TOKEN.type == readsym) {
        getNextToken(tokenList);
        if (TOKEN.type != identsym) {
            printf("\n%s", errormsg[2]);
            exit(0);
        }
        int symIdx = symbolTableSearch(TOKEN.name, lexLevel, 2);
        if (symIdx == -1) {
            printf("\n%s", errormsg[7]);
            exit(0);
        }
        getNextToken(tokenList);
        emit(9, "SYS", 0, 2);
        emit(4, "STO", lexLevel - table[symIdx].level, table[symIdx].addr);
        return;
    }
    if (TOKEN.type == writesym) {
        getNextToken(tokenList);
        expression(tokenList, lexLevel);
        emit(9, "SYS", 0, 1);
        return;
    }
}

// Procedure to handle condition
void condition(lexeme *tokenList, int lexLevel) {
    if (TOKEN.type == oddsym) {
        getNextToken(tokenList);
        expression(tokenList, lexLevel);
        emit(2, "OPR", 0, 6);
    }
    else {
        expression(tokenList, lexLevel);
        if (TOKEN.type == eqlsym) {
            getNextToken(tokenList);
            expression(tokenList, lexLevel);
            emit(2, "OPR", 0, 8);
        }
        else if (TOKEN.type == neqsym) {
            getNextToken(tokenList);
            expression(tokenList, lexLevel);
            emit(2, "OPR", 0, 9);
        }
        else if (TOKEN.type == lessym) {
            getNextToken(tokenList);
            expression(tokenList, lexLevel);
            emit(2, "OPR", 0, 10);
        }
        else if (TOKEN.type == leqsym) {
            getNextToken(tokenList);
            expression(tokenList, lexLevel);
            emit(2, "OPR", 0, 11);
        }
        else if (TOKEN.type == gtrsym) {
            getNextToken(tokenList);
            expression(tokenList, lexLevel);
            emit(2, "OPR", 0, 12);
        }
        else if (TOKEN.type == geqsym) {
            getNextToken(tokenList);
            expression(tokenList, lexLevel);
            emit(2, "OPR", 0, 13);
        }
        else {
            printf("\n%s", errormsg[13]);
            exit(0);
        }
    }
}

// Procedure to handle expression
void expression(lexeme *tokenList, int lexLevel) {
    if(TOKEN.type == minussym) {
        getNextToken(tokenList);
        term(tokenList, lexLevel);
        emit(2, "OPR", 0, 1);
        while (TOKEN.type == plussym || TOKEN.type == minussym) {
            if (TOKEN.type == plussym) {
                getNextToken(tokenList);
                term(tokenList, lexLevel);
                emit(2, "OPR", 0, 2);
            }
            else {
                getNextToken(tokenList);
                term(tokenList, lexLevel);
                emit(2, "OPR", 0, 3);
            }
        }
    }
    else {
        if (TOKEN.type == plussym) {
            getNextToken(tokenList);
        }
        term(tokenList, lexLevel);
        while (TOKEN.type == plussym || TOKEN.type == minussym) {
            if (TOKEN.type == plussym) {
                getNextToken(tokenList);
                term(tokenList, lexLevel);
                emit(2, "OPR", 0, 2);
            }
            else {
                getNextToken(tokenList);
                term(tokenList, lexLevel);
                emit(2, "OPR", 0, 3);
            }
        }
    }
}

// Procedure to handle term
void term(lexeme *tokenList, int lexLevel) {
    factor(tokenList, lexLevel);
    while(TOKEN.type == multsym || TOKEN.type == slashsym) {
        if (TOKEN.type == multsym) {
            getNextToken(tokenList);
            factor(tokenList, lexLevel);
            emit(2, "OPR", 0, 4);
        }
        else if (TOKEN.type == slashsym) {
            getNextToken(tokenList);
            factor(tokenList, lexLevel);
            emit(2, "OPR", 0, 5);
        }
        else {
            getNextToken(tokenList);
            factor(tokenList, lexLevel);
            emit(2, "OPR", 0, 7);
        }
    }
}

// Procedure to handle factor
void factor(lexeme *tokenList, int lexLevel) {
    if (TOKEN.type == identsym) {
        int symIdxV = symbolTableSearch(TOKEN.name, lexLevel, 2);
        int symIdxC = symbolTableSearch(TOKEN.name, lexLevel, 1);
        if (symIdxV == -1 && symIdxC == -1) {
            printf("\n%s", errormsg[7]);
            exit(0);
        }
        else if (symIdxC == -1 || (symIdxV != -1 && table[symIdxV].level > table[symIdxC].level)) {
            emit(3, "LOD", lexLevel - table[symIdxV].level, table[symIdxV].addr);
        }
        else {
            emit(1, "LIT", 0, table[symIdxC].val);
        }
        getNextToken(tokenList);
    }
    else if (TOKEN.type == numbersym) {
        emit(1, "LIT", 0, TOKEN.value);
        getNextToken(tokenList);
    }
    else if (TOKEN.type == lparentsym) {
        getNextToken(tokenList);
        expression(tokenList, lexLevel);
        if (TOKEN.type != rparentsym) {
            printf("\n%s", errormsg[14]);
            exit(0);
        }
        getNextToken(tokenList);
    }
    else if (TOKEN.type == callsym) {
        statement(tokenList, lexLevel);
    }
    else {
        printf("\n%s", errormsg[15]);
        exit(0);
    }
}
    



