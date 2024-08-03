===============================================================================
C.Basic font editor FEINT for fx-CG10/20/50/Graph90+E
Font File Manager (FFM) subroutine  Version 1.24 Manual
by Colon/sentaro21

===============================================================================
What is "Font File Manager (FFM)"?
===============================================================================
"Font File Manager (FFM)" is the program which runs on Add-In Casio Basic Interpreter "C.Basic for CG" which developed by sentaro21.
Using "C.Basic for CG," you can display external fonts in your Casio Basic programs.
Thankfully, Tsuru and sentaro21 developed "FEINT" which is the program to edit and save external fonts on your calculator.
This application is being developed to make more effective use of "C.Basic for CG" and "FEINT."
"Font File Manager (FFM)" makes it easy for you to manage external font files created using "FEINT."
Currently, "FEINT" and "Font File Manager (FFM)" are still in development, so they operate independently.
In the future, these two applications will be able to work together.

===============================================================================
Operational requirements
===============================================================================
To run this program, you need a calculator for one of the following models:

- fx-CG10
- fx-CG20
- fx-CG50
- Graph 90+E

Then, please install "C.Basic for CG" Ver. 1.44β or later on your calculator.

For detailed installation instructions, please look at this site that made by Krtyski,"e-Gadget" administrator.
https://egadget2.web.fc2.com/CBasic/Interpreter/CBasic_interpreter.html

Next, start up "C.Basic" and please open the "Setup" page by [SHIFT] - [MENU] (SET UP), then set the "HeapRAM Size" to 118 KB or less.
If the "HeapRAM Size" is 119 KB or more, the program will run, but you will not be able to display the background image on the screen.

===============================================================================
CAUTION!!!
===============================================================================
Do not disable the '#Break0 command in the source code!!
If the program stops running while a popup window is displayed, it will no longer run correctly.
In most cases, it will be restored by restarting "C. Basic," but in the worst case, the system will crash!!!

This program is password protected to prevent accidental editing of the source code.
The password is "FFMColon". You are free to modify the source code, but please do so at your own risk.

===============================================================================
Installation of "Font File Manager (FFM)"
===============================================================================
Transfer "FFM0124.g3m" file to storage memory of your calculator.
Optionally, transfer the font files to storage memory as well.
Currently, this app can only manage font files in the same folder as "FFM0124.g3m" file.


===============================================================================
How to use
===============================================================================
1. File list screen
        [The first page]
         -[F1](LOAD) … Set the file as font data
         -[F2](VIEW) … Display the bitmap on the screen (Press any key to return)
         -[F4](DETAIL) … Displays detailed information on font files (does not display information on invalid files)
         -[F5](DELETE) … Delete file
         -[F6](→) … To the next page

        [2nd page]
         -[F1](SEARCH) … Search for files
         -[F5](CHAR) … Displays the current font data
         -[F6](→) … Go to the previous page

        [Common to first and second pages]
         -[↑] … select the previous file
         -[↓] … Select the next file

         -[EXIT] … To the top of the file list or exit

         -[SHIFT]
          --[EXIT](QUIT) … Exit
          --[OPTN] … Background ON / OFF
          --[7](CAPTURE)… Screen capture
          --[AC/ON](OFF) … Power off

2. Display the detailed information of the font file
        Displays file name, font file type, and file header information.
         -[F1](LOAD) … Set the file as font data
         -[F2](VIEW) … Display the bitmap on the screen (Press any key to return)
         -[F5](DELETE) … Delete file
         -[F6](CHAR) … Displays the current font data
         -[↑] … select the previous file
         -[↓] … Select the next file

         -[EXIT] … Return to file list

         -[SHIFT]
          --[EXIT](QUIT) … Return to file list
          --[7](CAPTURE) … Screen capture
          --[AC/ON] (OFF) … Power off

3.Display the current font data (message is in English only)
        Press the [1] to [4] keys to display the corresponding font data.
        Press the [EXIT] key to return to the file list screen.

        Press the Up / Down key to display a page break when displaying font data.
        You can also return to the previous screen by pressing the [EXIT] key.


===============================================================================
Change log
===============================================================================
v1.24 18 Jan 2020 A Version supporting specification change in IsExist().
        -Changed code to accept spec change in IsExist() command on C.Basic for CG ver 1.44β.

