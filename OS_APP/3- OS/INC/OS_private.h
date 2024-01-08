/* ************************************************ */
/* Author      : Abdalrahman Amin	                */
/* SWC         : Scheduler			                */
/* Layer       : OS                                 */
/* Version     : 1.0                                */
/* Date        : January 7, 2024          		    */
/* Last Edit   : N/A 			                    */
/* ************************************************ */

#ifndef _OS_PRIVATE_H_
#define _OS_PRIVATE_H_


/* Config file */
/* Task stack in CFG file */
#define SIZE_TASK_STACK		1024u
#define SIZE_SCHED_STACK	1024u

/* SRAM */
#define SRAM_START			0x20000000u
#define SRAM_SIZE			(127*1024)
#define SRAM_END			((SRAM_START) + (SRAM_SIZE) ) /* - 1024 */


#define OS_TIDLESTACK_START  (SRAM_END)
#define OS_T1STACK_START     (SRAM_END)	- (1* (SIZE_TASK_STACK))
#define OS_T2STACK_START     (SRAM_END) - (2* (SIZE_TASK_STACK))
#define OS_T3STACK_START     (SRAM_END) - (3* (SIZE_TASK_STACK))
#define OS_T4STACK_START     (SRAM_END) - (4* (SIZE_TASK_STACK))
#define OS_SCHEDSTACK_START  (SRAM_END) - (5* (SIZE_TASK_STACK))


#define DUMMY_XPSR			0x01000000u

#define DUMMY_LR			0xfffffffd



typedef enum
{
	OS_STATE_SUSPENDED,
	OS_STATE_ACTIVE,
}OS_TASKState_t;


typedef struct 
{
	u32 PSPValue;
	u16 Periodicity;
	u16 BlockCount;
	OS_TASKState_t CurrentState;
	void (* TaskHandler)(void);
}OS_TCB_t;

#define OS_NUMBER_OF_TASKS  5




__attribute__ ((naked)) void Stack_voidSchedulerStackInit(u32 ARG_u32SchedTaskOS);

u32 OS_u32GetCurrentPSPValue(void);

void OS_voidSavePSPValue( u32 ARG_u32CurrentPSPValue);

static void OS_voidScheduler(void);

void Enable_FaultHandler(void);

__attribute__((naked)) void OS_voidChangeToPSP(void);

void OS_voidIdleTaskHandler(void);


#endif
