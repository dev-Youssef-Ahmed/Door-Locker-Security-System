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

![Image](https://github.com/users/dev-Youssef-Ahmed/projects/1/assets/153888401/f69e4e19-af33-4531-9e5c-b2a1552f92df)

2. The Welcome message:

![Image](https://github.com/users/dev-Youssef-Ahmed/projects/1/assets/153888401/af8e09a3-042f-423d-9b0e-ce9322439aa2)

3. The rules of password:

![Image](https://github.com/users/dev-Youssef-Ahmed/projects/1/assets/153888401/25c43899-ee5f-416e-aad0-0581cf74ea21)

4. Enter new password:

![Image](https://github.com/users/dev-Youssef-Ahmed/projects/1/assets/153888401/8794a2b5-62f6-4b33-97fc-573798965e69)

5. Confirm password:

![Image](https://github.com/users/dev-Youssef-Ahmed/projects/1/assets/153888401/6922e5e1-246f-4f08-9864-0fddde58b827)

6. Passwords matched:

![Image](https://github.com/users/dev-Youssef-Ahmed/projects/1/assets/153888401/c0b13b44-3599-4c83-806a-f12b8811a548)

7. Passwords mismatched:

![Image](https://github.com/users/dev-Youssef-Ahmed/projects/1/assets/153888401/a641e89e-5539-44a1-8c21-6818749ab1f5)

8. Enter the password:

![Image](https://github.com/users/dev-Youssef-Ahmed/projects/1/assets/153888401/9969570d-838e-48ee-85a8-16702181eb12)

9. Main menu:

![Image](https://github.com/users/dev-Youssef-Ahmed/projects/1/assets/153888401/1d79a870-9e37-48b7-9ce9-351a8748fb82)

10. Door open:

![Image](https://github.com/users/dev-Youssef-Ahmed/projects/1/assets/153888401/67a164e8-ee6b-4c75-a4fe-dd33efd43b68)

11. Door hold:

![Image](https://github.com/users/dev-Youssef-Ahmed/projects/1/assets/153888401/3c292ac3-e98f-4786-93d1-b7b2a7f5b65e)

12. Door close:

![Image](https://github.com/users/dev-Youssef-Ahmed/projects/1/assets/153888401/9f897e00-c822-41dd-9529-53802b332aec)

13. Wrong password:

![Image](https://github.com/users/dev-Youssef-Ahmed/projects/1/assets/153888401/cb1d9c18-b9c3-4ae3-9da7-272055a5b4b7)

14. System locked:

![Image](https://github.com/users/dev-Youssef-Ahmed/projects/1/assets/153888401/95bde51d-7915-4c95-a301-51f6f6b94b7d)
