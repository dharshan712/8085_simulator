#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>

/*********** Predefined Macros **********************/

#define source_file_name "asm_code.asm"
#define source_file_mode "r"                    /*Setting the mode for file containing the assembly code - read mode*/

#define destination_file_name "asm_code.bin"
#define destination_file_mode "w"               /*Setting the mode for file going to contain the machine code - write mode*/

#define max_num_of_label 20
#define label_max_size   6                      /*Includes Semi-colon Eg-> START: */

#define instructions_array_size (13+label_max_size+4)   /*Largest Instruction in 8085 is LXI SP,1000H. To store this we need 13 bytes. 4 bytes reserved for safety. If any extra spaces arises.*/
#define mnemonic_array_size      (label_max_size+2)     /*Largest mnemonic in 8085 contains only 4 characters. Eg: XCHG , Also we are using same array to store label. So we are using max label size. Extra 1 is for '\0' and another one is reserved.*/

#define max_hex_string_size      10

#define enable 1
#define disable 0


/*********** Function Prototypes *******************/

void instructions(void);

void label_memorize(void);
void mnemonic_cmp(void);
void operation(void);
void mnemonic_opcode_generator(int,int);
void update_opcode(int);
void hex_to_dec(int,int,int);
int string_to_num(int,int);

void memory_reg_visualization(int,int);
void flags_update(void);

void dataTransfer(void);
void dataManipulation_arithmeticADD(void);
void dataManipulation_arithmeticSUB(void);
void dataManipulation_logical(void);
void branch(void);
void stack_operations(void);
void restart(void);

void add(unsigned char *);
void add_pair(void);

void increment(unsigned char *);
void increment_pair(unsigned char * , unsigned char *);

void complement(unsigned char *);
void complement_2(unsigned char *);

void sub(unsigned char *);
void decrement(unsigned char *);
void decrement_pair(unsigned char * , unsigned char *);

void logical_operation(unsigned char *);

void push(void);
void pop(void);

void execute(void);

enum registers {

    A = 0b111,
    B = 0b000,
    C = 0b001,
    D = 0b010,
    E = 0b011,
    H = 0b100,
    L = 0b101,
    M = 0b110

};

enum reg_pairs{

    BC ,
    DE,
    HL,
    SP_PSW
};

struct flag_register
{
    unsigned char carry_flag     : 1;
    unsigned char reserved_0     : 1;
    unsigned char parity_flag    : 1;
    unsigned char reserved_1     : 1;
    unsigned char auxillary_flag : 1;
    unsigned char reserved_2     : 1;
    unsigned char zero_flag      : 1;
    unsigned char sign_flag      : 1;
}flags;

struct register_set
{
    unsigned char regA;
    unsigned char regB;
    unsigned char regC;
    unsigned char regD;
    unsigned char regE;
    unsigned char regH;
    unsigned char regL;
    unsigned char regW;
    unsigned char regZ;
}reg;

char mnemonics_set[][6]={"MOV" ,"MVI" ,"LXI" ,"LDA" ,"STA" ,"LHLD","SHLD","XCHG",
                        "ADD" ,"ADC" ,"ADI" ,"ACI" ,"SUB" ,"SBB" ,"SUI" ,"SBI" ,"INR","DCR","DAA","CMA","CMC","STC",
                        "ANA" ,"XRA" ,"ORA" ,"CMP" ,"ANI" ,"XRI" ,"ORI" ,"CPI" ,
                        "JMP" ,"JNZ" ,"JZ"  ,"JNC" ,"JC"  ,"JPO" ,"JPE" ,"JP"  ,"JM" ,
                        "CALL","CNZ" ,"CZ"  ,"CNC" ,"CC"  ,"CPO" ,"CPE" ,"CP"  ,"CM" ,
                        "RET" ,"RNZ" ,"RZ"  ,"RNC" ,"RC"  ,"RPO" ,"RPE" ,"RP"  ,"RM" ,
                        "PCHL","IN"  ,"OUT" ,"XTHL","SPHL","HLT" ,"NOP" ,"RST" ,"EI" ,"DI","RIM","SIM",
                        "RLC" ,"RRC" ,"RAL" ,"RAR" ,
                        "DAD" ,"DAD" ,"DAD" ,"DAD" ,
                        "INX" ,"INX" ,"INX" ,"INX" ,
                        "DCX" ,"DCX" ,"DCX" ,"DCX" ,
                        "PUSH","PUSH","PUSH","PUSH",
                        "POP" ,"POP" ,"POP" ,"POP" ,
                        "LDAX","LDAX","STAX","STAX"
                        };

char operation_select[]={2,3,3,4,4,4,4,0,
                         1,1,4,4,1,1,4,4,1,1,0,0,0,0,
                         1,1,1,1,4,4,4,4,
                         4,4,4,4,4,4,4,4,4,
                         4,4,4,4,4,4,4,4,4,
                         0,0,0,0,0,0,0,0,0,
                         0,4,4,0,0,0,0,1,0,0,0,0,
                         0,0,0,0,
                         5,5,5,5,
                         5,5,5,5,
                         5,5,5,5,
                         5,5,5,5,
                         5,5,5,5,
                         5,5,5,5
};

char mnemonic_equivalent_opcodes[]={0x40,0x06,0x01,0x3A,0x32,0x2A,0x22,0xEB,
                                    0x80,0x88,0xC6,0xCE,0x90,0x98,0xD6,0xDE,0x04,0x05,0x27,0x2F,0x3F,0x37,
                                    0xA0,0xA8,0xB0,0xB8,0xE6,0xEE,0xF6,0xFE,
                                    0xC3,0xC2,0xCA,0xD2,0xDA,0xE2,0xEA,0xF2,0xFA,
                                    0xCD,0xC4,0xCC,0xD4,0xDC,0xE4,0xEC,0xF4,0xFC,
                                    0xC9,0xC0,0xC8,0xD0,0xD8,0xE0,0xE8,0xF0,0xF8,
                                    0xE9,0xDB,0xD3,0xE3,0xF9,0x76,0x00,0xC7,0xFB,0xF3,0x20,0x30,
                                    0x07,0x0F,0x17,0x1F,
                                    0x09,0x19,0x29,0x39,
                                    0x03,0x13,0x23,0x33,
                                    0x0B,0x1B,0x2B,0x3B,
                                    0xC5,0xD5,0xE5,0xF5,
                                    0xC1,0xD1,0xE1,0xF1,
                                    0x0A,0x1A,0x02,0x12
};


char instructions_size[]={1,2,3,3,3,3,3,1,
                          1,1,2,2,1,1,2,2,1,1,1,1,1,1,
                          1,1,1,1,2,2,2,2,
                          3,3,3,3,3,3,3,3,3,
                          3,3,3,3,3,3,3,3,3,
                          1,1,1,1,1,1,1,1,1,
                          1,2,2,1,1,1,1,1,1,1,1,1,
                          1,1,1,1,
                          1,1,1,1,
                          1,1,1,1,
                          1,1,1,1,
                          1,1,1,1,
                          1,1,1,1,
                          1,1,1,1
};

char instructions_array[instructions_array_size];
int  iterator,position;

char mnemonic_array[mnemonic_array_size];
int mnemonic_arr_iterator;

char label_set[max_num_of_label][label_max_size+2];
int  label_address[max_num_of_label];
int label_arr_iterator;
char label_flag;

char hex_string[max_hex_string_size];
int  num_of_spaces;

unsigned char equivalent_opcode;
unsigned char byte1_opcode;
char byte1_flag;

int address;
char address_flag;

int program_counter;
int program_counter_copy;
char program_counter_flag;

unsigned char opcode;

int stack_pointer = 0xFFFF;

