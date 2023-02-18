
/***********************************************************************************************************
 * Module :main
 *
 * File Name :HMI_ECU_main.c
 *
 * Description: source file for the main function of the system
 *
 * Author:Esraa Ali
 *
 ***********************************************************************************************************/
#include "HMI_system.h"
//#define ENTER_KEY 13
//uint16 Password1;
//uint8 g_matchedflag;
//uint16 Password2;
//void Password_savePassword(void)
//{
//	uint8 key2,key;
//	LCD_clearScreen();
//	do
//	{
//
//		LCD_displayStringRowColumn(0, 0, "Plz Enter pass:");
//		LCD_moveCursor(1, 0);
//		while(KEYPAD_getPressedKey() != ENTER_KEY){
//			for(uint8 i = 0 ; i < 5 ; i++)
//			{
//				key = KEYPAD_getPressedKey();
//				if(key <= 9 && key >= 0)
//				{
//					Password1=Password1*10+key;
//					//LCD_displayCharacter('*');
//					LCD_intgerToString(key);
//				}
//				else
//				{
//					i--;
//				}
//				_delay_ms(500);
//			}
//		}
//
//
//		LCD_clearScreen();
//		LCD_displayStringRowColumn(0,0," Re-Enter pass:");
//		LCD_moveCursor(1, 0);
//		while(KEYPAD_getPressedKey() != ENTER_KEY){
//			for(uint8 i = 0 ; i < 5 ; i++)
//			{
//				key2 = KEYPAD_getPressedKey();
//				if(key2 <= 9 && key2 >= 0)
//				{
//					Password1=Password1*10+key2;
//					//LCD_displayCharacter('*');
//					LCD_intgerToString(key2);
//				}
//				else
//				{
//					i--;
//				}
//				_delay_ms(500);
//			}
//		}
//
//		if(Password1 != Password2)
//		{
//			g_matchedflag= 0;
//			break;
//		}
//
//		LCD_clearScreen();
//		if(g_matchedflag== 1)
//		{
//			LCD_displayStringRowColumn(0, 4, "Matched");
//			UART_sendByte(Password1);
//			_delay_ms(50);
//			UART_sendByte((uint8)(Password1>>8));
//
//
//		}
//		else
//		{
//			LCD_displayStringRowColumn(0, 0, "UnMatched");
//		}
//		_delay_ms(1000);
//		LCD_clearScreen();
//
//	}while(g_matchedflag == 0);
//}

int main(void){
	//intialize the LCD
	LCD_init();

	/* Initialize the UART driver with Baud-rate = 9600 bits/sec */
	UART_config uart_config ={Async,enable_evenPolarity,one_bit};
	UART_init(&uart_config);

	/*initialize the timer driver
	 * channel -> timer0
	 * clock -> 1024
	 * mode -> overflow mode (normal)*/
	TIMER_config timer_config ={timer0,t_clk1024,normal,0};
	Timer_init(&timer_config);
	Timer_setCallBack(timer0,timer_decrement_counter);

	/*open pin of global interrupt the I-bit*/

	SREG |= (1 << 7);

	g_order = enterPass;
//	Password_savePassword();
	while (1) {

		switch (g_order) {
		/*
		 * At the start if the system ask the user to enter pass and confirm it
		 */
		case enterPass:

			Enter_Password();
			/*checking if the password matched then go to step 1
			 * if unmatched repeat this step again
			 */
			if (checking_value == correctPass) {
				g_order = sendByUART;
			} else {
				LCD_clearScreen();
				LCD_moveCursor(0,0);
				LCD_displayString("Wrong password!");
				_delay_ms(1000);
				LCD_clearScreen();
				g_order =enterPass ;
			}
			LCD_clearScreen();

			break;

			/*
			 * if the password confirmed send it through the UART to the Control_ECU
			 */
		case sendByUART:
			LCD_clearScreen();
			LCD_moveCursor(0,0);
			LCD_displayString("Correct pass!");
			_delay_ms(500);
			send_Pass_ToMcu2();
			LCD_clearScreen();
			LCD_clearScreen();
			LCD_moveCursor(0,0);
			LCD_displayString("Password saved!");
			_delay_ms(500);
			g_order = displayMenu;
			break;

			/*
			 * display the option menu
			 * + -> open door
			 * - -> change password
			 */
		case displayMenu:
			display_optionsMenu();
			if (selected_option == '+') {
				g_order = doorState;
			} else if (selected_option == '-') {
				g_order = changePass;
			}

			break;
			/*
			 * This case we open the system :
			 * enter the saved pass if the pass matched with the eeprom {
			 * open the door and turn on the DC_motor and rotate clock wise  for 15 seconds
			 * and wait 3 seconds and close the door and rotate the DC_motor Anti clock wise for 15 seconds }
			 * if the password unmatched their is 3 attempts to enter the correct and if the user does not
			 * enter the correct one the buzzer will turn on for 60 seconds and
			 */
		case doorState:
			LCD_clearScreen();
			LCD_moveCursor(0, 0);
			LCD_displayString("Enter password");
			LCD_moveCursor(1, 4);
			enter_saved_pass();
			LCD_moveCursor(1, 4);
			send_Pass_ToMcu2();
			LCD_clearScreen();
			checker = UART_recieveByte();
			check_Pass(checker);
			if(do_case)
				matched_State();
			g_order=displayMenu;
			break;
			/*
			 * This case change the password by reset the old password and change it
			 */
		case changePass:
			LCD_clearScreen();
			LCD_moveCursor(0, 0);
			LCD_displayString("Enter old pass");
			LCD_moveCursor(1, 4);
			/*Enter the old password */
			enter_saved_pass();
			LCD_moveCursor(1, 4);
			/*Then send to MCU2 to compare with that saved in EEprom*/
			send_Pass_ToMcu2();
			LCD_clearScreen();
			/*receive the state from MCU2 in checker
			 * if the checker = 1 so enter the new password twice and save in eeprom
			 * if checker = 0 so return to case 0 to try again to enter the correct old password
			 */
			checker = UART_recieveByte();
			check_Pass(checker);
			if(do_case){
				Enter_Password();
				if (checking_value == correctPass) {
					g_order = sendByUART;
				} else {
					LCD_clearScreen();
					LCD_moveCursor(0, 0);
					LCD_displayString("Wrong Password!");
					_delay_ms(1000);
					LCD_clearScreen();
					g_order = enterPass;
				}
				LCD_clearScreen();
			}
			break;
			/*
			 * the code will go to this step in step the user entered wrong password for 3 times
			 * and turn on the buzzer for 60 seconds
			 * and display warning message
			 */
		case wrongPassword:
			wrong_Passwords();
			g_order=displayMenu;
			break;

		}
	}
}


