/* ************************************************ */
/* Author      : Abdalrahman Amin	                */
/* SWC         : Scheduler			                */
/* Layer       : OS                                 */
/* Version     : 1.0                                */
/* Date        : January 7, 2024          		    */
/* Last Edit   : N/A 			                    */
/* ************************************************ */


/* Library Includes */
#include "LSTD_types.h"
#include "LBIT_math.h"

/* SWC Includes */
#include "MSYSTICK_interface.h"
#include "MSCB_private.h"

#include "OS_private.h"
#include "OS_interface.h"

/* need to create a flag for SchedulerTask or otherTask to be used inside PendSV */
//u32* OS_pu32SchedulerTaskPSP=0;

u8 OS_u8CreateTaskFlag =0;

u8 OS_u8Flag = 0; /* This Flag to indicate OS Start */

OS_TCB_t OS_SysTasks[OS_NUMBER_OF_TASKS] ={0};
u16 OS_u16TaskPeriodicity[OS_NUMBER_OF_TASKS] ;

u8 OS_CurrentTask = 4u; /* Idle Task */
//u32 OS_GlobalTickCount = 0; /* Global Tick Count */


static volatile u32 OS_u32TickCount =0; /* Global Tick Count */


__attribute__((naked)) void Stack_voidSchedulerStackInit(u32 ARG_u32SchedTaskOS)
{
	/* Store Parameter 1 in MSP */
	__asm volatile("MSR MSP , R0");

	/* return */
	__asm volatile("BX LR");

}

static void OS_voidScheduler(void)
{
	u8 L_u8i;
	for(L_u8i = 0; L_u8i< OS_NUMBER_OF_TASKS ; L_u8i++)
	{
		if(OS_STATE_ACTIVE == OS_SysTasks[L_u8i].CurrentState)
		{
			if( 0 == OS_u16TaskPeriodicity[L_u8i] )
			{
				if( OS_SysTasks[L_u8i].TaskHandler == NULL_POINTER )
				{
					continue;
				}

				OS_CurrentTask = L_u8i;
				OS_u16TaskPeriodicity[L_u8i] = OS_SysTasks[L_u8i].Periodicity;
				//				OS_SysTasks[L_u8i].TaskHandler();
				 break ;
			}
			else
			{
				OS_u16TaskPeriodicity[L_u8i]--;
			}
		}
		else
		{
			if(OS_SysTasks[L_u8i].BlockCount <= OS_u32TickCount && OS_SysTasks[L_u8i].BlockCount != 0)
			{
				OS_SysTasks[L_u8i].CurrentState = OS_STATE_ACTIVE;
				OS_SysTasks[L_u8i].BlockCount   = 0;
			}
			else
			{
				/* Do nothing Misra */
			}

		}
	}

}

void OS_voidStartOS(void)
{
	/* Change to PSP */
	OS_voidChangeToPSP();

	Enable_FaultHandler();

	/* Initialize STK */
	MSTK_stderrInit(MSTK_CLOCKSRC_AHB_8);

	/* Initialize stack for scheduler  MSP */
	Stack_voidSchedulerStackInit(OS_SCHEDSTACK_START);

	/* Create IDLE task */
	OS_stderrCreateTask(OS_voidIdleTaskHandler, 0, 4, 0);

	/* OS Enable set Flag */
	OS_u8Flag=1;

	/* Tick every 1ms,  2000 ==> 1 ms */
	MSTK_stderrStartPeriodic(1999);

	/* Call Idle Task */
	OS_voidIdleTaskHandler();

}