unsigned char memory[65536];

int result;

int start_addr;
int end_addr;
char yes_no;
int step_by_step = disable;                                             //Setting the execution mode. Enable -> Step by step execution, Disable -> Single Execution

/*
Function Name: label_memorize
Functionality: Searching for labels and storing them and their address.
*/

void label_memorize()
{
    FILE *fp;
    fp = fopen(source_file_name,source_file_mode);

    if(fp == NULL)
    {
        printf("Error in Opening the ASM file");
    }
    else
    {
        while(fgets(instructions_array,sizeof(instructions_array),fp))
        {
            if(instructions_array[0]!='\n')                             //Checking whether it is empty line
            {
                mnemonic_cmp();                                         //Checking whether the line is denoted or referenced by any label
                iterator = 0;

                if(label_flag==1)
                {
                    label_flag=0;                                       //Clearing the flag

                    while(instructions_array[iterator]!=':')            //Extracting the label and storing it in the seperate array.
                    {
                        label_set[label_arr_iterator][iterator]=instructions_array[iterator];
                        iterator++;
                    }
                    label_set[label_arr_iterator][iterator]='\0';
                    label_address[label_arr_iterator] = program_counter;    //Also storing its address
                    label_arr_iterator++;
                }

                if(!program_counter_flag)
                {
                    program_counter += instructions_size[position];         //Incrementing the program counter based on the size of the instructions.
                }
                program_counter_flag=0;
            }
        }
        fclose(fp);
    }
}

/*
Function name: mnemonic_cmp
Functionality:
    1) Extracting the mnemonic
    2) Extracting the labels
    3) Initializing the program counter.
*/

void mnemonic_cmp()
{
    mnemonic_arr_iterator=0;
    iterator =0;

    while(instructions_array[iterator]!=':' && instructions_array[iterator]!=10)        //Checking whether the line is denoted or referenced by any label
    {
        iterator++;
    }

    if(instructions_array[iterator]==10)                                                //If nothing, then come back to initial position
    {
        iterator = 0;
    }
    else
    {
        iterator+=1;
        while(instructions_array[iterator]==' ')                                        //Checking for any spaces after label. Eg: SKIP: MOV A,B. The space after ':'.
        {
            iterator++;
        }
        label_flag=1;
    }

    while(instructions_array[iterator]!=' ' && instructions_array[iterator]!=10)        //Extracting the mnemoic. Eg: MOV A,B, we will iterate upto first space to get mnemonic'MOV'. Another condition for instructions like XCHG, it does not contains space, it contains end of line character 'LF' = 10.
    {
        mnemonic_array[mnemonic_arr_iterator]=instructions_array[iterator];
        iterator++;
        mnemonic_arr_iterator++;
    }
    mnemonic_array[mnemonic_arr_iterator]='\0';

    if(!strcmp(mnemonic_array,"ORG"))                                                   //Setting the address to load the program into that address.
    {
        program_counter_flag = 1;
        update_opcode(iterator);
        program_counter = address;
        program_counter_copy = program_counter;
    }
    else
    {
        position=0;
        while(strcmp(mnemonic_array,mnemonics_set[position]))                           //Finding the position of extracted mnemonic in mnemonic set array.
        {
            position++;
        }
   }
}

/*

Function Name: operation
Functionality: Based on mnemonic, it does required operation.

case 0: If the line contains only mnemonic.             Eg: XCHG.
case 1: (M)nemonic (R)egister instructions.             Eg: ADD A.
case 2: (M)nemonic (R)egister Register instructions.    Eg: MOV A,B
case 3: (M)nemonic (R)egister value instructions.       Eg: MVI A,10h
case 4: (M)nemonic (V)alue instructions.                Eg: LDA 1000h
case 5: (M)nemonic (R)egister_pair instructions.        Eg: PUSH B

*/
void operation()                                                                        //Based on mnemonic, we do required operation
{
    equivalent_opcode = mnemonic_equivalent_opcodes[position];

    switch(operation_select[position])
    {
    case 1:
        if(position==16 || position==17 || position==64)                                //As per Datasheet.
            mnemonic_opcode_generator(iterator+1,3);
        else
            mnemonic_opcode_generator(iterator+1,0);
        break;

    case 2:
        mnemonic_opcode_generator(iterator+1,3);
        while(instructions_array[iterator+3]==' ')
            iterator++;
        mnemonic_opcode_generator(iterator+3,0);
        break;

    case 3:
        mnemonic_opcode_generator(iterator+1,3);
        update_opcode(iterator);
        break;

    case 4:
        update_opcode(iterator);
        break;

    case 5:
        switch(instructions_array[iterator+1])
        {
        case 'B':
            position += BC;
            break;

        case 'D':
            position += DE;
            break;

        case 'H':
            position += HL;
            break;

        case 'S':
            position += SP_PSW;
            break;

        case 'P':
            position += SP_PSW;
        }
        equivalent_opcode =  mnemonic_equivalent_opcodes[position];
    }
}

/*
Function name: mnemonic_opcode_generator
Functionality: Generating the equivalent opcode.

    Eg: General:  MOV r1,r2 -> 0 1 D D D S S S
        CODE   :  MOV A, B -> Destination : A register, Source: B register
    Based on value stored on registers enum, code will be generated by required number of left shifts.
*/

void mnemonic_opcode_generator(int pos,int num_of_shifts)
{
    switch(instructions_array[pos])
    {
        case 'A':
            equivalent_opcode |= (A << num_of_shifts);
            break;

        case 'B':
            equivalent_opcode |= (B << num_of_shifts);
            break;

        case 'C':
            equivalent_opcode |= (C << num_of_shifts);
            break;

        case 'D':
            equivalent_opcode |= (D << num_of_shifts);
            break;

        case 'E':
            equivalent_opcode |= (E << num_of_shifts);
            break;

        case 'H':
            equivalent_opcode |= (H << num_of_shifts);
            break;

        case 'L':
            equivalent_opcode |= (L << num_of_shifts);
            break;

        case 'M':
            equivalent_opcode |= (M << num_of_shifts);
            break;

        case '0':
            equivalent_opcode |= (0 << num_of_shifts);
            break;

        case '1':
            equivalent_opcode |= (1 << num_of_shifts);
            break;

        case '2':
            equivalent_opcode |= (2 << num_of_shifts);
            break;

        case '3':
            equivalent_opcode |= (3 << num_of_shifts);
            break;

        case '4':
            equivalent_opcode |= (4 << num_of_shifts);
            break;

        case '5':
            equivalent_opcode |= (5 << num_of_shifts);
            break;

        case '6':
            equivalent_opcode |= (6 << num_of_shifts);
            break;

        case '7':
            equivalent_opcode |= (7 << num_of_shifts);
            break;

        default:
            equivalent_opcode |= (M << num_of_shifts);
            break;

    }
}

/*
Function Name: hex_to_dec
Functionality: Converting the hexadecimal number represented as string into an equivalent decimal value.
*/

void hex_to_dec(int pos,int power,int byte_select)
{
    switch(instructions_array[pos])
    {
        case 'A':
        case 'a':
            address += 10 * pow(16,power);
            break;

        case 'B':
        case 'b':
            address += 11 * pow(16,power);
            break;

        case 'C':
        case 'c':
            address += 12 * pow(16,power);
            break;

        case 'D':
        case 'd':
            address += 13 * pow(16,power);
            break;

        case 'E':
        case 'e':
            address += 14 * pow(16,power);
            break;

        case 'F':
        case 'f':
            address += 15 * pow(16,power);
            break;

        default:
            address += (instructions_array[pos]-48) * pow(16,power);
    }

    if(byte1_flag==1)
        byte1_opcode = (unsigned char)address;
}

