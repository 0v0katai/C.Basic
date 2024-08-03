Please look at this site that made by Krtyski.(e-Gadget administrator)
https://egadget2.web.fc2.com/CBasic/Interpreter/CBasic_interpreter.html
This site and following text is translated by Krtyski(e-Gadget administrator) & Lephenixnoir(Planet-Casio administrator).
Thanks very much for your support.

===============================================================================

Add-In Casio Basic Interpreter (& Compiler) for CG ver. 1.x beta version

copyright(c)2015-2017-2020 by sentaro21
e-mail sentaro21@pm.matrix.jp

last updated by Krtyski/sentaro21, 9 Feb 2020
===============================================================================

**If you feel your Casio Basic programs are not running fast enough, C.Basic can free you of that frustration!**

Get started with genuine Casio Basic programs, then enjoy extended commands that Casio Basic alone cannot perform.

- Currently **C.Basic for CG** (Basic interpreter) runs on Prizm fx-CG10 and fx-CG20 and fx-CG50 / Graph 90+E
- **C.Basic for CG** will evolve into a Casio Basic compiler "**C:Basic for CG**", which will run programs much faster than C.Basic (10 times from the looks of it).

All the instructions supported by C.Basic are listed in the `"Command_List_CG.txt"` and `"ManualCG_EN.txt"` files included in the distribution package.

======= CAUTION !!! ========
In order to support Prizm fx-CG10 and fx-CG20, the over clock feature was built in because of its slower screen refresh.
58.98MHz(default) -> 117.96Mhz (nearly Ptune2 [F3] preset)
============================

## Pros

- C.Basic is mostly compatible with genuine Casio Basic.
- Programs run at least 10 times faster in C.Basic.
- C.Basic's List and File editors are easier to use than the usual ones.
- Can execute programs from storage memory, and supports sub-folder structures.
- Some Casio Basic commands are extended for more features.
- Newly-implemented commands for more powerful and flexible programming.

## Cons

- C.Basic does not cover all of the original Basic commands.
- Calculations don't yield the same results as the usual interpreter because C.Basic use double-precision floating point instead of fixed-point BCD.
- C.Basic stores PICT files in storage memory, hence using them is slower (at least until version 0.87 which uses the heap area from the main memory).
- There must be some bugs, unfortunately ^^ Please provide us with a bug report if you find any.


## Almost compatible with vanilla Casio Basic

Flawless compatibility is our goal, but some differences are intentional:

- `?` and `?→` input commands are displayed on a single line. Displaying a string longer than 21 characters will result in horizontal scroll instead of line wrap.
- When a graphing sketch commands is followed by the multi-statement command `:`, the screen is not refreshed.
- Program running time is displayed when the program ends. This function can be triggered in the setup page.
- Comment delimiters `'` are allowed before carriage returns and Disps (output), and `:` can be included in the comment string.

C.Basic also takes in user-friendly features of fx-5800P:

