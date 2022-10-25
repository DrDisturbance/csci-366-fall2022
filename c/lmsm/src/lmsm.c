#include "lmsm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


//======================================================
//  Instruction Implementation
//======================================================

void lmsm_i_call(lmsm *our_little_machine) {
    lmsm_stack *newstack = malloc(sizeof(lmsm_stack));
    newstack->value = our_little_machine->program_counter;
    our_little_machine->program_counter = our_little_machine->accumulator->value;
    our_little_machine->accumulator = our_little_machine->accumulator->next;
    newstack->next = our_little_machine->call_stack;
    our_little_machine->call_stack = newstack;
}

void lmsm_i_return(lmsm *our_little_machine) {
    our_little_machine->program_counter = our_little_machine->call_stack->value;
    our_little_machine->call_stack = our_little_machine->call_stack->next;
}

void lmsm_i_push(lmsm *our_little_machine) {
    lmsm_stack *newstack = malloc(sizeof(lmsm_stack));
    newstack->value = 0;
    newstack->next = our_little_machine->accumulator;
    our_little_machine->accumulator = newstack;
}

void lmsm_i_pop(lmsm *our_little_machine) {
    lmsm_stack *current = our_little_machine->accumulator;
    lmsm_stack *next = current->next;
    if (current == NULL) {
        our_little_machine->status = STATUS_HALTED;
        our_little_machine->error_code = ERROR_EMPTY_STACK;
    } else if (next == NULL) {
        our_little_machine->status = STATUS_HALTED;
        our_little_machine->error_code = ERROR_EMPTY_STACK;
    } else {
        our_little_machine->accumulator = our_little_machine->accumulator->next;
    }
}

void lmsm_i_dup(lmsm *our_little_machine) {
    lmsm_stack *newstack = malloc(sizeof(lmsm_stack));
    newstack->value = our_little_machine->accumulator->value;
    newstack->next = our_little_machine->accumulator->next;
    our_little_machine->accumulator->next = newstack;
}

int check_stack(lmsm *our_little_machine) {
    lmsm_stack *current = our_little_machine->accumulator;
    if (current == NULL) {
        our_little_machine->status = STATUS_HALTED;
        our_little_machine->error_code = ERROR_EMPTY_STACK;
        return 0;
    }
    lmsm_stack *next = current->next;
    if (next == NULL) {
        our_little_machine->status = STATUS_HALTED;
        our_little_machine->error_code = ERROR_EMPTY_STACK;
        return 0;
    }
    return 1;
}

void lmsm_i_sadd(lmsm *our_little_machine) {
    if (!check_stack(our_little_machine)) {
        return;
    }
    lmsm_stack *current = our_little_machine->accumulator;
    lmsm_stack *next = current->next;
    lmsm_stack *new = malloc(sizeof(lmsm_stack));
    new->value = current->value + next->value;
    new->next = next->next;
    our_little_machine->accumulator = new;

    free(current);
    free(next);
}

void lmsm_i_ssub(lmsm *our_little_machine) {
    if (!check_stack(our_little_machine)) {
        return;
    }
    lmsm_stack *current = our_little_machine->accumulator;
    lmsm_stack *next = current->next;
    lmsm_stack *new = malloc(sizeof(lmsm_stack));
    new->value = next->value - current->value;
    new->next = next->next;
    our_little_machine->accumulator = new;

    free(current);
    free(next);
}

void lmsm_i_smax(lmsm *our_little_machine) {
    if (!check_stack(our_little_machine)) {
        return;
    }
    lmsm_stack *current = our_little_machine->accumulator;
    lmsm_stack *next = current->next;
    lmsm_stack *new = malloc(sizeof(lmsm_stack));
    if(current->value > next->value){
        new->value = current->value;
    } else {
        new->value = next->value;
    }
    new->next = next->next;
    our_little_machine->accumulator = new;
    free(current);
    free(next);
}

void lmsm_i_smin(lmsm *our_little_machine) {
    if (!check_stack(our_little_machine)) {
        return;
    }
    lmsm_stack *current = our_little_machine->accumulator;
    lmsm_stack *next = current->next;
    lmsm_stack *new = malloc(sizeof(lmsm_stack));
    if(current->value < next->value){
        new->value = current->value;
    } else {
        new->value = next->value;
    }
    new->next = next->next;
    our_little_machine->accumulator = new;
    free(current);
    free(next);
}

