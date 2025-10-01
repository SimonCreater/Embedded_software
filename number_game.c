/*
*********************************************************************************************************
*                                          숫자 게임 프로그램
*                                      Number Game Program
*                          MicroC/OS-II 스케줄러 자료구조 활용
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)            */
#define  N_TASKS                        10       /* Number of identical tasks                          */

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
OS_STK        TaskStartStk[TASK_STK_SIZE];
char          TaskData[N_TASKS];                      /* Parameters to pass to each task               */
OS_EVENT* RandomSem;

/* 숫자 게임을 위한 전역 변수 */
INT8U final = 64;

/* 색상 순환을 위한 변수 */
INT8U color_index = 0;
INT8U colors[4] = {
    DISP_FGND_RED + DISP_BGND_RED,        /* 빨강 배경 */
    DISP_FGND_BLUE + DISP_BGND_BLUE,      /* 파랑 배경 */
    DISP_FGND_GREEN + DISP_BGND_GREEN,    /* 초록 배경 */
    DISP_FGND_BROWN + DISP_BGND_BROWN     /* 갈색 배경 */
};

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void  Task(void* data);                       /* Function prototypes of tasks                  */
void  TaskStart(void* data);                  /* Function prototypes of Startup task           */
static  void  TaskStartCreateTasks(void);
static  void  TaskStartDispInit(void);
static  void  TaskStartDisp(void);

/* 스케줄러 관련 함수들 */
INT8U FindHighestPriorityTask(INT8U num1, INT8U num2, INT8U num3, INT8U num4);
void DisplayMinimumNumber(INT8U min_num, INT8U color, INT8U position);

/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

