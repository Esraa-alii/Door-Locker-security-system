/******************************************************************************
 *
 * Module: system
 *
 * File Name: Control_system.h
 *
 * Description: Header file for the system
 *
 * Author: Esraa Ali
 *
 *****************************************************************************/

#ifndef CONTROL_SYSTEM_H_
#define CONTROL_SYSTEM_H_

/***********************************************************************************************************
 *                                        Includes                                                      *
 ***********************************************************************************************************/
#include"std_types.h"
#include"timer.h"
#include"uart.h"
#include"dc_motor.h"
#include"twi.h"
#include"external_eeprom.h"
#include"buzzer.h"
#include"gpio.h"
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

/***********************************************************************************************************
 *                                        Definitions                                                      *
 ***********************************************************************************************************/
#define NUMBER_OF_OVERFLOWS_PER_SECOND 32
#define M1_READY 0x20
#define M2_READY 0x10
//slave address
#define ADDRESS 0x01

/***********************************************************************************************************
 *                                       Extern Variables                                                 *
 **********************************************************************************************************/
extern uint8 volatile g_order;
extern uint16 volatile g_timer_tick;
extern uint8 checkPass;
extern uint8 password_saved[5];
extern uint8 password_recieved[5];
extern uint8 i;
extern uint8 selected_option;
extern uint8 attempts;
extern uint8 counter;

/***************************************************************************************************************
                                        Types Declaration
 **************************************************************************************************************/
typedef enum{
	wrongPass,correctPass
};

typedef enum{
	enterPass,sendByUART,displayMenu,doorState,changePass,wrongPassword
};
/**********************************************************************************************************
 *                                      Function Prototypes                                               *
 **********************************************************************************************************/

/***************************************************
 * Description : count time for call back
 * Argument    : None
 * Returns     : None
 ***************************************************/

void timer_decrement_counter(void);
void timer(uint8 volatile g_seconds_required);

/***************************************************
 * Description : Receive password from HMI_ECU
 * Argument    : None
 * Returns     : None
 ***************************************************/
void recieve_Pass(void);

/***************************************************
 * Description : Check if the 2 passwords matching
 * Argument    : None
 * Returns     : None
 ***************************************************/

void check_matchingEEPROM();

/***************************************************
 * Description : Save password to EEPROM
 * Argument    : None
 * Returns     : None
 ***************************************************/

void save_in_EEPROM();

/***************************************************
 * Description : Receive option menu from HMI_ECU
 * 					+ -> OPEN DOOR
 * 					- -> CHANGE PASSWORD
 * Argument    : None
 * Returns     : None
 ***************************************************/

void recieve_optionMenu(void);

/***************************************************
 * Description : if the user choose + from the menu -> turn o the dc motor(door)
 * Argument    : None
 * Returns     : None
 ***************************************************/

void matched_State(void);

/***************************************************
 * Description : if the user enter wrong pass for 3 times -> activate alarming system
 * Argument    : None
 * Returns     : None
 ***************************************************/
void wrong_Passwords(void);

#endif