void lmsm_i_smul(lmsm *our_little_machine) {
    if (!check_stack(our_little_machine)) {
        return;
    }
    lmsm_stack *current = our_little_machine->accumulator;
    lmsm_stack *next = current->next;
    lmsm_stack *new = malloc(sizeof(lmsm_stack));
    new->value = current->value * next->value;
    new->next = next->next;
    our_little_machine->accumulator = new;

    free(current);
    free(next);
}

void lmsm_i_sdiv(lmsm *our_little_machine) {
    if (!check_stack(our_little_machine)) {
        return;
    }
    lmsm_stack *current = our_little_machine->accumulator;
    lmsm_stack *next = current->next;
    lmsm_stack *new = malloc(sizeof(lmsm_stack));
    new->value = next->value / current->value;
    new->next = next->next;
    our_little_machine->accumulator = new;

    free(current);
    free(next);
}

void append(char* s, char c) {
    int len = strlen(s);
    s[len] = c;
    s[len+1] = '\0';
}

void lmsm_i_out(lmsm *our_little_machine) {
    char new[4];
    char original[strlen(our_little_machine->output_buffer)];
    strcpy(original, our_little_machine->output_buffer);
    sprintf(new, "%d",our_little_machine->accumulator->value);
    strcat(original, new);
    strcat(original, " ");
    strcpy(our_little_machine->output_buffer, original);
}

void lmsm_i_inp(lmsm *our_little_machine) {
    printf("> ");
    int myInt;
    int result = scanf("%d", &myInt);
    if (result == EOF) {
        our_little_machine->error_code = ERROR_UNKNOWN_INSTRUCTION;
        our_little_machine->status = STATUS_HALTED;
    } else if (result == 0) {
        while (fgetc(stdin) != '\n')
            ;
    } else {
        our_little_machine->accumulator->value = myInt;
    }
}

void lmsm_i_load(lmsm *our_little_machine, int location) {
    our_little_machine->accumulator->value = our_little_machine->memory[location];
}

void lmsm_i_add(lmsm *our_little_machine, int location) {
    our_little_machine->accumulator->value += our_little_machine->memory[location];
}

void lmsm_i_sub(lmsm *our_little_machine, int location) {
    our_little_machine->accumulator->value -= our_little_machine->memory[location];
}

void lmsm_i_load_immediate(lmsm *our_little_machine, int value) {
    our_little_machine->accumulator->value = value;
}

void lmsm_i_store(lmsm *our_little_machine, int location) {
    our_little_machine->memory[location] = our_little_machine->accumulator->value;
}

void lmsm_i_halt(lmsm *our_little_machine) {
    our_little_machine->status = STATUS_HALTED;
}

void lmsm_i_branch_unconditional(lmsm *our_little_machine, int location) {
    //#lmsm_exec_instruction(our_little_machine, our_little_machine->memory[location]);
    our_little_machine->program_counter = location;
}

void lmsm_i_branch_if_zero(lmsm *our_little_machine, int location) {
    if(our_little_machine->accumulator->value == 0){
        //#lmsm_exec_instruction(our_little_machine, our_little_machine->memory[location]);
        our_little_machine->program_counter = location;
    }
}

void lmsm_i_branch_if_positive(lmsm *our_little_machine, int location) {
    if(our_little_machine->accumulator->value >= 0){
        //#lmsm_exec_instruction(our_little_machine, our_little_machine->memory[location]);
        our_little_machine->program_counter = location;
    }
}

void lmsm_cap_accumulator_value(lmsm *our_little_machine){
    printf("Accumulator value original: %d", our_little_machine->accumulator->value);
    if(our_little_machine->accumulator->value > 999){
        our_little_machine->accumulator->value = 999;
    } else if(our_little_machine->accumulator->value < -999){
        our_little_machine->accumulator->value = -999;
    }
    printf("New accumulator value: %d", our_little_machine->accumulator->value);
}

void lmsm_step(lmsm *our_little_machine) {
    if(our_little_machine->status != STATUS_HALTED) {
        our_little_machine->current_instruction = our_little_machine->memory[our_little_machine->program_counter];
        our_little_machine->program_counter += 1;
        lmsm_exec_instruction(our_little_machine, our_little_machine->current_instruction);

    }
}

//======================================================
//  LMSM Implementation
//======================================================

