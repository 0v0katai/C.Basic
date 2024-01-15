Please look at this site that made by Krtyski.(e-Gadget administrator)
http://egadget2.web.fc2.com/CBasic/Interpreter/CBasic_interpreter.html
This site and following text is translated by Krtyski(e-Gadget administrator) & Lephenixnoir(Planet-Casio administrator).
Thanks very much for your support.

===============================================================================

Add-In Casio Basic Interpreter (& Compiler) Ver. 2.x beta version

Copyright(c)2015-2020 by sentaro21
E-mail sentaro21@pm.matrix.jp

Last updated by sentaro21/Krtyski/CalcLoverHK 10/02/2020

===============================================================================

**If you feel your Casio Basic programs are not running fast enough, C.Basic can free you of that frustration!**

Get started with genuine Casio Basic programs, then enjoy extended commands that Casio Basic alone cannot perform.

- Currently **C.Basic** (Basic interpreter) runs on fx-9860G(SH3), fx-9860GII(SH3), fx-9860GII(SH4A), and fx-CG10/20/50.
- **C.Basic** will evolve into a Casio Basic compiler "**C:Basic**", which will run programs much faster than C.Basic (10 times from the looks of it).

All the instructions supported by C.Basic are listed in the "Command_List.txt" and "Manual_EN.txt" files included in the distribution package.


## Pros

- C.Basic is mostly compatible with genuine Casio Basic.
- Programs run at least 10 times faster in C.Basic.
- C.Basic's List and File editors are easier to use than the usual ones.
- Can execute programs from storage memory/main memory, and supports sub-folder structures.
- Some Casio Basic commands are extended for more features.
- Newly-implemented commands for more powerful and flexible programming.

## Cons

