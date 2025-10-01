

#include "includes.h"
#include <time.h>


#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)            */
#define  N_TASKS                         1       /* Number of identical tasks                          */


OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
OS_STK        TaskStartStk[TASK_STK_SIZE];


void  Task(void* data);                       /* Function prototypes of tasks                  */
void  TaskStart(void* data);                  /* Function prototypes of Startup task           */
static  void  TaskStartCreateTasks(void* data);
static  void  TaskStartDispInit(void);
static  void  TaskStartDisp(void);

/*$PAGE*/


void  main(void)
{
    srand(time(NULL));
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    OSTaskCreate(TaskStart, (void*)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSStart();                                             /* Start multitasking                       */
}


void  TaskStart(void* pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    char       s[100];
    INT16S     key;
    char ch;

    printf("Madatory : 1, Optional : 2\n");
    scanf("%c", &ch);

    pdata = pdata;                                         /* Prevent compiler warning                 */

    TaskStartDispInit();                                   /* Initialize the display                   */

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    OSStatInit();                                          /* Initialize uC/OS-II's statistics         */


    TaskStartCreateTasks((void*)&ch);                                /* Create all the application tasks         */

    for (;;) {
        TaskStartDisp();                                  /* Update the display                       */


        if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
            if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn();                            /* Return to DOS                            */
            }
        }

        OSCtxSwCtr = 0;                                    /* Clear context switch counter             */
        OSTimeDlyHMSM(0, 0, 1, 0);                         /* Wait one second                          */
    }
}

/*$PAGE*/