- C.Basic supports a `"?A"` command as fx-5800P, as opposed to fx-9860G / fx-9860GII / fx-CG series which only support `"?→A"`.
- String output command `" "` is fully compatible with fx-5800P thus a but different from fx-9860G / fx-9860GII. See [Topic](http://egadget.blog.fc2.com/blog-entry-520.html) for details.


======= CAUTION !!! ========
We don't believe that C.Basic could damage your calculator, but the main memory could still be unexpectedly damaged or destroyed, which would require a whole memory reset. Hence it is strongly recommended that you backup your main and storage memory data. Please be aware that you should only use C.Basic at your sole risk.
============================

======= Disclaimer of Warranty =======
Use of C.Basic and C:Basic (hereinafter SOFTWARE) is at user’s sole risk. All materials, information, products, software, programs and service provided “as is”, with no warranties or guarantees whatsoever. Creators of SOFTWARE and support team who provide support documents, support web site, information and sample programs (hereinafter DEVELOPMENT TEAM) expressly disclaim to the fullest extent permitted by law, all express, implied, statutory, and other warranties, guarantees, or representations, including, without limitation, the warranties of merchantability, fitness for a particular purpose, and non-infringement of proprietary and intellectual property rights. Without limitation, DEVELOPMENT TEAM makes no warranty or guarantee that development of SOFTWARE will be uninterrupted, timely, secure, or error-free.
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

(*Ver.0.53 or later)
 -Added setting of the setup to use the program file of MCS(main memory) directly.
  You can change the destination from MCS from storage, storage to MCS in preservation and the copy of the file.
  At the storage memory mode, copying, a destination become MCS when they choose [F3](MCS).
  Copying, a destination become the storage memory when they choose [F1](g3m)[F2](g1m) at the MCS mode.


=================================================================================
Quick Manual - Development Environment of C.Basic
=================================================================================

---------------------------------------------------------------------------------
File List Page
---------------------------------------------------------------------------------
-[UP]   Move cursor to the previous file.
-[DOWN] Move cursor ro the next file.
Pressing A to Z, cursor jumps to a file name which starts with the pressed alphabet.
This feature is fx-5800P compatible but not fx-CG.
Press [.] key to jump to a file name which starts with "~".
Press [EXP(x10)]key to jump to top of folder list.

-[F1] (EXE)      Run selected file.
-[F2] (EDIT)     Edit selected file.
-[F3] (NEW)      Create new file.
-[F4] (COPY)     Copy selected file.
-[F5] (DELETE)   Deleted selected file.
-[F6] ( > )      Move to next menu.
  [F1] (>txt)      Convert selected source file to text file.
  [F2] (RENAME)    Rename file.
  [F3] (Fav.)      Set/Reset selected file to/from "Favorite".
  [F4] (Fv.up)     Move upward selected Favorite file.
  [F5] (Fv.dw)     Move downward selected Favorite file.
  [F6] ( > )       Move to previous menu.
   [F1] (MKEFLDR)    Make new Folder
   [F2] (RENFLDR)    Rename Folder
   [F6] ( > )        Move to the first menu.

-[EXE]  Run the selected file.

-[SHIFT]+
  [EXIT](QUIT)   Move back to root from sub-folder.
  [UP]           Move upward selected Favorite file.
  [DOWN]         Move downward selected Favorite file.
  [F1] (Var)     Go to Variables Review Page.
  [F2] (Mat)     Go to Mat Variables Review Page.
  [F3] (V-W)     Go to ViewWindow setting value review page.
  [F4] (Pass)    Set/Reset pass word.
  [F6] (Debug)   Start Debug mode & open Editor page.

-[VARS] (Variables Review Page)
  [F1] (A<>a)          Switch between display of capital and small letter variables.
  [F2] (Init)          Initialize all the displayed variables.
  [F3] (D<>I)          Siwitch between Integer and Double variables.
  [F6] (>Hex/>Dec)     Switch display of values between in Hex and Dec.
  [SHIFT]+[UP]/[DOWN]  Scroll single page up and down.
  ※ Local variabe is marked with "=".

-[SHIFT]
  [MENU] (SETUP) Go to Setup page.
  [MENU][F6]     Pop up Version information.

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
      (MENU))    SelectCommand  (fx-CG mode)
-[F4] (A<>a)     Switch between Capital and small letter.
-[F5] (CHAR)     Go to Character Select page.
-[F6] (EXE)      Run program or restart program when it's in pause.

-[SHIFT]+
  [EXIT](QUIT)        Go back to File List page.
  [LEFT]              Change font size & line spacing smaller.
  [RIGHT]             Change font size & line spacing bigger.
  [UP]                Scroll 1 page (6 lines) up to top.
  [DOWN]              Scroll 1 page (6 lines) down to end.
  [F1] (Var)          Go to Variables page.
  [F2] (Mat)          Go to Mat Variables page.
  [F3] (V-WIN)        Go to ViewWindow page.
  [F4] (SKTCH)        Select Command. (fx-CG mode)
       (Dump)/(List)  Switch between "Basic List" and "Hex Dump" display.(fx-5800P mode)
  [F5] (A<>a)         Switch between Capital and small letter.
  [F6] (CHAR)         Go to Character Select page. (fx-CG mode)
       (G<>T)         Switch Graphics and Text screen.(fx-5800P mode)

  [3] Input keycode of Getkey command.
  [6] Pop up Color Picker to input color code.

-[OPTN]  Pop up Command List supported by [OPTN] of fx-CG.(fx-5800P mode)
         Select Command. (fx-CG mode)

-[VARS]  Pop up Comamnd List supported by [VARS] of fx-CG.(fx-5800P mode)
         Select Command. (fx-CG mode)

-[MENU]  Command input history (chronological order/input frequency order is changed by right and left key.)
  [AC/ON] Erase history

-[SHIFT][VARS] (PRGM)  Pop up Command List supported by [PRGM] of fx-CG.

-[SHIFT][MENU] (SETUP) Go to Setup page.(fx-5800P mode)
               (SETUP) Select Command.  (fx-CG mode)

-[ALPH][DEL] (UNDO) Delete or Paste operations are restored to the previous state.(only once)

-[EXIT]   Go back to previous page, cancel clip mode or go back to File List page.

