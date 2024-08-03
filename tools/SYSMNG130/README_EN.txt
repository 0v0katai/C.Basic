===============================================================================
fx-CG 10/20/50 C.Basic for CG
Advanced system management program
System Manager Version 1.3.0 Manual
by Colon

===============================================================================
About System Manager
===============================================================================
This program was created by "C.Basic for CG" which developed by sentaro21.
Advanced settings and management that are not possible with genuine System Manager are possible.

===============================================================================
warning
===============================================================================
I(Colon) don't mind modifying the source code, but I don't recommend it because of the heavy use of system calls.
Changes to the source code may cause damage to the calculator and memory contents.
If you still want to change it, ask it me(Colon) or do your own risk.
Any changes that would make the program useful would be welcome !!

This program is password protected to prevent accidental editing of the source code.
The password is "SyMngCln". You are free to modify the source code, but please do so at your own risk.

===============================================================================
Operational requirements
===============================================================================
To run this program, you need a calculator for one of the following models:

- fx-CG10
- fx-CG20
- fx-CG50
- Graph 90+E

Then, please install "C.Basic for CG" Ver. 1.44βbuild10 or later on your calculator.

When using ver 0.49α or later, set the “HeapRAM Size”of the setup to 118 KB or less.
If you installed the Japanese add-in created by Colon ver 0.01 or later, this program works in Japanese.

===============================================================================
how to use
===============================================================================
Only the parts that are different from the genuine System Manager.

1. System menu screen
	It is almost genuine compatible, but there is no RESET and OS Update.
	-[F1] (DISPLAY): Adjust the brightness of the display screen (partial function expansion)
	  -[F2](PWRProp): Set auto power off time and backlight lighting time (partial function expansion)
	  -[F3](LANGUAGE) … Set language
	  -[F4](VERSION) … Display version information
	  -[F6]→[F1](BattSet) … Select the type of battery to be used
	  -[F6]→[F2](UserName) … Register user name

2. Adjusting the brightness of the display screen (partial function expansion)
	You can set more advanced settings than genuine.

	-Select the item with the up/down keys.
	 "EN2"is the main backlight level, and "PWM DUTY" is the sub backlight level.
	-The main backlight level (EN2) has two levels(LOW/HIGH).
	-Press [-] or [←] to switch to LOW, or press [+] or [→] to switch to HIGH.
	-The sub backlight level (PWM DUTY) is 250 steps from 6 to 255.
	-Press [-] or [←] to decrease one level, and press [+] or [→] to increase one level.
	 Press [EXE] to enter manual input mode.
	-1 and 5 are rounded to 6, and above 256 are rounded to 255.
	 If you enter a value less than 1 or press [EXE] again without entering anything, the level will remain unchanged.
	 *less than 6 are invalid and cannot be set in this program.
	-The function key can be used to change to the same level as genuine.
	-When the level is the same as genuine, the function menu is reversed color.
	 *[F1](0) is the same value as level 0 of the calculator's self-diagnosis function (Easter Egg).

	The value set in the genuine System Manager cannot be changed.
	Basically, when the power is turned on again, it returns to the value set in genuine System Manager.
	However, when the power is turned [OFF] then [ON] on the DISPLAY setting screen of this program, the value of the advanced setting is retained.

3. Set auto power-off time and backlight lighting time (partial function expansion)
	You can set more advanced settings than genuine.

	-Select the item with the up and down keys.
	-Press [-] or [←] to decrease one level, and press [+] or [→] to increase one level.
	-The auto power off time setting is valid from 1 minute to 9999 minutes (in 1 minute increments).
	-The backlight lighting time setting is a value between 30 seconds and 63 minutes and 30 seconds (in increments of 30 seconds) and less than the auto power off time.
	-If you try to set the auto power off time shorter than the backlight lighting time, the backlight lighting time is automatically set to the same as the auto power off time.
	-Press [EXE] when setting the auto power off time to enter manual input mode.
	If you enter a value less than 1 or more than 10000, or press [EXE] again without entering anything, the value will remain unchanged.

	The value set in the genuine System Manager is changed. If a value that is not genuine is set, the correct value cannot be displayed in the genuine System Manager.


Other operations are the same as the genuine System Manager.

===============================================================================
Change log
===============================================================================
v1.3 2020/01/30 Disable backlight setting of the emulator
	-Changed the operating requirements to C.Basic for CG ver 1.44β build 10 or later.
	-Since the emulator does not the backlight setting, the display screen brightness setting has been disabled.
	 You can continue to use it on the real calculator.
	-In C.Basic for CG, the new format of the function key icon indicating that it is selected is defined.
	 Because of this change, the display of the function menu on the display brightness setting screen has been changed.
	-Fixed the bug that an error occurred with integer mode of C.Basic.

v1.2 2019/12/16 Various improvements
	-Changed the operating requirements to C.Basic for CG ver 0.47α or later.
	-to prevent inadvertent editing of the source code, not bundled the txt file and locked the password in the g3m file.
	-Changed to the specification that "Bye !!" is displayed at the end of the program.
	-Changed to the specification that the confirmation pop-up is displayed when C.Basic for CG ver 0.49α or later and "HeapRAM Size" is 119 KB or more.
	-Fixed the bug that could be started in fx series.


v1.1 2018/04/20 Status line modified version
	-Fixed the status line disappearing or not updating after manual input of [F1](DISPLAY) and [F2](PWRProp).

v1.0 2018/03/26 Fatal bugs fixed version
	-Changed to the specification that can not input a real number less than 1 in the manual input mode for auto power off time.
	 Because of this change, , the problem that the calculator could not be started in v0.0 has been resolved.
	-Changed the screen configuration of [F1](DISPLAY).
	-Added the function to change to the same level as genuine with function keys.
	-Added multilingual status messages.
	 Because of this change, the status message is not displayed on the language setting screen.
	-Changed the specification that display the message prompting you to upgrade the version when the Version command does not worked.
	[03/26 replacement v1.0.1]
	-Changed to the specification that a real number of 10,000 or more cannot be input in the manual input mode of the auto power off time.
	-Added minor version number to comments for replacement.


v0.0 2018/03/23 First released (the calculator may become unbootable)