/* the Task ID is the priortiy so only one func can take priority 1 or .. 255 */
STD_error_t OS_stderrCreateTask( void (*ARG_pvoidfhandler)(void), u16 ARG_u16Periodicity , u8 ARG_u8Priority , u8 ARG_u8FirstDelay)
{
	STD_error_t L_stderrState=E_OK;

	if(NULL_POINTER != ARG_pvoidfhandler)
	{
		/* task not overwrite another task in array OS_SysTasks */
		if( (ARG_u8Priority < OS_NUMBER_OF_TASKS) && (NULL_POINTER == OS_SysTasks[ARG_u8Priority].TaskHandler) )
		{
			/* pointer to initialize stack */
			u32 * L_pu32TaskPSP ;
			u8 L_u8Iterator;

			OS_SysTasks[ARG_u8Priority].TaskHandler = ARG_pvoidfhandler;
			OS_SysTasks[ARG_u8Priority].Periodicity = ARG_u16Periodicity;
			OS_SysTasks[ARG_u8Priority].BlockCount  = 0;
			OS_SysTasks[ARG_u8Priority].CurrentState= OS_STATE_ACTIVE;
			OS_u16TaskPeriodicity[ARG_u8Priority]   = ARG_u8FirstDelay;

			if(ARG_u8Priority == 4 )
			{
				/* Idle Task */
				OS_SysTasks[ARG_u8Priority].PSPValue = OS_TIDLESTACK_START ;
			}
			else
			{
				OS_SysTasks[ARG_u8Priority].PSPValue = (SRAM_END - ( (ARG_u8Priority+1) * (SIZE_TASK_STACK) )) ;
			}


			L_pu32TaskPSP = (u32* ) OS_SysTasks[ARG_u8Priority].PSPValue ;

			L_pu32TaskPSP -- ; /* XPSR register */
			*L_pu32TaskPSP = DUMMY_XPSR;

			L_pu32TaskPSP -- ; /* PC register */
			*L_pu32TaskPSP = (u32 )OS_SysTasks[ARG_u8Priority].TaskHandler;

			L_pu32TaskPSP -- ; /* LR register */
			*L_pu32TaskPSP = (u32 )DUMMY_LR;

			/* R0 to R12 = 0 */
			for( L_u8Iterator=0; L_u8Iterator < 13 ; L_u8Iterator++)
			{
				L_pu32TaskPSP -- ;
				*L_pu32TaskPSP = 0;
			}

			/* save PSP value */
			OS_SysTasks[ARG_u8Priority].PSPValue = (u32) L_pu32TaskPSP;

		}
		else
		{
			L_stderrState=E_NOK;
		}
	}
	else
	{
		L_stderrState=E_NULL_POINTER;
	}
	
	/* IDLE Task not call scheduler  */
	if(ARG_u8Priority != 4 && OS_u8Flag == 1 )
	{

		OS_u8CreateTaskFlag = 1;
		/* Trigger PendSV here */
		SCB->ICSR |= 1 << 28 ;

	}

	return L_stderrState;
}

STD_error_t OS_stderrDeleteTask( u8 ARG_u8Priority)
{
	STD_error_t L_stderrState=E_OK;
	if(ARG_u8Priority < OS_NUMBER_OF_TASKS)
	{
		if(NULL_POINTER != OS_SysTasks[ARG_u8Priority].TaskHandler)
		{
			/* delete task */
			OS_SysTasks[ARG_u8Priority].TaskHandler= NULL_POINTER;
			OS_SysTasks[ARG_u8Priority].Periodicity= 0;
			OS_SysTasks[ARG_u8Priority].CurrentState= OS_STATE_SUSPENDED;
			
		}
		else
		{
			L_stderrState=E_NOK; /* empty*/
		}
	}
	else
	{
		L_stderrState=E_NOK;
	}
	
	return L_stderrState;
}

STD_error_t OS_stderrActivateTask( u8 ARG_u8Priority)
{
	STD_error_t L_stderrState=E_OK;
	if(ARG_u8Priority < OS_NUMBER_OF_TASKS)
	{
		if(NULL_POINTER != OS_SysTasks[ARG_u8Priority].TaskHandler)
		{
			OS_SysTasks[ARG_u8Priority].CurrentState= OS_STATE_ACTIVE;
		}
		else
		{
			L_stderrState=E_NOK; /* empty*/
		}
	}
	else
	{
		L_stderrState=E_NOK;
	}
	
	return L_stderrState;
	
}

STD_error_t OS_stderrSuspendTask( u8 ARG_u8Priority)
{
	STD_error_t L_stderrState=E_OK;

	if(ARG_u8Priority < OS_NUMBER_OF_TASKS)
	{
		if(NULL_POINTER != OS_SysTasks[ARG_u8Priority].TaskHandler)
		{
			OS_SysTasks[ARG_u8Priority].CurrentState= OS_STATE_SUSPENDED;
		}
		else
		{
			L_stderrState=E_NOK; /* empty*/
		}
	}
	else
	{
		L_stderrState=E_NOK;
	}
	
	return L_stderrState;
	
}

__attribute__((naked)) void OS_voidChangeToPSP(void)
{

	/* Assign New PSP Value in SRAM_END */
	__asm volatile(" .equ SRAM_END , ( 0x20000000U+ ((128*1024)) )");

	/* LOAD it in R0 */
	__asm volatile("LDR R0, =SRAM_END");

	__asm volatile("MSR PSP , R0");


	/* change SP to PSP using CONTROL Register */
	__asm volatile("MOV R0, #0x02");

	__asm volatile("MSR CONTROL,R0");

	/* exit */
	__asm volatile("BX LR");

}

void OS_voidTaskDelay(u16 ARG_u16Ticks)
{
	/* Disable interrupts for atomic operation	*/
	__asm volatile("CPSID I");

	/* Set The Block Ticks */
	OS_SysTasks[OS_CurrentTask].BlockCount = OS_u32TickCount + ARG_u16Ticks;

	/* update task state */
	OS_SysTasks[OS_CurrentTask].CurrentState = OS_STATE_SUSPENDED;

	/* Re-enable interrupts */
	__asm volatile("CPSIE I");

	/* Trigger PendSV here */
	SCB->ICSR |= 1 << 28 ;

}