-------------------------------------------------------------------------------
Debug Mode Editor Page
-------------------------------------------------------------------------------
Debug mode page comes up when program is broken intentionally or by error. Right example above shows Debug Mode Edotr Page that mini-font is applied in setup page.

In Degub Mode, status line is getting inverted on C.Basic for FX or becomes cyan color background.

-[F1](Cont) Continue program from cursor position.

-[F2](TRACE) Trace program from cursor position and come back to Editor page in Debug Mode right after the traced command runs, .

-[F3](STEPS) Trace and Step Over comamnd from cursor position and come back to Editor page in Debug Mode right after the command runs. When sub-routine runs, the sub-routine is executed to the end.

-[F4](S.Out) Step Out from cursor position and come back to Editor page in Debug Mode right after the command runs. When the sursor is in sub-routine the sub-routine is executed to the end.

-[F5](L<>S) Switch Editor page and program running screen. When the program running screen is displayed, debug operaiton with [F1] - [F4] is still available.

-[F6]( > )  Switch Debug manu and Editor menu. When program running screen is displayed, pressing [F6](>) displays Debug menu.

-[EXIT]     Go back to previous page, cancel clip mode or go back to normal (not in Debug mode) Editor page.

(*In the debug mode, status line becomes cyan color background.)

-------------------------------------------------------------------------------
Program Running Page
-------------------------------------------------------------------------------
During program is running;

-[AC]  Stop program.
  [EXIT]  Go to Editor in Debug mode. Cursor locates at break point.
  [Left/Right]  Go to Editor in Debug mode. Cursor locates at break point.
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
In Mat/List Page you can review and edit Mat or List or Vct. Operation procedure of Mat and List and Vct is consistant, since List is internaly implemented as Mat.
(The Mat marked by * in this page is alocated by addressing.)

