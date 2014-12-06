/*
 * commands.h
 *
 *  Created on: May 1, 2013
 *      Author: Montoya
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_



/* Before we can write to RAM  we need to initialize the HTC chip
    Commands in order to initizile consit of 3 bits(100) followed by
    9 bits of the command code
    Power on

    SYS DIS              CMD= 0000-0000-x Turn off oscil
    COM Option           CMD= 0010-ABxx-x commons options
    Master Mode          CMD= 0001-01xx-x Master Mode
    Slave Mode           CMD= 0001-00xx-x Slave Mode
    SYS On               CMD= 0000-0001-x Enable system oscil
    LED ON               CMD= 0000-0011-x LEDs ON

    we can then write to ram now and in order to write to ram which
    consit of 3 bits (101) followed by 7 bits of address and 4 bits of Data
    Write RAM Data
    Update RAM Data

 */
#define HT1632_ID_CMD 0x04      /* ID = 100 - Commands */
#define HT1632_ID_RD  0x06      /* ID = 110 - Read RAM */
#define HT1632_ID_WR  0x05      /* ID = 101 - Write RAM */
#define HT1632_ID_BITS (1<<2)   /* IDs are 3 bits */

#define HT1632_CMD_SYSDIS 0x00  /* CMD= 0000-0000-x Turn off oscil */                  //1
#define HT1632_CMD_SYSON  0x01  /* CMD= 0000-0001-x Enable system oscil */             //5
#define HT1632_CMD_LEDOFF 0x02  /* CMD= 0000-0010-x LED duty cycle gen off */
#define HT1632_CMD_LEDON  0x03  /* CMD= 0000-0011-x LEDs ON */                         //6
#define HT1632_CMD_BLOFF  0x08  /* CMD= 0000-1000-x Blink ON */
#define HT1632_CMD_BLON   0x09  /* CMD= 0000-1001-x Blink Off */
#define HT1632_CMD_SLVMD  0x10  /* CMD= 0001-00xx-x Slave Mode */                      //
#define HT1632_CMD_MSTMD  0x14  /* CMD= 0001-01xx-x Master Mode */                     //3
#define HT1632_CMD_RCCLK  0x18  /* CMD= 0001-10xx-x Use on-chip clock */               //4
#define HT1632_CMD_EXTCLK 0x1C  /* CMD= 0001-11xx-x Use external clock */
#define HT1632_CMD_COMS00 0x20  /* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_COMS01 0x24  /* CMD= 0010-ABxx-x commons options */                 //2
#define HT1632_CMD_COMS10 0x28  /* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_COMS11 0x2C  /* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_PWM    0xA0  /* CMD= 101x-PPPP-x PWM duty cycle */
#define HT1632_CMD_BITS (1<<7)
#define HT1632_WRITE 0x5
#define HT1632_LED_ON 0x03


#endif /* COMMANDS_H_ */
