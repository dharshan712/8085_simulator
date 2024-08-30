#include "main.h"

int main()
{
    instructions();

    label_memorize();

    FILE *fp,*destination_fp;

    fp = fopen(source_file_name,source_file_mode);                          //Opening the source file and destination file in their respective mode.
    destination_fp = fopen(destination_file_name,destination_file_mode);

    if(fp == NULL)
    {
        printf("Error in Opening the ASM file");
    }
    else
    {
        while(fgets(instructions_array,sizeof(instructions_array),fp))
        {
            if(instructions_array[0]!='\n')
            {
                mnemonic_cmp();                                             //Extracting the mnemonic and comparing it with mnemonic set and extracting its equivalent opcode.

                if(!program_counter_flag)                                   //Checking whether it is a program counter initialization assembler directive.The flag will be set in mnemonic_cmp function.
                {
                    operation();
                }

                if(destination_fp==NULL)
                {
                    printf("Error in Opening the destination file");
                }
                else if(!program_counter_flag)
                {
                    sprintf(hex_string, "0x%04X : ", program_counter);      //Converting into an hexadecimal string and writing into an destination file.
                    fputs(hex_string,destination_fp);

                    if(instructions_size[position]==1)                      //If it is 1 byte instruction
                    {
                        sprintf(hex_string, "%02X ", equivalent_opcode);
                        fputs(hex_string,destination_fp);

                        memory[program_counter] = equivalent_opcode;
                    }
                    else if(byte1_flag==1)                                  //If it is 2 byte instruction
                    {
                        byte1_flag=0;

                        sprintf(hex_string, "%02X ", equivalent_opcode);
                        fputs(hex_string,destination_fp);
                        memory[program_counter] = equivalent_opcode;

                        sprintf(hex_string, "%02X ", byte1_opcode);
                        fputs(hex_string,destination_fp);
                        memory[program_counter+1] = byte1_opcode;
                    }
                    else if(address_flag==1)                                //If it is 4 byte instruction
                    {
                        sprintf(hex_string, "%02X ", equivalent_opcode);
                        fputs(hex_string,destination_fp);
                        memory[program_counter] = equivalent_opcode;

                        sprintf(hex_string, "%04X ", address);
                        fputc(hex_string[2],destination_fp);
                        fputc(hex_string[3],destination_fp);
                        memory[program_counter+1] = (unsigned char)(address & 0xFF);

                        fputc(' ',destination_fp);

                        fputc(hex_string[0],destination_fp);
                        fputc(hex_string[1],destination_fp);
                        memory[program_counter+2] = (unsigned char)((address>>8) & 0xFF);
                    }
                    program_counter += instructions_size[position];
                    num_of_spaces=0;                                        //Doing for alignment purpose in destination file.

                    if(instructions_size[position]==1)
                        num_of_spaces = 5;
                    else if(instructions_size[position]==2)
                        num_of_spaces = 2;

                    for(int i=0;i<(10+num_of_spaces);i++)
                        fputc(' ',destination_fp);

                    fputs(instructions_array,destination_fp);
                }
                 program_counter_flag = 0;
            }
            fputc('\n',destination_fp);                                     //Entering into a New line in destination file.
        }
         printf("Successfully generated the Machine Code and the program is loaded at memory location %.4x\n",program_counter_copy);

         printf("\nNeed to visualize Output? ");
         scanf(" %c",&yes_no);
         if(yes_no=='y')
            memory_reg_visualization(program_counter_copy,(program_counter_copy+0x00FF));
    }

    fclose(fp);                                                              //Closing the files.
    fclose(destination_fp);

    printf("\nExecution Begins");                                           //8085 Simulator
    execute();

    printf("\nNeed to visualize Output? ");
    scanf(" %c",&yes_no);
    if(yes_no=='y')
    {
        printf("Please enter the required starting and ending address of memory in decimal values: ");
        scanf("%d %d",&start_addr,&end_addr);
        memory_reg_visualization(start_addr,end_addr);
    }
    return 3;
}