/*
Function Name: update_opcode
Functionality: Generating the opcodes for numeric values.
*/

void update_opcode(int pos)
{
    int num_of_digits=0;

    if(position==1 || position==2)
    {
        while(instructions_array[iterator]!=',')
                    iterator++;
        pos = iterator;

        while(instructions_array[iterator+1]==' ')
            iterator++;
        pos = iterator;

    }

    if(instructions_size[position]==2)
    {
        byte1_flag = 1;
        byte1_opcode = 0;
        address_flag = 0;
        address = 0;

        if(instructions_array[pos+2]=='h'||instructions_array[pos+2]=='H')
        {
            hex_to_dec(pos+1,0,1);
        }
        else if(instructions_array[pos+3]=='h'||instructions_array[pos+3]=='H')
        {
            hex_to_dec(pos+1,1,1);
            hex_to_dec(pos+2,0,1);
        }

        else if(instructions_array[pos+2]==10)
        {
           byte1_opcode = (unsigned char)string_to_num(pos,1);
        }
        else if(instructions_array[pos+3]==10)
        {
           byte1_opcode = (unsigned char)string_to_num(pos,2);
        }
        else if(instructions_array[pos+4]==10)
        {
            byte1_opcode = (unsigned char)string_to_num(pos,3);
        }
    }
    else if(instructions_size[position]==3 || program_counter_flag)
    {
        int pos_copy = pos;
        int found = 0;
        if(instructions_array[pos+1]>57 && position!=1 && position!=2)
        {
            mnemonic_arr_iterator=0;
            pos+=1;

            while(instructions_array[pos]!=10)
            {
                if(instructions_array[pos]!=' ')
                {
                    mnemonic_array[mnemonic_arr_iterator]=instructions_array[pos];
                    mnemonic_arr_iterator++;
                }
                pos++;
            }

            mnemonic_array[mnemonic_arr_iterator]='\0';

            for(int i=0;i<label_arr_iterator;i++)
            {
                if(!strcmp(mnemonic_array,label_set[i]))
                {
                    address = label_address[i];
                    found = 1;
                }
            }
            address_flag = 1;
        }
        if(found==0)
        {
            pos = pos_copy;
            byte1_flag = 0;
            address_flag = 1;
            address = 0;
            byte1_opcode = 0;

            if(instructions_array[pos+2]=='h'||instructions_array[pos+2]=='H')
            {
                address_flag = 1;
                num_of_digits = 1;
                for(int i=1;i<=num_of_digits;i++)
                {
                    hex_to_dec(pos+i,num_of_digits-i,3);
                }
            }
            else if(instructions_array[pos+3]=='h'||instructions_array[pos+3]=='H')
            {
                address_flag = 1;
                num_of_digits = 2;
                for(int i=1;i<=num_of_digits;i++)
                {
                    hex_to_dec(pos+i,num_of_digits-i,3);
                }
            }
            else if(instructions_array[pos+4]=='h'||instructions_array[pos+4]=='H')
            {
                address_flag = 1;
                num_of_digits = 3;
                for(int i=1;i<=num_of_digits;i++)
                {
                    hex_to_dec(pos+i,num_of_digits-i,3);
                }
            }
            else if(instructions_array[pos+5]=='h'||instructions_array[pos+5]=='H')
            {
                address_flag = 1;
                num_of_digits = 4;
                for(int i=1;i<=num_of_digits;i++)
                {
                    hex_to_dec(pos+i,num_of_digits-i,3);
                }
            }
            else if(instructions_array[pos+2]==10)
            {
                address = string_to_num(pos,1);
            }
            else if(instructions_array[pos+3]==10)
            {
                address = string_to_num(pos,2);
            }
            else if(instructions_array[pos+4]==10)
            {
                address = string_to_num(pos,3);
            }
            else if(instructions_array[pos+5]==10)
            {
                address = string_to_num(pos,4);
            }
            else if(instructions_array[pos+6]==10)
            {
                address = string_to_num(pos,5);
            }
        }
    }
}

/*
Function Name: string_to_num
Functionality: Converting the string into an equivalent decimal value.
*/

int string_to_num(int pos,int num_of_digits)
{
    int numeric_value=0;

    for(int i=0;i<num_of_digits;i++)
    {
        numeric_value += (instructions_array[pos+i+1]-48)*(pow(10,num_of_digits-i-1));
    }

    return numeric_value;
}

/*
Function Name: memory_reg_visualization
Functionality: To visualize the values stored in registers and specified memory location.
*/

void memory_reg_visualization(int memory_pos1,int memory_pos2)
{
    //reg.regA = 5;
    //flags.parity_flag =1;
    //memory[32853] = 0xAB;
    printf("\n                    Registers                  |                      Flags                      ");
    printf("\n");
    printf("\n                A : 0x%.2x                       |           Carry Flag    : %d                    ",reg.regA,flags.carry_flag);
    printf("\n                B : 0x%.2x                       |           Parity Flag   : %d                    ",reg.regB,flags.parity_flag);
    printf("\n                C : 0x%.2x                       |           Auxillary Flag: %d                    ",reg.regC,flags.auxillary_flag);
    printf("\n                D : 0x%.2x                       |           Zero Flag     : %d                    ",reg.regD,flags.zero_flag);
    printf("\n                E : 0x%.2x                       |           Sign Flag     : %d                    ",reg.regE,flags.sign_flag);
    printf("\n                H : 0x%.2x                       |                                                 ",reg.regH);
    printf("\n                L : 0x%.2x                       |                                                 ",reg.regL);
    printf("\n              PSW : 0x%.2x                       |                                                 ",flags);
    printf("\n");
    printf("\n   Program Counter: 0x%.4x                     |           Stack Pointer : 0x%.4x                ",program_counter,stack_pointer);
    printf("\n");
    printf("-----------------------------------------------------------------------------------------------------");
    printf("\n                                            Memory View                                          \n");
    printf("-----------------------------------------------------------------------------------------------------");
    printf("\n         0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F              ");
    printf("\n");
    for(int i=memory_pos1;i<=memory_pos2;i=i+16)
    {
        printf("\n0x%.4x : ",i);
        for(int j=i;j<i+16;j++)
        {
            printf("%.2x   ",memory[j]);
        }
    }
    printf("\n-----------------------------------------------------------------------------------------------------\n");
}

/*
Function Name: execute
Functionality: Based on Opcode, it does required operation.
*/

