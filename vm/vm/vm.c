//
//  vm.c
//
//  Authors: Carolina Santos and Jacob Campbell
//

#include <stdio.h>
#include <stdlib.h>

// Constants
#define MAX_STACK_HEIGHT 50
#define MAX_CODE_LENGTH 100

// Struct for instructions
typedef struct instruction {
    int opcode;
    char op[4];
    int l;
    int m;
} instruction;

// Find base L levels down
int base(int stack[], int level, int BP) {
    int base = BP;
    int counter = level;
    while (counter > 0) {
        base = stack[base];
        counter--;
    }
    return base;
}

// main function
int main(int argc, char **argv) {
    // Initial values
    int SP = -1;
    int BP = 0;
    int PC = 0;
    int halt = 1;
    instruction IR;
    instruction* instructions;
    int ARfrequency[MAX_STACK_HEIGHT];
    int stack[MAX_STACK_HEIGHT];
    
    // Initialize stack array & AR frequency array
    for (int i = 0; i < MAX_STACK_HEIGHT; i++) {
        stack[i] = 0;
        ARfrequency[i] = 0;
    }
    
    // Allocate memory for instruction stack
    instructions = (instruction*)malloc(MAX_CODE_LENGTH * sizeof(instruction));
    
    // Read input file
    FILE *f = fopen(argv[1], "r");
    
    // Store every instruction from the file into the instruction stack
    while (!feof(f)) {
        for(int i = 0; i < MAX_CODE_LENGTH; i++) {
                fscanf(f,"%d", &instructions[i].opcode);
                fscanf(f,"%d", &instructions[i].l);
                fscanf(f,"%d", &instructions[i].m);
        }
    }
    
    // Output formatting
    fprintf(stdout, "\t\t PC    BP    SP    stack");
    fprintf(stdout, "\nInitial values: %2d    %2d    %2d    \n", PC, BP, SP);
    
    // Loop through until halt flag is zero
    while(halt != 0) {
        // Fetch cycle
        IR = instructions[PC];
        PC = PC + 1;
        // Execute cycle
        switch (IR.opcode) {
            // LIT 0, M
            case 1:
                fprintf(stdout, "%2d ", PC - 1);
                fprintf(stdout, "LIT  ");
                SP = SP + 1;
                stack[SP] = IR.m;
                break;
            // OPR 0, #
            case 2:
                fprintf(stdout, "%2d ", PC - 1);
                // (0 <= # <= 13)
                switch (IR.m) {
                    // RTN
                    case 0:
                        fprintf(stdout, "RTN  ");
                        stack[BP - 1] = stack[SP];
                        SP = BP - 1;
                        BP = stack[SP + 2];
                        PC = stack[SP + 3];
                        break;
                    // NEG
                    case 1:
                        fprintf(stdout, "NEG  ");
                        stack[SP] = -1 * stack[SP];
                        break;
                    // ADD
                    case 2:
                        fprintf(stdout, "ADD  ");
                        SP = SP - 1;
                        stack[SP] = stack[SP] + stack[SP + 1];
                        break;
                    // SUB
                    case 3:
                        fprintf(stdout, "SUB  ");
                        SP = SP - 1;
                        stack[SP] = stack[SP] - stack[SP + 1];
                        break;
                    // MUL
                    case 4:
                        fprintf(stdout, "MUL  ");
                        SP = SP - 1;
                        stack[SP] = stack[SP] * stack[SP + 1];
                        break;
                    // DIV
                    case 5:
                        fprintf(stdout, "DIV  ");
                        SP = SP - 1;
                        stack[SP] = stack[SP] / stack[SP + 1];
                        break;
                    // ODD
                    case 6:
                        fprintf(stdout, "ODD  ");
                        stack[SP] = stack[SP] % 2;
                        break;
                    // MOD
                    case 7:
                        fprintf(stdout, "MOD  ");
                        SP = SP - 1;
                        stack[SP] = stack[SP] % stack[SP + 1];
                        break;
                    // EQL
                    case 8:
                        fprintf(stdout, "EQL  ");
                        SP = SP - 1;
                        stack[SP] = (stack[SP] == stack[SP + 1]);
                        break;
                    // NEQ
                    case 9:
                        fprintf(stdout, "NEQ  ");
                        SP = SP - 1;
                        stack[SP] = (stack[SP] != stack[SP + 1]);
                        break;
                    // LSS
                    case 10:
                        fprintf(stdout, "LSS  ");
                        SP = SP - 1;
                        stack[SP] = (stack[SP] < stack[SP + 1]);
                        break;
                    // LEQ
                    case 11:
                        fprintf(stdout, "LEQ  ");
                        SP = SP - 1;
                        stack[SP] = (stack[SP] <= stack[SP + 1]);
                        break;
                    // GTR
                    case 12:
                        fprintf(stdout, "GTR  ");
                        SP = SP - 1;
                        stack[SP] = (stack[SP] > stack[SP + 1]);
                        break;
                    // GEQ
                    case 13:
                        fprintf(stdout, "GEQ  ");
                        SP = SP - 1;
                        stack[SP] = (stack[SP] >= stack[SP + 1]);
                        break;
                    default:
                        break;
                }
                break;
            // LOD L, M
            case 3:
                fprintf(stdout, "%2d ", PC - 1);
                fprintf(stdout, "LOD  ");
                SP = SP + 1;
                stack[SP] = stack[base(stack, IR.l, BP) + IR.m];
                break;
            // STO L, M
            case 4:
                fprintf(stdout, "%2d ", PC - 1);
                fprintf(stdout, "STO  ");
                stack[base(stack, IR.l, BP) + IR.m] = stack[SP];
                SP = SP - 1;
                break;
            // CAL L, M
            case 5:
                fprintf(stdout, "%2d ", PC - 1);
                fprintf(stdout, "CAL  ");
                stack[SP + 1] = base(stack, IR.l, BP);
                stack[SP + 2] = BP;
                stack[SP + 3] = PC;
                stack[SP + 4] = stack[SP];
                BP = SP + 1;
                PC = IR.m;
                break;
            // INC 0, M
            case 6:
                fprintf(stdout, "%2d ", PC - 1);
                fprintf(stdout, "INC  ");
                SP = SP + IR.m;
                break;
            // JMP 0, M
            case 7:
                fprintf(stdout, "%2d ", PC - 1);
                fprintf(stdout, "JMP  ");
                PC = IR.m;
                break;
            // JPC 0, M
            case 8:
                fprintf(stdout, "%2d ", PC - 1);
                fprintf(stdout, "JPC  ");
                if(stack[SP] == 0){
                    PC = IR.m;
                }
                SP = SP - 1;
                break;
            // SYS 0, #
            case 9:
                switch (IR.m) {
                    case 1:
                        // Write the top stack element to the screen
                        fprintf(stdout, "Top of Stack Value: ");
                        fprintf(stdout, "%d\n", stack[SP]);
                        SP = SP - 1;
                        fprintf(stdout, "%2d ", PC - 1);
                        fprintf(stdout, "SYS  ");
                        break;
                    case 2:
                        // Read in input from the user and store it on top of the stack
                        SP = SP + 1;
                        fprintf(stdout, "Please Enter an Integer: \n");
                        scanf("%d", &stack[SP]);
                        fprintf(stdout, "%2d ", PC - 1);
                        fprintf(stdout, "SYS  ");
                        break;
                    case 3:
                        // Set halt flag to zero
                        halt = 0;
                        fprintf(stdout, "%2d ", PC - 1);
                        fprintf(stdout, "SYS  ");
                    default:
                        break;
                }
            default:
                break;
        }
        
        // Output
        fprintf(stdout, "%2d ", IR.l);
        fprintf(stdout, "%2d    ", IR.m);
        fprintf(stdout, "%2d    ", PC);
        fprintf(stdout, "%2d    ", BP);
        fprintf(stdout, "%2d    ", SP);
        
        // Check if another activation record has been created
        // And if so, set the the AR frequency at index BP to indicate
        // that a new one was created at that specific index
        if (BP != 0) {
            ARfrequency[BP] = 1;
        }
    
        // Stack output
        for (int i = 0; i <= SP; i++) {
            // Print '|' to separate activation records
            if(ARfrequency[i] == 1) {
                fprintf(stdout, "| ");
            }
            fprintf(stdout, "%d ", stack[i]);
        }
        fprintf(stdout, "\n");
    }
    return 0;
}