void  main(void)
{
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    RandomSem = OSSemCreate(1);                          /* Random number semaphore                  */

    OSTaskCreate(TaskStart, (void*)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSStart();                                             /* Start multitasking                       */
}

/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
void  TaskStart(void* pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    char       s[100];
    INT16S     key;

    pdata = pdata;                                         /* Prevent compiler warning                 */

    TaskStartDispInit();                                   /* Initialize the display                   */

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    OSStatInit();                                          /* Initialize uC/OS-II's statistics         */

    TaskStartCreateTasks();                                /* Create all the application tasks         */

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

/*
*********************************************************************************************************
*                                        INITIALIZE THE DISPLAY
*********************************************************************************************************
*/

static  void  TaskStartDispInit(void)
{
    PC_DispStr(0, 0, "                                                                                ", DISP_FGND_WHITE + DISP_BGND_RED + DISP_BLINK);
    PC_DispStr(0, 1, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 2, "                                EXAMPLE #1                                      ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 3, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
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
}

/*
*********************************************************************************************************
*                                           UPDATE THE DISPLAY
*********************************************************************************************************
*/

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

/*
*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

static  void  TaskStartCreateTasks(void)
{
    /* 숫자 게임을 위해 하나의 태스크만 생성 */
    TaskData[0] = '0';
    OSTaskCreate(Task, (void*)&TaskData[0], &TaskStk[0][TASK_STK_SIZE - 1], 1);
}

/*
*********************************************************************************************************
*                                    숫자 게임 태스크 (NUMBER GAME TASK)
*********************************************************************************************************
*/

void  Task(void* pdata)
{
    INT8U  num1, num2, num3, num4;
    INT8U  temp;
    INT8U  err;
    char   s[80];
    static INT8U position = 0;  /* 화면에 표시할 위치 */

    pdata = pdata;  /* Prevent compiler warning */

    /* final을 64로 초기화 */
    final = 64;

    /* 화면 초기 정보 표시 */
    sprintf(s, "Number Game - Finding minimum using OS scheduler data structures              ");
    PC_DispStr(0, 5, s, DISP_FGND_WHITE + DISP_BGND_BLACK);

    sprintf(s, "Generated 4 numbers: [  ,  ,  ,  ] -> MIN:    final=%2d                     ", final);
    PC_DispStr(0, 7, s, DISP_FGND_WHITE + DISP_BGND_BLACK);

    sprintf(s, "Colored minimums:                                                           ");
    PC_DispStr(0, 9, s, DISP_FGND_WHITE + DISP_BGND_BLACK);

    /* final이 0이 될 때까지 반복 */
    while (final != 0) {
        /* 세마포어를 획득하여 랜덤 함수 사용 */
        OSSemPend(RandomSem, 0, &err);

        /* 4개의 랜덤 숫자 생성 (0~63 범위) */
        num1 = random(64);
        num2 = random(64);
        num3 = random(64);
        num4 = random(64);

        OSSemPost(RandomSem);

        /* 생성된 4개 숫자 표시 */
        sprintf(s, "Generated 4 numbers: [%2d,%2d,%2d,%2d] -> ", num1, num2, num3, num4);
        PC_DispStr(0, 7, s, DISP_FGND_WHITE + DISP_BGND_BLACK);

        /* 스케줄러 자료구조를 사용하여 최소값 찾기 */
        temp = FindHighestPriorityTask(num1, num2, num3, num4);

        /* 찾은 최소값과 현재 final 표시 */
        sprintf(s, "MIN: %2d, final=%2d", temp, final);
        PC_DispStr(40, 7, s, DISP_FGND_WHITE + DISP_BGND_BLACK);

        /* temp가 final보다 작을 때만 색칠하고 final 업데이트 */
        if (temp < final) {
            /* final에 temp 값 대입 */
            final = temp;

            /* 찾은 최소값을 해당 색상으로 화면에 표시 */
            DisplayMinimumNumber(temp, colors[color_index], position);

            /* 상태 정보 업데이트 */
            sprintf(s, "NEW MIN: %2d < %2d, Color: %s                                           ",
                temp, (final == temp) ? 999 : final + 1, /* 이전 final 표시 */
                (color_index == 0) ? "RED" :
                (color_index == 1) ? "BLUE" :
                (color_index == 2) ? "GREEN" : "BROWN");
            PC_DispStr(0, 12, s, DISP_FGND_YELLOW + DISP_BGND_BLACK);

            /* 다음 색상과 위치로 이동 */
            color_index = (color_index + 1) % 4;
            position++;
            if (position > 25) position = 0; /* 화면 폭 제한 */

        }
        else {
            sprintf(s, "No change: %2d >= %2d                                                   ", temp, final);
            PC_DispStr(0, 12, s, DISP_FGND_WHITE + DISP_BGND_BLACK);
        }

        /* final이 0인지 체크 */
        if (final == 0) {
            sprintf(s, "PROGRAM FINISHED! final reached 0                                       ");
            PC_DispStr(0, 14, s, DISP_FGND_WHITE + DISP_BGND_RED + DISP_BLINK);
            break;
        }

        /* 딜레이 */
        OSTimeDly(200);
    }

    /* 프로그램 종료 상태 유지 */
    for (;;) {
        OSTimeDly(1000);
    }
}

/*
*********************************************************************************************************
*                          스케줄러 자료구조를 사용한 최소값 찾기 함수
*********************************************************************************************************
*/

INT8U FindHighestPriorityTask(INT8U num1, INT8U num2, INT8U num3, INT8U num4)
{
    INT8U temp_OSRdyGrp = 0;
    INT8U temp_OSRdyTbl[8] = { 0 };  /* 8개 그룹 */
    INT8U y, x;
    INT8U priority;

    /* 4개 숫자를 ready list에 삽입 */
    /* num1 삽입 */
    temp_OSRdyGrp |= OSMapTbl[num1 >> 3];
    temp_OSRdyTbl[num1 >> 3] |= OSMapTbl[num1 & 0x07];

    /* num2 삽입 */
    temp_OSRdyGrp |= OSMapTbl[num2 >> 3];
    temp_OSRdyTbl[num2 >> 3] |= OSMapTbl[num2 & 0x07];

    /* num3 삽입 */
    temp_OSRdyGrp |= OSMapTbl[num3 >> 3];
    temp_OSRdyTbl[num3 >> 3] |= OSMapTbl[num3 & 0x07];

    /* num4 삽입 */
    temp_OSRdyGrp |= OSMapTbl[num4 >> 3];
    temp_OSRdyTbl[num4 >> 3] |= OSMapTbl[num4 & 0x07];

    /* OSUnMapTbl을 사용하여 최고 우선순위(최소값) 찾기 */
    y = OSUnMapTbl[temp_OSRdyGrp];
    x = OSUnMapTbl[temp_OSRdyTbl[y]];
    priority = (y << 3) + x;

    return priority;
}

/*
*********************************************************************************************************
*                                       랜덤 숫자들을 화면에 표시
*********************************************************************************************************
*/

void DisplayMinimumNumber(INT8U min_num, INT8U color, INT8U position)
{
    char s[4];

    /* 찾은 최소값을 특정 위치에 해당 색상으로 표시 */
    sprintf(s, "%2d ", min_num);
    PC_DispStr(position * 3, 10, s, DISP_FGND_WHITE + color);
}