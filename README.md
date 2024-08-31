                                         "8085 Assembler & Simulator"

    Instructions: 
        1 -> Enter your assembly code in .asm file and keep that file in the same folder, where your .c & .h files are available.
        2 -> Enter the Assembly code only in UPPER CASE letters.
                   Eg: MVI A,24H 
        3 -> Assembly code should always ends with HLT instructions.
        4 -> If you need to specify the memory location where you need to flash your code,  use ORG Assembler directive at starting of your ASM code to specify the address.
        5 -> Structure of code should be 
                      ORG 16-bit_address 
                      your_asm_code      
                           ...           
                      HLT               
        6 -> To represent Hexadecimal number use only suffix H or h, do not use prefix 0x
                      Eg: MVI A,24H
        7 -> If you need to execute your code step_by_step, just enable the step_by_step global variable.
                   In main.h file, you will find step_by_step global variable, for that just assign -> enable.
        8 -> To execute your code in single run, just assign disable to step_by_step global variable.
        9 -> If you need to switch from step_by_step to single run mode, just enter character y, 
               when console displays -> Need to run continuosly?
        10 -> To visualize the output (registers & memory) , just enter character y, 
               when console displays -> Need to visualize output? 
        11 -> Enter the required starting and ending address of memory in decimal format
               Eg: If Required memory location is 0800H to 08FF, just enter: 2048 2303
        12 -> After execution, you will find your machine code in .bin file, 
               which will be created automatically by the code.It will be located in same folder.
    
