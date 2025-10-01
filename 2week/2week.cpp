/*
*********************************************************************************************************
* uC/OS-II
* The Real-Time Kernel
*
* (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
* All Rights Reserved
*
* LECTURE EXAMPLE (4-COLOR CYCLING) - CORRECTED VERSION
* This code is modified to meet the lecture requirements.
* - 4 Tasks are created with different priorities.
* - Each task paints the screen with a specific color.
* - Tasks use Semaphores to signal the next task in sequence.
* - This prevents the rapid-fire painting issue and ensures each color is visible.
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
* CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE                 512       /* 각 태스크 스택의 크기 (# of WORDs) */
#define  N_TASKS                        4        /* 4개의 색상 태스크를 위해 4로 수정 */

/*
*********************************************************************************************************
* VARIABLES
*********************************************************************************************************
*/

OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];      /* 각 태스크를 위한 스택 메모리 공간 */
OS_STK        TaskStartStk[TASK_STK_SIZE];

/* 태스크 간의 순서 제어를 위한 세마포어 변수 선언 */
OS_EVENT* RedSem;
OS_EVENT* BlueSem;
OS_EVENT* BrownSem;
OS_EVENT* GreenSem;

/*
*********************************************************************************************************
* FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/* 각 색상을 담당할 4개의 태스크 함수 프로토타입 선언 */
void  TaskRed(void* data);
void  TaskBlue(void* data);
void  TaskBrown(void* data);
void  TaskGreen(void* data);

void  TaskStart(void* data);                  /* 시작 태스크 함수 */
static  void  TaskStartCreateTasks(void);
static  void  TaskStartDispInit(void);
static  void  TaskStartDisp(void);

/*
*********************************************************************************************************
* MAIN
*********************************************************************************************************
*/
void  main(void)
{
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);

    OSInit();

    PC_DOSSaveReturn();
    PC_VectSet(uCOS, OSCtxSw);

    /* 각 태스크가 기다릴 세마포어를 생성. 초기 카운트는 0 (열쇠 없음) */
    RedSem = OSSemCreate(0);
    BlueSem = OSSemCreate(0);
    BrownSem = OSSemCreate(0);
    GreenSem = OSSemCreate(0);

    OSTaskCreate(TaskStart, (void*)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSStart();
}

/*
*********************************************************************************************************
* STARTUP TASK
*********************************************************************************************************
*/
void  TaskStart(void* pdata)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif
    INT16S     key;

    pdata = pdata;

    TaskStartDispInit();

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);
    PC_SetTickRate(OS_TICKS_PER_SEC);
    OS_EXIT_CRITICAL();

    OSStatInit();

    TaskStartCreateTasks();

    /* 색상 순환을 시작시키기 위해 첫 번째 태스크(TaskRed)의 세마포어를 Post(Signal) */
    OSSemPost(RedSem);

    /*
     * ★★★★★ 수정된 부분 ★★★★★
     * 시작 태스크(TaskStart)가 다른 태스크 실행을 방해하지 않도록
     * 자신의 우선순위를 가장 낮은 태스크(Green, Prio 7)보다 더 낮게 변경합니다.
     * 이제 TaskStart는 다른 태스크가 모두 잠들었을 때만 실행됩니다.
     */
    OSTaskChangePrio(OS_PRIO_SELF, 8);

    /* 무한 루프를 돌며 키보드 입력 확인 및 통계 정보 업데이트를 담당 */
    for (;;) {
        TaskStartDisp();                                  /* Update the display                       */

        if (PC_GetKey(&key) == TRUE) {                    /* See if key has been pressed              */
            if (key == 0x1B) {                            /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn();                           /* Return to DOS                            */
            }
        }

        OSCtxSwCtr = 0;                                   /* Clear context switch counter             */
        OSTimeDlyHMSM(0, 0, 1, 0);                        /* Wait one second                          */
    }
}