- C.Basic does not cover all of the original Basic commands.
- Calculations don't yield the same results as the usual interpreter because C.Basic use double-precision floating point instead of fixed-point BCD.
- C.Basic stores PICT files in storage memory, hence using them is slower (at least until version 0.87 which uses the heap area from the main memory).
- There must be some bugs, unfortunately (; Please provide us with a bug report if you find any.


## Almost compatible with vanilla Casio Basic

Flawless compatibility is our goal, but some differences are intentional:

- `?` and `?->` input commands are displayed on a single line. Displaying a string longer than 21 characters will result in horizontal scroll instead of line wrap.
- When a graphing sketch commands is followed by the multi-statement command `:`, the screen is not refreshed.
- Program running time is displayed when the program ends. This function can be triggered in the setup page.
- Comment delimiters `'` are allowed before carriage returns and Disps (output), and `:` can be included in the comment string.

C.Basic also takes in user-friendly features of fx-5800P:

- C.Basic supports a `"?A"` command as fx-5800P, as opposed to fx-9860G / fx-9860GII which only support `"?->A"`.
- String output command `" "` is fully compatible with fx-5800P thus a but different from fx-9860G / fx-9860GII. See [Topic](http://egadget.blog.fc2.com/blog-entry-520.html) for details.


======= CAUTION !!! ========
We don't believe that C.Basic could damage your calculator, but the main memory could still be unexpectedly damaged or destroyed, which would require a whole memory reset. Hence it is strongly recommended that you backup your main and storage memory data. Please be aware that you should only use C.Basic at your sole risk.
============================

======= Disclaimer of Warranty =======
Use of C.Basic and C:Basic (hereinafter SOFTWARE) is at user's sole risk. All materials, information, products, software, programs and service provided as is・ with no warranties or guarantees whatsoever. Creators of SOFTWARE and support team who provide support documents, support web site, information and sample programs (hereinafter DEVELOPMENT TEAM) expressly disclaim to the fullest extent permitted by law, all express, implied, statutory, and other warranties, guarantees, or representations, including, without limitation, the warranties of merchantability, fitness for a particular purpose, and non-infringement of proprietary and intellectual property rights. Without limitation, DEVELOPMENT TEAM makes no warranty or guarantee that development of SOFTWARE will be uninterrupted, timely, secure, or error-free.
======================================


-------------------------------------------------------------------------------
# Starting up C.Basic

To start up **C.Basic**, press `[MENU]`, select the C.Basic icon, then press `[EXE]`.

There are two ways for copying a program file (`g1m` file) developed for the original Casio Basic interpreter into the File List in C.Basic:

## Using the pre-installed memory manager

- To start up the memory manager, press `[MENU]`, select the memory manager icon and press `[EXE]`.
- Press `[F1]` to display the contents of the main memory.
- Move the cursor down to `<PROGRAM>` and press `[EXE]`. You should now be seeing the list of all Basic Casio programs currently installed for the usual interpreter.
- Select the program file that you want to copy.
- Press `[F1](SEL)` then `[F2](COPY)`.
- Select `"ROOT"`, press `[EXE]`; the calculator will ask you for a file name.
- Enter an appropriate file name or just `"A"`, then press `[EXE]`.
- Press the `[MENU]` key to go back to the main menu.
- Start C.Basic.
- In the File List select the name you just entered, press `[F6]` and then `[F2](REN)`.
- You will be prompted with `[Rename file Name?]`, and the original file name will be displayed (even though you typed in `"A"`). Press `[EXE]`.
- Now the file name has been corrected.

## Using the PC link software (FA-124)

FA-124 is a PC link software which is included in the packages of fx-9860G and fx-9860GII. For detailed instructions, please refer to its manual.

Once you have downloaded a program's source file (`g1m` file), you can use the PC link software to copy it into C.Basic's File List.


=================================================================================
Quick Manual - Development Environment of C.Basic
=================================================================================

---------------------------------------------------------------------------------
File List Page
---------------------------------------------------------------------------------
-[UP]   Move cursor to the previous file.
-[DOWN] Move cursor ro the next file.
Pressing A to Z, cursor jumps to a file name which starts with the pressed alphabet.
This feature is fx-5800P compatible but not fx-9860G / fx-9860GII.
Press [.] key to jump to a file name which starts with "~".
Press [EXP(x10)]key to jump to top of folder list.

-[F1] (EXE)     Run selected file.
-[F2] (EDIT)    Edit selected file.
-[F3] (NEW)     Create new file.
-[F4] (COPY)    Copy selected file.
-[F5] (DEL)     Deleted selected file.
-[F6] ( > )     Move to next menu.
  [F1] (>Txt)     Convert selected source file (g1m file) to text file.
  [F2] (REN)      Rename file.
  [F3] (Fav.)     Set/Reset selected file to/from "Favorite".
  [F4] (Fv.up)    Move upward selected Favorite file.
  [F5] (Fv.dw)    Move downward selected Favorite file.
  [F6] ( > )      Move to previous menu.
   [F1] (MK.F)      Make new Folder
   [F2] (RN.F)      Rename Folder
   [F6] ( > )       Move to the first menu.

-[EXE]  Run the selected file.

-[SHIFT]+
  [EXIT](QUIT)  Move back to root from sub-folder.
  [LEFT]        Contrast(dw) (*internal function of OS)
  [RIGHT]       Contrast(up) (*internal function of OS)
  [F1] (Var)    Go to Variables Review Page.
  [F2] (Mat)    Go to Mat Variables Review Page.
  [F3] (V-W)    Go to ViewWindow setting value review page.
  [F4] (Pass)   Set/Reset pass word.
  [F6] (Debg)   Start Debug mode & open Editor page.

-[VARS] (Variables Review Page)
  [F1] (A<>a)          Switch between display of capital and small letter variables.
  [F2] (Init)          Initialize all the displayed variables.
  [F3] (D<>I)          Siwitch between Integer and Double variables.
  [F6] (>Hex/>Dec)     Switch display of values between in Hex and Dec.
  [SHIFT]+[UP]/[DOWN]  Scroll single page up and down.
  ※ Local variabe is marked with "=".

-[SHIFT][MENU] (SETUP) Initial Settings
  [MENU]               Go to Setup page.
  [MENU][F6]           Pop up Version information.

-[OPTN]  Set/Reset selected file to/from "Favorite".

-[EXIT]  Move cursor back to first file in the list.
         When cursor is at the first file in sub-folder, move back to root.


-------------------------------------------------------------------------------
Editor Page
-------------------------------------------------------------------------------
Guide marker displayed in very right of Editor Page indicates cursor position in source file.

-[F1] (JUMP)     Display JUMP Sub-menu.
  [F1] (TOP)     Jump to top line of source file.
  [F2] (BTM)     Jump to end line of source file.
  [F3] (GOTO)    Jump to a line where you want to go.
  [F5] (SkipUp)  Skip lines (preset # of line is available in Setup page) toward to top.
  [F6] (SkipDw)  Skip lines (preset # of line is available in Setup page) toward to end.

-[F2] (SRC)      Go to String Search / Replacement page.
-[F3] (CMD)      Go to Command Select page. (this is like fx-5800P command list)(fx-5800P mode)
      (MENU))    SelectCommand  (fx-9860G mode)
-[F4] (A<>a)     Switch between Capital and small letter.
-[F5] (CHAR)     Go to Character Select page.
-[F6] (EXE)      Run program or restart program when it's in pause.

-[SHIFT]+
  [EXIT](QUIT)        Go back to File List page.
  [LEFT]              Contrast(dw) (*internal function of OS)
  [RIGHT]             Contrast(up) (*internal function of OS)
  [UP]                Scroll 1 page (6 lines) up to top.
  [DOWN]              Scroll 1 page (6 lines) down to end.
  [F1] (Var)          Go to Variables page.
  [F2] (Mat)          Go to Mat Variables page.
  [F3] (V-WIN)          Go to ViewWindow page.
  [F4] (SKTCH)        Select Command. (fx-9860G mode)
       (Dump)/(List)  Switch between "Basic List" and "Hex Dump" display.(fx-5800P mode)
  [F5] (Dump)/(List)  Switch between "Basic List" and "Hex Dump" display.
  [F6] (CHAR)         Go to Character Select page. (fx-9860G mode)
       (G<>T)         Switch Graphics and Text screen.(fx-5800P mode)

  [3] Input keycode of Getkey command.

-[OPTN]  Pop up Command List supported by [OPTN] of fx-9860G/GII.(fx-5800P mode)
         Select Command. (fx-9860G mode)

-[VARS]  Pop up Comamnd List supported by [VARS] of fx-9860G/GII.(fx-5800P mode)
         Select Command. (fx-9860G mode)

-[MENU]  Command input history (chronological order/input frequency order is changed by right and left key.)
  [AC/ON] Erase history

-[SHIFT][VARS] (PRGM)  Pop up Command List supported by [PRGM] of fx-9860G/GII.

-[SHIFT][MENU] (SETUP) Go to Setup page.(fx-5800P mode)
               (SETUP) Select Command.  (fx-9860G mode)

-[ALPH][DEL] (UNDO) Delete or Paste operations are restored to the previous state.(only once)

-[EXIT]   Go back to previous page, cancel clip mode or go back to File List page.

-------------------------------------------------------------------------------
Debug Mode Editor Page
-------------------------------------------------------------------------------
Debug mode page comes up when program is broken intentionally or by error. Right example above shows Debug Mode Edotr Page that mini-font is applied in setup page.

In Degub Mode, status line is getting inverted on C.Basic for FX or becomes cyan color background.

-[F1](Cont) Continue program from cursor position.

-[F2](Trce) Trace program from cursor position and come back to Editor page in Debug Mode right after the traced command runs,

-[F3](Step) Trace and Step Over comamnd from cursor position and come back to Editor page in Debug Mode right after the command runs. When sub-routine runs, the sub-routine is executed to the end.

-[F4](S.ot) Step Out from cursor position and come back to Editor page in Debug Mode right after the command runs. When the sursor is in sub-routine the sub-routine is executed to the end.

-[F5](L<>S) Switch Editor page and program running screen. When the Program Running Screen is displayed, debug operaiton with [F1] - [F4] is still available.

-[F6]( > )  Switch Debug menu to and from Editor menu. When Program Running Screen is displayed, pressing [F6](>) displays Editor menu.

-[EXIT]     Go back to previous page, cancel clip mode or go back to normal (not in Debug mode) Editor page.

(*In the debug mode, status line becomes inverted.)

-------------------------------------------------------------------------------
Program Running Page
-------------------------------------------------------------------------------
During program is running;

-[AC]  Stop program.
  [EXIT]  Go to Editor in Debug mode. Cursor locates at break point.
  [Left]  Go to Editor in Debug mode. Cursor locates at break point.
  [F1]    Resume program.
  [EXE]   Resume program.

When program is halted by ▲ command (- Disp - is displayed on screen);
-[SHIFT]+
  [F1](Var)    Go to Variables Page
  [F2](Mat)    Go to Mat/List Page
  [F3](V-W)    Go to ViewWindow Page
  [F6](G<>T)   Switch between Graphics and Text screen
-[EXE]  Resume program.

-------------------------------------------------------------------------------
Mat/List/Vct Page
-------------------------------------------------------------------------------
In this page Mat, List and Vct can be reviewed. Operation procedure of Mat, List and Vct is same, since List and Vect are internaly implemented as extended Matrix.
(The Mat marked by * in this page is alocated by addressing.)

-[F1] (DEL)  Delete a Mat/List.
-[F2] (DEL-ALL)        Delete All Mat/List/Vct.
-[F3] (DIM)            Set the dimension , element size, an index start value of the Mat/List/Vct.
-[F4] (INITIAL)        Initialize all elements of the Mat/List/Vct of chosen.
-[F5] (Mat:/Lst:/Vct:) Select a number and jump to Mat/List/Vct of the number in alphabetical order.
-[F6] (A<>a)           Switch between capital letter and small letter of Mat name.

-[EXE]   Enter to Edit Mode of Mat/List/Vct. (in details see below)

-[SHIFT]+[F1] (>Mat)  Select Mat
-[SHIFT]+[F2] (>List) Select List
-[SHIFT]+[F3] (>Vct)  Select Vct

-[OPTN]  Switch showing between Mat and List.
-[VARS]  Switch showing between Mat and Vct.

-[LEFT]  backword 26.
-[RIGHT] forword 26.


[Operation in Edit Mode]
[SHIFT]+[8](CLIP)  All data of selected Mat/List/Vct is clipped in buffer.
[SHIFT]+[9](PASTE) Clipped data is pasted to selected Mat/List/Vct from buffer.


to edit numerical value;
 -[F1] (EDIT)         Edit the element of the Mat/List/Vct.
 -[F2] (GO)           Go to element set by row and column.
 -[F3] (INITIAL)      Initialize all elements.
 -[F4] (>x,y / >m,n)  Switch Matrix indication between [m,n] and [x,y].

 -[SHIFT]+[F5] or [F5] Switch element values between in decimal and binary.
        (applied only to nybble (4 bits), byte (1 byte) and word (2 bytes))
 -[SHIFT]+[F6] or [F6] Switch element values between in decimal and hexadecimal.
        (not applied to 1 bit and a complex number (16 bytes), double number (8 bytes) just displays internal data)

 -[SHIFT]+[F2] or [OPTN]  Enter to Dot Editor Mode.
 -[SHIFT]+[F3] or [VARS]  Enter to Character Strings Edit Mode.

 Only for g3m mode on C.Basic for CG (excluding 1 bit Mat/List/Vct);
 -[SHIFT]
    [5] Input 4 bit color value in nibble (4 bits) or 16 bits color value in other element number type.
    [6] Input 16 bits color value (excluding nibble (4 bits) Mat/List/Vct).


to edit Dots;
 Each element value of matrix can be handled as color code.
 - Using in 1 bit matrix or in g1m mode, 0(zero) = black and 1 = white.

 -[F1] (0 <> 1)  Change the value of the element with 0<>1. (1 bit Mat and g1m mode)

-------------------------------------------------------------------------------
Search/Replace for Text
-------------------------------------------------------------------------------
[Search character string]
Operatin procedure;
1. Input character string and press [F1](SEARCH) or [EXE].
2. To search next, press [F1](SEARCH) or [EXE] again.
3. Press [F6](RETRY) to back to search page.

[Replace character string]
Operatin procedure;
1. Input character string to search, and press [F2](REPL).
2. Then input character string to be replaced with, and press [F1](SEARCH), [F2](REPL) or [EXE].
3. To search more for replacement, press [F1](SEARCH) or [EXE] again.
4. Then press [F2](REPL) to replace again.
5. To replace all, press [F3](ALL) or [F4](ALL+). To quit replacement, press [AC/ON].
   When pressing [F4] (ALL+), progress status is not dusplayed.

-------------------------------------------------------------------------------
Incompatible Spec against genuine Casio Basic
-------------------------------------------------------------------------------
Compatibility with genuine Casio Basic is our target, but not 100%;

An input line by command "?" and "?->" are only in single line. A string of characters more than 21 columns is displayed still in the horizontal single line by scrolling, not in multiple lines.
When ":" (Multi-statement Command) comes right after graphing sketch command, graphic screen is not re-drawn.
Program running time is displayed after the program quit. This function can be set on or off in setup page.
A Comment Text Delimiter ' is available for comment-out before Carriage Return or Disps (output), but ":" (Multi-statement Command) can be included the commented out string.

C.Basic also takes in user-friendly feature of fx-5800P;

C.Basic does support a script "?A" supported by fx-5800P. This is not supported by fx-9860G / fx-9860GII (script "?->A" only is supported).
Function of string output command " " is fully compatible with fx-5800P in details but not exactly compatible with fx-9860G/fx-986GII. See Topics in details.

-------------------------------------------------------------------------------
Select Character Page
-------------------------------------------------------------------------------
In addition to character set of genuine Casio Basic, more characters are supported.

In "Character Select" page;
[F6]                      ASCII, Kana, external character can be applied.
[SHIFT]/[F1]〜[F6]        Continuous input is available.
[OPTN]                    Switch between mini-font and normal font.
[VARS] - C.Basic for FX   Switch between OS supported the mini-font and normal font.
[VARS] - C.Basic for CG   Switch between C.Basic font and OS supported font.
When OS supported font is displayed, special character can be input without escape.

-------------------------------------------------------------------------------
Differences in Graph 35 + E II
-------------------------------------------------------------------------------
-There is 3MB of storage memory.
-Hidden memory is limited to 188KB.
-As can create two or more folders, you can change the root folder.

-------------------------------------------------------------------------------
Setup Page
-------------------------------------------------------------------------------
Setup items - compatible with genuine Casio Basic

Angle:       Rad / Deg / Grad
Draw Type:   on / off
Coord:       on / off
Grid:        on / off
Axes:        on / off
Label:       on / off
Derivative:  on / off
Sketch Line: Normal / Thick / Broken / Dot


Setup items - newly provided and extended for C.Basic
-------------------------------------------------------------------------------
Display: Fix / Sci / Nrm / Eng
  Max number of digit after decimal point can be 15. Setting 0 (zero) max digit is 16.
  Nrm1 or Nrm2 is fully compatible with genuine Casio Basic and max digit is 10.
    - Nrm1: -0.01<x<0.01
    - Nrm2: -0.000000001<x<0.000000001
    - Other number (0 or 3 - 15) is to set max number of digit.
  Eng: select among Eng Off, Eng On (/E) and 3 digit separator (/3)
  ※ Related command: Eng, Norm, Fix, Sci
-------------------------------------------------------------------------------
Syntax Help   : On/Off
 the help dispyay at the command input of editor.
-------------------------------------------------------------------------------
SetupRecover  : On/Off
  set it whether you return setting compatible with genuine CasioBasic after a program in SetupRecover.
-------------------------------------------------------------------------------
Command Input :C.Basic/Standard
  Select Standard(fx-9860G) method or C.Basic(fx-5800P) method.
  You can change the mode of "" output specifications compatible with fx-5800P and  FX/CG.
 - Related command: '#58
                    '#98
-------------------------------------------------------------------------------
Max Mem Mode : on/off
 Set to use the available maximum memory.
-------------------------------------------------------------------------------
EnableExFont : on/off
 Set to use external font.
-------------------------------------------------------------------------------
Edit ExtFont  : On/Off
 enable external font in editor.
-------------------------------------------------------------------------------
EditFontSize  :Standard/Mini/Mini_rev/Mini UnderCursor/Mini_rev_UnderCSR
  Set to Editor font size.
-------------------------------------------------------------------------------
HideStatLine  :On/Off
  On :Hide the status line and use for editing.
  Off:Display the status line.
-------------------------------------------------------------------------------
Edit Indent+  : Off/1/2/4 Save-
 enable auto indent in editor.
 Off: diable auto indent.
  1 : Set indent width to 1.
  2 : Set indent width to 1.
  4 : Set indent width to 1.
 Save-: delete blank spaces including indents when saving program. (=to compatible with Casio Basic)
-------------------------------------------------------------------------------
Edit LineNum :on/off
  Set to line number display.
-------------------------------------------------------------------------------
EditListChar : List / reverseL / Thick L
  Select List display character.
-------------------------------------------------------------------------------
Use Hidden RAM: on / off
  Set if C.Basic uses hidden RAM or not.
-------------------------------------------------------------------------------
HiddenRAM Init: on / off
  When use the hidden RAM, Mat&List at the time of C.Basic start, to initialize or not.
-------------------------------------------------------------------------------
Max Pict No: 20~99
  When use the hidden RAM, you can use more pict file.
  (*)When you change a value, Pict&Mat&List is reset.
-------------------------------------------------------------------------------
Max List No: 52~1040
  When use the hidden RAM, you can use more List.
  (*)When you change a value, Pict&Mat&List is reset.
-------------------------------------------------------------------------------
AT DebugMode  : on / off
  When [AC] break, debug mode on/off.
-------------------------------------------------------------------------------
ExitDM Popup  : on / off
  Set to popup of exit Debug Mode.
-------------------------------------------------------------------------------
Break Stop: on / off
  Set if [AC] key break is accepted or not.
  When it's off, you cannot break program by [AC] key.
- Related command: '#Break 0	'#Break 1
-------------------------------------------------------------------------------
Exec TimeDsp: on / off  /reset  /%HR
  Set if program running time is displayed or not after the program quits.
  Timer starts at first line of program. ? or Disps command reset the timer.
  In the case of GetKey1/2, the timer is suspended and starts after command again.
  on reset: The timer is reset and starts again after GetKey1/2.
  %HR: use 1/32768s timer (only SH4A)
  - Related command: '#GetKeyC  '#GetKeyR
-------------------------------------------------------------------------------
IfEnd Check: on / off
  Set if one-to-one correspondence of "If" and "IfEnd" statement is checked or not.
-------------------------------------------------------------------------------
ACBreak: on / off
  Set ACBreak command to be enable or disable.
  - Related command: ACBreak
-------------------------------------------------------------------------------
Force Return:None/ F1/ EXE/ F1&EXE
  force return to file mode at [AC]break.
  (Example) Force Return set to [F1]key
  -File list→[F1](EXE)→[AC]→[EXIT]… Returns to the file list.
  -File list→[EXE]→[AC]→[EXIT] …… Returns to the editor.
  -File list→[F1](EXE]→[AC]→[EXE](restart)→[AC]→[EXIT] …… Returns to the file list.
  (The operation depends on which key is pressed first.)
-------------------------------------------------------------------------------
Key 1st time: 25 ms - 1000 ms (default value is 500 ms)
  Set time before key repeat starts (in 25 ms interval)
-------------------------------------------------------------------------------
Key Rep time: 25 ms - 500 ms (default value is 125 ms)
  Set key repeat duration time (in 25 ms interval)
-------------------------------------------------------------------------------
SkipUp/Down: 1 - 9999
  Set number of skipping page for SkipUp ([SHIFT][Up]) or SkipDown ([SHIFT][Down]).
-------------------------------------------------------------------------------
Mat Dsp mode: [m,n] / [X,Y]
  Set matrix display format in Matrix Editor page.
  Format [m,n] and [X,Y] are in transposed matrix each other, but just only the appearance on screen is different. The internal matrix data is still same, won't be changed.
-------------------------------------------------------------------------------
Matrix base: 0 / 1
  Set index of matrix starts with 0 or 1.
  When the start index is set to 0, left-top pixel comes to be available to use and left-top coordinate of device coordinates can be (0, 0).
  - Related command: '#Mat 0, '#Mat 1
-------------------------------------------------------------------------------
DATE: 2017/01/17 TUE
  Set date of internal RTC. A day of the week is automaticly set.
-------------------------------------------------------------------------------
TIME: 23:59:59
  Set time of internal RTC
  - Related command: DATE, TIME
-------------------------------------------------------------------------------
Root Folder:/			(* Only Graph 35+E II is valid.)
  [F1]:to set the root folder.
  [F2]:to set the current folder.
  - Related command:
   '#R/		// to set the root folder.
   '#R.		// to set the current folder.
-------------------------------------------------------------------------------
Auto file Save:on/off
  Set to auto save without a popup.
-------------------------------------------------------------------------------
Force g1m save: on / off
  Set "save automatically" or "not save" g1m file, after run or edit of text file.
-------------------------------------------------------------------------------
Pict mode: S.Mem / Heap / Both / MainMem
  - S.Mem: Pict file is created in storage memory/SD and it takes a bit longer.
  - Heap: Pict file is not actually created, but compatible file is allocated in main memory.
  - Both: Stored always in storage memory/SD.
  - Main: Pict file is compatible with genuine CasioBasic.
-------------------------------------------------------------------------------
Storage mode: S.Mem / SD / MainMem
  S.Mem: Files in storage memory is used for run and edit.
  SD: Files in SD is used for run and edit (available oonly for SD verison of fx-9860G series
  Main mem: Files in MCS(Mein Memory) is used for run and edit.
-------------------------------------------------------------------------------
RefrshCtl DD: off / Grp / All
  Set how to refresh screen at running of display and draw command.
  Off: No extra refresh control is carried out, which is compatible with genuine Casio Basic.
  Grp: Only graphics draw commands make screen refreshed, excepting ClrText, Locate, Text, LocateYX, " " (this exception is compatible with former version of C.basic).
  All: All the display and draw commands male screen refreshed. PutDispDD should be used as may be necessary.
  Time: Set refresh control tme in interval of 1/128 sec. Defalt value is 3 then refresh control is every 1/42 sec.
	It is not refreshed when  0 is set.
  - Related command: RefrshCtrl, RefrshTime
-------------------------------------------------------------------------------
Wait count : 0~9999
 Set to wait for slowdown execution speed.
  - Related command:Wait
-------------------------------------------------------------------------------
Execute mode: Dbl# / Int% / CPLX
  Set running mode as default.
  - Related command: '#CBasic, '#CBASIC, '#CBINT, '#CBint '#CBCPLX '#CBcplx
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
Acknowledgment
-------------------------------------------------------------------------------

To investigate and implement internal specification,
"SuperH-based fx calculators (version 20)"
gave useful information.

For file handling in C.Basic,
"https://community.casiocalc.org/topic/6836-wsc-fvm-v12-casio-fx-9860-series-on-calc-c-compiler/
Wsc & Fvm V1.2 - Casio Fx-9860 Series On-calc C Compiler"
gave useful information.

For Japanese Kana Fonts, font data and output routine in the following site is used;
"http://www.d-b.ne.jp/~k-kita/fx_sdk_005.html"


For detailed feature and function of original Casio Basic, following site is referenced;
e-Gadget - Programming Calculator by Krtyski
"https://egadget.blog.fc2.com/"
(almost contents are in Japanese).

https://community.casiocalc.org/topic/7637-cbasic-international-release/
https://codewalr.us/index.php?topic=1818.0
https://www.planet-casio.com/Fr/forums/topic14738-1-C.Basic.html
Their opinions were very useful.

I am grateful for everyone's cooperation.

by sentaro21

-------------------------------------------------------------------------------
related link
-------------------------------------------------------------------------------
https://egadget2.web.fc2.com/CBasic/Interpreter/CBasic_interpreter.html
https://community.casiocalc.org/topic/7637-cbasic-international-release/
https://codewalr.us/index.php?topic=1818.0
https://www.planet-casio.com/Fr/forums/topic14738-1-C.Basic.html

-------------------------------------------------------------------------------
Licensing
-------------------------------------------------------------------------------
This software is free software, in accordance with GPLv2.