static  void  TaskStartDispInit(void)
{
    /*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
    /*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
    PC_DispStr(0, 0, "                         uC/OS-II, The Real-Time Kernel                         ", DISP_FGND_WHITE + DISP_BGND_RED + DISP_BLINK);
    PC_DispStr(0, 1, "                                Jean J. Labrosse                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 2, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 3, "                                    EXAMPLE #1                                  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 4, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 5, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 6, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 7, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 8, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 9, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 10, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 11, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 12, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 13, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 14, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 15, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 16, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 17, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 18, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 19, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 20, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 21, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 22, "#Tasks          :        CPU Usage:     %                                       ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 23, "#Task switch/sec:                                                               ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 24, "                            <-PRESS 'ESC' TO QUIT->                             ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY + DISP_BLINK);
    /*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
    /*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
}

/*$PAGE*/


static  void  TaskStartDisp(void)
{
    char   s[80];


    sprintf(s, "%5d", OSTaskCtr);                                  /* Display #tasks running               */
    PC_DispStr(18, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

#if OS_TASK_STAT_EN > 0
    sprintf(s, "%3d", OSCPUUsage);                                 /* Display CPU usage in %               */
    PC_DispStr(36, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);
#endif

    sprintf(s, "%5d", OSCtxSwCtr);                                 /* Display #context switches per second */
    PC_DispStr(18, 23, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

    sprintf(s, "V%1d.%02d", OSVersion() / 100, OSVersion() % 100); /* Display uC/OS-II's version number    */
    PC_DispStr(75, 24, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

    switch (_8087) {                                               /* Display whether FPU present          */
    case 0:
        PC_DispStr(71, 22, " NO  FPU ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        break;

    case 1:
        PC_DispStr(71, 22, " 8087 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        break;

    case 2:
        PC_DispStr(71, 22, "80287 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        break;

    case 3:
        PC_DispStr(71, 22, "80387 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        break;
    }
}

/*$PAGE*/


static  void  TaskStartCreateTasks(void* data)
{
    OSTaskCreate(Task, data, &TaskStk[0][TASK_STK_SIZE - 1], 1);
}

// Define maximum random number and tables
INT8U  const  myMapTbl[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
INT8U  const  myUnMapTbl[] = {
  0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x00 to 0x0F                             */
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x10 to 0x1F                             */
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x20 to 0x2F                             */
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x30 to 0x3F                             */
  6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x40 to 0x4F                             */
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x50 to 0x5F                             */
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x60 to 0x6F                             */
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x70 to 0x7F                             */
  7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x80 to 0x8F                             */
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x90 to 0x9F                             */
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xA0 to 0xAF                             */
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xB0 to 0xBF                             */
  6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xC0 to 0xCF                             */
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xD0 to 0xDF                             */
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xE0 to 0xEF                             */
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0        /* 0xF0 to 0xFF                             */
};

INT8U const maxUnMapTbl[] = {
  0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,       /* 0x00 to 0x0F                             */
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,       /* 0x10 to 0x1F                             */
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,       /* 0x20 to 0x2F                             */
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,       /* 0x30 to 0x3F                             */
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,       /* 0x40 to 0x4F                             */
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,       /* 0x50 to 0x5F                             */
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,       /* 0x60 to 0x6F                             */
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,       /* 0x70 to 0x7F                             */
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,       /* 0x80 to 0x8F                             */
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,       /* 0x90 to 0x9F                             */
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,       /* 0xA0 to 0xAF                             */
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,       /* 0xB0 to 0xBF                             */
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,       /* 0xC0 to 0xCF                             */
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,       /* 0xD0 to 0xDF                             */
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,       /* 0xE0 to 0xEF                             */
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7        /* 0xF0 to 0xFF                             */
};

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/




void clearScr() {
    INT8U y;
    for (y = 5; y <= 20; y++) {
        PC_DispClrRow(y, DISP_BGND_LIGHT_GRAY);
    }
}

/*
*********************************************************************************************************
*                                            rd64 FUNCTION
*********************************************************************************************************
*/
INT8U rd64(INT8U* myRdyTbl, INT8U* myRdyGrp) {
    INT8U rand_num;
    INT8U head, bit;

    while (1) {
        // Generate random number (0~63)
        rand_num = rand() % 64;
        head = rand_num >> 3;  // Divide by 8 to get group (0-7)
        bit = rand_num & 0x07; // Get bit position within group (0-7)

        // Check if the number already exists
        if ((myRdyTbl[head] & myMapTbl[bit]) == 0) {
            // Number doesn't exist, so insert it
            myRdyTbl[head] |= myMapTbl[bit];        // Set bit in ready table
            *myRdyGrp |= myMapTbl[head];            // Set bit in ready group
            return rand_num;
        }
        // If number exists, continue loop to find another one
    }
}

/*
*********************************************************************************************************
*                                            TASK FUNCTION
*********************************************************************************************************
*/
void Task(void* pdata)
{
    // Variable declaration section
    INT8U myRdyTbl[8];
    INT8U myRdyGrp;
    INT8U rand_num[4];
    INT8U final, min, max, head;
    INT8U i, j, x, y;
    INT8U color = 0;
    INT8U column = 0;
    INT8U n, m;
    INT8U prio_y, prio_x;

    char data = *((char*)pdata);


    if (data == '1') {
        y = 5;

        for (j = 0; j < 4; j++) {
            final = 63;

            while (1)
            {

                myRdyGrp = 0x00;
                memset(myRdyTbl, 0x00, sizeof(myRdyTbl));
                x = column;


                for (i = 0; i < 4; i++) {
                    rand_num[i] = rd64(myRdyTbl, &myRdyGrp);
                    PC_DispChar(x++, y, '0' + (rand_num[i] / 10), DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
                    PC_DispChar(x++, y, '0' + (rand_num[i] % 10), DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
                    x++;
                }


                prio_y = myUnMapTbl[myRdyGrp];


                prio_x = myUnMapTbl[myRdyTbl[prio_y]];


                min = (prio_y << 3) + prio_x;

                // If min is less than last one
                if (min < final) {
                    switch (color)
                    {
                    case 0: // red
                        PC_DispChar(x++, y, '0' + (min / 10), DISP_FGND_WHITE + DISP_BGND_RED);
                        PC_DispChar(x++, y, '0' + (min % 10), DISP_FGND_WHITE + DISP_BGND_RED);
                        break;
                    case 1: // blue
                        PC_DispChar(x++, y, '0' + (min / 10), DISP_FGND_WHITE + DISP_BGND_BLUE);
                        PC_DispChar(x++, y, '0' + (min % 10), DISP_FGND_WHITE + DISP_BGND_BLUE);
                        break;
                    case 2: // green
                        PC_DispChar(x++, y, '0' + (min / 10), DISP_FGND_WHITE + DISP_BGND_GREEN);
                        PC_DispChar(x++, y, '0' + (min % 10), DISP_FGND_WHITE + DISP_BGND_GREEN);
                        break;
                    case 3: // brown
                        PC_DispChar(x++, y, '0' + (min / 10), DISP_FGND_WHITE + DISP_BGND_BROWN);
                        PC_DispChar(x++, y, '0' + (min % 10), DISP_FGND_WHITE + DISP_BGND_BROWN);
                        break;
                    }
                    color = (color + 1) % 4;
                    final = min;
                }

                // Next row
                y++;
                // If column is full -> next column
                if (y > 20) {
                    y = 5;
                    column += 15;
                }
                // If page is full -> clear screen
                if (column > 65) {
                    OSTimeDly(1000);
                    clearScr();
                    column = 0;
                }

                OSTimeDlyHMSM(0, 0, 0, 500);

                if (min == 0) break;
            }
        }
    }
    // case 2
    else if (data == '2') {
        // Optional implementation can be added here
    }

    // Kill the task itself. Without this, the task is run 2 times.
    OSTaskDel(OS_PRIO_SELF);
}