/*
*********************************************************************************************************
* INITIALIZE THE DISPLAY
*********************************************************************************************************
*/
static  void  TaskStartDispInit(void)
{
    PC_DispStr(0, 0, "                   uC/OS-II, 4-Color Cycling Program (Corrected)                  ", DISP_FGND_WHITE + DISP_BGND_RED + DISP_BLINK);
    PC_DispStr(0, 1, "                                Jean J. Labrosse                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 2, "                          (Using Semaphores for Task Sync)                      ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 3, "        Red (Prio 4) -> Blue (Prio 5) -> Brown (Prio 6) -> Green (Prio 7)        ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 4, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 22, "#Tasks          :        CPU Usage:     %                                       ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 23, "#Task switch/sec:                                                               ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 24, "                            <-PRESS 'ESC' TO QUIT->                             ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY + DISP_BLINK);
}

/*
*********************************************************************************************************
* UPDATE THE DISPLAY
*********************************************************************************************************
*/
static  void  TaskStartDisp(void)
{
    char   s[80];

    sprintf(s, "%5d", OSTaskCtr);
    PC_DispStr(18, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

#if OS_TASK_STAT_EN > 0
    sprintf(s, "%3d", OSCPUUsage);
    PC_DispStr(36, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);
#endif

    sprintf(s, "%5d", OSCtxSwCtr);
    PC_DispStr(18, 23, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

    sprintf(s, "V%1d.%02d", OSVersion() / 100, OSVersion() % 100);
    PC_DispStr(75, 24, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);
}

/*
*********************************************************************************************************
* CREATE TASKS
*********************************************************************************************************
*/
static  void  TaskStartCreateTasks(void)
{
    /* TaskStart(Prio 0) 보다 낮은 우선순위로 색상 태스크들을 생성 */
    OSTaskCreate(TaskRed,
        (void*)0,
        &TaskStk[0][TASK_STK_SIZE - 1],
        4);      /* Red   Task: Priority 4 (가장 높음) */

    OSTaskCreate(TaskBlue,
        (void*)0,
        &TaskStk[1][TASK_STK_SIZE - 1],
        5);      /* Blue  Task: Priority 5 */

    OSTaskCreate(TaskBrown,
        (void*)0,
        &TaskStk[2][TASK_STK_SIZE - 1],
        6);      /* Brown Task: Priority 6 */

    OSTaskCreate(TaskGreen,
        (void*)0,
        &TaskStk[3][TASK_STK_SIZE - 1],
        7);      /* Green Task: Priority 7 (가장 낮음) */
}

/*
*********************************************************************************************************
* TASKS
*********************************************************************************************************
*/

/* 화면을 특정 색상으로 채우는 공통 함수 */
static void PaintScreen(INT8U color)
{
    INT8U i, j;
    for (i = 5; i < 22; i++) {
        for (j = 0; j < 80; j++) {
            PC_DispChar(j, i, ' ', color);
        }
    }
}

/* Red 색상 태스크 (Priority 4) */
void  TaskRed(void* pdata)
{
    INT8U err;
    pdata = pdata;
    for (;;) {
        OSSemPend(RedSem, 0, &err);      /* 자신의 차례가 될 때까지 세마포어를 기다림(Pend) */
        PaintScreen(DISP_FGND_RED + DISP_BGND_RED);
        OSTimeDlyHMSM(0, 0, 1, 0);        /* 1초 동안 색상을 보여줌 */
        OSSemPost(BlueSem);              /* 다음 태스크(Blue)를 깨움(Post) */
    }
}

/* Blue 색상 태스크 (Priority 5) */
void  TaskBlue(void* pdata)
{
    INT8U err;
    pdata = pdata;
    for (;;) {
        OSSemPend(BlueSem, 0, &err);
        PaintScreen(DISP_FGND_BLUE + DISP_BGND_BLUE);
        OSTimeDlyHMSM(0, 0, 2, 0);
        OSSemPost(BrownSem);
    }
}

/* Brown 색상 태스크 (Priority 6) */
void  TaskBrown(void* pdata)
{
    INT8U err;
    pdata = pdata;
    for (;;) {
        OSSemPend(BrownSem, 0, &err);
        PaintScreen(DISP_FGND_BROWN + DISP_BGND_BROWN);
        OSTimeDlyHMSM(0, 0, 3, 0);
        OSSemPost(GreenSem);
    }
}

/* Green 색상 태스크 (Priority 7) */
void  TaskGreen(void* pdata)
{
    INT8U err;
    pdata = pdata;
    for (;;) {
        OSSemPend(GreenSem, 0, &err);
        PaintScreen(DISP_FGND_GREEN + DISP_BGND_GREEN);
        OSTimeDlyHMSM(0, 0, 4, 0);
        OSSemPost(RedSem);               /* 마지막 태스크는 처음 태스크(Red)를 깨워 순환 구조를 만듦 */
    }
}

