/* *****************************************************************************
 * CB_error.h -- Error macro header
 * Copyright (C) 2015-2024 Sentaro21 <sentaro21@pm.matrix.jp>
 *
 * This file is part of C.Basic.
 * C.Basic is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2.0 of the License,
 * or (at your option) any later version.
 *
 * C.Basic is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with C.Basic; if not, see <https://www.gnu.org/licenses/>.
 * ************************************************************************** */

/** This header file defines the return value of errors supported by C.Basic.

    The assigned value of each error is based on the order of the error message
    table in the appendix section of the fx-9860G II software manual, page 330,
    α-1.

    Changing these values affects the output of `System(-9)`. */

extern int g_error_prog;
extern int g_error_ptr;
extern int g_error_type;

#define SyntaxERR           1       // Syntax ERROR
#define MathERR             2       // Math ERROR
#define GoERR               3       // Go ERROR
#define NestingERR          4       // Nesting ERROR
#define StackERR            5       // Stack ERROR
#define MemoryERR           6       // Memory ERROR
#define ArgumentERR         7       // Argument ERROR
#define DimensionERR        8       // Dimension ERROR
#define RangeERR            9       // Range ERROR
// #define ConditionERR 10
#define NonRealERR          11      // Non-real ERROR
// #define ComplexNumberinList 12
// #define ComplexNumberinMatrix 13
// #define ComplexNumberinMatrixOrVector 14
// #define ComplexNumberinData 15
// #define CantSimplify 16
// #define CantSolver 17
// #define NoVariable 18
// #define ConversionERR 19
#define ComERR              20      // COM ERROR
#define TransmitERR         21      // Transmit ERROR
#define ReceiveERR          22      // Receive ERRORs
// #define MemoryFull 23
// #define InvalidDataSize 24
// #define TimeOut 25
// #define CircularERR 26
// #define PleaseReconnect 27
#define TooMuchData         28      // Too much data
// #define FragmentationERR 29
// #define InvalidType 30
// #define StorageMemoryFull 31
// #define OutOfRangeERR 32

#define MissingNext         33      // Missing `Next`
#define MissingFor          34      // Missing `For`
#define MissingWhileEnd     35      // Missing `WhileEnd`
#define MissingWhile        36      // Missing `While`
#define MissingLpWhile      37      // Missing `LpWhile`
#define MissingDo           38      // Missing `Do`
#define NotInLoop           39      // Not in loop (`Break`)
#define ZeroDivision        40      // Zero division
#define UndefinedLabel      41      // Undefined label
#define NotEnoughMemory     42      // Not enough memory
#define StringTooLong       43      // String too long
#define UndefinedMatrix     44      // Undefined matrix
#define InvalidSize         45      // Invalid matrix byte size
#define ProgNotFound        46      // Program not found
#define TooManyProg         47      // Too many programs
#define MissingIfEnd        48      // Missing `IfEnd`
#define MissingIf           49      // Missing `If`
#define MissingSwitch       50      // Missing `Switch`
// #define DefaultWithoutSwitchERR 51
#define MissingSwitchEnd    52      // Missing `SwitchEnd`
// #define SwitchEndWithoutSwitchERR 53
#define FileNotFound        54      // File not found
// #define ElementSizeERR 55
// #define AlreadyOpenERR 56
#define ComClosed           57      // COM Closed
#define InvalidType         58      // Invalid type
#define OutOfDomain         59      // Out of domain
#define UndefinedAlias      60      // Undefined alias
#define UndefinedFunc       61      // Undefined function
#define UnsupportedItem     62      // Unsupported item
#define VarMemoryFull       63      // Too many extended/alias variables
#define AssignERR           64      // Assign ERROR
#define AddressERR          65      // Address ERROR
#define TimeOut             66      // Time out
#define MissingExcept       70      // Missing `Except`
#define MissingTry          71      // Missing `Try`
#define FileERR             100     // File ERROR

/**
 * Main function to raise error for exceptions.
 * The following error information will be saved:
 * - Input to `g_error_type`;
 * - current interpreter cursor position to `g_error_ptr`;
 * - current program position to `g_error_prog`
 */
void CB_Error(int error_macro);

/* Main function to call `ERROR()`. */
void CB_ErrMsg(int error_macro);

/**
 * Subsidiary function to define the error text and draw a Casio style
 * error pop-up.
 * The maximum length of the text the pop-up can hold is 17 characters.
 */
void ERROR(char *error_text);