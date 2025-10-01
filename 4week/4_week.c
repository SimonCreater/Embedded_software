/*
*********************************************************************************************************
* uC/OS-II
* The Real-Time Kernel
*
* (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
* All Rights Reserved
*
* EXAMPLE #1
* (MODIFIED TO MERGE THE PROVIDED IMAGE'S CONTENT)
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
* CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE         512     /* 각 태스크 스택의 크기 (# of WORDs) */
#define  N_TASKS                 5     /* 생성할 태스크의 수 (이미지 내용으로 변경) */
#define  MSG_QUEUE_SIZE          4     /* 메시지 큐의 크기 (이미지 내용에서 추가) */

/*
*********************************************************************************************************
* VARIABLES
*********************************************************************************************************
*/

// --- 이미지 내용으로 변경된 변수 선언부 ---
OS_STK   TaskStk[N_TASKS][TASK_STK_SIZE];     /* 태스크 스택 배열 */
OS_STK   TaskStartStk[TASK_STK_SIZE];
char     TaskData[N_TASKS];                   /* 각 태스크에 전달할 파라미터 */

OS_EVENT *RandomSem;
OS_EVENT *Ack;
OS_EVENT *Tx;
OS_EVENT *AckQueue[4];
OS_EVENT *TxQueue[4];
// --- 변경 완료 ---

/*
*********************************************************************************************************
* FUNCTION PROTOTYPES
*********************************************************************************************************
*/

// --- 이미지 내용으로 변경된 함수 원형 ---
void  RandTask1(void *data);
void  RandTask2(void *data);
void  RandTask3(void *data);
void  RandTask4(void *data);
void  DecisionTask(void *data);
// --- 변경 완료 ---

void  TaskStart(void *data);                  /* 시작 태스크의 함수 원형 */
static  void  TaskStartCreateTasks(void);
static  void  TaskStartDispInit(void);
static  void  TaskStartDisp(void);


/*$PAGE*/
/*
*********************************************************************************************************
* MAIN
*********************************************************************************************************
*/

void  main (void)
{
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);   /* 화면 지우기 */

    OSInit();                                           /* uC/OS-II 초기화 */

    PC_DOSSaveReturn();                                 /* DOS로 돌아가기 위한 환경 저장 */
    PC_VectSet(uCOS, OSCtxSw);                          /* uC/OS-II의 컨텍스트 스위치 벡터 설치 */

    RandomSem   = OSSemCreate(1);                       /* Random 숫자 생성용 세마포어 생성 */
    
    // 참고: 이미지에 추가된 다른 OS_EVENT들도 여기서 생성(OSSemCreate, OSQCreate 등)해야 한다.
    //       하지만 구체적인 타입(세마포어, 큐 등)을 알 수 없어 선언만 반영했다.

    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSStart();                                          /* 멀티태스킹 시작 */
}


