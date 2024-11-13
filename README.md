
# Stm32f7-bootloader 
UART bootloader for STM32F7 microcontroller.

### Table of contents
- [Introduction](#introduction)
- [Project Organization](#project-organization)
- [How it works](#how-it-works)
  - [Overall](#overall)
  - [STM32F767ZI Memory Organization](#stm32f767zi-memory-organization)
  - [Reset Sequence and Memory Aliasing of the MCU](#reset-sequence-and-memory-aliasing-of-the-mcu)
- [How to use it](#how-to-use-it)
  - [Embedded](#embedded)
  - [PC](#pc)
  - [Porting](#porting)
- [References](#references)
- [Acknowledgments](#acknowledgments)

### Introduction
This repository contains a custom bootloader developed for the STM32F767ZI Nucleo board. The bootloader enables firmware updates over UART using the XMODEM protocol, making it suitable for applications requiring  serial-based firmware updates.

Features
 - **UART Communication**[3]: Utilizes UART to interface with a host device for firmware upload.
 - **XMODEM Protocol**[1][2]: Implements XMODEM for reliable, packet-based data transmission,    ensuring accurate delivery and error detection.
 - Supports 128 and 1024 bytes data length
 
 ### Project Organization
* 📂 [Documents](https://github.com/mattsousaa/STM32F767ZI_Bootloader/tree/master/00_Documents) - Board and design reference manuals.
* 📂 [Bootloader](https://github.com/mattsousaa/STM32F767ZI_Bootloader/tree/master/01_Bootloader) - Individual Bootloader project.
* 📂 [Application](https://github.com/mattsousaa/STM32F767ZI_Bootloader/tree/master/02_User_app_STM32F7xxx) - User test application.


### How it Works

#### Overall
The bootloader was developed for the STM32F767ZI Nucleo board, requiring only the st-link on board  connected to the PD8 (RX) and PD9 (TX) pins(USART3).

<img src="https://github.com/Ayub2001/stm32F7_Bootloader/blob/master/Documents/Screenshot%202024-11-13%20202444.png" > <br>
*Figure 1. Pinout of the system.*

Upon start-up, the system sends a welcome message through UART. Instead of checking the user button, the bootloader immediately enters bootloader mode, turns on the green (PC9) LED, and waits for a new binary file to be received. Once the file is received, the bootloader loads it, and if no update is in progress, it automatically jumps to the user application.

<img src="https://github.com/Ayub2001/stm32F7_Bootloader/blob/master/Documents/Screenshot%202024-11-13%20204428.png" > <br>
*Figure 2. Brief overview of the workflow of the system.*

The Xmodem protocol is clearly explained in the [references](#references).

#### STM32F767ZI Memory Organization
* Internal Flash memory also called as Embedded Flash memory of 2MB
* Internal SRAM1 of 368KB
* Internal SRAM2 of 16KB
* System Memory (ROM) of 60KB
* OTP memory of 1024 bytes
* Option bytes memory of 32 bytes
* Backup RAM of 4KB

#### Reset Sequence and Memory Aliasing of the MCU
1. When you reset the MCU, the PC of the processor is loaded with the value *0x0000_0000*.
2. Then processor reads the value @ memory location *0x0000_0000* into MSP (Main stack pointer register). That means, processor first initializes the Stack pointer register.
3. After that, processor reads the value @ memory location *0x0000_0004* in to PC.
4. PC jumps to the reset handler.
5. A reset handler is just a C or Assembly function written by you to carry out any initializations required.
6. From reset handler you call your main function of the application.

### How to use it
#### Embedded
To use the bootloader, just get the softwares mentioned in the [Introduction](#introduction) and flash it. 

I included an example binary file called "blinky.bin"(Application) in the root folder. It blinks the blue  LED on the board.
To make your own binary, you have to modify the memory location in the linker script (STM32F767ZITX_FLASH.ld):
```
FLASH (rx)      : ORIGIN = 0x8008000,
```
And the vector table offset in system_stm32f7xx.c
```
#define VECT_TAB_OFFSET  0x00008000U
```
Last step is you have to generate a \*.bin file:


#### PC
To update the firmware, you need a terminal software, that supports Xmodem. I recommend  Tera Term [[5]](#references).

Configure them in the following way:
- Baud rate: 115200
- Data bits: 8
- Parity: none
- Stop bits: 1

In Tera Term: select *File* >> *Transfer* >> *Xmodem* >> *Send* and then open the binary file.
<img src="https://github.com/Ayub2001/stm32F7_Bootloader/blob/master/Documents/Screenshot%202024-11-13%20223622.png" > <br>
*Figure : Tera Term.*

In case everything was fine, then the output should be the same:
```
================================
Created By Oubrahim
UART Bootloader
================================

Please send a new binary file with Xmodem protocol to update the firmware.
CCCCCCC
Firmware updated!
Jumping to user application...
```

#### Porting
I included the *.ioc file, so the drivers can be regenerated for any ST microntroller (if it has similar memory structure).
If you have a non-ST microntroller, then the xmodem protocol can be reused, but nothing else.
### Acknowledgments
Special thanks to Sigma Embedded [5]

### References
[1] [Xmodem protocol with CRC](https://web.mit.edu/6.115/www/amulet/xmodem.htm)<br>

[2] [Chuck Forsberg - XMODEM/YMODEM PROTOCOL REFERENCE](http://www.blunk-electronic.de/train-z/pdf/xymodem.pdf)<br>

[3] [UART](https://www.geeksforgeeks.org/universal-asynchronous-receiver-transmitter-uart-protocol/)<br>

[4] [Tera Term](https://ttssh2.osdn.jp/)<br>

[5] [Sigma Embedded ]( https://www.linkedin.com/company/sigma-embedded/)<br>


