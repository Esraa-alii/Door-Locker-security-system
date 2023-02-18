/******************************************************************************
 *
 * Module: system
 *
 * File Name: HMI_system.c
 *
 * Description: source file for the system
 *
 * Author: Esraa Ali
 *
 *******************************************************************************/

#include "HMI_system.h"

/***********************************************************************************************************
 *                                       Global Variables                                                  *
 ***********************************************************************************************************/
uint16 volatile g_timer_tick = 0;
uint8 volatile g_order;
uint8 pass[5];
uint8 confirmPass[5];
uint8 i;
uint8 do_case;
uint8 volatile selected_option;
uint8 volatile checker;
uint8 volatile checking_value;


/***********************************************************************************************************
 *                                      Function Definitions                                               *
 **********************************************************************************************************/
/***************************************************
 * Description : count time for call back
 * Argument    : None
 * Returns     : None
 ***************************************************/
void timer_decrement_counter(void) {
	/*increases the global tick each time the timer interrupt*/
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
 * Description : Ask the user to enter the pass and confirm it
 * Argument    : None
 * Returns     : None
 ***************************************************/
void Enter_Password(void) {
	/*receive the first password from the keypad and store at array*/
	LCD_displayString("Enter Password");
	LCD_moveCursor(1, 4);
	for (i = 0; i < 5; i++) {
		pass[i] = KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		_delay_ms(500);
		/*if we press on ON/C key it clear the pass that entered and would start again from 0*/
		if (pass[i] == 13) {
			/*clear the screen */
			LCD_clearScreen();
			/*enter the new password */
			LCD_displayString("Enter new password");
			LCD_moveCursor(0, 0);

		}
	}
	/*Enter the second password from the keypad and store at array */
	LCD_clearScreen();
	LCD_displayString("Re-Enter Pass");
	LCD_moveCursor(1, 4);
	for (i = 0; i < 5; i++) {
		confirmPass[i] = KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		_delay_ms(500);
		/*if we press on ON/C key it clear the pass that entered and would start again from 0*/
		if (confirmPass[i] == 13) {
			/*clear the screen */
			LCD_clearScreen();
			/*enter the new password */
			LCD_displayString("Enter new password");
			LCD_moveCursor(2, 0);

		}
	}
	/*calling the function to compare between two passwords */
	check_matchedPass();
}

/***************************************************
 * Description : check if the 2 pass matched or not
 * Argument    : None
 * Returns     : None
 ***************************************************/

void check_matchedPass() {
	for (i = 0; i < 5; i++) {
		if (pass[i] != confirmPass[i]) {
			LCD_clearScreen();
			LCD_moveCursor(0,0);
			checking_value = wrongPass;
			break;
		}
	}
	if(i==5){
		checking_value = correctPass;
	}
}

/***************************************************
 * Description : send pass to Control_ECU by UART
 * Argument    : None
 * Returns     : None
 ***************************************************/
void send_Pass_ToMcu2(void){
	//	LCD_displayString("Press=to send password");
	if (KEYPAD_getPressedKey() == '=') {
		for (i = 0; i < 5; i++) {
			UART_sendByte(pass[i]);
		}
	}

}

/***************************************************
 * Description : take choice from options menu
 * 				+ -> open door
 * 				- -> change pass
 * Argument    : None
 * Returns     : None
 ***************************************************/
void options_menu(void) {

	/*repeats until the user choose one of two options */
	do {
		selected_option = KEYPAD_getPressedKey();
	} while ((selected_option != '+') && (selected_option != '-'));

	/*sends to MCU2 the option which the user chose*/
	UART_sendByte(selected_option);
	LCD_clearScreen();

	_delay_ms(500);
	/*decide where we need the code run */
	if (selected_option == '+') {
		g_order = doorState;
	} else if (selected_option == '-') {
		g_order = changePass;
	}

}

/***************************************************
 * Description : store entered pass from keypad to an array
 * Argument    : None
 * Returns     : None
 ***************************************************/

void enter_saved_pass(void) {
	for (i = 0; i < 5; i++) {
		pass[i] = KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		_delay_ms(500);
		/*if we press on ON/C key it clear the pass that entered and would start again from 0*/
		if (pass[i] == 13) {
			/*clear the screen */
			LCD_clearScreen();
			/*enter the new password */
			LCD_displayString("Enter new password");
			LCD_moveCursor(0, 0);
			i = 0;
		}
	}

}
/***************************************************
 * Description : display options menu
 * 				+ -> open door
 * 				- -> change pass
 * Argument    : None
 * Returns     : None
 ***************************************************/
void display_optionsMenu(void) {
	LCD_clearScreen();
	LCD_moveCursor(0, 0);
	LCD_displayString("+: Open door ");
	LCD_moveCursor(1, 0);
	LCD_displayString("-: Change Pass");
	options_menu();
}

/***************************************************
 * Description : check if the received pass is correct or not
 * Argument    : check password
 * Returns     : None
 ***************************************************/
void check_Pass(uint8 check_password) {
	/*In this case if the password unmatched
	 *so display on the screen unmatched
	 *and receive from the MCU2  the step that will go to
	 */
	if (check_password == 0) {
		LCD_displayString("Wrong Password");
		_delay_ms(60000);
		LCD_clearScreen();
		g_order = UART_recieveByte();
		do_case=wrongPass;

	}

	/*
	 * In this case if the password is matched
	 * So display on the screen Matched password
	 * and open the door and turn on the DC_motor and rotate clock wise for 15 seconds
	 * and display (The door is opening) while the DC_motor is rotating
	 *
	 */
	else if (check_password == 1) {
		LCD_displayString("correct Password");
		_delay_ms(1000);
		LCD_clearScreen();
		do_case=correctPass;
	}
}

/***************************************************
 * Description : display the state of dc motor -> opening / stop / closing
 * Argument    : None
 * Returns     : None
 ***************************************************/
void matched_State(void) {
	/*display on the lcd message for 15 seconds then clear the screen*/
	LCD_displayString("Opening the door");
	timer(15);
	LCD_clearScreen();
	LCD_moveCursor(0, 0);
	/*display on the lcd message for 3 seconds then clear the screen*/
	LCD_displayString("Door close in 3s");
	timer(3);
	LCD_clearScreen();
	LCD_moveCursor(0, 0);
	/*display on the lcd message for 15 seconds then clear the screen*/
	LCD_displayString("Closing The door");
	timer(15);
	LCD_clearScreen();
}

/***************************************************
 * Description : if the user enter wrong pass for 3 times -> activate alarming system
 * Argument    : None
 * Returns     : None
 ***************************************************/
void wrong_Passwords(void) {
	LCD_clearScreen();
	LCD_moveCursor(0, 0);
	LCD_displayString("Wrong Password!");
	_delay_ms(60000);
	timer(30);
	display_optionsMenu();
}