void lmsm_exec_instruction(lmsm *our_little_machine, int instruction) {

    // TODO - dispatch the rest of the instruction set and implement
    //        the instructions above

    if (instruction == 0) {
        lmsm_i_halt(our_little_machine);
    } else if (100 <= instruction && instruction <= 199) {
        lmsm_i_add(our_little_machine, instruction - 100);
    } else if(200 <= instruction && instruction <= 299){
        lmsm_i_sub(our_little_machine, instruction - 200);
    } else if(300 <= instruction && instruction <= 399){
        lmsm_i_store(our_little_machine, instruction - 300);
    }else if(400 <= instruction && instruction <= 499){
        lmsm_i_load_immediate(our_little_machine, instruction - 400);
    }else if(500 <= instruction && instruction <= 599){
        lmsm_i_load(our_little_machine, instruction - 500);
    }else if(600 <= instruction && instruction <= 699){
        lmsm_i_branch_unconditional(our_little_machine, instruction - 600);
    }else if(700 <= instruction && instruction <= 799){
        lmsm_i_branch_if_zero(our_little_machine, instruction - 700);
    }else if(800 <= instruction && instruction <= 899){
        lmsm_i_branch_if_positive(our_little_machine, instruction - 800);
    }else if(instruction == 901){
        lmsm_i_inp(our_little_machine);
    }else if(instruction == 902){
        lmsm_i_out(our_little_machine);
    }else if(instruction == 920){
        lmsm_i_push(our_little_machine);
    }else if(instruction == 921){
        lmsm_i_pop(our_little_machine);
    }else if(instruction == 922){
        lmsm_i_dup(our_little_machine);
    }else if(instruction == 923){
        lmsm_i_sadd(our_little_machine);
    }else if(instruction == 924){
        lmsm_i_ssub(our_little_machine);
    }else if(instruction == 925){
        lmsm_i_smax(our_little_machine);
    }else if(instruction == 926){
        lmsm_i_smin(our_little_machine);
    }else if(instruction == 927){
        lmsm_i_smul(our_little_machine);
    }else if(instruction == 928){
        lmsm_i_sdiv(our_little_machine);
    }else if(instruction == 910){
        lmsm_i_call(our_little_machine);
    }else if(instruction == 911){
        lmsm_i_return(our_little_machine);
    }else {
        our_little_machine->error_code = ERROR_UNKNOWN_INSTRUCTION;
        our_little_machine->status = STATUS_HALTED;
    }
    lmsm_cap_accumulator_value(our_little_machine);
}

void lmsm_load(lmsm *our_little_machine, int *program, int length) {
    for (int i = 0; i < length; ++i) {
        our_little_machine->memory[i] = program[i];
    }
}

void lmsm_delete_stack(lmsm_stack *current) {
    if (current->next) {
        lmsm_delete_stack(current->next);
    }
    free(current);
}

void lmsm_init(lmsm *the_machine) {
    the_machine->accumulator->value = 0;
    the_machine->accumulator->next = NULL;
    the_machine->status = STATUS_READY;
    the_machine->error_code = ERROR_NONE;
    the_machine->program_counter = 0;
    the_machine->current_instruction = 0;
    the_machine->call_stack->value = 0;
    the_machine->call_stack->next = NULL;
    memset(the_machine->output_buffer, 0, sizeof(char) * 1000);
    memset(the_machine->memory, 0, sizeof(int) * 100);
}

void lmsm_reset(lmsm *our_little_machine) {
    lmsm_delete_stack(our_little_machine->accumulator);
    our_little_machine->accumulator = malloc(sizeof(lmsm_stack));
    lmsm_delete_stack(our_little_machine->call_stack);
    our_little_machine->call_stack = malloc(sizeof(lmsm_stack));
    lmsm_init(our_little_machine);
}

void lmsm_run(lmsm *our_little_machine) {
    our_little_machine->status = STATUS_RUNNING;
    while (our_little_machine->status != STATUS_HALTED) {
        lmsm_step(our_little_machine);
    }
}

lmsm *lmsm_create() {
    lmsm *the_machine = malloc(sizeof(lmsm));
    the_machine->accumulator = malloc(sizeof(lmsm_stack));
    the_machine->call_stack = malloc(sizeof(lmsm_stack));
    lmsm_init(the_machine);
    return the_machine;
}

void lmsm_delete(lmsm *the_machine) {
    lmsm_delete_stack(the_machine->accumulator);
    lmsm_delete_stack(the_machine->call_stack);
    free(the_machine);
}