void execute()
{
    program_counter = program_counter_copy;
    opcode = memory[program_counter];
    while(opcode!=0x76)                                                                 //Executing until we reach HLT Instruction, whose equivalent opcode is 0x76
    {
       opcode = memory[program_counter];

       if((opcode >= 0x40 && opcode <= 0x7F)||                                          // 1. MOV register , register or memory     2. MOV memory, register
           opcode == 0x3E || opcode == 0x06 || opcode == 0x0E || opcode == 0x16 ||      // MVI register or memory , Immediate data
           opcode == 0x1E || opcode == 0x26 || opcode == 0x2E || opcode == 0x36 ||
           opcode == 0x01 || opcode == 0x11 || opcode == 0x21 || opcode == 0x31 ||      // Load Immediate (reg.pair)
           opcode == 0x0A || opcode == 0x1A || opcode == 0x02 || opcode == 0x12 ||      // Load/Store A In-direct Eg: LDAX B
           opcode == 0x3A || opcode == 0x32 ||                                          // Load/Store A direct    Eg: LDA addr
           opcode == 0x2A || opcode == 0x22 ||                                          // Load/Store HL direct   Eg: LHLD addr
           opcode == 0xEB )                                                             // Exchange HL with DE
       {
           dataTransfer();
       }
       else if((opcode >= 0x80 && opcode <= 0x8F)||                                         // 1. ADD register or memory      2. ADC register or memory
                opcode == 0xC6 || opcode == 0xCE ||                                         // Add Immediate
                opcode == 0x09 || opcode == 0x19 || opcode == 0x29 || opcode == 0x39 ||     // Double Length Add
                opcode == 0x3C || opcode == 0x04 || opcode == 0x0C || opcode == 0x14 ||     // Increment
                opcode == 0x1C || opcode == 0x24 || opcode == 0x2C || opcode == 0x34 ||
                opcode == 0x03 || opcode == 0x13 || opcode == 0x23 || opcode == 0x33        //Increment Register Pair
               )
       {
           dataManipulation_arithmeticADD();
       }
       else if((opcode >= 0x90 && opcode <= 0x9F)||                                         // 1.SUB register or memory       2. SBB register or memory
                opcode == 0xD6 || opcode == 0xDE ||                                         // Subtract Immediate
                opcode == 0x3D || opcode == 0x05 || opcode == 0x0D || opcode == 0x15 ||     // Decrement
                opcode == 0x1D || opcode == 0x25 || opcode == 0x2D || opcode == 0x35 ||
                opcode == 0x0B || opcode == 0x1B || opcode == 0x2B || opcode == 0x3B        // Decrement Register Pair
               )
       {
           dataManipulation_arithmeticSUB();
       }
       else if((opcode >= 0xA0 && opcode <= 0xBF)||                                         // AND OR XOR CMP register or memory
                opcode == 0xE6 || opcode == 0xEE || opcode == 0xF6 || opcode == 0xFE ||     // Logical Immediate
                opcode == 0x07 || opcode == 0x0F || opcode == 0x17 || opcode == 0x1F        // Rotate
               )
       {
           dataManipulation_logical();
       }
       else if(opcode == 0x2F)                                                              // Complement Accumulator
       {
           complement(&reg.regA);
       }
       else if(opcode == 0x3F)                                                              // Complement Carry
       {
           if(flags.carry_flag)
                flags.carry_flag=0;
            else
                flags.carry_flag=1;
       }
       else if(opcode == 0x37)                                                              // Set Carry
       {
           flags.carry_flag=1;
       }
       else if(opcode == 0xC3 || opcode == 0xC2 || opcode == 0xCA || opcode == 0xD2 || opcode == 0xDA ||    // Jump
               opcode == 0xE2 || opcode == 0xEA || opcode == 0xF2 || opcode == 0xFA ||
               opcode == 0xCD || opcode == 0xC4 || opcode == 0xCC || opcode == 0xD4 || opcode == 0xDC ||    // Call
               opcode == 0xE4 || opcode == 0xEC || opcode == 0xF4 || opcode == 0xFC ||
               opcode == 0xC9 || opcode == 0xC0 || opcode == 0xC8 || opcode == 0xD0 || opcode == 0xD8 ||    // Return
               opcode == 0xE0 || opcode == 0xE8 || opcode == 0xF0 || opcode == 0xF8 ||
               opcode == 0xE9
               )
       {
           branch();
       }
       else if(opcode == 0xC5 || opcode == 0xD5 || opcode == 0xE5 || opcode == 0xF5 ||                      // Push
               opcode == 0xC1 || opcode == 0xD1 || opcode == 0xE1 || opcode == 0xF1 ||                      // POP
               opcode == 0xE3 ||                                                                            // XTHL
               opcode == 0xF9                                                                               // SPHL
               )
       {
           stack_operations();
       }
       else if(opcode == 0x00)                                                                              // NOP
       {

       }
       else if(opcode == 0xC7 || opcode == 0xCF || opcode == 0xD7 || opcode == 0xDF ||                      // Restart
               opcode == 0xE7 || opcode == 0xEF || opcode == 0xF7 || opcode == 0xFF
               )
       {
           restart();
       }
       else if(opcode == 0x27)                                                                              // DAA
       {
           reg.regW = 0x06;

            if(flags.auxillary_flag==1 && flags.carry_flag==1)
            {
                reg.regW = 0x66;
                add(&reg.regW);
            }
            else if((flags.auxillary_flag==1 || (reg.regA & 0x0F)>9 ))
            {
                add(&reg.regW);
                if(flags.auxillary_flag==1)
                {
                    if(((reg.regA>>4)&0x0F) > 9)
                    {
                        reg.regW = 0x60;
                        add(&reg.regW);
                        flags.auxillary_flag =1;
                    }
                }
                else
                {
                    if(((reg.regA>>4)&0x0F) > 9)
                    {
                        reg.regW = 0x60;
                        add(&reg.regW);
                    }
                }
            }
            else if(flags.carry_flag==1 || (((reg.regA>>4)&0x0F) > 9) )
            {
                reg.regW = 0x60;
                add(&reg.regW);
            }
       }
        program_counter++;
        opcode = memory[program_counter];
        if(step_by_step)
        {
            printf("\nNeed to visualize Output? ");
            scanf(" %c",&yes_no);
            if(yes_no=='y')
            {
                printf("Please enter the required starting and ending address of memory in decimal values: ");
                scanf("%d %d",&start_addr,&end_addr);
                memory_reg_visualization(start_addr,end_addr);
            }

            printf("\nNeed to run continuosly? ");
            scanf(" %c",&yes_no);
            if(yes_no=='y')
                step_by_step = 0;
        }
    }
}

void restart()
{
    switch(memory[program_counter])
    {
    case 0xC7:
        push();
        program_counter = -1;
        break;

    case 0xCF:
        push();
        program_counter = (1<<3)-1;
        break;

    case 0xD7:
        push();
        program_counter = (2<<3)-1;
        break;

    case 0xDF:
        push();
        program_counter = (3<<3)-1;
        break;

    case 0xE7:
        push();
        program_counter = (4<<3)-1;
        break;

    case 0xEF:
        push();
        program_counter = (5<<3)-1;
        break;

    case 0xF7:
        push();
        program_counter = (6<<3)-1;
        break;

    case 0xFF:
        push();
        program_counter = (7<<3)-1;
        break;
    }
}

void stack_operations()
{
    switch(memory[program_counter])
    {
    case 0xC5:
    case 0xD5:
    case 0xE5:
    case 0xF5:
        push();
        break;

    case 0xC1:
    case 0xD1:
    case 0xE1:
    case 0xF1:
        pop();
        break;

    case 0xE3:
        reg.regZ = memory[stack_pointer];
        reg.regW = memory[stack_pointer+1];
        memory[stack_pointer]=reg.regL;
        memory[stack_pointer+1]=reg.regH;
        reg.regL = reg.regZ;
        reg.regH = reg.regW;
        break;

    case 0xF9:
        program_counter = (reg.regH << 8)+reg.regL -1;
        break;
    }
}

