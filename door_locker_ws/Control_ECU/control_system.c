
/******************************************************************************
 *
 * Module: system
 *
 * File Name: control_system.c
 *
 * Description: source file for the system
 *
 * Author: Esraa Ali
 *
 *****************************************************************************/

#include "Control_system.h"

/***********************************************************************************************************
 *                                             Global variables                                           *
 **********************************************************************************************************/

uint8 volatile g_order = 0;
uint16 volatile g_timer_tick = 0;
uint8 checkPass;
uint8 password_saved[5];
uint8 password_recieved[5];
uint8 i;
uint8 selected_option = 0;
uint8 attempts = 0;
uint8 counter = 0;


/***********************************************************************************************************
 *                                           Function Definitions                                          *
 **********************************************************************************************************/

/***************************************************
 * Description : count time for call back
 * Argument    : None
 * Returns     : None
 ***************************************************/

void timer_decrement_counter(void) {
	//inctement tick every interrupt
	g_timer_tick++;
}

void timer(uint8 volatile g_seconds_required) {
	TIMER_config timer_config ={timer0,t_clk1024,normal,0};
	Timer_init(&timer_config);
	Timer_setCallBack(timer0,timer_decrement_counter);
	while (g_timer_tick != ((NUMBER_OF_OVERFLOWS_PER_SECOND) * g_seconds_required));
	Timer_deInit(timer0);
	g_timer_tick = 0;
}
/***************************************************
 * Description : Receive password from HMI_ECU
 * Argument    : None
 * Returns     : None
 ***************************************************/
void recieve_Pass(void) {
	/*Receiving the first password from the MCU1 (byte by byte)*/
	for (i = 0; i < 5; i++) {
		password_recieved[i] = UART_recieveByte();
	}
}

/***************************************************
 * Description : Check if the 2 passwords matching
 * Argument    : None
 * Returns     : None
 ***************************************************/

void check_matchingEEPROM() {

	for (i = 0; i < 5; i++) {
		/*read the password from EEPROM*/
		EEPROM_readByte(ADDRESS + i, &password_saved[i]);
		if (password_saved[i] != password_recieved[i]) {
			checkPass = wrongPass;
			break;
		}
	}
	if (i == 5) {
		checkPass = correctPass;
	}
}

/***************************************************
 * Description : Save password to EEPROM
 * Argument    : None
 * Returns     : None
 ***************************************************/
void save_in_EEPROM() {

	for (i = 0; i < 5; i++) {
		EEPROM_writeByte((ADDRESS + i), password_recieved[i]);
		_delay_ms(10);
	}
}

/***************************************************
 * Description : Receive option menu from HMI_ECU
 * 					+ -> OPEN DOOR
 * 					- -> CHANGE PASSWORD
 * Argument    : None
 * Returns     : None
 ***************************************************/
void recieve_optionMenu(void) {
	selected_option = UART_recieveByte();
	if (selected_option == '+') {
		g_order = doorState;
		selected_option=0;
	} else if (selected_option == '-') {
		g_order = changePass;
		selected_option=0;
	}
}

/***************************************************
 * Description : if the user choose + from the menu -> turn on the dc motor(door)
 * Argument    : None
 * Returns     : None
 *************************************************/

void matched_State(void) {
	DcMotor_Rotate(DC_Motor_clockwise);
	timer(15);
	DcMotor_Rotate(DC_Motor_stop);
	timer(3);
	DcMotor_Rotate(DC_Motor_Anticlockwise);
	timer(15);
	DcMotor_Rotate(DC_Motor_stop);
}

/***************************************************
 * Description : if the user enter wrong pass for 3 times -> activate alarming system
 * Argument    : None
 * Returns     : None
 ***************************************************/
void wrong_Passwords(void) {
	/*Turn on the buzzer for 60 seconds */
	Buzzer_on();
	_delay_ms(60000);
	//	timer(30);
	//	/*Stop the buzzer and timer  after 60 seconds  */
	//	Timer_deInit(timer1);
	Buzzer_off();
}