v1.23 16 Dec 2019 Various improvements version.
        -Fixed the bug that caused an error when there was no bmp file in the same directory.(in v1.22)
        -In order not to accidentally edit the source code, the text file was stopped from being bundled and the password lock was applied to the g3m file.
        -"Bye !!" is displayed at the end of the program.
        -The confirmation pop-up screen is displayed when "HeapRAM Size" is 119 KB or more.
        -Fixed the bug that was able to start in fx-9860G/ Slim/fx-9860GII.


v1.22 07 Aug 2019 Version that supports deletion of IsError().
        -Changed the processing by deleting the IsError() command in C.Basic for CG ver 1.00β.
        (This change is by sentaro21.)


v1.21 27 Apr 2019 Version that improved font reference screen.
        -Fixed the credit display of the comment.
        -Fixed the bug that icons are not displayed correctly when reloading the file list.
        -Changed the font reference screen to the same specifications as FEINT ver3.5.
        -Fixed the bug that does not work in C.Basic for CG ver 0.80α or later.


v1.20 18 Jun 2018 Version that added search and deletion.
        -Added search and deletion of Bmp files.
        -Changed the key press.
        -Deleted [OPTN] key that The language change function temporarily assigned.
        -The current font can be referenced from the file details screen.
        -The font type notation has been unified to FEINT. (Standard→Normal)
        -The installation status of external fonts can check on the current font reference screen.
        -The file list background can be deleted by [SHIFT]+[OPTN].
        -Fixed that [EXIT] did not return to the top of the file list.
        -Changed to be alpha lock or shift mode when in file list.
        -Changed Getkey1 to Getkey@1, and the same jump function as the original can be used.
        -Fixed the function menu of [F1](LOAD).
        -Fixed the grammatical use of "&" in the comment at the beginning.
        -Improved file list reading.
        -Changed the status line to OS management and the status message has been changed.
        -Replaced PrintXY system call to Locate command.
        -Changed to SysCall 0x1E41 that Colon discovered  to simplify the display of the scroll bar.
        -The icons used in the file list are included in the g3m file. BMPICONU.bmp and BMPICONS.bmp are no longer needed.
         As a result, the operation has been supported when there is no Bmp file in the current folder.
        -Changed setup to strictly.
        -Changed the operating requirements to C.Basic for CG ver 0.50α or later.
        -Improved the algorithm.


v1.11 22 Apr 2018 Version that added Italian, Japanese.
        -Added support for Italian on OS 3.11 and Japanese for the Colon creation add-in.
        (The supported total 9 languages.)
                English
                French
                Spanish
                Portuguese
                German
                Chinese
                Italian
                Russian
                Japanese
        -Changed the original message display method using GB font to PrintXY system call.
        -Changed to not use external fonts where unnecessary.
        -Fixed the problem that the status line could not be displayed when the pop-up indicating invalid file was displayed.
        -Improved the status line display algorithm.
        -When it cannot be operated depending on the version of C.Basic for CG, a message prompting to upgrade is displayed.
        - SetFont 1 is executed in C.Basic for CG ver 0.48 or later.
        -The file name has been changed to "FFM"+version number.


v1.10 15 Mar 2018 Multilingual version.
        Pop-up window related
        -Changed the popup window to OS compatible.
         (Because of this change, for the sake of safety, the interruption by the AC key has been disabled.)

        Language related
        -Supported multilingual message display.
        -The supported total 7 languages.
                English
                French
                Spanish
                Portuguese
                German
                Chinese
                Russian
         (Because of this change, the message content has been partially changed.)
        -As a provisional specification,
         press the [OPTN] key when displaying the file list to display the same language setting screen as the OS.
         Press the [EXIT] key to return to the file list screen.
 
        Other
        -Included external character mini sample font (created by FEINT).
        -The message when displaying the current font (English only) has been changed to "Showing current font".
        -Updated date information was added to the comment in the source.
        -Improved the algorithm.


v1.01 14 Mar 2018 Version that file size displayed.
        -The file size is displayed on the bmp file list.
         (Because of this change, the reading time has become longer, so the progress bar is displayed when the reading file list.
        -Key repeat was permitted by Getkey1 operation.
        -Improved the algorithm.


v1.00 13 Mar 2018 First released