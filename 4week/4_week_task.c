
void Task(void * pdata) {
	INT8U err;

	INT8U push_number;
	int get_number[4];
	
	INT8U i, j;

	INT8U min;		// Variable to store the minimum value
	INT8U min_task;	
	int task_number = (int)(*(char*)pdata-48); // index of each task (Since pdata is of char type, subtracting 48 (ASCII '0') converts it to an int)

	char push_letter; 
	char get_letter; 

	int fgnd_color, bgnd_color;

	char s[10];

	// If pdata is 0-3, it's a random task, if it's 4, decision task.
	if (*(char*)pdata == '4') { // decision task
		for (;;) {
			for (i = 0;  i < N_TASK - 1; i++) {
				if (select == 1) {
					// Wait until the random number arrives. Store the value in the get_number array.
					INT8U* received_data = (INT8U*)OSMboxPend(mbox_to_decision[i], 0, &err);
					get_number[i] = *received_data;
				}
				else if (select == 2) {
					INT8U* received_data;
					OSQPend(queue_to_decision, 0, &err);
					received_data = (INT8U*)OSQPend(queue_to_decision, 0, &err);
					get_number[i] = *received_data;
				}
			}
			
			// Find the smallest number among the 4 random numbers sent by the random tasks.
			min = get_number[0];
			min_task = 0;
			for (i = 1; i < N_TASK - 1; i++) {
				if (get_number[i] < min) {
					min = get_number[i];
					min_task = i;
				}
			}
			
			// Send W or L to each random task
			for (i = 0; i < N_TASK - 1; i++) {
				if (i == min_task) {
					push_letter = 'W';
				}
				else {
					push_letter = 'L';
				}

				if (select == 1) {
					// Send W or L to the random task.
					OSMboxPost(mbox_to_random[i], (void*)&push_letter);
				}
				else if (select == 2) {
					OSQPost(queue_to_random, (void*)&push_letter);
				}
			}
			OSTimeDlyHMSM(0, 0, 5, 0);
		}
	}
	else { // Random tasks (0-3)
		for (;;) {
			// Generate random number from 0 to 63
			push_number = random(64);
			sprintf(s, "%2d", push_number);

			// Display task number and generated number
			PC_DispStr(0 + 18 * task_number, 4, "task", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			PC_DispChar(4 + 18 * task_number, 4, *(char*)pdata, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			PC_DispStr(6 + 18 * task_number, 4, s, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);

			if (select == 1) {
				// Post random number to decision task and wait for response
				OSMboxPost(mbox_to_decision[task_number], (void*)&push_number);
				char* received_letter = (char*)OSMboxPend(mbox_to_random[task_number], 0, &err);
				get_letter = *received_letter;
			}
			else if (select == 2) {
				OSQPost(queue_to_decision, (void*)&push_number);
				char* received_letter = (char*)OSQPend(queue_to_random, 0, &err);
				get_letter = *received_letter;
			}
			
			// Set colors based on task priority (task number)
			if (*(char*)pdata == '0') {
				bgnd_color = DISP_BGND_RED;
				fgnd_color = DISP_FGND_RED;
			}
			else if (*(char*)pdata == '1') {
				bgnd_color = DISP_BGND_CYAN;  // Using cyan instead of yellow as yellow might not be available
				fgnd_color = DISP_FGND_CYAN;
			}
			else if (*(char*)pdata == '2') {
				bgnd_color = DISP_BGND_BLUE;
				fgnd_color = DISP_FGND_BLUE;
			}
			else if (*(char*)pdata == '3') {
				bgnd_color = DISP_BGND_GREEN;
				fgnd_color = DISP_FGND_GREEN;
			}
			
			// Display the result letter (W or L)
			PC_DispStr(8 + 18 * task_number, 4, "[", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			PC_DispStr(10 + 18 * task_number, 4, "]", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			PC_DispChar(9 + 18 * task_number, 4, get_letter, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);

			// If this task won (received 'W'), fill the screen with its color
			if (get_letter == 'W') {
				for (j = 5; j < 20; j++) {
					for (i = 0; i < 80; i++) {
						PC_DispChar(i, j, ' ', fgnd_color + bgnd_color);
					}
				}
			}
			OSTimeDlyHMSM(0, 0, 5, 0);
		}
	}
}