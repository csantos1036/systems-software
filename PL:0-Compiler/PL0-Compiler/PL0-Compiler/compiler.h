//
//  compiler.h
//
//  Authors: Carolina Santos and Jacob Campbell
//

typedef enum
{
	modsym = 1, identsym, numbersym, plussym, minussym, 
	multsym, slashsym, oddsym, eqlsym, neqsym, 
	lessym, leqsym, gtrsym, geqsym, lparentsym, 
	rparentsym, commasym, semicolonsym, periodsym, becomessym, 
	beginsym, endsym, ifsym, thensym, whilesym, 
	dosym, callsym,	constsym, varsym, procsym, 
	writesym, readsym, elsesym, returnsym
} token_type;

typedef struct lexeme
{
    char *name;
    int value;
	int type;
	
} lexeme;

typedef struct instruction
{
	int opcode;
	char op[4];
	int l;
	int m;
	
} instruction;

typedef struct symbol
{
	int kind;               // const = 1, var = 2, proc = 3
	char name[10];          // name up to 11 chars
	int val;                // number
	int level;              // L level
	int addr;               // M address
    int mark;               // to indicate that code has been generated already for                     // a block.
    int param;              // to indicate if the parameter for a procedure has                         // been declared
} symbol;


lexeme* lex_analyze(char *inputfile, int print);
void execute(instruction *code, int print);
instruction *parse(lexeme *tokenList, int print);
int base(int stack[], int level, int BP);
