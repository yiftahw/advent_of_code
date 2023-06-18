#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/*
part a:
the cpu holds:
- 1 register named x - 32 bits signed, starts at 1
- 1 cycle counter - 32 bits unsigned, starts at 1

2 commands:
- noop - does nothing, takes 1 cycle
- addx v - adds v to x, takes 2 cycle

we need to calculate the "signal strength" (x * cycle number) of the given program at cycles 20, 60, 100, 140, 180, 220
*/

#define TARGET_CYCLES_COUNT 6
#define FIRST_TARGET_CYCLE 20
#define TARGET_CYCLE_STEP 40

#define CPU_INITIALIZER {1, 1, 0, NULL}

typedef struct
{
    int32_t x;
    uint32_t cycle;
    uint32_t signal_strength_sum;
    uint32_t* target_cycles;
} cpu_t;

typedef enum
{
    NOOP,
    ADDX,
    LAST_VALID_OPCODE = ADDX
} opcode_t;

typedef struct
{
    opcode_t opcode;
    int32_t value; // only used for ADDX
} instruction_t;

instruction_t parse_line(const char* line)
{
    /* the function assumes the structure of addx and noop commands,
    a more robust solution would use strtok here */
    instruction_t instruction = {0};
    if (!line)
    {
        printf("instruction is null\n");
        return instruction;
    }
    if (strncmp(line, "noop", 4) == 0)
    {
        instruction.opcode = NOOP;
    }
    else if (strncmp(line, "addx", 4) == 0)
    {
        instruction.opcode = ADDX;
        instruction.value = atoi(line + 5);
    }
    else
    {
        printf("invalid instruction: %s\n", line);
    }
    return instruction;
}

void exec_instruction(cpu_t* cpu, instruction_t instruction)
{
    if(!cpu || !cpu->target_cycles || !instruction.opcode > LAST_VALID_OPCODE)
    {
        printf("exec_instruction(): invalid input!\n");
        return;
    }
    uint32_t cycles = 0;
    while (true)
    {
        // check if the current cycle is a target cycle
        for (int i = 0; i < TARGET_CYCLES_COUNT; i++)
        {
            if (cpu->cycle == cpu->target_cycles[i])
            {
                cpu->signal_strength_sum += cpu->x * cpu->cycle;
            }
        }

        // tick the cycle counter
        cpu->cycle++;
        cycles++;

        // execute the instructions
        if (instruction.opcode == ADDX && cycles == 2)
        {
            cpu->x += instruction.value;
            break;
        }
        else if (instruction.opcode == NOOP && cycles == 1)
        {
            break;
        }
    }
}

int main()
{
    // open input file
    char line_buffer[64] = {0};
    const char* input = "input.txt";
    FILE* file = fopen(input, "r");
    if (!file)
    {
        printf("failed to open file: %s\n", input);
        return 1;
    }

    // init cpu
    cpu_t cpu = CPU_INITIALIZER;
    cpu.target_cycles = calloc(TARGET_CYCLES_COUNT, sizeof(uint32_t));
    for (int i = 0; i < TARGET_CYCLES_COUNT; i++)
    {
        cpu.target_cycles[i] = FIRST_TARGET_CYCLE + i * TARGET_CYCLE_STEP;
    }

    // parse instructions
    while (fgets(line_buffer, sizeof(line_buffer), file))
    {
        instruction_t instruction = parse_line(line_buffer);
        exec_instruction(&cpu, instruction);
    }

    // part a:
    printf("part a: %u\n", cpu.signal_strength_sum);
}