#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

/*
part a:
the cpu holds:
- 1 register named x_register - 32 bits signed, starts at 1
- 1 cycle counter - 32 bits unsigned, starts at 1

2 commands:
- noop - does nothing, takes 1 cycle
- addx v - adds v to x_register, takes 2 cycle

we need to calculate the "signal strength" (x_register * cycle number) of the given program at cycles 20, 60, 100, 140, 180, 220
*/

/*
part b:
the screen size is 40 x 6
the global cycle counter represents the pixel being rendered [divide by the screen width to get the row]
need to render a pixel to the screen
a sprite with a width of 3 pixels starts at the top left corner, 
during each cycle, the pixel is ON if the sprite covers the current pixel being rendered,
the sprite position is determind by the x register value.
*/

#define TARGET_CYCLES_COUNT 6
#define FIRST_TARGET_CYCLE 20
#define TARGET_CYCLE_STEP 40

#define CRT_WIDTH 40
#define CRT_HRIGHT 6

const char OFF_PIXEL = '.';
const char ON_PIXEL = '#';

typedef struct
{
    int32_t x_register;
    uint32_t signal_strength_sum;
    uint32_t* target_cycles;
} cpu_t;

#define CPU_INITIALIZER {1, 0, NULL}

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

void exec_instruction(uint32_t* global_cycle_counter, cpu_t* cpu, instruction_t instruction)
{
    if(!cpu || !cpu->target_cycles || !instruction.opcode > LAST_VALID_OPCODE)
    {
        printf("exec_instruction(): invalid input!\n");
        return;
    }
    uint32_t cycles = 0;
    while (true)
    {
        // part a
        // check if the current cycle is a target cycle
        for (int i = 0; i < TARGET_CYCLES_COUNT; i++)
        {
            if ((*global_cycle_counter) == cpu->target_cycles[i])
            {
                cpu->signal_strength_sum += cpu->x_register * (*global_cycle_counter);
            }
        }

        // part b
        // determine if the current pixel is ON
        // the x register marks the middle of the sprite (should always be between 1 and 40)
        //! @todo extract the pixels to a 2d array and print it later
        char current_pixel_state = OFF_PIXEL;
        uint32_t current_pixel_x_pos = (((*global_cycle_counter) - 1) % CRT_WIDTH);// + 1;
        uint32_t current_pixel_y_pos = (((*global_cycle_counter) - 1) / CRT_WIDTH);// + 1;

        if (current_pixel_x_pos == 0) // start of new line
        {
            printf("\n");
        }

        for(uint32_t i = 0; i < 3; i++)
        {
            uint32_t offset = -1 + i;
            //uint32_t spirte_pixel_i = ((cpu->x_register + offset - 1) % CRT_WIDTH) + 1;
            uint32_t spirte_pixel_i = ((cpu->x_register + offset) % CRT_WIDTH);
            if (current_pixel_x_pos == spirte_pixel_i)
            {
                current_pixel_state = ON_PIXEL;
            }
        }
        // print the pixel
        printf("%c", current_pixel_state);
        

        // tick the cycle counter
        (*global_cycle_counter)++;
        cycles++;

        // execute the instructions
        if (instruction.opcode == ADDX && cycles == 2)
        {
            cpu->x_register += instruction.value;
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
    uint32_t global_cycle_counter = 1;
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
        exec_instruction(&global_cycle_counter, &cpu, instruction);
    }

    // part a:
    printf("\npart a: %u\n", cpu.signal_strength_sum);
}