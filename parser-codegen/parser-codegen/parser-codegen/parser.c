//
//  parser.c
//
//  Authors: Carolina Santos and Jacob Campbell
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

// Global variable storing current lexeme/token
lexeme TOKEN;
// Global instruction stuct array to use as input for emit
instruction *code;
// Global symbol struct array to use as a symbol table
symbol *table;

// Global initial variables
int codeidx = 0;
int tokenListIdx = 0;
int symbolIdx = 0;

// Error messages
char *errormsg[] = {
	"",
	"Error : program must end with period\n",
	"Error : const, var, and read keywords must be followed by identifier\n",
	"Error : symbol name has already been declared\n",
	"Error : constants must be assigned with =\n",
	"Error : constants must be assigned an integer value\n",
	"Error : constant and variable declarations must be followed by a semicolon\n",
	"Error : undeclared symbol\n",
	"Error : only variable values may be altered\n",
	"Error : assignment statements must use :=\n",
	"Error : begin must be followed by end\n",
	"Error : if must be followed by then\n",
	"Error : while must be followed by do\n",
	"Error : condition must contain comparison operator\n",
	"Error : right parenthesis must follow left parenthesis\n",
	"Error : arithmetic equations must contain operands, parentheses, numbers, or symbols\n"
};

// Prototypes
void emit(int opcode, char op[], int l, int m);
void program(lexeme *tokenList);
void block(lexeme *tokenList);
void const_declaration(lexeme *tokenList);
int var_declaration(lexeme *tokenList);
void statement(lexeme *tokenList);
void condition(lexeme *tokenList);
void expression(lexeme *tokenList);
void term(lexeme *tokenList);
void factor(lexeme *tokenList);

// Return next token
void getNextToken(lexeme *tokenList) {
    tokenListIdx++;
    TOKEN = tokenList[tokenListIdx];
}

/*
 * Parse list of tokens and fill out the symbol table
 * If the program does not follow the grammar, a message
 * indicating the type  of  error  present.
 */
