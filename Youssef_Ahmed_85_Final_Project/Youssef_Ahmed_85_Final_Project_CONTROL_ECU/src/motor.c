#include <motor.h>

static DcMotor_DirectionType motor_state;
static DcMotor_SpeedType motor_speed;

void DcMotor_Init(void)
{
	/* initialize motor EN1 pin */
	GPIO_setupPinDirection(MOTOR_EN1_PORT, MOTOR_EN1_PIN, PIN_OUTPUT);
	GPIO_writePin(MOTOR_EN1_PORT, MOTOR_EN1_PIN, LOGIC_LOW);

	/* initialize IN1 pin */
	GPIO_setupPinDirection(MOTOR_IN1_PORT, MOTOR_IN1_PIN, PIN_OUTPUT);
	GPIO_writePin(MOTOR_IN1_PORT, MOTOR_IN1_PIN, LOGIC_LOW);

	/* initialize IN2 pin */
	GPIO_setupPinDirection(MOTOR_IN2_PORT, MOTOR_IN2_PIN, PIN_OUTPUT);
	GPIO_writePin(MOTOR_IN2_PORT, MOTOR_IN2_PIN, LOGIC_LOW);

	/* Set timer to 0 duty cycle */
	Timer0_PWM_Init(STOP);

	/* set the initial motor state as STOP */
	motor_state = MOTOR_DIRECTION_OFF;
	motor_speed = MOTOR_SPEED_OFF;
	DcMotor_Rotate(motor_state, motor_speed);
}

/* This function will rotate the motor with a direction and speed which are passed to it as inputs*/
/* The states are: stop, clockwise (CW), and anti-clockwise (A_CW), A_CW will not be used in our application */
void DcMotor_Rotate(DcMotor_DirectionType state, DcMotor_SpeedType speed)
{
	/* check if current state was changed that that stored in the global variable */
	if (state != motor_state)
	{
		/* Now the motor state changed, so lets check to which state exactly */
		switch(state)
		{
			case MOTOR_DIRECTION_CW:
			{
				/* start PWM with the given speed */
				Timer0_PWM_Init(speed);

				/* Rotate the motor CW */
				GPIO_writePin(MOTOR_IN1_PORT, MOTOR_IN1_PIN, LOGIC_HIGH);
				GPIO_writePin(MOTOR_IN2_PORT, MOTOR_IN2_PIN, LOGIC_LOW);

				/* Change the current state of the global variable */
				motor_state = MOTOR_DIRECTION_CW;

				break;
			}

			case MOTOR_DIRECTION_ACW:
			{
				/* start PWM with the given speed */
				Timer0_PWM_Init(speed);

				/* Rotate the motor A-CW */
				GPIO_writePin(MOTOR_IN1_PORT, MOTOR_IN1_PIN, LOGIC_LOW);
				GPIO_writePin(MOTOR_IN2_PORT, MOTOR_IN2_PIN, LOGIC_HIGH);

				/* Change the current state of the global variable */
				motor_state = MOTOR_DIRECTION_ACW;
				break;
			}
			/* Add this empty default here so if any undefined state occurs,
			 * then the motor will go to stop case directly */
			default:

			case MOTOR_DIRECTION_OFF:
			{
				/* stop the PWM and the EN1 */
				Timer0_PWM_Init(STOP);

				/*
				 * Stop the motor by applying HIGH on both of its INs
				 * Note:	Applying LOW works too! but HIGH is better to
				 * 			be not care to any noise signals may occur
				 */
				GPIO_writePin(MOTOR_IN1_PORT, MOTOR_IN1_PIN, LOGIC_HIGH);
				GPIO_writePin(MOTOR_IN2_PORT, MOTOR_IN2_PIN, LOGIC_HIGH);

				/* Change the current state of the global variable */
				motor_state = MOTOR_DIRECTION_OFF;
				break;
			}
		}
	}
	/* if the state not changed, keep rotating the motor with the required speed */
	else
	{
		Timer0_PWM_Init(speed);
	}
}
