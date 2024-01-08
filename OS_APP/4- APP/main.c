/* ************************************************ */
/* Author      : Abdalrahman Amin	                */
/* SWC         : main				                */
/* Layer       : APP                                */
/* Version     : 1.0                                */
/* Date        : January 7, 2024          		    */
/* Last Edit   : N/A 			                    */
/* ************************************************ */


#include <stdint.h>
#include <stdio.h>

#include "LBIT_math.h"
#include "LSTD_types.h"

#include "MGPIO_interface.h"
#include "MRCC_interface.h"
#include "MSYSTICK_interface.h"
#include "OS_interface.h"


void Task1_Handler(void); /* Piriority 0 */
void Task2_Handler(void); /* Piriority 1 */
void Task3_Handler(void); /* Piriority 2 */
void Task4_Handler(void); /* Piriority 3 */

Semaphore_t G_SemaphorePrint;

void ConfigureChannles(void)
{

	MGPIO_CONFIG_t Channels=
	{
			.Port=PORTA,
			.Pin=PIN4,
			.Mode=OUTPUT,
			.Speed=LOW,
			.OutputType=PUSH_PULL,
	};

	for(u8 L_u8I = 0; L_u8I < 4 ; L_u8I++)
	{
		MGPIO_stderrPinConfig(&Channels);
		Channels.Pin++;
	}
}

void HardFault_Handler(void){

}
void MemManage_Handler(void){

}
void BusFault_Handler(void){

}
void UsageFault_Handler(void){

}



int main(void)
{
	/* RCC Init HSI Clock 16Mhz */
	MRCC_voidInit();

	MRCC_stderrEnablePeripheralClock(AHB1_GPIOA_EN);
	ConfigureChannles();

	/* *
	 * pointer To function,
	 * periodicity,
	 * priority,
	 * first delay
	 * */
	OS_stderrCreateTask(&Task1_Handler, 0, 0, 0);
	OS_stderrCreateTask(&Task2_Handler, 0, 1, 0);
	OS_stderrCreateTask(&Task3_Handler, 0, 2, 0);
	OS_stderrCreateTask(&Task4_Handler, 0, 3, 0);

	/* Start OS */
	OS_voidStartOS();

	/* Loop forever */
	for(;;);

}

/* create 4 task use printf */
void Task1_Handler(void)
{
	/* create semaphore */
	OS_stderrCreateBinarySemaphore(&G_SemaphorePrint);

	while(1)
	{
		/* Take Semaphore */
		if( E_OK == OS_stderrTakeSemaphore(&G_SemaphorePrint, 10))
		{
			printf("Hello Form Task1 Hander .. \r\n\n");
			MGPIO_stderrTogglePinValue(PORTA, PIN4);

			/* Release Semaphore */
			OS_stderrReleaseSemaphore(&G_SemaphorePrint);
		}

		/* Take Ticks 1 Tick == 1 ms */
		OS_voidTaskDelay( 1000 ) ; /* 1.0s */

	}
}
void Task2_Handler(void)
{

	while(1)
	{

		/* Take Semaphore */
		if( E_OK == OS_stderrTakeSemaphore(&G_SemaphorePrint, 10))
		{
			printf("Hello Form Task2 Hander .. \r\n\n");
			MGPIO_stderrTogglePinValue(PORTA, PIN5);

			/* Release Semaphore */
			OS_stderrReleaseSemaphore(&G_SemaphorePrint);
		}

		/* Take Ticks, 1 Tick == 1 ms */
		OS_voidTaskDelay( 500 ) ; /* 0.5s */
	}
}
void Task3_Handler(void)
{
	while(1)
	{
		/* Take Semaphore */
		if( E_OK == OS_stderrTakeSemaphore(&G_SemaphorePrint, 10))
		{
			printf("Hello Form Task3 Hander .. \r\n\n");
			MGPIO_stderrTogglePinValue(PORTA, PIN6);

			/* Release Semaphore */
			OS_stderrReleaseSemaphore(&G_SemaphorePrint);
		}

		/* Take Ticks 1 Tick == 1 ms */
		OS_voidTaskDelay( 250 ) ; /* 0.25s */
	}
}
void Task4_Handler(void)
{
	while(1)
	{
		/* Take Semaphore */
		if( E_OK == OS_stderrTakeSemaphore(&G_SemaphorePrint, 10))
		{
			printf("Hello Form Task4 Hander .. \r\n\n");
			MGPIO_stderrTogglePinValue(PORTA, PIN7);

			/* Release Semaphore */
			OS_stderrReleaseSemaphore(&G_SemaphorePrint);
		}

		/* Take Ticks 1 Tick == 1 ms */
		OS_voidTaskDelay( 125 ) ; /* 0.125s */
	}
}



