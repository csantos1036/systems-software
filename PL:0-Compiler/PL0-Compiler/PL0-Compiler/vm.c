//
//  vm.c
//
//  Authors: Carolina Santos and Jacob Campbell
//

#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"

// Constants
#define MAX_STACK_HEIGHT 50
#define MAX_CODE_LENGTH 200

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

void execute(instruction *code, int print)
{
	printf("\n\n");
    
    // Initial values
    int SP = -1;
    int BP = 0;
    int PC = 0;
    int halt = 1;
    instruction IR;
    int ARfrequency[MAX_STACK_HEIGHT];
    int stack[MAX_STACK_HEIGHT];
    
    // Initialize stack array & AR frequency array
    for (int i = 0; i < MAX_STACK_HEIGHT; i++) {
        stack[i] = 0;
        ARfrequency[i] = 0;
    }
    
    // Output formatting
    if(print == 1) {
        printf("\t\t PC    BP    SP    stack");
        printf("\nInitial values: %2d    %2d    %2d    \n", PC, BP, SP);
    }
    
    // Loop through until halt flag is zero
    while(halt != 0) {
        // Fetch cycle
        IR = code[PC];
        PC = PC + 1;
        // Execute cycle
        switch (IR.opcode) {
            // LIT 0, M
            case 1:
                if(print == 1) {
                printf("%2d ", PC - 1);
                printf("LIT  ");
                }
                SP = SP + 1;
                stack[SP] = IR.m;
                break;
            // OPR 0, #
            case 2:
                if(print == 1) {
                printf("%2d ", PC - 1);
                printf("OPR  ");
                }
                // (0 <= # <= 13)
                switch (IR.m) {
                    // RTN
                    case 0:
                        stack[BP - 1] = stack[SP];
                        SP = BP - 1;
                        BP = stack[SP + 2];
                        PC = stack[SP + 3];
                        break;
                    // NEG
                    case 1:
                        stack[SP] = -1 * stack[SP];
                        break;
                    // ADD
                    case 2:
                        SP = SP - 1;
                        stack[SP] = stack[SP] + stack[SP + 1];
                        break;
                    // SUB
                    case 3:
                        SP = SP - 1;
                        stack[SP] = stack[SP] - stack[SP + 1];
                        break;
                    // MUL
                    case 4:
                        SP = SP - 1;
                        stack[SP] = stack[SP] * stack[SP + 1];
                        break;
                    // DIV
                    case 5:
                        SP = SP - 1;
                        stack[SP] = stack[SP] / stack[SP + 1];
                        break;
                    // ODD
                    case 6:
                        stack[SP] = stack[SP] % 2;
                        break;
                    // MOD
                    case 7:
                        SP = SP - 1;
                        stack[SP] = stack[SP] % stack[SP + 1];
                        break;
                    // EQL
                    case 8:
                        SP = SP - 1;
                        stack[SP] = (stack[SP] == stack[SP + 1]);
                        break;
                    // NEQ
                    case 9:
                        SP = SP - 1;
                        stack[SP] = (stack[SP] != stack[SP + 1]);
                        break;
                    // LSS
                    case 10:
                        SP = SP - 1;
                        stack[SP] = (stack[SP] < stack[SP + 1]);
                        break;
                    // LEQ
                    case 11:
                        SP = SP - 1;
                        stack[SP] = (stack[SP] <= stack[SP + 1]);
                        break;
                    // GTR
                    case 12:
                        SP = SP - 1;
                        stack[SP] = (stack[SP] > stack[SP + 1]);
                        break;
                    // GEQ
                    case 13:
                        SP = SP - 1;
                        stack[SP] = (stack[SP] >= stack[SP + 1]);
                        break;
                    default:
                        break;
                }
                break;
            // LOD L, M
            case 3:
                if(print == 1) {
                    printf("%2d ", PC - 1);
                    printf("LOD  ");
                }
                SP = SP + 1;
                stack[SP] = stack[base(stack, IR.l, BP) + IR.m];
                break;
            // STO L, M
            case 4:
                if(print == 1) {
                    printf("%2d ", PC - 1);
                    printf("STO  ");
                }
                stack[base(stack, IR.l, BP) + IR.m] = stack[SP];
                SP = SP - 1;
                break;
            // CAL L, M
            case 5:
                if(print == 1) {
                    printf("%2d ", PC - 1);
                    printf("CAL  ");
                }
                stack[SP + 1] = base(stack, IR.l, BP);
                stack[SP + 2] = BP;
                stack[SP + 3] = PC;
                stack[SP + 4] = stack[SP];
                BP = SP + 1;
                PC = IR.m;
                break;
            // INC 0, M
            case 6:
                if(print == 1) {
                    printf("%2d ", PC - 1);
                    printf("INC  ");
                }
                SP = SP + IR.m;
                break;
            // JMP 0, M
            case 7:
                if(print == 1) {
                    printf("%2d ", PC - 1);
                    printf("JMP  ");
                }
                PC = IR.m;
                break;
            // JPC 0, M
            case 8:
                if(print == 1) {
                    printf("%2d ", PC - 1);
                    printf("JPC  ");
                }
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
                        if(print == 1) {
                        printf("Top of Stack Value: ");
                        printf("%d\n", stack[SP]);
                        printf("%2d ", PC - 1);
                        printf("SYS  ");
                        }
                        SP = SP - 1;
                        break;
                    case 2:
                        // Read in input from the user and store it on top of the stack
                        SP = SP + 1;
                        if (print == 1) {
                        printf("Please Enter an Integer: \n");
                        scanf("%d", &stack[SP]);
                        printf("%2d ", PC - 1);
                        printf("SYS  ");
                        }
                        break;
                    case 3:
                        // Set halt flag to zero
                        halt = 0;
                        if (print == 1) {
                            printf("%2d ", PC - 1);
                            printf("SYS  ");
                        }
                    default:
                        break;
                }
            default:
                break;
        }
        
        // Output
        if(print == 1) {
            printf("%2d ", IR.l);
            printf("%2d    ", IR.m);
            printf("%2d    ", PC);
            printf("%2d    ", BP);
            printf("%2d    ", SP);
        }
        
        // Check if another activation record has been created
        // And if so, set the the AR frequency at index BP to indicate
        // that a new one was created at that specific index
        if (BP != 0) {
            ARfrequency[BP] = 1;
        }
    
        if (print == 1) {
            // Stack output
            for (int i = 0; i <= SP; i++) {
                // Print '|' to separate activation records
                if(ARfrequency[i] == 1) {
                    printf("| ");
                }
                printf("%d ", stack[i]);
            }
            printf("\n");
        }
    }
	return;
}
