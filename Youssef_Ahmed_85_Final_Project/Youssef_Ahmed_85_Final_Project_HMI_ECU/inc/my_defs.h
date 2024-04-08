//#ifndef INC_MY_DEFS_H_
//#define INC_MY_DEFS_H_
//
///* Macros to avoid magic numbers */
//#define ZERO			(0)
//#define ONE				(1)
//#define TWO				(2)
//#define FIVE			(5)
//#define TEN				(10)
//#define ELEVEN			(11)
//#define HUNDERED		(100)
//#define THOUSAND		(1000)
//#define TEN_THOUSAND	(10000)
//
///* Predefined password information */
//#define PASSWORD_LENGTH				(FIVE)
//#define PASSWORD_LCD_START_DIGIT	(ELEVEN)
//#define PASSWORD_LCD_STOP_DIGIT		(PASSWORD_LCD_START_DIGIT + PASSWORD_LENGTH - ONE)
//
//#define DIGITS_MIN					(ZERO)
//#define DIGITS_MAX					(FIVE)
//
///* Function-like macros */
//#define DELAY_IN_SECONDS(value)		(_delay_ms(value*THOUSAND))
//#define CONCATINATE(digit1, digit2, digit3, digit4, digit5)		((digit1*TEN_THOUSAND) + (digit2*THOUSAND) +\
//																 (digit3*HUNDERED) + (digit4*TEN) + (digit5))
//
///* Miscellaneous Macros */
//#define DISABLE		0
//#define ENABLE		1
//#define ONE_BIT		1
//
//
//#endif /* INC_MY_DEFS_H_ */