[Operation in Review Mode]
-[F1] (DEL)            Delete selected Mat/List.
-[F2] (DEL-ALL)        Delete all Mat/List.
-[F3] (DIM)            Set dimension, element size and start index of selected Mat/List/Vct.
-[F4] (INITIAL)        Initialize all elements of selected Mat/List/Vct.
-[F5] (Mat#/Lst#/Vct#) Select a number and jump to Mat/List/Vct of the number in alphabetical order.
-[F6] (A<>a)           Switch between capital letter and small letter of Mat name.

-[EXE]   Enter to Edit Mode of Mat/List/Vct. (in details see below)

-[SHIFT]+[F1] (>Mat)  Select Mat
-[SHIFT]+[F2] (>List) Select List
-[SHIFT]+[F3] (>Vct)  Select Vct

-[OPTN]  Switch showing between Mat and List.
-[VARS]  Switch showing between Mat and Vct.

-[LEFT]  backword 26.
-[RIGHT] forword 26.

-[SHIFT]+[UP][DOWN] page scroll

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
 - Using in nibble (4 bits matrix), 4 bits color value is applied.

 -[F1] (0 <> 1)  Switch element value between 0 and 1.(1 bit Mat or g1m mode)
 -[F1]  Change to the color displayed by the function menu. (other than the 1 bit Mat of the g3m mode)
   *It change to black as same color

 Only as for the following, g3m mode (except the 1 bit Mat/List)
 -[SHIFT]
    [5] Input a 16 bits color other than it with a 4 bits color in the nibble (4 bits).
    [6] Input a 16 bits color. (except the nibble (4 bits) Mat/List)

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

C.Basic does support a script "?A" supported by fx-5800P. This is not supported by fx-CG (script "?->A" only is supported).
Function of string output command " " is fully compatible with fx-5800P in details but not exactly compatible with fx-CG. See Topics in details.


-------------------------------------------------------------------------------
Select Character Page
-------------------------------------------------------------------------------
In addition to character set of genuine Casio Basic, more characters are supported.

In "Character Select" page;
[F6]                      ASCII, Kana, external character can be applied.
[SHIFT]/[F1]～[F6]        Continuous input is available.
[OPTN]                    Switch between mini-font and normal font.
[VARS] - C.Basic for FX   Switch between OS supported the mini-font and normal font.
[VARS] - C.Basic for CG   Switch between C.Basic font and OS supported font.
When OS supported font is displayed, special character can be input without escape.

(about GB font)
In order to use GB font, in "Setup" page "Edit GB Font" setting should be "On" or "Full". Then
[x^2]  to switch between "GB code input mode" and other font input mode.

[F1]  Jump to beginning 0xA0**.
[F2]  Go back 16 pages.
[F3]  Go back 1 page.
[F4]  Go forward 1 page.
[F5]  Go forward 16 pages.
[F6]  Jump to end 0xFE**.

*At the time of GB code indication, escape(0x5C) may be necessary to display special character.
*Character with the escape(0x5C) is shown with yellow background.

===============================================================================
Setup Page
===============================================================================
Setup items - compatible with genuine Casio Basic

Angle       :Deg/Rad/Gra
Complex Mode:Real/a+bi/r∠θ
Draw Type   :Connect/Plot
Coord       :on/off
Grid        :on/off/Line
Axes        :on/off/Scale
Label       :on/off
Derivative  :on/off
Background  :None / Pict1-20
Plot/LineCol:Black etc.
Sketch Line :Normal/Thick/Broken/Dot/Thin
---------------------------------------------------

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
Exp Display  : E / Stdx10 / Allx10
	　　　select exponent character.
	　　　E:      -1.23e99　   previous character.
	      Stdx10: -1.23(x10)99 new character only standard font size.
	      Allx10: -1.23(x10)99 new character all font size.

- Related command: '#exp 0	//　previous character.
		   '#exp 1	//　new character only standard font size.
		   '#exp 2	//　new character all font size.
-------------------------------------------------------------------------------
Syntax Help   : On/Off
  the help dispyay at the command input of editor.
-------------------------------------------------------------------------------
SetupRecover  : On/Off
  set it whether you return setting compatible with genuine CasioBasic after a program in SetupRecover.
-------------------------------------------------------------------------------
Command Input :C.Basic/Standard/>5800/>9800
  Select Standard(fx-CG) method or C.Basic(fx-5800P) method.
  You can change the mode of "" output specifications compatible with fx-5800P and  FX/CG.
 - Related command: '#58
                    '#98
-------------------------------------------------------------------------------
[X]character : [0x90]/[X]
  To select character of [X/Theta/T]key.
-------------------------------------------------------------------------------
EnableExFont : on/off
  Set to use external font.
-------------------------------------------------------------------------------
Edit Indent+  : Off/1/2/4 Save-
 enable auto indent in editor.
 Off: diable auto indent.
  1 : Set indent width to 1.
  2 : Set indent width to 1.
  4 : Set indent width to 1.
 Save-: delete blank spaces including indents when saving program. (=to compatible with Casio Basic)
-------------------------------------------------------------------------------
Edit ExtFont  : On/Off
  enable external font in editor.
-------------------------------------------------------------------------------
Edit GB Font  : On/Off/Full
  To select GB font display on editor.
  Off : normal. not display GB font.
  On  : display GB font without F7xx,F9xx,E5xx,E6xx,E7xx
  Full: display all GB font.
-------------------------------------------------------------------------------
EditFontSize  : Standard/Mini/MiniMini/MiniMiniFX /+Fixed pitch/+Gap
  Set to Editor font size.
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
Edit-backCol  : Black/Blue/Red/Magenta/Green/Cyan/Yellow/White
 select back color of editor.
-------------------------------------------------------------------------------
Edit-baseCol  : Black/Blue/Red/Magenta/Green/Cyan/Yellow/White
 select base color of editor.
-------------------------------------------------------------------------------
E-NumericCol  : Black/Blue/Red/Magenta/Green/Cyan/Yellow/White
 select numeric color of editor.
-------------------------------------------------------------------------------
E-CommandCol  : Black/Blue/Red/Magenta/Green/Cyan/Yellow/White
 select command color of editor.
-------------------------------------------------------------------------------
Ed-QuotColor  : Black/Blue/Red/Magenta/Green/Cyan/Yellow/White
 select strings color of editor.
-------------------------------------------------------------------------------
E-CommentCol  : Black/Blue/Red/Magenta/Green/Cyan/Yellow/White
 select comment color of editor.
-------------------------------------------------------------------------------
E-LineNumCol  : Black/Blue/Red/Magenta/Green/Cyan/Yellow/White
 select line number color of editor.
-------------------------------------------------------------------------------
Auto OverClk  : on/off
 Set auto overclock for CG10/20
-------------------------------------------------------------------------------
Heap RAM Size : 96KB/117KB/127KB/SIZE
 Set maximam heap size of C.Basic use.
 (* You cannot change it at the time of the program editing.)
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
  (*) This setting is invalid.
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
- Related command: '#Break0	'#Break1
-------------------------------------------------------------------------------
Exec TimeDsp: on / off  /reset  /%HR
  Set if program running time is displayed or not after the program quits.
  Timer starts at first line of program. ? or Disps command reset the timer.
  In the case of Getkey1/2, the timer is suspended and starts after command again.
  on reset: The timer is reset and starts again after Getkey1/2.
  %HR: use 1/32768s timer
  - Related command: '#GetkeyC  '#GetkeyR
-------------------------------------------------------------------------------
IfEnd Check: on / off
  Set if one-to-one correspondence of "If" and "IfEnd" statement is chacked or not.
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
  Format [m,n] and [X,Y] are in transposed matrix each other, but just only the apperance on screen is different. The internal matrix data is still same, won't be changed.
-------------------------------------------------------------------------------
Matrix base: 0 / 1
  Set index of matrix starts with 0 or 1.
  When the start index is set to 0, lfet-top pixel comes to be available to use and left-yop coordinate of device coordinates can be (0, 0).
  - Related command: '#Mat 0, '#Mat 1
-------------------------------------------------------------------------------
DATE: 2017/01/17 TUE
  Set date of internal RTC. A day of the week is autmatically set.
-------------------------------------------------------------------------------
TIME: 23:59:59
  Set time of internal RTC
  - Related command: DATE, TIME
-------------------------------------------------------------------------------
Favorite Col : Black/Blue/Red/Magenta/Green/Cyan/Yellow/White/16bit Color
  select favorite file color of editor.
-------------------------------------------------------------------------------
Auto file Save:on/off
  Set to auto save without a popup.
-------------------------------------------------------------------------------
Force g1m save: on / off
  Set "save automatically" or "not save" g1m file, after run or edit of text file.
-------------------------------------------------------------------------------
Direct GB cnvt : on/off
  Set GB code without text conversion.
 (* conversion supports The GB mode of the editor.)
-------------------------------------------------------------------------------
Pict mode: S.Mem / Heap / Both
  - S.Mem: Pict file is created in storage memory and it takes a bit longer.
  - Heap: Pict file is not actually created, but compatible file is allocaed in main memory. (recommend)
  - Both: Stored always in storage memory/SD.
  - Clear:Deletes the Pict compatible file created in main memory at program start every time.
-------------------------------------------------------------------------------
Storage mode: S.Mem / MainMem
  S.Mem: Files in storage memory is used for run and edit.
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
Wait count : 0～9999
 Set to wait for slowdown execution speed.
  - Related command:Wait
-------------------------------------------------------------------------------
G1M/G3M mode : auto/g3m/g1m
 Set to execute mode as default.
 - Related command:
  '#G1M
  '#g1m
  '#G3M
  '#g3m
-------------------------------------------------------------------------------
Execute mode: Dbl# / Int% / CPLX
  Set running mode as default.
  - Related command: '#CBasic, '#CBASIC, '#CBINT, '#CBint '#CBCPLX '#CBcplx
-------------------------------------------------------------------------------


===============================================================================
Acknowledgment
===============================================================================

To investigate and implement internal specification,
"SuperH-based fx calculators (version 20)"
gave useful information.

This add-in program is compiled by miniSDK1.09.
If there was not miniSDK, this add-in would not be born.
Allow me to use this opportunity to offer my thanks.


For file handling in C.Basic,
https://community.casiocalc.org/topic/6836-wsc-fvm-v12-casio-fx-9860-series-on-calc-c-compiler/
Wsc & Fvm V1.2 - Casio Fx-9860 Series On-calc C Compiler"
gave useful information.

For Japanese Kana Fonts, font data and output routine in the following site is used;
http://www.d-b.ne.jp/~k-kita/fx_sdk_005.html"


For detailed feature and function of original Casio Basic, following site is referenced;
e-Gadget - Programming Calculator by Krtyski
https://egadget.blog.fc2.com/
(almost contents are in Japanese).

http://community.casiocalc.org/topic/7637-cbasic-international-release/
https://codewalr.us/index.php?topic=1818.0
https://www.planet-casio.com/Fr/forums/topic14738-1-C.Basic.html
https://www.planet-casio.com/Fr/forums/lecture_sujet.php?id=15240
There opinions were very useful.

I am grateful for everyone's cooperation.

by sentaro21

===============================================================================
related link
===============================================================================
https://egadget2.web.fc2.com/CBasic/Interpreter/CBasic_interpreter.html
https://egadget.blog.fc2.com/blog-entry-630.html (part1)
https://egadget.blog.fc2.com/blog-entry-658.html (part2)
https://egadget.blog.fc2.com/blog-entry-664.html (part3)

http://community.casiocalc.org/topic/7637-cbasic-international-release/
https://codewalr.us/index.php?topic=1818.0
https://www.planet-casio.com/Fr/forums/topic14738-1-C.Basic.html
https://www.planet-casio.com/Fr/forums/lecture_sujet.php?id=15240

===============================================================================
Licensing
===============================================================================
This software is free software, in accordance with GPLv2.