#include <ctype.h>
#include <fxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <timer.h>

typedef struct  {
	double real;
	double imag;
} complex;

#include "CB_complex.h"
#include "CB_interpreter.h"
#include "KeyScan.h"
#include "CB_inp.h"
#include "CB_kana.h"
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

#include "CB_error.h"
#include "fx_syscall.h"
#include "fxCG_Registers.h"
#include "MonochromeLib.h"
#include "CB_MonochromeLib.h"