void branch()
{
    switch(memory[program_counter])
    {
    case 0xC3:
        program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        break;

    case 0xC2:
        if(flags.zero_flag==0)
        {
            program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        }
        else
        {
            program_counter += 2;
        }
        break;

    case 0xCA:
        if(flags.zero_flag!=0)
        {
            program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        }
        else
        {
            program_counter += 2;
        }
        break;

    case 0xD2:
        if(flags.carry_flag==0)
        {
            program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        }
        else
        {
            program_counter += 2;
        }
        break;

    case 0xDA:
        if(flags.carry_flag!=0)
        {
            program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        }
        else
        {
            program_counter += 2;
        }
        break;

    case 0xE2:
        if(flags.parity_flag==0)
        {
            program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        }
        else
        {
            program_counter += 2;
        }
        break;

    case 0xEA:
        if(flags.parity_flag!=0)
        {
            program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        }
        else
        {
            program_counter += 2;
        }
        break;

    case 0xF2:
        if(flags.sign_flag==0)
        {
            program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        }
        else
        {
            program_counter += 2;
        }
        break;

    case 0xFA:
        if(flags.sign_flag!=0)
        {
            program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        }
        else
        {
            program_counter += 2;
        }
        break;

    case 0xCD:
        push();
        program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        break;

    case 0xC9:
        pop();
        break;

    case 0xC4:
        if(flags.zero_flag==0)
        {
            push();
            program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        }
        else
        {
            program_counter += 2;
        }
        break;

    case 0xC0:
        if(flags.zero_flag==0)
            pop();
        break;

    case 0xCC:
        if(flags.zero_flag==1)
        {
            push();
            program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        }
        else
        {
            program_counter += 2;
        }
        break;

    case 0xC8:
        if(flags.zero_flag==1)
            pop();
        break;

    case 0xD4:
        if(flags.carry_flag==0)
        {
            push();
            program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        }
        else
        {
            program_counter += 2;
        }
        break;

    case 0xD0:
        if(flags.carry_flag==0)
            pop();
        break;

    case 0xDC:
        if(flags.carry_flag==1)
        {
            push();
            program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        }
        else
        {
            program_counter += 2;
        }
        break;

    case 0xD8:
        if(flags.carry_flag==1)
            pop();
        break;

    case 0xE4:
        if(flags.parity_flag==0)
        {
            push();
            program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        }
        else
        {
            program_counter += 2;
        }
        break;

    case 0xE0:
        if(flags.parity_flag==0)
            pop();
        break;

    case 0xEC:
        if(flags.parity_flag==1)
        {
            push();
            program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        }
        else
        {
            program_counter += 2;
        }
        break;

    case 0xE8:
        if(flags.parity_flag==1)
            pop();
        break;

    case 0xF4:
        if(flags.sign_flag==0)
        {
            push();
            program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        }
        else
        {
            program_counter += 2;
        }
        break;

    case 0xF0:
        if(flags.sign_flag==0)
            pop();
        break;

    case 0xFC:
        if(flags.sign_flag==1)
        {
            push();
            program_counter = (memory[program_counter+2]<<8)+memory[program_counter+1]-1;
        }
        else
        {
            program_counter += 2;
        }
        break;

    case 0xF8:
        if(flags.sign_flag==1)
            pop();
        break;

    case 0xE9:
        program_counter = (reg.regH << 8)+reg.regL - 1;
        break;
    }
}

void dataManipulation_logical()
{
    switch(memory[program_counter])
    {
    case 0xA7:
    case 0xB7:
    case 0xAF:
        logical_operation(&reg.regA);
        break;

    case 0xA0:
    case 0xB0:
    case 0xA8:
        logical_operation(&reg.regB);
        break;

    case 0xA1:
    case 0xB1:
    case 0xA9:
        logical_operation(&reg.regC);
        break;

    case 0xA2:
    case 0xB2:
    case 0xAA:
        logical_operation(&reg.regD);
        break;

    case 0xA3:
    case 0xB3:
    case 0xAB:
        logical_operation(&reg.regE);
        break;

    case 0xA4:
    case 0xB4:
    case 0xAC:
        logical_operation(&reg.regH);
        break;

    case 0xA5:
    case 0xB5:
    case 0xAD:
        logical_operation(&reg.regL);
        break;

    case 0xA6:
    case 0xB6:
    case 0xAE:
        address = (reg.regH << 8) + reg.regL;
        reg.regW = memory[address];
        logical_operation(&reg.regW);
        memory[address]=reg.regW;
        break;

    case 0xE6:
    case 0xEE:
    case 0xF6:
        reg.regW = memory[program_counter+1];
        logical_operation(&reg.regW);
        program_counter++;
        break;

    case 0xBF:
        reg.regW = reg.regA;
        complement(&reg.regW);
        sub(&reg.regW);
        break;

    case 0xB8:
        reg.regW = reg.regB;
        complement(&reg.regW);
        sub(&reg.regW);
        break;

    case 0xB9:
        reg.regW = reg.regC;
        complement(&reg.regW);
        sub(&reg.regW);
        break;

    case 0xBA:
        reg.regW = reg.regD;
        complement(&reg.regW);
        sub(&reg.regW);
        break;

    case 0xBB:
        reg.regW = reg.regE;
        complement(&reg.regW);
        sub(&reg.regW);
        break;

    case 0xBC:
        reg.regW = reg.regH;
        complement(&reg.regW);
        sub(&reg.regW);
        break;

    case 0xBD:
        reg.regW = reg.regL;
        complement(&reg.regW);
        sub(&reg.regW);
        break;

    case 0xBE:
        address = (reg.regH << 8)+reg.regL;
        reg.regW = memory[address];
        complement(&reg.regW);
        sub(&reg.regW);
        break;

    case 0xFE:
        reg.regW = memory[program_counter+1];
        complement(&reg.regW);
        sub(&reg.regW);
        program_counter++;
        break;

    case 0x07:
        flags.carry_flag = (reg.regA >> 7)&0x01;
        if(flags.carry_flag==1)
        {
            reg.regA -= 128;
            reg.regA = reg.regA << 1;
            reg.regA += 1;
        }
        else
            reg.regA = reg.regA << 1;
        break;

    case 0x0F:
        flags.carry_flag = (reg.regA)&0x01;
        if(flags.carry_flag==1)
        {
            reg.regA = reg.regA >> 1;
            reg.regA += 128;
        }
        else
            reg.regA = reg.regA >> 1;
        break;

    case 0x17:
        result = ( reg.regA << 1 ) + flags.carry_flag;
        if(result > 255)
        {
            flags.carry_flag = 1;
            result -= 256;
        }
        else
        {
            flags.carry_flag = 0;
        }
        reg.regA = result;
        break;

    case 0x1F:
        if(flags.carry_flag==1 && (reg.regA&0x01))
        {
            reg.regA = reg.regA >> 1;
            reg.regA += 128;
        }
        else if(flags.carry_flag==0 && (reg.regA&0x01))
        {
            flags.carry_flag =1;
            reg.regA = reg.regA >> 1;
        }
        else if(flags.carry_flag==1 && !(reg.regA&0x01))
        {
            flags.carry_flag = 0;
            reg.regA = reg.regA >> 1;
            reg.regA += 128;
        }
        else
        {
            reg.regA = reg.regA >> 1;
        }
        break;
    }
}

