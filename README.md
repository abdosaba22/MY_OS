# Custom OS for ARM-based STM32 Microcontrollers

## Overview
This repository contains the source code for a custom operating system developed for ARM Cortex-M microcontrollers, specifically targeting the STM32NucleoF446RE board. The OS includes a scheduler with task management, a binary semaphore implementation, and an example of a multi-tasking environment.

## Features
- Task scheduling based on priority
- Context switch using MSP and PSP
- PendSV for context switching
- Systick Timer for task delays
- Binary semaphores for task synchronization
- SWV ITM for debugging with printf statements
- Toggle Pin functionality for task signaling

## Supported Platforms
- STM32NucleoF446RE
- ARM Cortex-M microcontrollers (with proper adaptation)

## Getting Started
Follow these steps to get started with the custom OS:

1. **Setup Development Environment:**
   - Install the necessary toolchain and IDE for STM32 development.
   - Ensure you have the STM32CubeMX or similar tool for configuring the microcontroller.

2. **Compile and Flash the OS:**
   - Clone this repository to your local machine.
   - Open the project in your STM32 IDE and configure it using STM32CubeMX if required.
   - Compile the project and flash it onto the STM32NucleoF446RE board.

3. **Configure and Build the Project:**
   - Customize the `Task1_Handler` and other tasks as needed.
   - Adjust the scheduler parameters and settings in the configuration files.

4. **Debugging with SWV ITM:**
   - Use SWV ITM for debugging by adding printf statements to your tasks.
   - Toggle pins for additional signaling during debugging.

## Example Tasks
Here's an example of a task function (`Task1_Handler`) that prints a message and toggles a GPIO pin:

```c
void Task1_Handler(void) {
    OS_stderrCreateBinarySemaphore(&G_SemaphorePrint);

    while (1) {
        if (E_OK == OS_stderrTakeSemaphore(&G_SemaphorePrint, 10)) {
            printf("Hello from Task1 Handler...\r\n\n");
            MGPIO_stderrTogglePinValue(PORTA, PIN4);
            OS_stderrReleaseSemaphore(&G_SemaphorePrint);
        }

        OS_voidTaskDelay(1000);  // 1.0s delay
    }
}
