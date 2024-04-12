The Door Locker Security System is the final project of Standard Embedded Systems Diploma at Edges for Training Academy, under the expert guidance of Mohamed Tarek.

The project involves creating a password-based door unlocking system using AVR ATmega32. It utilizes various drivers including GPIO, Keypad, LCD, Timer, UART, I2C, INT2, EEPROM, Buzzer, and DC Motor for functionality.

There are two Electronic Control Units (ECUs) used here:

- Human-Machine Interface (HMI) ECU:
> Is just responsible interaction with the user just take inputs through keypad and display messages on the LCD.

> HMI_ECU MCAL: GPIO, UART, TIMER1, and INT2.

> HMI_ECU HAL: LCD and KEYPAD.

- Control ECU: 
> Is responsible for all the processing and decisions in the system like password checking, open the door and activate the system alarm.

> CONTROL_ECU MCAL: GPIO, UART, TIMER1, INT2, PWM_TIMER0, and I2C.

> CONTROL_ECU HAL: BUZZER, EEPROM, and DC MOTOR.

![image](https://github.com/dev-Youssef-Ahmed/Door-Locker-Security-System/assets/153888401/06c0634a-b373-4f17-a43d-4dc55eaab9d1)

Here is the system sequence, HMI_ECU action start with letter "H" and CONTROL_ECU action "C". Be aware that at anytime one of ECUs is working, the other is polling and not doing any action; this to maintain a safe synchronization between them. Now, let us assume it is the first time to install the system and here are the sequences:

1. C: Check if the password state in EEPROM is equal to "password not existed"
2. H: A welcome message appears.
3. H: A message informs the user with the rules of the password displayed.
4. H: Asking the user to enter his password, then send it through the UART to C.
5. C: Get the password without taking any action.
6. H: Asking the user to confirm his password, then send it through the UART to C.
7. C: Get the password then compare it with the first one, then send the result to H.
8. H: If mismatched, the user will be asked to re-confirm the password for times less than MAX_INVALID_TRIALS (see Note 2) time, but I reached, the user will repeat from step three.
9. C: If matched, try to save the password for times less than MAX_INVALID_TRIALS, if reached due to any EEPROM issue, restart the system.
10. H: If matched, and saved to EEPROM, a message show successfully saved password appears.
11. H: Asking the user to enter his password, then send it through the UART to C.
12. C: Get the password then compare it with the saved one, then send the result to H.
13. H: If wrong, the user will be asked to re-enter the password for times less than MAX_INVALID_TRIALS time, but I reached, system will be locked for one minute, and the buzzer will be switched ON by C for one minute too, then restart from step eleven.
14. H: If correct password, the user now can navigate to main menu, to select one of two options: PLUS symbol to open the door, or MINUS symbol to change the password, get the command then send it through the UART to C.
15. H: If command is to change the password, then restart from step three.
16. H: If command is to open the door, then display a message that door is opening for fifteen seconds, and C will rotate the motor clockwise for the same duration.
17. H: Display a message that door is holding for three seconds, and C will stop the motor for the same duration.
18. H: Display a message that door is closing for fifteen seconds, and C will rotate the motor anti-clockwise for the same duration, then restart from step eleven.

Note 1: In case it was not the first time to install the system, the CONTROL_ECU will find that password state in the EEPROM (see step one) refers to that password existing, then jump to step eleven directly.

Note 2: The MAX_INVALID_TRIALS is a constant could be changed in the code, my MAX_INVALID_TRIALS = 3.

Note 3: All time durations used here are adjustable in the code.

Note 4: The RESET button is addition to reset the two MCUs and the EEPROM too, instead of stop running the simulation and re-insert a new EEPROM to apply tests! 

Note 5: The proteus file contain three sheets, the user interface sheet, the ECUs sheet, and the background sheet.

Finally, here are some screenshots from the project:

1. The User Interface:

![image](https://github.com/dev-Youssef-Ahmed/Door-Locker-Security-System/assets/153888401/3b21960c-b427-4c13-8401-c00b566d7f1c)

2. The Welcome message:

![image](https://github.com/dev-Youssef-Ahmed/Door-Locker-Security-System/assets/153888401/91a3d656-2262-4d24-b3d1-cf5e96b1134c)

3. The rules of password:

![image](https://github.com/dev-Youssef-Ahmed/Door-Locker-Security-System/assets/153888401/f9d524f4-4710-4041-b6fd-e2a480854350)

4. Enter new password:

![image](https://github.com/dev-Youssef-Ahmed/Door-Locker-Security-System/assets/153888401/db21df51-5021-431e-bada-82202f7e0a6c)

5. Confirm password:

![image](https://github.com/dev-Youssef-Ahmed/Door-Locker-Security-System/assets/153888401/90cd6026-1e24-4d73-99ee-023f07155114)

6. Passwords matched:

![image](https://github.com/dev-Youssef-Ahmed/Door-Locker-Security-System/assets/153888401/5967dc97-72ff-4639-ac30-dcb05130f50f)

7. Passwords mismatched:

![image](https://github.com/dev-Youssef-Ahmed/Door-Locker-Security-System/assets/153888401/544fc52d-4c3e-442a-b80b-cd3b6ac8a2de)

8. Enter the password:

![image](https://github.com/dev-Youssef-Ahmed/Door-Locker-Security-System/assets/153888401/59009da6-e901-4857-b78b-388eadcb6bbd)

9. Main menu:

![image](https://github.com/dev-Youssef-Ahmed/Door-Locker-Security-System/assets/153888401/ef20df61-421b-4c55-8964-ba9d13061f33)

10. Door open:

![image](https://github.com/dev-Youssef-Ahmed/Door-Locker-Security-System/assets/153888401/544aab00-2ea7-44c4-a745-136e6379545f)

11. Door hold:

![image](https://github.com/dev-Youssef-Ahmed/Door-Locker-Security-System/assets/153888401/f18aaa3b-28b9-4056-8df1-e922778a08a5)

12. Door close:

![image](https://github.com/dev-Youssef-Ahmed/Door-Locker-Security-System/assets/153888401/ac0edd58-9a3b-4985-bdbb-0ad0f09a0fd7)

13. Wrong password:

![image](https://github.com/dev-Youssef-Ahmed/Door-Locker-Security-System/assets/153888401/5cb719b8-8435-4641-a2e4-19326fa75689)

14. System locked:

![image](https://github.com/dev-Youssef-Ahmed/Door-Locker-Security-System/assets/153888401/f4aa0d37-0adf-4b41-9475-05783a79628d)