void dataManipulation_arithmeticSUB()
{
    switch(memory[program_counter])
    {
    case 0x97:
        reg.regW = reg.regA;
        complement(&reg.regW);
        sub(&reg.regW);
        break;

    case 0x90:
        reg.regW = reg.regB;
        complement(&reg.regW);
        sub(&reg.regW);
        break;

    case 0x91:
        reg.regW = reg.regC;
        complement(&reg.regW);
        sub(&reg.regW);
        break;

    case 0x92:
        reg.regW = reg.regD;
        complement(&reg.regW);
        sub(&reg.regW);
        break;

    case 0x93:
        reg.regW = reg.regE;
        complement(&reg.regW);
        sub(&reg.regW);
        break;

    case 0x94:
        reg.regW = reg.regH;
        complement(&reg.regW);
        sub(&reg.regW);
        break;

    case 0x95:
        reg.regW = reg.regL;
        complement(&reg.regW);
        sub(&reg.regW);
        break;

    case 0x96:
        address = (reg.regH << 8)+reg.regL;
        reg.regW = memory[address];
        complement(&reg.regW);
        sub(&reg.regW);
        break;

    case 0x9F:
        reg.regW = reg.regA + flags.carry_flag;
        complement(&reg.regW);
        complement_2(&reg.regW);
        sub(&reg.regW);
        break;

    case 0x98:
        reg.regW = reg.regB + flags.carry_flag;
        complement(&reg.regW);
        complement_2(&reg.regW);
        sub(&reg.regW);
        break;

    case 0x99:
        reg.regW = reg.regC + flags.carry_flag;
        complement(&reg.regW);
        complement_2(&reg.regW);
        sub(&reg.regW);
        break;

    case 0x9A:
        reg.regW = reg.regD + flags.carry_flag;
        complement(&reg.regW);
        complement_2(&reg.regW);
        sub(&reg.regW);
        break;

    case 0x9B:
        reg.regW = reg.regE + flags.carry_flag;
        complement(&reg.regW);
        complement_2(&reg.regW);
        sub(&reg.regW);
        break;

    case 0x9C:
        reg.regW = reg.regH + flags.carry_flag;
        complement(&reg.regW);
        complement_2(&reg.regW);
        sub(&reg.regW);
        break;

    case 0x9D:
        reg.regW = reg.regL + flags.carry_flag;
        complement(&reg.regW);
        complement_2(&reg.regW);
        sub(&reg.regW);
        break;

    case 0x9E:
        address = (reg.regH << 8)+reg.regL;
        reg.regW = memory[address]+flags.carry_flag;
        complement(&reg.regW);
        complement_2(&reg.regW);
        sub(&reg.regW);
        break;

    case 0xD6:
        reg.regW = memory[program_counter+1];
        complement(&reg.regW);
        sub(&reg.regW);
        program_counter++;
        break;

    case 0xDE:
        reg.regW = memory[program_counter+1]+flags.carry_flag;
        complement(&reg.regW);
        complement_2(&reg.regW);
        sub(&reg.regW);
        program_counter++;
        break;

    case 0x3D:
        decrement(&reg.regA);
        break;

    case 0x05:
        decrement(&reg.regB);
        break;

    case 0x0D:
        decrement(&reg.regC);
        break;

    case 0x15:
        decrement(&reg.regD);
        break;

    case 0x1D:
        decrement(&reg.regE);
        break;

    case 0x25:
        decrement(&reg.regH);
        break;

    case 0x2D:
        decrement(&reg.regL);
        break;

    case 0x35:
        address = (reg.regH << 8)+reg.regL;
        reg.regW = memory[address];
        decrement(&reg.regW);
        memory[address] = reg.regW;
        break;

    case 0x0B:
        decrement_pair(&reg.regB,&reg.regC);
        break;

    case 0x1B:
        decrement_pair(&reg.regD,&reg.regE);
        break;

    case 0x2B:
        decrement_pair(&reg.regH,&reg.regL);
        break;

    case 0x3B:
        reg.regW = (stack_pointer >> 8) & 0x00FF ;
        reg.regZ = (stack_pointer) & 0x00FF ;
        decrement_pair(&reg.regW,&reg.regZ);
        stack_pointer = (reg.regW << 8) + reg.regZ;
        break;
    }
}

void dataManipulation_arithmeticADD()
{
    switch(memory[program_counter])
    {
    case 0x87:
    case 0x8F:
        add(&reg.regA);
        break;

    case 0x80:
    case 0x88:
        add(&reg.regB);
        break;

    case 0x81:
    case 0x89:
        add(&reg.regC);
        break;

    case 0x82:
    case 0x8A:
        add(&reg.regD);
        break;

    case 0x83:
    case 0x8B:
        add(&reg.regE);
        break;

    case 0x84:
    case 0x8C:
        add(&reg.regH);
        break;

    case 0x85:
    case 0x8D:
        add(&reg.regL);
        break;

    case 0x86:
    case 0x8E:
        address = (reg.regH << 8)+reg.regL;
        reg.regW = memory[address];
        add(&reg.regW);
        break;

    case 0xC6:
    case 0xCE:
        reg.regW = memory[program_counter+1];
        add(&reg.regW);
        program_counter++;
        break;

    case 0x09:
        reg.regW = reg.regB;
        reg.regZ = reg.regC;
        add_pair();
        break;

    case 0x19:
        reg.regW = reg.regD;
        reg.regZ = reg.regE;
        add_pair();
        break;

    case 0x29:
        reg.regW = reg.regH;
        reg.regZ = reg.regL;
        add_pair();
        break;

    case 0x39:
        reg.regW = (stack_pointer >> 8) & 0x00FF ;
        reg.regZ = (stack_pointer) & 0x00FF ;
        add_pair();
        break;

    case 0x3C:
        increment(&reg.regA);
        break;

    case 0x04:
        increment(&reg.regB);
        break;

    case 0x0C:
        increment(&reg.regC);
        break;

    case 0x14:
        increment(&reg.regD);
        break;

    case 0x1C:
        increment(&reg.regE);
        break;

    case 0x24:
        increment(&reg.regH);
        break;

    case 0x2C:
        increment(&reg.regL);
        break;

    case 0x34:
        address = (reg.regH << 8)+reg.regL;
        reg.regW = memory[address];
        increment(&reg.regW);
        memory[address] = reg.regW;
        break;

    case 0x03:
        increment_pair(&reg.regB,&reg.regC);
        break;

    case 0x13:
        increment_pair(&reg.regD,&reg.regE);
        break;

    case 0x23:
        increment_pair(&reg.regH,&reg.regL);
        break;

    case 0x33:
        reg.regW = (stack_pointer >> 8) & 0x00FF ;
        reg.regZ = (stack_pointer) & 0x00FF ;
        increment_pair(&reg.regW,&reg.regZ);
        stack_pointer = (reg.regW << 8) + reg.regZ;
        break;
    }
}

