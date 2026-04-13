/* Copyright (c) 2009 SpinCore Technologies, Inc.
 *   http://www.spincore.com
 *
 * This software is provided 'as-is', without any express or implied warranty. 
 * In no event will the authors be held liable for any damages arising from the 
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, 
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software in a
 * product, an acknowledgment in the product documentation would be appreciated
 * but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */
 
/** 
 * \file dds2_wait_test.c
 * \brief 
 * The example program tests the WAIT opcode on the PBDDS-II.
 * This program will produce the pulse pattern of a chain of 10 pulses that are high
 * for 50 microseconds, low for 50 microseconds, and then waits for a user trigger.
 * After triggering the pulse pattern changes to a chain of 10 pulses that are high
 * for 10 microseconds, low for 10 microseconds.
 * \ingroup ddsII
 */

#include <stdlib.h>
#include <stdio.h>

#include "spinapi.h"

#define CLOCK_FREQ 75.0

// User friendly names for the control bits
#define FLAG_STATES (0x40000+0x08)
#define START_LOCATION (0x40000+0x07)

int detect_boards();
int select_board(int numBoards);

int
main (int argc, char *argv[])
{
	int start, status, loop2;
	int numBoards;
	
	//Uncommenting the line below will generate a debug log in your current
	//directory that can help debug any problems that you may be experiencing   
	//pb_set_debug(1); 

	printf ("Copyright (c) 2010 SpinCore Technologies, Inc.\n\n");
    	
	printf("Using SpinAPI library version %s\n", pb_get_version());
	
	if((numBoards = detect_boards()) > 1) { /*If there is more than one board in the system, have the user specify.*/
		select_board(numBoards); /*Request the board numbet to use from the user*/
	}   
	
	if (pb_init () != 0) {
		printf ("Error initializing board: %s\n", pb_get_error ());
		system("pause");
		return -1;
	}
		
    printf("This example program tests the TTL outputs and the WAIT functionality of the PBDDS-II.\n\n");
    system("pause");
    printf("\n\n");
 
	pb_set_defaults ();
	pb_core_clock (CLOCK_FREQ);
	
	pb_write_register (FLAG_STATES, 0x0);
	pb_write_register (START_LOCATION, 0);

//The next section is where the actual pulse program is written:
//The line below shows which parameter/argument corresponds to which register or flag.

/***** SPINCORE_API int pb_inst_dds2(int freq0, int phase0, int amp0, int dds_en0, int phase_reset0, int freq1, int phase1, int amp1, int dds_en1, int phase_reset1, int flags, int inst, int inst_data, double length) ****/

  pb_start_programming (PULSE_PROGRAM);	//This line is used to start programming the pulse program.

/********************************************************************************************************
* Each piece of the pulse program is specified by a pb_inst_dds2(...) command                           *
* Here, all parameters related to the analog outputs (the first ten) are set to zero.                   *
*                                                                                                       *
********************************************************************************************************/

    // Instruction 0 - "LOOP1" - Loop 10 times, instruction delay 50ms.
	// Flags = 0xFFF, OPCODE = LOOP
	start = pb_inst_dds2 (0,0,0,0,0,0,0,0,0,0, 0xfff, LOOP, 10, 50.0 * ms);
    
	// Instruction 1 - End the loop, delay 50ms.
	// Flags = 0x000, OPCODE = END_LOOP
	pb_inst_dds2 (0,0,0,0,0,0,0,0,0,0, 0x000, END_LOOP, start, 50.0 * ms);

	// Instruction 2 - Wait for a hardware or software trigger.
	// Flags = 0x000, OPCODE = WAIT
	pb_inst_dds2 (0,0,0,0,0,0,0,0,0,0, 0x000, WAIT, 0, 50.0 * ms);
	
	// Instruction 3 - "LOOP2" - Loop 10 times, instruction delay 10ms.
	// Flags = 0xFFF, OPCODE = LOOP
	loop2 = pb_inst_dds2 (0,0,0,0,0,0,0,0,0,0, 0xfff, LOOP, 10, 10.0 * ms);
    
	// Instruction 4 - End the loop, delay 50ms.
	// Flags = 0x000, OPCODE = END_LOOP
	pb_inst_dds2 (0,0,0,0,0,0,0,0,0,0, 0x000, END_LOOP, loop2, 50.0 * ms);
	
	// Instruction 5 - Wait for a hardware or software trigger.
	// Flags = 0x000, OPCODE = WAIT
	pb_inst_dds2 (0,0,0,0,0,0,0,0,0,0, 0x000, WAIT, 0, 50.0 * ms);
	
	// Instruction 6 - Branch back to the beginning of the program.
	// Flags = 0x000, OPCODE = BRANCH
	pb_inst_dds2 (0,0,0,0,0,0,0,0,0,0, 0x000, BRANCH, start, 50.0 * ms);
    
    pb_stop_programming ();	// This line ends the pulse program

	
/*********** END PULSE PROGRAM **************************************************************************/    

	// Reset the board and read back the status
	pb_reset();  

	status = pb_read_status();
	printf("status: %d \n", status);
	printf(pb_status_message());
	printf("\n");
	system("pause");
	
	// Send a software trigger (i.e. pb_start()) to the board to begin execution of the program	and 
	// Read the status, board should be running
	// loop1 (or start) will run first
	pb_start();

	status = pb_read_status();
	printf("status: %d \n", status);
	printf(pb_status_message());
	printf("\n");
	system("pause");
	
	// Re-read the status, board should be waiting	
	status = pb_read_status();
	printf("status: %d \n", status);
	printf(pb_status_message());
	printf("\n");
	system("pause");
	
	
	// Re-trigger the board and check the status, board should be running
	// loop2 will run (shorter pulses should be visible on the scope)
	pb_start();
	
	status = pb_read_status();
	printf("status: %d \n", status);
	printf(pb_status_message());
	printf("\n");
	system("pause");
	
	// Re-read the status, board should be waiting	
	status = pb_read_status();
	printf("status: %d \n", status);
	printf(pb_status_message());
	printf("\n");
	system("pause");
	
	// Stop the board and read the status
	pb_stop();
	
	status = pb_read_status();
	printf("status: %d \n", status);
	printf(pb_status_message());
	printf("\n");
	system("pause");
	
    pb_close();

	return 0;
}

int
detect_boards()
{
	int numBoards;

	numBoards = pb_count_boards();	/*Count the number of boards */

    if (numBoards <= 0) {
		printf("No Boards were detected in your system. Verify that the board is correctly powered and connected.\n\n");
		system("PAUSE");
		exit(-1);
	}
	
	return numBoards;
}

int
select_board(int numBoards)
{
	int choice;
	
	do {
		printf("Found %d boards in your system. Which board should be used? (0-%d): ",numBoards, numBoards - 1);
		fflush(stdin);
		scanf("%d", &choice);
		
		if (choice < 0 || choice >= numBoards) {
			printf("Invalid Board Number (%d).\n", choice);
		}
	} while (choice < 0 || choice >= numBoards);

	pb_select_board(choice);
	printf("Board %d selected.\n", choice);
	
	return 0;
}
