/******************************************************************************
 *
 * Module: main
 *
 * File Name: Control_ECU_main.c
 *
 * Description: source file for the main function of the system
 *
 * Author: Esraa Ali
 *
 *******************************************************************************/

#include "Control_system.h"

int main(void) {
	/*Initialize the buzzer*/
	Buzzer_init();

	/*Initialize the DC_Motor*/
	DcMotor_Init();

	/* Initialize the UART driver with Baud-rate = 9600 bits/sec */
	UART_config uart_config ={Async,enable_evenPolarity,one_bit};
	// initialization of UART
	UART_init(&uart_config);

	//	initialize the I2C driver whith prescaler 1
	I2C_config i2c_config = {prescaler_1,2,ADDRESS};
	TWI_init(&i2c_config);

	/*initialize the timer driver
	 * channel -> timer0
	 * clock -> 1024
	 * mode -> overflow mode (normal)*/

	TIMER_config timer_config ={timer0,t_clk1024,normal,0};
	Timer_init(&timer_config);
	Timer_setCallBack(timer0,timer_decrement_counter);

	/*Enable global interrupt I-Bit*/
	SREG |= (1 << 7);

	g_order = enterPass;
	while (1) {

		switch (g_order) {
		/*
		 *case of the start of the system
		 * recieve the entred password through UART
		 */
		case enterPass:
			recieve_Pass();
			g_order = sendByUART;
			break;
			/*
			 *At this case we save the new password at eeprom after check if the two passwords matching or not
			 */
		case sendByUART:
			save_in_EEPROM();
			g_order = displayMenu;
			break;
			/*
			 * Receive option menu
			 * + for open door
			 * - for change password
			 */

		case displayMenu:
			recieve_optionMenu();
			break;

			/*
			 * At this case there is 3 attempts to enter the pass correct
			 * If the password is wrong -> the buzzer will turn on
			 */
		case doorState:
			/*Receive the pass from MCU1*/
			recieve_Pass();
			/*check if the receive pass matching with the saved one or not */
			check_matchingEEPROM();
			/*send the check of passwords */
			UART_sendByte(checkPass);
			/*check if the user try the 3 attempts or not */
			if (checkPass == wrongPass) {
				counter++;
				g_order=doorState;
				/*the user run out the range (3attempts) so turn on the buzzer  */
				if (counter == 3) {
					counter = 0;
					g_order = wrongPassword;
					UART_sendByte(g_order);
					break;
				}
				UART_sendByte(g_order);
				break;
			} else if (checkPass == correctPass) {
				/*
				 *If the pass is correct then make the counter=0
				 * And turn on the dc_motor to rotate clock wise 15 seconds counted by timer
				 */
				counter = 0;
				matched_State();
				g_order=2;
				break;
			}
			break;

			/*
			 * This case reset the password by entering the old one and change it
			 * There are 3 attempts for entering the old password correct
			 * if the user do not enter the password correct in 3 attempts so it will stop and
			 * the buzzer will turn on 60 seconds counted by timer
			 */
		case changePass:
			/*Receive the pass from MCU1*/
			recieve_Pass();
			/*check if the receive pass matching with the saved one or not */
			check_matchingEEPROM();
			/*send the check password */
			UART_sendByte(checkPass);
			/*check if the user try the 3 attempts or not */
			if (checkPass == 0) {
				counter++;
				g_order=changePass;
				/*the user run out the range (3attempts) so turn the buzzer  */
				if (counter == 3) {
					counter = 0;
					g_order = wrongPassword;
					UART_sendByte(g_order);
					break;
				}

				UART_sendByte(g_order);
			} else if (checkPass == correctPass) {
				/*
				 *If the pass is correct then make the counter=0
				 * and then receive the new password from MCU1
				 * and save the new password at eeprom
				 */
				counter = 0;
				recieve_Pass();
				save_in_EEPROM();

			}
			recieve_optionMenu();
			break;
			/*
			 * The code will run this case in case the user entered wrong password for 3 times
			 * Turn on the buzzer for 60 seconds counted by timer
			 */
		case wrongPassword:
			wrong_Passwords();
			g_order=displayMenu;
			break;
		}
	}
}