instruction *parse(lexeme *tokenList, int print)
{
    // Initialize arrays
	code = malloc(500 * sizeof(instruction));
    table = malloc(500 * sizeof(symbol));
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

// Checks to make sure identifier exists in the table
int symbolTableCheck(char *name){
    // Return index if it already exists in the table
    for (int i = 0; i < 500; i++) {
        if(strcmp(name, table[i].name) == 0) {
            return i;
        }
    }
    // Return -1 if does not exist
    return -1;
}

// Procedure to handle program
void program(lexeme *tokenList) {
    block(tokenList);
    if (TOKEN.type != periodsym) {
        printf("\n%s", errormsg[1]);
        exit(0);
    }
    emit(9, "SYS", 0, 3);
}

// Procedure to handle block
void block(lexeme *tokenList) {
    const_declaration(tokenList);
    int numVars = var_declaration(tokenList);
    emit(6, "INC", 0, 4 + numVars);
    // error sometime after last line
    statement(tokenList);
}

// Procedure to handle const declaration
void const_declaration(lexeme *tokenList){
    if (TOKEN.type == constsym) {
        do {
            getNextToken(tokenList);
            if(TOKEN.type != identsym){
                printf("\n%s", errormsg[2]);
                exit(0);
            }
            if(symbolTableCheck(TOKEN.name) != -1) {
                printf("\n%s", errormsg[3]);
                exit(0);
            }
            char * identName = malloc(12 * sizeof(char));
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
            table[symbolIdx].kind = 1;
            strcpy(table[symbolIdx].name, identName);
            table[symbolIdx].val = TOKEN.value;
            table[symbolIdx].level = 0;
            table[symbolIdx].addr = 0;
            symbolIdx++;
            getNextToken(tokenList);
        }
        while(TOKEN.type == commasym);
        if (TOKEN.type != semicolonsym) {
            printf("\n%s", errormsg[6]);
            exit(0);
        }
        getNextToken(tokenList);
    }
}

// Procedure to handle var declaration
int var_declaration(lexeme *tokenList) {
    int numVars = 0;
    if(TOKEN.type == varsym) {
        do {
            numVars++;
            getNextToken(tokenList);
            if(TOKEN.type != identsym) {
                printf("\n%s", errormsg[2]);
                exit(0);
            }
            if(symbolTableCheck(TOKEN.name) != -1) {
                printf("\n%s", errormsg[3]);
                exit(0);
            }
            table[symbolIdx].kind = 2;
            strcpy(table[symbolIdx].name, TOKEN.name);
            table[symbolIdx].val = 0;
            table[symbolIdx].level = 0;
            table[symbolIdx].addr = numVars + 3;
            symbolIdx++;
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

// Procedure to handle statement
void statement(lexeme *tokenList) {
    if(TOKEN.type == identsym) {
        int symIdx = symbolTableCheck(TOKEN.name);
        if(symIdx == -1) {
            printf("\n%s", errormsg[7]);
            exit(0);
        }
        if(table[symIdx].kind != 2) {
            printf("\n%s", errormsg[8]);
            exit(0);
        }
        getNextToken(tokenList);
        if(TOKEN.type != becomessym){
            printf("\n%s", errormsg[9]);
            exit(0);
        }
        getNextToken(tokenList);
        expression(tokenList);
        emit(4, "STO", 0, table[symIdx].addr);
        return;
    }
    if(TOKEN.type == beginsym) {
        do {
            getNextToken(tokenList);
            statement(tokenList);
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
        condition(tokenList);
        int jpcIdx = codeidx;
        emit(8, "JPC", 0, jpcIdx);
        if (TOKEN.type != thensym) {
            printf("\n%s", errormsg[11]);
            exit(0);
        }
        getNextToken(tokenList);
        statement(tokenList);
        code[jpcIdx].m = codeidx;
        return;
    }
    if(TOKEN.type == whilesym) {
        getNextToken(tokenList);
        int loopIdx = codeidx;
        condition(tokenList);
        if(TOKEN.type != dosym) {
            printf("\n%s", errormsg[12]);
            exit(0);
        }
        getNextToken(tokenList);
        int jpcIdx = codeidx;
        emit(8, "JPC", 0, jpcIdx);
        statement(tokenList);
        emit(7, "JMP", 0, loopIdx);
        code[jpcIdx].m = codeidx;
        return;
    }
    if (TOKEN.type == readsym) {
        getNextToken(tokenList);
        if (TOKEN.type != identsym) {
            printf("\n%s", errormsg[2]);
            exit(0);
        }
        int symIdx = symbolTableCheck(TOKEN.name);
        if (symIdx == -1) {
            printf("\n%s", errormsg[7]);
            exit(0);
        }
        if (table[symIdx].kind != 2) {
            printf("\n%s", errormsg[8]);
            exit(0);
        }
        getNextToken(tokenList);
        emit(9, "SYS", 0, 2);
        emit(4, "STO", 0, table[symIdx].addr);
        return;
    }
    if (TOKEN.type == writesym) {
        getNextToken(tokenList);
        expression(tokenList);
        emit(9, "SYS", 0, 1);
        return;
    }
}

// Procedure to handle condition
void condition(lexeme *tokenList) {
    if (TOKEN.type == oddsym) {
        getNextToken(tokenList);
        expression(tokenList);
        emit(2, "OPR", 0, 6);
    }
    else {
        expression(tokenList);
        if (TOKEN.type == eqlsym) {
            getNextToken(tokenList);
            expression(tokenList);
            emit(2, "OPR", 0, 8);
        }
        else if (TOKEN.type == neqsym) {
            getNextToken(tokenList);
            expression(tokenList);
            emit(2, "OPR", 0, 9);
        }
        else if (TOKEN.type == lessym) {
            getNextToken(tokenList);
            expression(tokenList);
            emit(2, "OPR", 0, 10);
        }
        else if (TOKEN.type == leqsym) {
            getNextToken(tokenList);
            expression(tokenList);
            emit(2, "OPR", 0, 11);
        }
        else if (TOKEN.type == gtrsym) {
            getNextToken(tokenList);
            expression(tokenList);
            emit(2, "OPR", 0, 12);
        }
        else if (TOKEN.type == geqsym) {
            getNextToken(tokenList);
            expression(tokenList);
            emit(2, "OPR", 0, 13);
        }
        else {
            printf("\n%s", errormsg[13]);
            exit(0);
        }
    }
}

// Procedure to handle expression
void expression(lexeme *tokenList) {
    if(TOKEN.type == minussym) {
        getNextToken(tokenList);
        term(tokenList);
        emit(2, "OPR", 0, 1);
        while (TOKEN.type == plussym || TOKEN.type == minussym) {
            if (TOKEN.type == plussym) {
                getNextToken(tokenList);
                term(tokenList);
                emit(2, "OPR", 0, 2);
            }
            else {
                getNextToken(tokenList);
                term(tokenList);
                emit(2, "OPR", 0, 3);
            }
        }
    }
    else {
        if (TOKEN.type == plussym) {
            getNextToken(tokenList);
        }
        term(tokenList);
        while (TOKEN.type == plussym || TOKEN.type == minussym) {
            if (TOKEN.type == plussym) {
                getNextToken(tokenList);
                term(tokenList);
                emit(2, "OPR", 0, 2);
            }
            else {
                getNextToken(tokenList);
                term(tokenList);
                emit(2, "OPR", 0, 3);
            }
        }
    }
}

// Procedure to handle term
void term(lexeme *tokenList) {
    factor(tokenList);
    while(TOKEN.type == multsym || TOKEN.type == slashsym) {
        if (TOKEN.type == multsym) {
            getNextToken(tokenList);
            factor(tokenList);
            emit(2, "OPR", 0, 4);
        }
        else if (TOKEN.type == slashsym) {
            getNextToken(tokenList);
            factor(tokenList);
            emit(2, "OPR", 0, 5);
        }
        else {
            getNextToken(tokenList);
            factor(tokenList);
            emit(2, "OPR", 0, 7);
        }
    }
}

// Procedure to handle factor
void factor(lexeme *tokenList) {
    if (TOKEN.type == identsym) {
        int symIdx = symbolTableCheck(TOKEN.name);
        if (symIdx == -1) {
            printf("\n%s", errormsg[7]);
            exit(0);
        }
        if(table[symIdx].kind == 1) {
            emit(1, "LIT", 0, table[symIdx].val);
        }
        else {
            emit(3, "LOD", 0, table[symIdx].addr);
        }
        getNextToken(tokenList);
    }
    else if (TOKEN.type == numbersym) {
        emit(1, "LIT", 0, TOKEN.value);
        getNextToken(tokenList);
    }
    else if (TOKEN.type == lparentsym) {
        getNextToken(tokenList);
        expression(tokenList);
        if (TOKEN.type != rparentsym) {
            printf("\n%s", errormsg[14]);
            exit(0);
        }
        getNextToken(tokenList);
    }
    else {
        printf("\n%s", errormsg[15]);
        exit(0);
    }
}
    