void dataTransfer()
{
    address = (reg.regH << 8)+reg.regL;

    switch(memory[program_counter])
    {
    case 0x7F:
        reg.regA = reg.regA;
        break;

    case 0x78:
        reg.regA = reg.regB;
        break;

    case 0x79:
        reg.regA = reg.regC;
        break;

    case 0x7A:
        reg.regA = reg.regD;
        break;

    case 0x7B:
        reg.regA = reg.regE;
        break;

    case 0x7C:
        reg.regA = reg.regH;
        break;

    case 0x7D:
        reg.regA = reg.regL;
        break;

    case 0x7E:
        reg.regA = memory[address];
        break;

    case 0x47:
        reg.regB = reg.regA;
        break;

    case 0x40:
        reg.regB = reg.regB;
        break;

    case 0x41:
        reg.regB = reg.regC;
        break;

    case 0x42:
        reg.regB = reg.regD;
        break;

    case 0x43:
        reg.regB = reg.regE;
        break;

    case 0x44:
        reg.regB = reg.regH;
        break;

    case 0x45:
        reg.regB = reg.regL;
        break;

    case 0x46:
        reg.regB = memory[address];
        break;

    case 0x4F:
        reg.regC = reg.regA;
        break;

    case 0x48:
        reg.regC = reg.regB;
        break;

    case 0x49:
        reg.regC = reg.regC;
        break;

    case 0x4A:
        reg.regC = reg.regD;
        break;

    case 0x4B:
        reg.regC = reg.regE;
        break;

    case 0x4C:
        reg.regC = reg.regH;
        break;

    case 0x4D:
        reg.regC = reg.regL;
        break;

    case 0x4E:
        reg.regC = memory[address];
        break;

    case 0x57:
        reg.regD = reg.regA;
        break;

    case 0x50:
        reg.regD = reg.regB;
        break;

    case 0x51:
        reg.regD = reg.regC;
        break;

    case 0x52:
        reg.regD = reg.regD;
        break;

    case 0x53:
        reg.regD = reg.regE;
        break;

    case 0x54:
        reg.regD = reg.regH;
        break;

    case 0x55:
        reg.regD = reg.regL;
        break;

    case 0x56:
        reg.regD = memory[address];
        break;

    case 0x5F:
        reg.regE = reg.regA;
        break;

    case 0x58:
        reg.regE = reg.regB;
        break;

    case 0x59:
        reg.regE = reg.regC;
        break;

    case 0x5A:
        reg.regE = reg.regD;
        break;

    case 0x5B:
        reg.regE = reg.regE;
        break;

    case 0x5C:
        reg.regE = reg.regH;
        break;

    case 0x5D:
        reg.regE = reg.regL;
        break;

    case 0x5E:
        reg.regE = memory[address];
        break;

    case 0x67:
        reg.regH = reg.regA;
        break;

    case 0x60:
        reg.regH = reg.regB;
        break;

    case 0x61:
        reg.regH = reg.regC;
        break;

    case 0x62:
        reg.regH = reg.regD;
        break;

    case 0x63:
        reg.regH = reg.regE;
        break;

    case 0x64:
        reg.regH = reg.regH;
        break;

    case 0x65:
        reg.regH = reg.regL;
        break;

    case 0x66:
        reg.regH = memory[address];
        break;

    case 0x6F:
        reg.regL = reg.regA;
        break;

    case 0x68:
        reg.regL = reg.regB;
        break;

    case 0x69:
        reg.regL = reg.regC;
        break;

    case 0x6A:
        reg.regL = reg.regD;
        break;

    case 0x6B:
        reg.regL = reg.regE;
        break;

    case 0x6C:
        reg.regL = reg.regH;
        break;

    case 0x6D:
        reg.regL = reg.regL;
        break;

    case 0x6E:
        reg.regL = memory[address];
        break;

    case 0x77:
        memory[address] = reg.regA;
        break;

    case 0x70:
        memory[address] = reg.regB;
        break;

    case 0x71:
        memory[address] = reg.regC;
        break;

    case 0x72:
        memory[address] = reg.regD;
        break;

    case 0x73:
        memory[address] = reg.regE;
        break;

    case 0x74:
        memory[address] = reg.regH;
        break;

    case 0x75:
        memory[address] = reg.regL;
        break;

    case 0x3E:
        program_counter++;
        reg.regA = memory[program_counter];
        break;

    case 0x06:
        program_counter++;
        reg.regB = memory[program_counter];
        break;

    case 0x0E:
        program_counter++;
        reg.regC = memory[program_counter];
        break;

    case 0x16:
        program_counter++;
        reg.regD = memory[program_counter];
        break;

    case 0x1E:
        program_counter++;
        reg.regE = memory[program_counter];
        break;

    case 0x26:
        program_counter++;
        reg.regH = memory[program_counter];
        break;

    case 0x2E:
        program_counter++;
        reg.regL = memory[program_counter];
        break;

    case 0x36:
        program_counter++;
        memory[address] = memory[program_counter];
        break;

    case 0x01:
        program_counter++;
        reg.regC = memory[program_counter];
        program_counter++;
        reg.regB = memory[program_counter];
        break;

    case 0x11:
        program_counter++;
        reg.regE = memory[program_counter];
        program_counter++;
        reg.regD = memory[program_counter];
        break;

    case 0x21:
        program_counter++;
        reg.regL = memory[program_counter];
        program_counter++;
        reg.regH = memory[program_counter];
        break;

    case 0x31:
        program_counter++;
        stack_pointer = memory[program_counter];
        program_counter++;
        stack_pointer += (memory[program_counter] << 8);
        break;

    case 0x0A:
        address = (reg.regB << 8) + reg.regC;
        reg.regA = memory[address];
        break;

    case 0x1A:
        address = (reg.regD << 8) + reg.regE;
        reg.regA = memory[address];
        break;

    case 0x02:
        address = (reg.regB << 8) + reg.regC;
        memory[address]=reg.regA;
        break;

    case 0x12:
        address = (reg.regD << 8) + reg.regE;
        memory[address]=reg.regA;
        break;

    case 0x3A:
        program_counter++;
        address = memory[program_counter];
        program_counter++;
        address += (memory[program_counter] << 8);
        reg.regA = memory[address];
        break;

    case 0x32:
        program_counter++;
        address = memory[program_counter];
        program_counter++;
        address += (memory[program_counter] << 8);
        memory[address] = reg.regA;
        break;

    case 0x2A:
        program_counter++;
        address = memory[program_counter];
        program_counter++;
        address += (memory[program_counter] << 8);
        reg.regL = memory[address];
        reg.regH = memory[address+1];
        break;

    case 0x22:
        program_counter++;
        address = memory[program_counter];
        program_counter++;
        address += (memory[program_counter] << 8);
        memory[address] = reg.regL;
        memory[address+1] = reg.regH;
        break;

    case 0xEB:
        reg.regW = reg.regD;
        reg.regZ = reg.regE;
        reg.regD = reg.regH;
        reg.regE = reg.regL;
        reg.regH = reg.regW;
        reg.regL = reg.regZ;
        break;
    }
}

void flags_update()
{
    int no_of_ones=0;

    if(result == 0)
    {
        flags.zero_flag = 1;
    }
    else
        flags.zero_flag = 0;

    if(result >= 0x80)
    {
        flags.sign_flag = 1;
    }
    else
        flags.sign_flag = 0;

    while(result>0)
    {
        if(result%2==1)
            no_of_ones++;
        result /= 2;
    }

    if(no_of_ones%2==0)
        flags.parity_flag=1;
    else
        flags.parity_flag=0;

}

void add(unsigned char *selected_reg)
{
    result = 0;

    if(memory[program_counter]<0x88 || memory[program_counter]==0xC6 || memory[program_counter]==0x27)
    {
        result = *selected_reg + reg.regA;

        if((*selected_reg & 0x0F)+(reg.regA & 0x0F) > 15)
        {
            flags.auxillary_flag = 1;
        }
        else
        {
            flags.auxillary_flag = 0;
        }

        if(result > 255)
        {
            result -= 256;
            flags.carry_flag = 1;
        }
        else
        {
            flags.carry_flag = 0;
        }

        reg.regA = result;
        flags_update();
    }
    else
    {
        result = *selected_reg + reg.regA + flags.carry_flag;

        if((*selected_reg & 0x0F)+(reg.regA & 0x0F)+ flags.carry_flag > 15)
        {
            flags.auxillary_flag = 1;
        }
        else
        {
            flags.auxillary_flag = 0;
        }

        if(result > 255)
        {
            result -= 256;
            flags.carry_flag = 1;
        }
        else
        {
            flags.carry_flag = 0;
        }

        reg.regA = result;
        flags_update();
    }
}



void add_pair()
{
    result = ((reg.regH << 8) + reg.regL) + ((reg.regW << 8) + reg.regZ);

    if(result > 65535)
    {
        result -= 65536;
        flags.carry_flag = 1;
    }
    else
    {
        flags.carry_flag = 0;
    }

    reg.regL = result & 0x00FF;
    reg.regH = (result >> 8) & 0x00FF;
}

void increment(unsigned char *selected_reg)
{
    result = *selected_reg + 1;

    if((*selected_reg & 0x0F)+ 1 > 15)
    {
        flags.auxillary_flag = 1;
    }
    else
    {
        flags.auxillary_flag = 0;
    }

    if(result > 255)
    {
        result -= 256;
    }

    *selected_reg = result;
    flags_update();
}

