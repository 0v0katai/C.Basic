/* *****************************************************************************
 * CB.h -- Main header file
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

#include <ctype.h>
#include <fxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <timer.h>

#include "_h_c_lib.h"
#include "assert.h"
#include "endian.h"
#include "ensigdsp.h"
#include "errno.h"
#include "float.h"
#include "limits.h"
#include "machine.h"
#include "setjmp.h"
#include "smachine.h"
#include "stdarg.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "umachine.h"

typedef struct  {
	double real;
	double imag;
} complex;

typedef char bool;
#define false 0
#define true 1

#include "CB_complex.h"
#include "CB_interpreter.h"
#include "KeyScan.h"
#include "CB_inp.h"
#include "CB_Kana.h"
#include "CB_io.h"
#include "CBI_interpreter.h"
#include "CB_glib.h"
#include "CB_glib2.h"
#include "CB_Eval.h"
#include "CBI_Eval.h"
#include "CB_file.h"
#include "CB_edit.h"
#include "CB_setup.h"
#include "CB_Time.h"
#include "CB_Matrix.h"
#include "CB_Str.h"
#include "CB_Help.h"
#include "config.h"
#include "CB_error.h"
#include "fx_syscall.h"
#include "fxCG_Registers.h"
#include "MonochromeLib.h"
#include "CB_MonochromeLib.h"
#include "CB_TextConv.h"

#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))
