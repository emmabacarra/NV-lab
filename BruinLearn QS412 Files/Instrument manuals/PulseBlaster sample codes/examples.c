
// Sample Pulse Generation Instructions
// A simple program to generate a square pulse will have two intervals as shown below:

// start= pb_inst(0xFFFFFF, CONTINUE, 0, 200.0*ms); 
// pb_inst(0x000000, BRANCH, start, 200.0*ms);

// The first line of the code above corresponds to the logical "one” on all output bits.  The second line
// corresponds to the logical "zero," after which the program branches (jumps) back to the beginning, thus
// resulting in a continuous generation of a square wave on all outputs.

// A complete C program will have, in addition to the two lines above, the initialization section, the
// closing section, and, optionally, the (software) trigger to start the execution immediately upon launch
// of the program.  For more detailed information on programming the board consult the section above for all 
// SpinAPI C functions or the examples below for complete usable programs.

//*
* PulseBlaster example 2
* This example makes use of all instructions (except WAIT).
*/
#include <stdio.h>
#define PB24
#include <spinapi.h>

int main(int argc, char **argv){
    int start, loop, sub;
    int status;

        printf ("Using spinapi library version %s\n", pb_get_version());
        if(pb_init() != 0) {
          printf ("Error initializing board: %s\n", pb_get_error());   
            return -1;
        }

      // Tell the driver what clock frequency the board has (in MHz)
        pb_core_clock(100.0);

        pb_start_programming(PULSE_PROGRAM);

        // Since we are going to jump forward in our program, we need to
      // define this variable by hand.  Instructions start at 0 and count up
    sub = 5;

        // Instruction format
        // int pb_inst(int flags, int inst, int inst_data, int length)

        // Instruction 0 - Jump to Subroutine at Instruction 5 in 1s
        start =    pb_inst(0xFFFFFF,JSR, sub, 1000.0 * ms);

        // Loop. Instructions 1 and 2 will be repeated 3 times
        // Instruction 1 - Beginning of Loop (Loop 3 times).  Continue to next instruction in 150ms
        loop =    pb_inst(0x0,LOOP,3,150.0 * ms);
   
    // Instruction 2 - End of Loop.  Return to beginning of loop or    
    // Continue to next instruction in 150ms
        pb_inst(0xFFFFFF,END_LOOP,loop,150.0 * ms);

        // Instruction 3 - Stay here for (5*100ms) then continue to Instruction
    // 4
        pb_inst(0x0,LONG_DELAY,5, 100.0 * ms);

        // Instruction 4 - Branch to "start" (Instruction 0) in 1 s
        pb_inst(0x0,BRANCH,start,1000.0*ms);

    // Subroutine
        // Instruction 5 - Continue to next instruction in .5 * s
        pb_inst(0x0,CONTINUE,0,500.0*ms);
   
    // Instruction 6 - Return from Subroutine to Instruction 1 in .5*s
        pb_inst(0xF0F0F0,RTS,0,500.0*ms);

        // End of pulse program
        pb_stop_programming();

        // Trigger the pulse program
    pb_start();

        //Read the status register
        status = pb_read_status();
        printf("status = %d", status);

        pb_close();
       
    return 0;
}