STD_error_t OS_stderrCreateBinarySemaphore(Semaphore_t *ARG_pSemaphoreName)
{
	STD_error_t L_stderrState = E_OK;

	if(ARG_pSemaphoreName != NULL_POINTER)
	{
		/* Initialize with 1 to be Free Unlocked */
		*ARG_pSemaphoreName = 1;

	}
	else
	{
		L_stderrState = E_NULL_POINTER;
	}

	return L_stderrState;
}

STD_error_t OS_stderrTakeSemaphore(Semaphore_t *ARG_pSemaphoreName, u32 ARG_u32Ticks)
{
	STD_error_t L_stderrState = E_OK;

	if(ARG_pSemaphoreName != NULL_POINTER)
	{
		/* Disable interrupts for atomic operation	*/
		__asm volatile("CPSID I");

		/* check the semaphore */
		if(*ARG_pSemaphoreName > 0 )
		{
			/* Semaphore is available */

			/* Take Semaphore */
			*ARG_pSemaphoreName --;

			/* Semaphore successfully taken */
			L_stderrState = E_OK ;
		}
		else if (ARG_u32Ticks == 0)
		{
			/* Not available and there is no Ticks to wait Semaphore */

			/* Semaphore Locked */
			L_stderrState = E_NOK ;
		}
		else
		{
			/* Need to wait the ticks then Check the Semaphore go on Block State */

			/* Set The Block Ticks */
			OS_SysTasks[OS_CurrentTask].BlockCount = OS_u32TickCount + ARG_u32Ticks;

			/* update task state */
			OS_SysTasks[OS_CurrentTask].CurrentState = OS_STATE_SUSPENDED;

			/* Re-enable interrupts */
			__asm volatile("CPSIE I");

			/* Trigger PendSV here to switch to another Task */
			SCB->ICSR |= 1 << 28 ;

			/* nop */
			__asm volatile("nop");

			/* Check Semaphore after wait Ticks */
			if(*ARG_pSemaphoreName > 0 )
			{
				/* Take Semaphore */
				*ARG_pSemaphoreName --;

				/* Semaphore successfully taken */
				L_stderrState = E_OK ;
			}
			else
			{
				L_stderrState = E_NOK ; /* Semaphore Locked */
			}

		}

	}
	else
	{
		L_stderrState = E_NULL_POINTER;
	}

	/* Re-enable interrupts */
	__asm volatile("CPSIE I");

	return L_stderrState;
}

STD_error_t OS_stderrReleaseSemaphore(Semaphore_t *ARG_pSemaphoreName)
{
	STD_error_t L_stderrState = E_OK;

	/* Disable interrupts for atomic operation	*/
	__asm volatile("CPSID I");

	if( ARG_pSemaphoreName != NULL_POINTER)
	{
		*ARG_pSemaphoreName = 1; /* Unlock the Binary Semaphore */
	}
	else
	{

		L_stderrState = E_NOK;
	}

	/* Re-enable interrupts */
	__asm volatile("CPSIE I");

	return L_stderrState;
}




void OS_voidIdleTaskHandler(void)
{
	while(1);
}

u32 OS_u32GetCurrentPSPValue(void)
{

	return OS_SysTasks[OS_CurrentTask].PSPValue;

}

void OS_voidSavePSPValue( u32 ARG_u32CurrentPSPValue)
{

	OS_SysTasks[OS_CurrentTask].PSPValue = ARG_u32CurrentPSPValue;
}

void Enable_FaultHandler(void)
{
	SCB->SHCSR |= 1 << 16 ; 	/* Enable Mem Manage Fault	*/
	SCB->SHCSR |= 1 << 17 ; 	/* Enable Bus Fault			*/
	SCB->SHCSR |= 1 << 18 ; 	/* Enable Usage Fault		*/

}

__attribute__((naked)) void PendSV_Handler(void)
{

	/* Save LR before Jump */
	__asm volatile("PUSH {LR}");

 /* Save context switch of Current task */
	/* Get current running Task's PSP value*/
	__asm volatile("MRS R0, PSP");

	/* Store SF2 (R4-> R11 ) */
	__asm volatile(" STMDB R0!, {R4-R11}");

	/* Save current PSP value */
	__asm volatile("BL OS_voidSavePSPValue");


 /* Retrieve context of next task */
	/* decide next Task to run */
	__asm volatile("BL OS_voidScheduler");

	/* get PSP value of Current Task */
	__asm volatile("BL OS_u32GetCurrentPSPValue");

	/* retrieve LR value */
	__asm volatile("POP {LR}");

	/* retrieve SF2 (R4-> R11 ) */
	__asm volatile("LDMIA R0!, {R4-R11}");

	/* update PSP value */
	__asm volatile("MSR PSP, R0");

	/* exit */
	__asm volatile("BX LR");

}

void SysTick_Handler(void)
{

	OS_u32TickCount ++ ;

	/* Trigger PendSV here */
	SCB->ICSR |= 1 << 28 ;

}
