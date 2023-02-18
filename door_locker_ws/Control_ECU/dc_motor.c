/******************************************************************************
 *
 * Module: DC motor
 *
 * File Name: lcd.c
 *
 * Description: Source file for the DC motor driver
 *
 * Author: Esraa Ali
 *
 *******************************************************************************/

#include "dc_motor.h" /* header file of DC motor*/
#include "common_macros.h"
#include "gpio.h" /* header file of GPIO*/
#include "avr/io.h" /* To use the DC motor register*/
#include "pwm.h" /* header file of PWM*/
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/***************************************************
 * Description : Initialize the direction of Motor
 * Argument    : None
 * Returns     : None
 ***************************************************/

void DcMotor_Init(void){
	GPIO_setupPinDirection(PORTC_ID,PIN5_ID,PIN_OUTPUT); /* set PB0 as Output PIN */
	GPIO_setupPinDirection(PORTC_ID,PIN6_ID,PIN_OUTPUT); /* set PB1 as Output PIN*/
	//initialize the motor to be stopped
	GPIO_writePin(PORTC_ID,PIN5_ID,LOGIC_LOW);
	GPIO_writePin(PORTB_ID,PIN6_ID,LOGIC_LOW);
}
/***************************************************
 * Description : -The function responsible for rotate the DC Motor CW/ or A-CW or
				 stop the motor based on the state input state value.
				 -Send the required duty cycle to the PWM driver based on the
				 required speed value
 * Argument    : Dcmotor state -> stop/clockwise/anti clockwise
 *
 * Returns      : No Return Type
 ***************************************************/
void DcMotor_Rotate(DC_Motor_state state )
{

 if (state==DC_Motor_clockwise)
{
	GPIO_writePin(DC_MOTOR_PORT,DC_MOTOR_FIRST_PIN,LOGIC_HIGH);
	GPIO_writePin(DC_MOTOR_PORT,DC_MOTOR_SECOND_PIN,LOGIC_LOW);
}
else if (state==DC_Motor_Anticlockwise)
{
	GPIO_writePin(DC_MOTOR_PORT,DC_MOTOR_FIRST_PIN,LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_PORT,DC_MOTOR_SECOND_PIN,LOGIC_HIGH);
}else{
	GPIO_writePin(DC_MOTOR_PORT,DC_MOTOR_FIRST_PIN,LOGIC_LOW);
	GPIO_writePin(DC_MOTOR_PORT,DC_MOTOR_SECOND_PIN,LOGIC_LOW);
}
}