void increment_pair(unsigned char *selected_reg1,unsigned char *selected_reg2)
{
    result = (*selected_reg1 << 8)+*selected_reg2+1;

    if(result > 65535)
    {
        result -= 65536;
    }

    *selected_reg2 = result & 0x00FF;
    *selected_reg1 = (result >> 8) & 0x00FF;
}

void complement(unsigned char *selected_reg)
{
    *selected_reg = 255 - *selected_reg;
}

void complement_2(unsigned char *selected_reg)
{
    *selected_reg +=1;
    if(*selected_reg>255)
    {
        *selected_reg -= 256;
    }
}

void sub(unsigned char *selected_reg)
{
    result = 0;

    if(memory[program_counter]<0x98 || memory[program_counter]==0xD6 || (memory[program_counter]>=0xB8 && memory[program_counter]<=0xBF)|| memory[program_counter]==0xFE)
    {
        result = *selected_reg + reg.regA+1;
        if((*selected_reg & 0x0F)+(reg.regA & 0x0F)+1 > 15)
        {
            flags.auxillary_flag = 1;
        }
        else
        {
            flags.auxillary_flag = 0;
        }

        if(result > 255)
        {
            result -= 256;
            flags.carry_flag = 0;
        }
        else
        {
            flags.carry_flag = 1;
        }
        if(!((memory[program_counter]>=0xB8 && memory[program_counter]<=0xBF)||memory[program_counter]==0xFE))
            reg.regA = result;
        flags_update();
    }
    else
    {
        result = *selected_reg + reg.regA;
        if((*selected_reg & 0x0F)+(reg.regA & 0x0F) > 15)
        {
            flags.auxillary_flag = 1;
        }
        else
        {
            flags.auxillary_flag = 0;
        }

        if(result > 255)
        {
            result -= 256;
            flags.carry_flag = 0;
            reg.regA = result;
        }
        else
        {
            flags.carry_flag = 1;
            reg.regA = result;
        }
        flags_update();
    }
}

void decrement(unsigned char *selected_reg)
{
    result = *selected_reg + 0xFF;
    if((*selected_reg & 0x0F)+ 0x0F > 15)
    {
        flags.auxillary_flag = 1;
    }
    else
    {
        flags.auxillary_flag = 0;
    }

    if(result > 255)
    {
        result -= 256;
    }
    *selected_reg = result;
    flags_update();
}

void decrement_pair(unsigned char *selected_reg1,unsigned char *selected_reg2)
{
    result = (*selected_reg1 << 8)+*selected_reg2-1;
    if(result > 65535)
    {
        result -= 65536;
    }
    *selected_reg2 = result & 0x00FF;
    *selected_reg1 = (result >> 8) & 0x00FF;
}

void logical_operation(unsigned char *selected_reg)
{
    if((memory[program_counter]>=0xA0 && memory[program_counter]<=0xA7) || memory[program_counter]==0xE6)
    {
        reg.regA &= *selected_reg;
    }
    else if((memory[program_counter]>=0xB0 && memory[program_counter]<=0xB7) || memory[program_counter]==0xF6)
    {
        reg.regA |= *selected_reg;
        flags.auxillary_flag = 0;
    }
    else if((memory[program_counter]>=0xA8 && memory[program_counter]<=0xAF) || memory[program_counter]==0xEE)
    {
        reg.regA ^= *selected_reg;
        flags.auxillary_flag = 0;
    }
    result = reg.regA;
    flags.carry_flag = 0;
    flags_update();
}

void push()
{
    stack_pointer--;
    switch(memory[program_counter])
    {
    case 0xC5:
        memory[stack_pointer] = reg.regB;
        stack_pointer--;
        memory[stack_pointer] = reg.regC;
        break;

    case 0xD5:
        memory[stack_pointer] = reg.regD;
        stack_pointer--;
        memory[stack_pointer] = reg.regE;
        break;

    case 0xE5:
        memory[stack_pointer] = reg.regH;
        stack_pointer--;
        memory[stack_pointer] = reg.regL;
        break;

    case 0xF5:
        memory[stack_pointer] = reg.regA;
        stack_pointer--;
        memory[stack_pointer] = flags.carry_flag+(flags.parity_flag<<2)+(flags.auxillary_flag<<4)+(flags.zero_flag<<6)+(flags.sign_flag<<7);;
        break;

    default:
         memory[stack_pointer] =  ((program_counter+3)>>8)&0x00FF;
         stack_pointer--;
         memory[stack_pointer] = (program_counter+3) & 0x00FF;
         break;
    }

}

void pop()
{
    switch(memory[program_counter])
    {
    case 0xC1:
        reg.regC = memory[stack_pointer];
        stack_pointer++;
        reg.regB = memory[stack_pointer];
        break;

    case 0xD1:
        reg.regE = memory[stack_pointer];
        stack_pointer++;
        reg.regD = memory[stack_pointer];
        break;

    case 0xE1:
        reg.regL = memory[stack_pointer];
        stack_pointer++;
        reg.regH = memory[stack_pointer];
        break;

    case 0xF1:
        flags.carry_flag = memory[stack_pointer] & 0x01;
        flags.parity_flag = (memory[stack_pointer]>>2) & 0x01;
        flags.auxillary_flag = (memory[stack_pointer]>>4) & 0x01;
        flags.zero_flag = (memory[stack_pointer]>>6) & 0x01;
        flags.sign_flag = (memory[stack_pointer]>>7) & 0x01;
        stack_pointer++;
        reg.regA = memory[stack_pointer];
        break;

    default:
        program_counter = memory[stack_pointer];
        stack_pointer++;
        program_counter += (memory[stack_pointer]<<8)-1;
        break;
    }
    stack_pointer++;
}

void instructions()
{
    printf("/******************************************************************************************************/");
    printf("\n                                   8085 Assembler & Simulator");

    printf("\nInstructions: \n");
    printf("\n      1 -> Enter your assembly code in .asm file and keep that file in the same folder, ");
    printf("\n           where your .c & .h files are available.");
    printf("\n      2 -> Enter the Assembly code only in UPPER CASE letters.");
    printf("\n               Eg: MVI A,24H ");
    printf("\n      3 -> Assembly code should always ends with HLT instructions.");
    printf("\n      4 -> If you need to specify the memory location where you need to flash your code, ");
    printf("\n           use ORG Assembler directive at starting of your ASM code to specify the address.\n");
    printf("\n      5 -> Structure of code should be ");
    printf("\n                    ORG 16-bit_address ");
    printf("\n                    your_asm_code      ");
    printf("\n                         ...           ");
    printf("\n                    HLT               \n");
    printf("\n      6 -> To represent Hexadecimal number use only suffix H or h, do not use prefix 0x");
    printf("\n               Eg: MVI A,24H \n");
    printf("\n      7 -> If you need to execute your code step_by_step, just enable the step_by_step global variable.");
    printf("\n               In main.h file, you will find step_by_step global variable, for that just assign -> enable.");
    printf("\n      8 -> To execute your code in single run, just assign disable to step_by_step global variable.\n");
    printf("\n      9 -> If you need to switch from step_by_step to single run mode, just enter character y, ");
    printf("\n           when console displays -> Need to run continuosly?");
    printf("\n     10 -> To visualize the output (registers & memory) , just enter character y, ");
    printf("\n           when console displays -> Need to visualize output? \n");
    printf("\n     11 -> Enter the required starting and ending address of memory in decimal formal");
    printf("\n                Eg: If Required memory location is 0800H to 08FF, just enter: 2048 2303\n");
    printf("\n     12 -> After execution, you will find your machine code in .bin file, ");
    printf("\n           which will be created automatically by the code.It will be located in same folder.");
    printf("\n/******************************************************************************************************/\n");
}


