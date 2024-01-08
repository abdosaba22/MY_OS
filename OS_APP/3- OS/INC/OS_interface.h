/* ************************************************ */
/* Author      : Abdalrahman Amin	                */
/* SWC         : Scheduler			                */
/* Layer       : OS                                 */
/* Version     : 1.0                                */
/* Date        : January 7, 2024          		    */
/* Last Edit   : N/A 			                    */
/* ************************************************ */


#ifndef _OS_INTERFACE_H_
#define _OS_INTERFACE_H_

typedef u8 Semaphore_t;

void OS_voidStartOS(void);

/* the Task ID is the priortiy so only one func can take priority 1 or .. 255 */
STD_error_t OS_stderrCreateTask( void (*ARG_pvoidfhandler)(void), u16 ARG_u16Periodicity , u8 ARG_u8Priority , u8 ARG_u8FirstDelay);

STD_error_t OS_stderrDeleteTask( u8 ARG_u8Priority);

STD_error_t OS_stderrActivateTask( u8 ARG_u8Priority);

STD_error_t OS_stderrSuspendTask( u8 ARG_u8Priority);

void OS_voidTaskDelay(u16 ARG_u16Ticks);

STD_error_t OS_stderrCreateBinarySemaphore(Semaphore_t *ARG_pSemaphoreName);

STD_error_t OS_stderrTakeSemaphore(Semaphore_t *ARG_pSemaphoreName, u32 ARG_u32Ticks);

STD_error_t OS_stderrReleaseSemaphore(Semaphore_t *ARG_pSemaphoreName);



#endif
