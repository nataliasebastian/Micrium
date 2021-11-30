/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*                          (c) Copyright 2009-2011; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                          APPLICATION CODE
*
*                                          Microsoft Windows
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : JJL
*                 FGK
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>


/*
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            LOCAL VARIABLES
*********************************************************************************************************
*/

static OS_TCB  AppTarea1_TCB;
static OS_TCB  AppTarea2_TCB;

static CPU_STK AppTarea1Pila[64];
static CPU_STK AppTarea2Pila[64];

static OS_Q  AppQ;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTarea1(void* p_arg);
static  void  AppTarea2(void* p_arg);


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

int  main(void) {
    OS_ERR  err;

    OSInit(&err);     /* Init uC/OS-III.*/
    BSP_Init();      /* Initialize BSP functions */
    CPU_Init();     /* Initialize uC/CPU services */

    /*Creación de la cola*/
    OSQCreate((OS_Q*)&AppQ,
        (CPU_CHAR*)"My App Queue", /*Nombre de la cola*/
        (OS_MSG_QTY)500, /*Mensajes que puede recibir*/
        (OS_ERR*)&err); /*Check for err*/

    /*Creación de la Tarea 1*/
    OSTaskCreate((OS_TCB*)&AppTarea1_TCB,
        (CPU_CHAR*)"App Task 1",
        (OS_TASK_PTR)AppTarea1,
        (void*)0,
        (OS_PRIO)5,
        (CPU_STK*)&AppTarea1Pila[0],
        (CPU_STK_SIZE)0,
        (CPU_STK_SIZE)64,
        (OS_MSG_QTY)0,
        (OS_TICK)0,
        (void*)0,
        (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
        (OS_ERR*)&err); /*Check for err*/

    /*Creación de la Tarea 2*/
    OSTaskCreate((OS_TCB*)&AppTarea2_TCB,
        (CPU_CHAR*)"App Task 2",
        (OS_TASK_PTR)AppTarea2,
        (void*)0,
        (OS_PRIO)6,
        (CPU_STK*)&AppTarea2Pila[0],
        (CPU_STK_SIZE)0,
        (CPU_STK_SIZE)64,
        (OS_MSG_QTY)0,
        (OS_TICK)0,
        (void*)0,
        (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
        (OS_ERR*)&err); /*Check for err*/


    OSStart(&err);/*Start multitasking (i.e. give control to uC/OS-III).*/
}




/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Argument(s) : p_arg       is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/


static  void  AppTarea1(void* p_arg)
{
    OS_ERR  err;
    CPU_INT32U random; /*Almacena el número aleatorio*/

    (void)p_arg;

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err); /*Compute CPU capacity with no task running */
#endif

    APP_TRACE_DBG(("uCOS-III is Running...\n\r"));

    while (DEF_ON) {  /*Task body, always written as an infinite loop*/
        OSTimeDlyHMSM(0, 0, 1, 0, /*Suspende la ejecución durante un segundo*/
            OS_OPT_TIME_DLY,
            &err);
        random = (CPU_INT32U)rand(); /*Genera un número aleatorio*/
        OSQPost((OS_Q*)&AppQ, /*Envía el número a la cola AppQ*/
            (void*)random,
            (OS_MSG_SIZE)sizeof(void*),
            (OS_OPT)OS_OPT_POST_FIFO,
            (OS_ERR*)&err); /*Check for err*/
    }
}

static  void  AppTarea2(void* p_arg)
{
    OS_ERR  err;
    CPU_INT32U random; /*Variable que almacena el valor leído*/
    OS_MSG_SIZE msg_size;
    CPU_TS ts;
    int i = 1; /*Inicializa el contador de números aleatorios leídos*/
    (void)p_arg;

    while (DEF_ON) { /*Task body, always written as an infinite loop*/

        random = (CPU_INT32U)OSQPend((OS_Q*)&AppQ, /*Lee y almacena el dato leído*/
            (OS_TICK)0,
            (OS_OPT)OS_OPT_PEND_BLOCKING,
            (OS_MSG_SIZE*)&msg_size,
            (CPU_TS*)&ts,
            (OS_ERR*)&err); /*Check for err*/
        APP_TRACE_DBG(("Numero aleatorio %d: %d\n\r", i, random));
        /*Muestra en pantalla el número aleatorio leído*/
        i++;   /*Incrementa el contador de números aleatorios leídos*/
    }
}