/*
*********************************************************************************************************
* STARTUP TASK
*********************************************************************************************************
*/
void  TaskStart (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                             /* CPU 상태 레지스터를 위한 저장 공간 할당 */
    OS_CPU_SR  cpu_sr;
#endif
    char       s[100];
    INT16S     key;


    pdata = pdata;                                      /* 컴파일러 경고 방지 */

    TaskStartDispInit();                                /* 디스플레이 초기화 */

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                        /* uC/OS-II의 클록 틱 ISR 설치 */
    PC_SetTickRate(OS_TICKS_PER_SEC);                   /* 틱 속도 재설정 */
    OS_EXIT_CRITICAL();

    OSStatInit();                                       /* uC/OS-II 통계 기능 초기화 */

    TaskStartCreateTasks();                             /* 모든 애플리케이션 태스크 생성 */

    for (;;) {
        TaskStartDisp();                                /* 디스플레이 업데이트 */

        if (PC_GetKey(&key) == TRUE) {                  /* 키가 눌렸는지 확인 */
            if (key == 0x1B) {                          /* ESC 키인지 확인 */
                PC_DOSReturn();                         /* DOS로 복귀 */
            }
        }

        OSCtxSwCtr = 0;                                 /* 컨텍스트 스위치 카운터 초기화 */
        OSTimeDlyHMSM(0, 0, 1, 0);                      /* 1초 대기 */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
* INITIALIZE THE DISPLAY
*********************************************************************************************************
*/

static  void  TaskStartDispInit (void)
{
/* 1111111111222222222233333333334444444444555555555566666666667777777777 */
/* 01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
    PC_DispStr( 0,  0, "                         uC/OS-II, The Real-Time Kernel                           ", DISP_FGND_WHITE + DISP_BGND_RED + DISP_BLINK);
    PC_DispStr( 0,  1, "                                Jean J. Labrosse                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  2, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  3, "                                   EXAMPLE #1                                   ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  4, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  5, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  6, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  7, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  8, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  9, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 10, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 11, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 12, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 13, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 14, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 15, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 16, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 17, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 18, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 19, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 20, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 21, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 22, "#Tasks          :       CPU Usage:     %                                       ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 23, "#Task switch/sec:                                                               ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 24, "                            <-PRESS 'ESC' TO QUIT->                             ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY + DISP_BLINK);
/* 1111111111222222222233333333334444444444555555555566666666667777777777 */
/* 01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
}

/*$PAGE*/
/*
*********************************************************************************************************
* UPDATE THE DISPLAY
*********************************************************************************************************
*/

static  void  TaskStartDisp (void)
{
    char   s[80];


    sprintf(s, "%5d", OSTaskCtr);                       /* 실행 중인 태스크 수 표시 */
    PC_DispStr(18, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

#if OS_TASK_STAT_EN > 0
    sprintf(s, "%3d", OSCPUUsage);                      /* CPU 사용률(%) 표시 */
    PC_DispStr(36, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);
#endif

    sprintf(s, "%5d", OSCtxSwCtr);                      /* 초당 컨텍스트 스위치 수 표시 */
    PC_DispStr(18, 23, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

    sprintf(s, "V%1d.%02d", OSVersion() / 100, OSVersion() % 100); /* uC/OS-II 버전 번호 표시 */
    PC_DispStr(75, 24, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

    switch (_8087) {                                    /* FPU 존재 여부 표시 */
        case 0:
             PC_DispStr(71, 22, " NO FPU ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
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
/*
*********************************************************************************************************
* CREATE TASKS
*********************************************************************************************************
*/

static  void  TaskStartCreateTasks (void)
{
    INT8U  i;

    // --- 이미지 내용에 맞춰 5개의 다른 태스크를 생성하도록 변경 ---
    TaskData[0] = '1';
    OSTaskCreate(RandTask1, (void *)&TaskData[0], &TaskStk[0][TASK_STK_SIZE - 1], 1);

    TaskData[1] = '2';
    OSTaskCreate(RandTask2, (void *)&TaskData[1], &TaskStk[1][TASK_STK_SIZE - 1], 2);
    
    TaskData[2] = '3';
    OSTaskCreate(RandTask3, (void *)&TaskData[2], &TaskStk[2][TASK_STK_SIZE - 1], 3);

    TaskData[3] = '4';
    OSTaskCreate(RandTask4, (void *)&TaskData[3], &TaskStk[3][TASK_STK_SIZE - 1], 4);
    
    TaskData[4] = 'D'; // Decision Task
    OSTaskCreate(DecisionTask, (void *)&TaskData[4], &TaskStk[4][TASK_STK_SIZE - 1], 5);
    // --- 변경 완료 ---
}


/*
*********************************************************************************************************
* TASKS
*********************************************************************************************************
*/

/* 참고: 
  RandTask1, RandTask2, RandTask3, RandTask4, DecisionTask 함수의 실제 구현은
  이미지에 포함되어 있지 않으므로 여기에 추가할 수 없습니다. 
  코드를 컴파일하고 실행하려면 해당 함수들의 구현이 필요합니다.
*/

// --- 기존 Task 함수는 더 이상 사용되지 않으므로 삭제함 ---
/*
void  Task (void *pdata)
{
    INT8U  x;
    INT8U  y;
    INT8U  err;


    for (;;) {
        OSSemPend(RandomSem, 0, &err);
        x = random(80);
        y = random(16);
        OSSemPost(RandomSem);
        PC_DispChar(x, y + 5, *(char *)pdata, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
        OSTimeDly(1);
    }
}
*/