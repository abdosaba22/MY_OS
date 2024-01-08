/* **************************************************** */
/* Author      	:	 Abd-alrahman Amin					*/
/* SWC         	:	 SCB								*/
/* MCu         	:	 Stm32F446RE						*/
/* Layer       	:	 MCAL              					*/
/* Version     	:	 1.0               					*/
/* Date        	:	 January 1, 2024					*/
/* Last Edit   	:	 N/A			   					*/
/* **************************************************** */

#ifndef _MSCB_PRIVATE_H_
#define _MSCB_PRIVATE_H_


/******************* SCB Peripheral Base Addresses ********************/
#define MSCB_BASE_ADDRESS 			0xE000ED00

/************************** SCB Register Definition Structure ***********************/

typedef struct {
  volatile u32 CPUID;   // CPUID Base Register
  volatile u32 ICSR;    // Interrupt Control and State Register
  volatile u32 VTOR;    // Vector Table Offset Register
  volatile u32 AIRCR;   // Application Interrupt and Reset Control Register
  volatile u32 SCR;     // System Control Register
  volatile u32 CCR;     // Configuration and Control Register
  volatile u32 SHPR1;   // System Handler Priority Register 1
  volatile u32 SHPR2;   // System Handler Priority Register 2
  volatile u32 SHPR3;   // System Handler Priority Register 3
  volatile u32 SHCSR;   // System Handler Control and State Register
  volatile u32 CFSR;    // Configurable Fault Status Register
  /*
            2B       1B     1B
  CFSR --> UFSR   | BFSR | MMFSR

  */
  volatile u32 HFSR;    // HardFault Status Register
  u32 RESERVED0;        // Padding to align next register to 32 bits
  volatile u32 MMAR;    // MemManage Fault Address Register
  volatile u32 BFAR;    // BusFault Address Register
  volatile u32 AFSR;    // Auxiliary Fault Status Register
} MSCB_REGDEf_t;


/************************ SCB Peripheral Definition **********************/
#define SCB 				((MSCB_REGDEf_t*)MSCB_BASE_ADDRESS)


#endif /* MSCB_PRIVATE_H_ */
