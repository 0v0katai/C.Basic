#include <ctype.h>
#include <fxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <timer.h>
#include "KeyScan.h"
#include "CB_io.h"
#include "CB_inp.h"
#include "CB_glib.h"
#include "CB_glib2.h"
#include "CB_Eval.h"
#include "CB_file.h"
#include "CB_edit.h"
#include "CB_setup.h"
#include "CB_Time.h"

#include "CB_interpreter.h"
#include "CBI_interpreter.h"
#include "CB_error.h"
#include "fx_syscall.h"
#include "CB_Matrix.h"


//-----------------------------------------------------------------------------
const char ConvList0000[][8]={
"",			// 00
"femto",	// 01
"pico",		// 02
"nano",		// 03
"micro",	// 04
"milli",	// 05
"kilo",		// 06
"Mega",		// 07
"Giga",		// 08
"Tera",		// 09
"Peta",		// 0A
"Exa",		// 0B
"Disps",	// 0C
"\r\n",		// 0D
"->",		// 0E
"Exp",		// 0F

"<=",		// 10
"<>",		// 11
">=",		// 12
"=>",		// 13
"f1",		// 14
"f2",		// 15
"f3",		// 16
"f4",		// 17
"f5",		// 18
"f6",		// 19
"&HA",		// 1A
"&HB",		// 1B
"&HC",		// 1C
"&HD",		// 1D
"&HE",		// 1E
"&HF",		// 1F

" ",		// 20
"Char!",	// 21
"\"",		// 22
"#",		// 23
"$",		// 24
"%",		// 25
"&",		// 26
"'",		// 27
"(",		// 28
")",		// 29
"**",		// 2A
"++",		// 2B
",",		// 2C
"Char-",	// 2D
".",		// 2E
"//"		// 2F
};

const char ConvList0080[][16]={

"Pol(",		// 80
"sin ",		// 81
"cos ",		// 82
"tan ",		// 83
"&h",		// 84
"ln ",		// 85
"Sqrt",		// 86
"(-)",		// 87
"nPr",		// 88
"+",		// 89
"xnor",		// 8A
"^<2>",		// 8B
"dms",		// 8C
"Integral(",// 8D
"Mod",		// 8E
"Sigmax^2",	// 8F

"",			// 90
"sin^-1 ",	// 91
"cos^-1 ",	// 92
"tan^-1 ",	// 93
"&d",		// 94
"log ", 	// 95
"Cbrt", 	// 96
"Abs ", 	// 97
"nCr",		// 98
"-",		// 99
"xor",		// 9A
"^<-1>",	// 9B
"deg",		// 9C
"@9D",		// 9D
"Med",		// 9E
"Sigmax",	// 9F
			   
"Rec(",		// A0
"sinh ",	// A1
"cosh ",	// A2
"tanh ",	// A3
"&o",		// A4
"e^",		// A5
"Int ", 	// A6
"Not ", 	// A7
"^",		// A8
"*",		// A9
"or",		// AA
"!",		// AB
"rad",		// AC
"minY", 	// AD
"minX", 	// AE
"Statn",	// AF
			   
"@00B0",	// B0
"sinh^-1 ", // B1
"cosh^-1 ", // B2
"tanh^-1 ", // B3
"&b",		// B4
"(10)", 	// B5
"Frac ",	// B6
"Neg ", 	// B7
"Xrt",		// B8
"/",		// B9
"and",		// BA
"frac", 	// BB
"gra",		// BC
"maxY", 	// BD
"maxX", 	// BE
"Sigmay2",	// BF

"Ans",		// C0
"Ran#", 	// C1
"x-bar",	// C2
"y-bar",	// C3
"sigmax",	// C4
"Sx",		// C5
"sigmay",	// C6
"Sy",		// C7
"Regression_a",	// C8
"Regression_b",	// C9
"Regression_r",	// CA
"x-hat",		// CB
"y-hat",		// CC
"<r>",		   	// CD
"Theta",		// CE
"Sigmay",		// CF
			   
"pi",			// D0
"Cls",		   	// D1
"@00D2",	   	// D2
"Rnd",		   	// D3
"&D",		  	// D4
"&H",		  	// D5
"&B",		  	// D6
"&O",		  	// D7
"@00D8",	  	// D8
"Norm ",	  	// D9
"Deg",		   	// DA
"Rad",		   	// DB
"Gra",		   	// DC
"Eng",		   	// DD
"Intg ",		// DE
"Sigmaxy",		// DF
			   
"Plot ",		// E0
"Line", 		// E1
"Lbl ", 		// E2
"Fix ", 		// E3
"Sci ", 		// E4
"", 			// E5
"", 			// E6
"", 			// E7
"Dsz ", 		// E8
"Isz ", 		// E9
"Factor ",		// EA
"ViewWindow ",	// EB
"Goto ",		// EC
"Prog ",		// ED
"Graph Y=",    	// EE
"Graph Integral", // EF
			   
"Graph Y>",		// F0
"Graph Y<", 	// F1
"Graph Y>=",	// F2
"Graph Y<=",	// F3
"Graph r=", 	// F4
"Graph(X,Y)=(", // F5
"Para,",		// F6
"", 			// F7
"@00F8",	   	// F8
"", 			// F9
"Gosub ",	   	// FA	@00FA
"ProbP(",		// FB
"ProbQ(",		// FC
"ProbR(",		// FD
"Probt(",		// FE
" ",		 	// FF

};

const char ConvList7F00[][16]={
"Xmin",				// 7F00
"Xmax", 			// 7F01
"Xscl", 			// 7F02
"@7F03",			// 7F03
"Ymin", 			// 7F04
"Ymax", 			// 7F05
"Yscl", 			// 7F06
"@7F07",			// 7F07
"TThetamin",		// 7F08
"TThetamax",		// 7F09
"TThetaptch",		// 7F0A
"Xfct", 			// 7F0B
"Yfct", 			// 7F0C
"D Start",			// 7F0D
"D End",			// 7F0E
"D pitch",			// 7F0F
					
"RightXmin",		// 7F10
"RightXmax",		// 7F11
"RightXscl",		// 7F12
"@7F13",			// 7F13
"RightYmin",		// 7F14
"RightYmax",		// 7F15
"RightYscl",		// 7F16
"@7F17",			// 7F17
"RightTThetamin",	// 7F18
"RightTThetamax",	// 7F19
"RightTThetaptch",	// 7F1A
"@7F1B",			// 7F1B
"@7F1C",			// 7F1C
"Regression_c", 	// 7F1D
"Regression_d", 	// 7F1E
"Regression_e", 	// 7F1F
					
"Max(", 			// 7F20
"Det ", 			// 7F21
"Arg ", 			// 7F22
"Conjg ",			// 7F23
"ReP ", 			// 7F24
"ImP ", 			// 7F25
"d/dx(",			// 7F26
"d^2/dx^2(",		// 7F27
"Solve(",			// 7F28
"Sigma(",			// 7F29
"FMin(",			// 7F2A
"FMax(",			// 7F2B
"Seq(", 			// 7F2C
"Min(", 			// 7F2D
"Mean(",			// 7F2E
"Median(",			// 7F2F
					
"SolveN(",			// 7F30
"@7F31",			// 7F31
"@7F32",			// 7F32
"@7F33",			// 7F33
"Red ", 			// 7F34
"Blue ",			// 7F35
"Green ",			// 7F36
"@7F37",			// 7F37
"@7F38",			// 7F38
"@7F39",			// 7F39
"MOD(", 			// 7F3A
"MOD_Exp(", 		// 7F3B
"GCD(", 			// 7F3C
"LCM(", 			// 7F3D
"StdDev(",			// 7F3E
"Variance(",		// 7F3F
					
"Mat ", 			// 7F40
"Trn ", 			// 7F41
"*Row ",			// 7F42
"*Row+ ",			// 7F43
"Row+ ",			// 7F44
"Swap ",			// 7F45
"Dim ", 			// 7F46
"Fill(",			// 7F47
"Identity ",		// 7F48
"Augment(", 		// 7F49
"List->Mat(",		// 7F4A
"Mat->List(",		// 7F4B
"Sum ", 			// 7F4C
"Prod ",			// 7F4D
"Percent ", 		// 7F4E
"Cuml ",			// 7F4F
					
"Imaginary",		// 7F50
"List ",			// 7F51
"Dlist ",			// 7F52
"Infinity", 		// 7F53
"Angle",			// 7F54
"Ref ", 			// 7F55
"Rref ",			// 7F56
"Conv", 			// 7F57
"@7F58",			// 7F58
"@7F59",			// 7F59
"@7F5A",			// 7F5A
"@7F5B",			// 7F5B
"@7F5C",			// 7F5C
"@7F5D",			// 7F5D
"@7F5E",			// 7F5E
"@7F5F",			// 7F5F
					
"Sim Coef", 		// 7F60
"Ply Coef", 		// 7F61
"Sim Result",		// 7F62
"Ply Result",		// 7F63
"Financial n",		// 7F64
"Financial I%", 	// 7F65
"Financial PV", 	// 7F66
"Financial PMT",	// 7F67
"Financial FV", 	// 7F68
"@7F69",			// 7F69
"List1",			// 7F6A
"List2",			// 7F6B
"List3",			// 7F6C
"List4",			// 7F6D
"List5",			// 7F6E
"List6",			// 7F6F
					
"@7F70",			// 7F70
"@7F71",			// 7F71
"@7F72",			// 7F72
"@7F73",			// 7F73
"@7F74",			// 7F74
"@7F75",			// 7F75
"Q1",				// 7F76
"Q3",				// 7F77
"x1",				// 7F78
"y1",				// 7F79
"x2",				// 7F7A
"y2",				// 7F7B
"x3",				// 7F7C
"y3",				// 7F7D
"@7F7E",			// 7F7E
"",				// 7F7F
					
"@7F80",			// 7F80
"@7F81",			// 7F81
"@7F82",			// 7F82
"@7F83",			// 7F83
"@7F84",			// 7F84
"logab(",			// 7F85
"RndFix(",			// 7F86
"RanInt#(", 		// 7F87
"RanList#(",		// 7F88
"RanBin#(", 		// 7F89
"RanNorm#(",		// 7F8A
"@7F8B",			// 7F8B
"Sigmaan",			// 7F8C
"Sigmabn",			// 7F8D
"Sigmacn",			// 7F8E
"Getkey",			// 7F8F
					
"F Result", 		// 7F90
"F Start",			// 7F91
"F End",			// 7F92
"F pitch",			// 7F93
"R Result", 		// 7F94
"R Start",			// 7F95
"R End",			// 7F96
"H Start",			// 7F97
"H pitch",			// 7F98
"@7F99",			// 7F99
"@7F9A",			// 7F9A
"@7F9B",			// 7F9B
">Simp",			// 7F9C
"@7F9D",			// 7F9D
"@7F9E",			// 7F9E
"@7F9F",			// 7F9F
					
"an ",				// 7FA0
"an+1", 			// 7FA1
"an+2", 			// 7FA2
"Subscriptn",		// 7FA3
"a0",				// 7FA4
"a1",				// 7FA5
"a2",				// 7FA6
"bn ",				// 7FA7
"bn+1", 			// 7FA8
"bn+2", 			// 7FA9
"b0",				// 7FAA
"b1",				// 7FAB
"b2",				// 7FAC
"anStart",			// 7FAD
"bnStart",			// 7FAE
"@7FAF",			// 7FAF
					
" And ", 			// 7FB0
" Or ",				// 7FB1
"@7FB2",			// 7FB2
"Not ", 			// 7FB3
" Xor ", 			// 7FB4
"Sigmaan+1",		// 7FB5
"Sigmabn+1",		// 7FB6
"Sigmacn+1",		// 7FB7
"@7FB8",			// 7FB8
"Sigmaan+2",		// 7FB9
"Sigmabn+2",		// 7FBA
"Sigmacn+2",		// 7FBB
"Int/ ",			// 7FBC
"Rmdr ",			// 7FBD
"Fa",				// 7FBE
"@7FBF",			// 7FBF
					
"n1",				// 7FC0
"n2",				// 7FC1
"x-bar1",			// 7FC2
"x-bar2",			// 7FC3
"sx1",				// 7FC4
"sx2",				// 7FC5
"Sxp",				// 7FC6
"p-hat",			// 7FC7
"p-hat1",			// 7FC8
"p-hat2",			// 7FC9
"Lower",			// 7FCA
"Upper",			// 7FCB
"P/Year",			// 7FCC
"C/Year",			// 7FCD
"Fb",				// 7FCE
"@7FCF",			// 7FCF
					
"F-Value",			// 7FD0
"z-Value",			// 7FD1
"p-Value",			// 7FD2
"t-Value",			// 7FD3
"se",				// 7FD4
"x^2",				// 7FD5
"r^2",				// 7FD6
"Adf",				// 7FD7
"Edf",				// 7FD8
"df",				// 7FD9
"SSa",				// 7FDA
"MSa",				// 7FDB
"SSe",				// 7FDC
"MSe",				// 7FDD
"Fab",				// 7FDE
"@7FDF",			// 7FDF
					
"Bdf",				// 7FE0
"ABdf", 			// 7FE1
"pa",				// 7FE2
"pb",				// 7FE3
"pab",				// 7FE4
"", 				// 7FE5
"", 				// 7FE6
"", 				// 7FE7
"@7FE8",			// 7FE8
"CellSum(", 		// 7FE9
"CellProd(",		// 7FEA
"CellMin(", 		// 7FEB
"CellMax(", 		// 7FEC
"CellMean(",		// 7FED
"CellMedian(",		// 7FEE
"CellIf(",			// 7FEF
					
"GraphY",			// 7FF0
"Graphr",			// 7FF1
"GraphXt",			// 7FF2
"GraphYt",			// 7FF3
"GraphX",			// 7FF4
"IsExist(",				// 7FF5
"", 				// 7FF6
"", 				// 7FF7
"", 				// 7FF8
"", 				// 7FF9
"", 				// 7FFA
"SSb",				// 7FFB
"SSab", 			// 7FFC
"MSb",				// 7FFD
"MSab", 			// 7FFE
"",				// 7FFF

};

const char ConvListF700[][17]={
"If ",				// F700
"Then ",			// F701
"Else ",			// F702
"IfEnd",			// F703
"For ",				// F704
" To ",				// F705
" Step ",			// F706
"Next",				// F707
"While ",			// F708
"WhileEnd",			// F709
"Do",				// F70A
"LpWhile ",			// F70B
"Return",			// F70C
"Break",			// F70D
"Stop",				// F70E
"ElseIf ",			// F70F

"Locate ",			// F710
"Send(",			// F711
"Receive(",			// F712
"OpenComport38k",	// F713
"CloseComport38k",	// F714
"Send38k ",			// F715
"Receive38k ",		// F716
"ACBreak",				// F717
"ClrText",			// F718
"ClrGraph",			// F719
"ClrList",			// F71A
"LinearReg(a+bx) ",	// F71B
"S-L-Normal",		// F71C
"S-L-Thick",		// F71D
"S-L-Broken",		// F71E
"S-L-Dot",			// F71F

"DrawGraph",		// F720
"PlotPhase ",		// F721
"DrawDyna",			// F722
"DrawStat",			// F723
"DrawFTG-Con",		// F724
"DrawFTG-Plt",		// F725
"DrawR-Con",		// F726
"DrawR-Plt",		// F727
"DrawRSigma-Con",	// F728
"DrawRSigma-Plt",	// F729
"DrawWeb ",			// F72A
"NormalG ",			// F72B
"ThickG ",			// F72C
"BrokenThickG ",	// F72D
"DispF-Tbl",		// F72E
"DispR-Tbl",		// F72F

"SimplifyAuto",		// F730
"SimplifyMan",		// F731
"NPPlot",			// F732
"Sinusoidal",		// F733
"SinReg ",			// F734
"Logistic",			// F735
"LogisticReg ",		// F736
"@F737",			// F737
"@F738",			// F738
"@F739",			// F739
"Pie",				// F73A
"@F73B",			// F73B
"Bar",				// F73C
"@F73D",			// F73D
"@F73E",			// F73E
"DotGet(",				// F73F

"1-Variable ",		// F740
"2-Variable ",		// F741
"LinearReg(ax+b) ",	// F742
"Med-MedLine ",		// F743
"QuadReg ",			// F744
"CubicReg ",		// F745
"QuartReg ",		// F746
"LogReg ",			// F747
"ExpReg(ae^bx) ",	// F748
"PowerReg ",		// F749
"S-Gph1 ",			// F74A
"S-Gph2 ",			// F74B
"S-Gph3 ",			// F74C
"Square",			// F74D
"Cross",			// F74E
"DotTrim(",				// F74F

"Scatter",			// F750
"xyLine",			// F751
"Hist",				// F752
"MedBox",			// F753
"@F754",			// F754
"N-Dist",			// F755
"Broken",			// F756
"Linear",			// F757
"Med-Med",			// F758
"Quad",				// F759
"Cubic",			// F75A
"Quart",			// F75B
"Log",				// F75C
"Exp(ae^bx)",		// F75D
"Power",			// F75E
"ExpReg(ab^x) ",	// F75F

"S-WindAuto",		// F760
"S-WindMan",		// F761
"Graph X=",			// F762
"Y=Type",			// F763
"r=Type",			// F764
"ParamType",		// F765
"@F766",			// F766
"X=Type",			// F767
"X>Type",			// F768
"X<Type",			// F769
"Y>Type",			// F76A
"Y<Type",			// F76B
"Y>=Type",			// F76C
"Y<=Type",			// F76D
"X>=Type",			// F76E
"X<=Type",			// F76F

"G-Connect",		// F770
"G-Plot",			// F771
"@F772",			// F772
"@F773",			// F773
"@F774",			// F774
"@F775",			// F775
"Resid-None",		// F776
"Resid-List ",		// F777
"BG-None",			// F778
"BG-Pict ",			// F779
"GridOff",			// F77A
"@F77B",			// F77B
"GridLine",			// F77C
"GridOn",			// F77D
"Exp(a^bx)",		// F77E
"",				// F77F

"D Var ",			// F780
"@F781",			// F781
"@F782",			// F782
"@F783",			// F783
"@F784",			// F784
"@F785",			// F785
"@F786",			// F786
"Q1Q3TypeStd",		// F787
"VarRange",			// F788
"Q1Q3TypeOnData",	// F789
"@F78A",			// F78A
"@F78B",			// F78B
"SketchNormal ",	// F78C
"SketchThick ",		// F78D
"SketchBroken ",	// F78E
"SketchDot ",		// F78F

"anType",			// F790
"an+1Type",			// F791
"an+2Type",			// F792
"StoPict ",			// F793
"RclPict ",			// F794
"StoGMEM ",			// F795
"RclGMEM ",			// F796
"StoV-Win ",		// F797
"RclV-Win ",		// F798
"Display%",			// F799
"DisplayData",		// F79A
"@F79B",			// F79B
"@F79C",			// F79C
"@F79D",			// F79D
"Menu ",			// F79E
"RclCapt",			// F79F

"Tangent ",			// F7A0
"Normal ",			// F7A1
"Inverse ",			// F7A2
"Vertical ",		// F7A3
"Horizontal ",		// F7A4
"Text ",			// F7A5
"Circle ",			// F7A6
"F-Line ",			// F7A7
"PlotOn ",			// F7A8
"PlotOff ",			// F7A9
"PlotChg ",			// F7AA
"PxlOn ",			// F7AB
"PxlOff ",			// F7AC
"PxlChg ",			// F7AD
"@F7AE",			// F7AE
"PxlTest(",			// F7AF

"SortA(",			// F7B0
"SortD(",			// F7B1
"VarList1",			// F7B2
"VarList2",			// F7B3
"VarList3",			// F7B4
"VarList4",			// F7B5
"VarList5",			// F7B6
"VarList6",			// F7B7
"File1",			// F7B8
"File2",			// F7B9
"File3",			// F7BA
"File4",			// F7BB
"File5",			// F7BC
"File6",			// F7BD
"Y=DrawSpeedNorm",	// F7BE
"Y=DrawSpeedHigh",	// F7BF

"FuncOn",			// F7C0
"SimulOn",			// F7C1
"AxesOn",			// F7C2
"CoordOn",			// F7C3
"LabelOn",			// F7C4
"DerivOn",			// F7C5
"LocusOn",			// F7C6
"SigmadispOn",		// F7C7
"G SelOn ",			// F7C8
"T SelOn ",			// F7C9
"D SelOn ",			// F7CA
"R SelOn ",			// F7CB
"DrawOn",			// F7CC
"ab/c",				// F7CD
"d/c",				// F7CE
"@F7CF",			// F7CF

"FuncOff",			// F7D0
"SimulOff",			// F7D1
"AxesOff",			// F7D2
"CoordOff",			// F7D3
"LabelOff",			// F7D4
"DerivOff",			// F7D5
"LocusOff",			// F7D6
"SigmadispOff",		// F7D7
"G SelOff ",		// F7D8
"T SelOff ",		// F7D9
"D SelOff ",		// F7DA
"R SelOff ",		// F7DB
"DrawOff",			// F7DC
"@F7DD",			// F7DD
"@F7DE",			// F7DE
"@F7DF",			// F7DF

"DotLife(",				// F7E0
"Rect ",				// F7E1
"FillRect ",			// F7E2
"LocateYX ",			// F7E3
"@F7E4",			// F7E4
"",					// F7E5
"",					// F7E6
"",					// F7E7
"ReadGraph(",			// F7E8
"WriteGraph ",			// F7E9
"Switch ",				// F7EA
"Case ",				// F7EB
"Default ",				// F7EC
"SwitchEnd",			// F7ED
"Save ",				// F7EE
"Load(",				// F7EF

"DotShape(",			// F7F0
"Local ",				// F7F1
"@F7F2",			// F7F2
"@F7F3",			// F7F3
"@F7F4",			// F7F4
"@F7F5",			// F7F5
"@F7F6",			// F7F6
"",					// F7F7
"@F7F8",			// F7F8
"",					// F7F9
"@F7FA",			// F7FA
"@F7FB",			// F7FB
"@F7FC",			// F7FC
"@F7FD",			// F7FD
"BackLight ",			// F7FE
"",			// F7FF

};

const char ConvListF900[][17]={
"@F900",			// F900
">&D",				// F901
">&H",				// F902
">&B",				// F903
">&O",				// F904
">DMS",				// F905
">a+bi",			// F906
">re^Theta",		// F907
"Real",				// F908
"a+bi",				// F909
"re^Theta",			// F90A
"EngOn",			// F90B
"EngOff",			// F90C
"@F90D",			// F90D
"@F90E",			// F90E
"@F90F",			// F90F

"Sel a0",			// F910
"Sel a1",			// F911
"cn ",				// F912
"cn+1",				// F913
"cn+2",				// F914
"c0",				// F915
"c1",				// F916
"c2",				// F917
"CnStart",			// F918
"IneqTypeIntsect",	// F919
"@F91A",			// F91A
"fn",				// F91B
"File ",			// F91C
"VarList ",			// F91D
"ClrMat ",			// F91E
"@F91F",			// F91F

"ZoomAuto",			// F920
"Xdot",				// F921
"R-Xdot",			// F922
"@F923",			// F923
"DrawDistNorm ",	// F924
"DrawDistT ",		// F925
"DrawDistChi ",		// F926
"DrawDistF ",		// F927
"None",				// F928
"StickLength",		// F929
"StickHoriz",		// F92A
"IneqTypeUnion",	// F92B
"Graph X>",			// F92C
"Graph X<",			// F92D
"Graph X>=",		// F92E
"Graph X<=",		// F92F

"StrJoin(",			// F930
"StrLen(",			// F931
"StrCmp(",			// F932
"StrSrc(",			// F933
"StrLeft(",			// F934
"StrRight(",		// F935
"StrMid(",			// F936
"Exp>Str(",			// F937
"Exp(",				// F938
"StrUpr(",			// F939
"StrLwr(",			// F93A
"StrInv(",			// F93B
"StrShift(",		// F93C
"StrRotate(",		// F93D
"@F93E",			// F93E
"Str ",				// F93F

"@F940",			// F940
"@F941",			// F941
"@F942",			// F942
"@F943",			// F943
"@F944",			// F944
"@F945",			// F945
"@F946",			// F946
"@F947",			// F947
"@F948",			// F948
"@F949",			// F949
"@F94A",			// F94A
"DotPut(",				// F94B
"@F94C",			// F94C
"@F94D",			// F94D
"@F94E",			// F94E
"@F94F",			// F94F

"@F950",			// F950
"@F951",			// F951
"@F952",			// F952
"@F953",			// F953
"@F954",			// F954
"@F955",			// F955
"@F956",			// F956
"@F957",			// F957
"@F958",			// F958
"@F959",			// F959
"@F95A",			// F95A
"@F95B",			// F95B
"@F95C",			// F95C
"@F95D",			// F95D
"@F95E",			// F95E
"@F95F",			// F95F

"@F960",			// F960
"@F961",			// F961
"@F962",			// F962
"@F963",			// F963
"@F964",			// F964
"@F965",			// F965
"@F966",			// F966
"@F967",			// F967
"@F968",			// F968
"@F969",			// F969
"@F96A",			// F96A
"@F96B",			// F96B
"@F96C",			// F96C
"@F96D",			// F96D
"@F96E",			// F96E
"@F96F",			// F96F

"@F970",			// F970
"@F971",			// F971
"@F972",			// F972
"ColorAuto ",		// F973
"ColorLighter ",	// F974
"ColorLinkX&Y",		// F975
"ColorLinkOnlyX",	// F976
"ColorLinkOnlyY",	// F977
"ColorLinkOn",		// F978
"ColorLinkOff",		// F979
"ColorNormal ",		// F97A
"ERROR",			// F97B
"BLANK",			// F97C
"ColorClr ",		// F97D
"ColorLinkX&Freq",	// F97E
"",			// F97F

"NormPD(",			// F980
"NormCD(",			// F981
"InvNormCD(",		// F982
"tPD(",				// F983
"tCD(",				// F984
"InvTCD(",			// F985
"ChiPD(",			// F986
"ChiCD(",			// F987
"InvChiCD(",		// F988
"FPD(",				// F989
"FCD(",				// F98A
"InvFCD(",			// F98B
"BinomialPD(",		// F98C
"BinomialCD(",		// F98D
"InvBinomialCD(",	// F98E
"PoissonPD(",		// F98F

"PoissonCD(",		// F990
"InvPoissonCD(",	// F991
"GeoPD(",			// F992
"GeoCD(",			// F993
"InvGeoCD(",		// F994
"HypergeoPD(",		// F995
"HypergeoCD(",		// F996
"InvHypergeoCD(",	// F997
"SetG-Color ",		// F998
"Plot/Line-Color ",	// F999
"AxesScale",		// F99A
"Black ",			// F99B
"@F99C",			// F99C
"Magenta ",			// F99D
"Cyan ",			// F99E
"Yellow ",			// F99F

"Smpl_SI(",			// F9A0
"Smpl_SFV(",		// F9A1
"Cmpd_n(",			// F9A2
"Cmpd_I%(",			// F9A3
"Cmpd_PV(",			// F9A4
"Cmpd_PMT(",		// F9A5
"Cmpd_FV(",			// F9A6
"Cash_NPV(",		// F9A7
"Cash_IRR(",		// F9A8
"Cash_PBP(",		// F9A9
"Cash_NFV(",		// F9AA
"Amt_BAL(",			// F9AB
"Amt_INT(",			// F9AC
"Amt_PRN(",			// F9AD
"Amt_SigmaINT(",	// F9AE
"Amt_SigmaPRN(",	// F9AF

"Cnvt_EFF(",		// F9B0
"Cnvt_APR(",		// F9B1
"Cost(",			// F9B2
"Sell(",			// F9B3
"Margin(",			// F9B4
"PmtEnd",			// F9B5
"PmtBgn",			// F9B6
"Bond_PRC(",		// F9B7
"Bond_YLD(",		// F9B8
"DateMode365",		// F9B9
"DateMode360",		// F9BA
"PeriodsAnnual",	// F9BB
"PeriodsSemi",		// F9BC
"Days_Prd(",		// F9BD
"@F9BE",			// F9BE
"@F9BF",			// F9BF

"@F9C0",			// F9C0
"@F9C1",			// F9C1
"@F9C2",			// F9C2
"@F9C3",			// F9C3
"@F9C4",			// F9C4
"@F9C5",			// F9C5
"@F9C6",			// F9C6
"@F9C7",			// F9C7
"@F9C8",			// F9C8
"@F9C9",			// F9C9
"@F9CA",			// F9CA
"@F9CB",			// F9CB
"@F9CC",			// F9CC
"@F9CD",			// F9CD
"@F9CE",			// F9CE
"@F9CF",			// F9CF

"@F9D0",			// F9D0
"@F9D1",			// F9D1
"@F9D2",			// F9D2
"@F9D3",			// F9D3
"@F9D4",			// F9D4
"@F9D5",			// F9D5
"@F9D6",			// F9D6
"@F9D7",			// F9D7
"@F9D8",			// F9D8
"@F9D9",			// F9D9
"@F9DA",			// F9DA
"@F9DB",			// F9DB
"@F9DC",			// F9DC
"@F9DD",			// F9DD
"@F9DE",			// F9DE
"@F9DF",			// F9DF

"OneSampleZTest ",	// F9E0
"TwoSampleZTest ",	// F9E1
"OnePropZTest ",	// F9E2
"TwoPropZTest ",	// F9E3
"@F9E4",			// F9E4
"",			// F9E5
"",			// F9E6
"",			// F9E7
"OneSampleTTest ",	// F9E8
"TwoSampleTTest ",	// F9E9
"LinRegTTest ",		// F9EA
"ChiGOFTest ",		// F9EB
"ChiTest ",			// F9EC
"TwoSampleFTest ",	// F9ED
"OneWayANOVA ",		// F9EE
"TwoWayANOVA ",		// F9EF

"x1InvN",			// F9F0
"x2InvN",			// F9F1
"xInv",				// F9F2
"SketchThin ",		// F9F3
"S-L-Thin",			// F9F4
"ThinG ",			// F9F5
"",			// F9F6
"",			// F9F7
"",			// F9F8
"",			// F9F9
"",			// F9FA
"zLow",				// F9FB
"zUp",				// F9FC
"tLow",				// F9FD
"tUp",				// F9FE
"",				// F9FF

};

const char ConvListE700[][13]={
"@E700",			// E700
"[ns]",				// E701
"[micros]",			// E702
"[ms]",				// E703
"[s]",				// E704
"[min]",			// E705
"[h]",				// E706
"[day]",			// E707
"[week]",			// E708
"[yr]",				// E709
"[s-yr]",			// E70A
"[t-yr]",			// E70B
"[Centigrade]",		// E70C
"[Kel]",			// E70D
"[Fahrenheit]",		// E70E
"[Rankine]",		// E70F

"[u]",				// E710
"[g]",				// E711
"[kg]",				// E712
"[lb]",				// E713
"[oz]",				// E714
"[slug]",			// E715
"[ton(short)]",		// E716
"[ton(long)]",		// E717
"@E718",			// E718
"[mton]",			// E719
"[l-atm]",			// E71A
"[ftlbf]",			// E71B
"[calIT]",			// E71C
"[calth]",			// E71D
"[Btu]",			// E71E
"[kWh]",			// E71F

"[kgfm]",			// E720
"[Pa]",				// E721
"[kPa]",			// E722
"[bar]",			// E723
"[mmH2O]",			// E724
"[mmHg]",			// E725
"[inH2O]",			// E726
"[inHg]",			// E727
"[lbf/in^2]",		// E728
"[kgf/cm^2]",		// E729
"[atm]",			// E72A
"[dyne]",			// E72B
"[New]",			// E72C
"[kgf]",			// E72D
"[lbf]",			// E72E
"[tonf]",			// E72F

"@E730",			// E730
"@E731",			// E731
"@E732",			// E732
"@E733",			// E733
"@E734",			// E734
"@E735",			// E735
"@E736",			// E736
"@E737",			// E737
"@E738",			// E738
"@E739",			// E739
"@E73A",			// E73A
"@E73B",			// E73B
"@E73C",			// E73C
"@E73D",			// E73D
"@E73E",			// E73E
"@E73F",			// E73F

"@E740",			// E740
"@E741",			// E741
"@E742",			// E742
"@E743",			// E743
"@E744",			// E744
"@E745",			// E745
"@E746",			// E746
"@E747",			// E747
"@E748",			// E748
"@E749",			// E749
"@E74A",			// E74A
"@E74B",			// E74B
"@E74C",			// E74C
"@E74D",			// E74D
"@E74E",			// E74E
"@E74F",			// E74F

"@E750",			// E750
"@E751",			// E751
"@E752",			// E752
"@E753",			// E753
"@E754",			// E754
"@E755",			// E755
"@E756",			// E756
"@E757",			// E757
"@E758",			// E758
"@E759",			// E759
"@E75A",			// E75A
"@E75B",			// E75B
"@E75C",			// E75C
"@E75D",			// E75D
"@E75E",			// E75E
"@E75F",			// E75F

"@E760",			// E760
"@E761",			// E761
"@E762",			// E762
"@E763",			// E763
"@E764",			// E764
"@E765",			// E765
"@E766",			// E766
"@E767",			// E767
"@E768",			// E768
"@E769",			// E769
"@E76A",			// E76A
"@E76B",			// E76B
"@E76C",			// E76C
"@E76D",			// E76D
"@E76E",			// E76E
"@E76F",			// E76F

"@E770",			// E770
"@E771",			// E771
"@E772",			// E772
"@E773",			// E773
"@E774",			// E774
"@E775",			// E775
"@E776",			// E776
"@E777",			// E777
"@E778",			// E778
"@E779",			// E779
"@E77A",			// E77A
"@E77B",			// E77B
"@E77C",			// E77C
"@E77D",			// E77D
"@E77E",			// E77E
"@E77F",			// E77F

"@E780",			// E780
"@E781",			// E781
"@E782",			// E782
"@E783",			// E783
"@E784",			// E784
"@E785",			// E785
"@E786",			// E786
"@E787",			// E787
"@E788",			// E788
"@E789",			// E789
"@E78A",			// E78A
"@E78B",			// E78B
"@E78C",			// E78C
"@E78D",			// E78D
"@E78E",			// E78E
"@E78F",			// E78F

"@E790",			// E790
"@E791",			// E791
"@E792",			// E792
"@E793",			// E793
"@E794",			// E794
"@E795",			// E795
"@E796",			// E796
"@E797",			// E797
"@E798",			// E798
"@E799",			// E799
"@E79A",			// E79A
"@E79B",			// E79B
"@E79C",			// E79C
"@E79D",			// E79D
"@E79E",			// E79E
"@E79F",			// E79F

"@E7A0",			// E7A0
"@E7A1",			// E7A1
"@E7A2",			// E7A2
"@E7A3",			// E7A3
"@E7A4",			// E7A4
"@E7A5",			// E7A5
"@E7A6",			// E7A6
"@E7A7",			// E7A7
"@E7A8",			// E7A8
"@E7A9",			// E7A9
"@E7AA",			// E7AA
"@E7AB",			// E7AB
"@E7AC",			// E7AC
"@E7AD",			// E7AD
"@E7AE",			// E7AE
"@E7AF",			// E7AF

"[fm]",				// E7B0
"@E7B1",			// E7B1
"[mm]",				// E7B2
"[cm]",				// E7B3
"[m]",				// E7B4
"[km]",				// E7B5
"[Mil]",			// E7B6
"[in]",				// E7B7
"[ft]",				// E7B8
"[yd]",				// E7B9
"[fath]",			// E7BA
"[rd]",				// E7BB
"@E7BC",			// E7BC
"[mile]",			// E7BD
"@E7BE",			// E7BE
"[n_mile]",			// E7BF

"[acre]",			// E7C0
"@E7C1",			// E7C1
"[ha]",				// E7C2
"[cm^2]",			// E7C3
"[m^2]",			// E7C4
"[km^2]",			// E7C5
"@E7C6",			// E7C6
"[in^2]",			// E7C7
"[ft^2]",			// E7C8
"[yd^2]",			// E7C9
"[mile^2]",			// E7CA
"[m/s]",			// E7CB
"[km/h]",			// E7CC
"[ft/s]",			// E7CD
"[mile/h]",			// E7CE
"[knot]",			// E7CF

"[mL]",				// E7D0
"[Lit]",			// E7D1
"[tsp]",			// E7D2
"[cm^3]",			// E7D3
"[m^3]",			// E7D4
"[tbsp]",			// E7D5
"@E7D6",			// E7D6
"[in^3]",			// E7D7
"[ft^3]",			// E7D8
"[fl_oz(UK)]",		// E7D9
"[fl_oz(US)]",		// E7DA
"[cup]",			// E7DB
"[pt]",				// E7DC
"[qt]",				// E7DD
"[gal(US)]",		// E7DE
"[gal(UK)]",		// E7DF

"@E7E0",			// E7E0
"@E7E1",			// E7E1
"[microm]",			// E7E2
"[mg]",				// E7E3
"[Ang]",			// E7E4
"",			// E7E5
"",			// E7E6
"",			// E7E7
"[AstU]",			// E7E8
"[l.y.]",			// E7E9
"[pc]",				// E7EA
"[ftlbf/s]",		// E7EB
"[calth/s]",		// E7EC
"[hp]",				// E7ED
"[Btu/min]",		// E7EE
"[Wat]",			// E7EF

"[eV]",				// E7F0
"[erg]",			// E7F1
"[Jou]",			// E7F2
"[cal15]",			// E7F3
"[kcal15]",			// E7F4
"[kcalth]",			// E7F5
"",			// E7F6
"",			// E7F7
"",			// E7F8
"",			// E7F9
"",			// E7FA
"[kcalIT]",			// E7FB
"@E7FC",			// E7FC
"@E7FD",			// E7FD
"@E7FE",			// E7FE
"",			// E7FF

};

//-----------------------------------------------------------------------------
typedef struct {
	short code;
	char str[16];
} tcnvopcodes;

const tcnvopcodes ConvOpCodeStrList[] = {
	{ 0x00DE, "Intg"},
	{ 0x00A6, "Int"},
	{ 0x0086, "Sqrt"},
	{ 0x0095, "log"},
	{ 0x0097, "Abs"},
	{ 0x00B6, "Frac"},
	{ 0x0085, "ln"},
	
	{ 0x00B1, "sinh^-1"},
	{ 0x00B2, "cosh^-1"},
	{ 0x00B3, "tanh^-1"},
	{ 0x00B1, "asinh"},
	{ 0x00B2, "acosh"},
	{ 0x00B3, "atanh"},
	{ 0x00A1, "sinh"},
	{ 0x00A2, "cosh"},
	{ 0x00A3, "tanh"},
	
	{ 0x0091, "sin^-1"},
	{ 0x0092, "cos^-1"},
	{ 0x0093, "tan^-1"},
	{ 0x0091, "asin"},
	{ 0x0092, "acos"},
	{ 0x0093, "atan"},
	{ 0x0081, "sin"},
	{ 0x0082, "cos"},
	{ 0x0083, "tan"},
	
	{ 0x00E0, "Plot"},
	{ 0x00ED, "Prog"},

	{ 0xF701, "Then" }, 
	{ 0xF70F, "ElseIf" }, 
	{ 0xF702, "Else" }, 
	{ 0xF700, "If" }, 

	{ 0xF7EC, "Default" }, 
	{ 0xF7EE, "Save" }, 

	{ 0, "" }
};

//-----------------------------------------------------------------------------
const char cbasicstr[]="'ProgramMode:RUN\r\n";

int OpcodeToText( char *srcbase, char *text, int maxsize ) {
	int i,cont=1;
	unsigned short code;
	int c=1;
	int ofst=0,len;
	int textofst=18;
	char *opstr;
	char flag=0;	// ' "
	
	strncpy( text, cbasicstr,18);	// header

	while ( cont ) {
	  tokenloop:
		code = GetOpcode( srcbase, ofst ) ;
		if ( code == 0 ) break;

		if ( ( code == 0x27 ) || ( code == 0x22 ) ) { // " '
			if ( flag == code ) flag=0; else flag=code;
		}

		if ( ( code != 0x21 ) && ( code != 0x2A ) && ( code != 0x2B ) && ( code != 0x2D ) && ( code != 0x2F ) && ( 0x20 <= code ) && ( code <= 0x7E ) ){
				text[textofst++] = code;
				ofst++;
		} else
		if ( ( 0xFFA0 <= code ) && ( code <= 0xFFDF ) ) {	// kana
				text[textofst++] = code & 0xFF;
				ofst+=2;
		} else
		if ( ( 0xE500 <= code ) && ( code <= 0xE6FF ) ) {	// 2byte
				if ( flag ) text[textofst++] ='_';
				text[textofst++] ='#';
				NumToHex( text+textofst, code, 4);
				textofst+=4;
				ofst+=2;
				if ( flag ) text[textofst++] ='_';
		} else {
			if ( ( 0x0001 <= code ) && ( code <= 0x002F ) ) opstr=ConvList0000[code];
			else
			if ( ( 0x0080 <= code ) && ( code <= 0x00FE ) ) opstr=ConvList0080[code-0x0080];
			else
			if ( ( 0x7F00 <= code ) && ( code <= 0x7FFF ) ) opstr=ConvList7F00[code-0x7F00];
			else
			if ( ( 0xF700 <= code ) && ( code <= 0xF7FF ) ) opstr=ConvListF700[code-0xF700];
			else
			if ( ( 0xF900 <= code ) && ( code <= 0xF9FF ) ) opstr=ConvListF900[code-0xF900];
			else
			if ( ( 0xE700 <= code ) && ( code <= 0xE7FF ) ) opstr=ConvListE700[code-0xE700];
			else {	// other code
				text[textofst++] ='@';
				NumToHex( text+textofst, code, 4);
				textofst+=4;
				ofst++; if ( code >= 0x100 ) ofst++; 
				goto tokenloop;
			}
			len = strlen( opstr );
			if ( ( flag ) && ( ( (0x20>code) && (code!=0x0D) ) || ( (code>0x7E) ) && ( (code!=0x89) && (code!=0x99) && (code!=0xA8) && (code!=0xA9) && (code!=0xB9) && (code!=0xAB) ) ) ) {
				text[textofst++] ='_';
				strncpy( text+textofst, opstr, len );
				textofst += len;
				ofst += OpcodeLen( code );
				text[textofst++] ='_';
			} else {
				strncpy( text+textofst, opstr, len );
				textofst += len;
				ofst += OpcodeLen( code );
			}
		}
		
		if ( code == 0x0D ) { //
			flag=0;
		}
		if ( code == 0x0C ) { //
			flag=0;
			if ( GetOpcode( srcbase, ofst ) != 0x0D ) {;
				text[textofst++] ='\r';
				text[textofst++] ='\n';
			}
		}
		if ( textofst > maxsize-16 ) return -1; // text buffer overflow
	}
	text[textofst] ='\0';
	return textofst;	//ok
}


//-----------------------------------------------------------------------------
int IsHex( char c ){
	if ( ( '0'<=c ) && ( c<='9' ) ) return c-'0';
	else
	if ( ( 'A'<=c ) && ( c<='F' ) ) return c-'A'+10;
	else
	if ( ( 'a'<=c ) && ( c<='f' ) ) return c-'a'+10;
	else return -1;
}

int str2hex4( char *buffer ,int *ofst) {
//	char hex[16]="0123456789AB";		// only align use
	int a,b,c,d;
	a=buffer[(*ofst)];
	a=IsHex(a);
	if ( a<0 ) return 0;
	b=buffer[(*ofst)+1];
	b=IsHex(b);
	if ( b<0 ) return 0;
	c=buffer[(*ofst)+2];
	c=IsHex(c);
	if ( c<0 ) return 0;
	d=buffer[(*ofst)+3];
	d=IsHex(d);
	if ( d<0 ) return 0;
	(*ofst)+=4;
	return a*4096+b*256+c*16+d;
}

int codecnv7F00( char *srcbase, char *text, int *ofst, int *textofst ) {
	char *opstr;
	unsigned short code;
	int len;
	int c=text[(*textofst)];
	for ( code=0x0000; code<=0x00FF; code++) {		// 0x7F00 - 0x7FFF
		opstr=ConvList7F00[code];
		if ( c == opstr[0] ) {
			len = strlen( opstr );
			if ( strncmp( text+(*textofst), opstr, len ) == 0 )  {
				srcbase[(*ofst)++] = 0x7F;
				srcbase[(*ofst)++] = code ;
				(*textofst) += len;
				return 0;	 	// matching
			}
		}
	}
	return c;	// no matching
}
int codecnvF700( char *srcbase, char *text, int *ofst, int *textofst ) {
	char *opstr;
	int code;
	int len;
	int c=text[(*textofst)];
	for ( code=0x00FF; code>=0x0000; code--) {		// 0xF700 - 0xF7FF
		opstr=ConvListF700[code];
		if ( c == opstr[0] ) {
			len = strlen( opstr );
			if ( strncmp( text+(*textofst), opstr, len ) == 0 )  {
				srcbase[(*ofst)++] = 0xF7;
				srcbase[(*ofst)++] = code ;
				(*textofst) += len;
				return 0;	 	// matching
			}
		}
	}
	return c;	// no matching
}
int codecnvF900( char *srcbase, char *text, int *ofst, int *textofst ) {
	char *opstr;
	unsigned short code;
	int len;
	int c=text[(*textofst)];
	for ( code=0x0000; code<=0x00FF; code++) {		// 0xF900 - 0xF9FF
		opstr=ConvListF900[code];
		if ( c == opstr[0] ) {
			len = strlen( opstr );
			if ( strncmp( text+(*textofst), opstr, len ) == 0 )  {
				srcbase[(*ofst)++] = 0xF9;
				srcbase[(*ofst)++] = code ;
				(*textofst) += len;
				return 0;	 	// matching
			}
		}
	}
	return c;	// no matching
}
int codecnvE700( char *srcbase, char *text, int *ofst, int *textofst ) {
	char *opstr;
	unsigned short code;
	int len;
	int c=text[(*textofst)];
	for ( code=0x0000; code<=0x00FF; code++) {		// 0xF900 - 0xF9FF
		opstr=ConvListE700[code];
		if ( c == opstr[0] ) {
			len = strlen( opstr );
			if ( strncmp( text+(*textofst), opstr, len ) == 0 )  {
				srcbase[(*ofst)++] = 0xE7;
				srcbase[(*ofst)++] = code ;
				(*textofst) += len;
				return 0;	 	// matching
			}
		}
	}
	return c;	// no matching
}
int codecnv0000( char *srcbase, char *text, int *ofst, int *textofst ) {
	char *opstr;
	unsigned short code;
	int len;
	int c=text[(*textofst)];
	for ( code=0x0001; code<=0x002F; code++) {		// 0x0001 - 0x002F
		opstr=ConvList0000[code];
		if ( c == opstr[0] ) {
			len = strlen( opstr );
			if ( strncmp( text+(*textofst), opstr, len ) == 0 )  {
				srcbase[(*ofst)++] = code ;
				(*textofst) += len;
				return 0;	 	// matching
			}
		}
	}
	return c;	// no matching
}
int codecnv0080( char *srcbase, char *text, int *ofst, int *textofst ) {
	char *opstr;
	unsigned short code;
	int len;
	int c=text[(*textofst)];
	for ( code=0x0000; code<=0x007E; code++) {		// 0x0080 - 0x00FE
		opstr=ConvList0080[code];
		if ( c == opstr[0] ) {
			len = strlen( opstr );
			if ( strncmp( text+(*textofst), opstr, len ) == 0 )  {
				srcbase[(*ofst)++] = code +0x0080;
				(*textofst) += len;
				return 0;	 	// matching
			}
		}
	}
	return c;	// no matching
}


int ex_codecnv( char *srcbase, char *text, int *ofst, int *textofst ) {
	char *opstr;
	unsigned short code;
	int i,len;
	int c=text[(*textofst)];
	i=0;
	do {
		opstr=ConvOpCodeStrList[i].str;
		code = ConvOpCodeStrList[i].code ;
		if ( code == 0 ) break;
		i++;
		len = strlen( opstr );
		if ( strncmp( text+(*textofst), opstr, len ) == 0 )  {
			if ( code > 0x100 ) {
				srcbase[(*ofst)++] = code >> 8;
			}
			srcbase[(*ofst)++] = code ;
			(*textofst) += len;
			return 0;	 	// matching
		}
	} while ( code ) ;
	return c;	// no matching
}


int TextToOpcode( char *filebase, char *text, int maxsize ) {
	char *srcbase;
	int i,cont=1;
	unsigned short opcode,code;
	int c=1,d,e;
	int ofst=0,len;
	int textofst=0;
	char *opstr;
	char flag=0;	// ' "
	char flag_=0;

	srcbase=filebase+0x56;
	if ( strncmp( text, cbasicstr, 18 ) == 0 ) textofst+=18;
	
	while ( cont ) {
	  tokenloop:
		if ( ofst > maxsize )  break;
		c = text[textofst];
		if ( ( flag_ ) && ( c=='_' ) ) c=text[++textofst];	// '_' skip
		flag_=0; 
		if ( c==0 ) break;
		if ( ( c == 0x27 ) || ( c == 0x22 ) ) { // " '
			if ( flag == c ) flag=0; else flag=c;
		}
		if ( flag ) {
			if ( c!='_' ) {
				if ( ( c!=0x0D ) && ( c!='+' ) && ( c!='-' ) && ( c!='*' ) && ( c!='/' ) && ( c!='!' ) && ( c!='^' ) ) goto tokenskip;
			} else {
				textofst++;	// '_'  skip
				flag_=c; 
			}
		}
		c=codecnv7F00( srcbase, text, &ofst, &textofst ) ;	// 0x7F00 - 0x7FFF
		if ( c==0 ) goto tokenloop;

		c=codecnvF900( srcbase, text, &ofst, &textofst ) ;	// 0xF900 - 0xF9FF
		if ( c==0 ) goto tokenloop;

		c=codecnvF700( srcbase, text, &ofst, &textofst ) ;	// 0xF700 - 0xF7FF
		if ( c==0 ) goto tokenloop;

		if ( ( c=='[' ) || ( c=='@' ) ) {
			c=codecnvE700( srcbase, text, &ofst, &textofst ) ;	// 0xE700 - 0xE7FF
			if ( c==0 ) goto tokenloop;
		}
		
		if ( c == 0x27 ) {	// '
				srcbase[ofst++] = c;
				textofst++;
				c = text[textofst];
				if ( c == '/' ) {
					srcbase[ofst++] = c;
					textofst++;
				}
				goto tokenloop;
		} else
		if ( c == '#' ) {
				textofst++;
				d=str2hex4( text, &textofst );
				if ( (0xE500<=d) && (d<=0xE7FF) ) {
					srcbase[ofst++] = d >> 8;
					srcbase[ofst++] = d & 0xFF;
				} else {
					srcbase[ofst++] = c;
				}
				goto tokenloop;
		} else
		if ( c != '(' ) {
			c=codecnv0000( srcbase, text, &ofst, &textofst ) ;	// 0x0001 - 0x002F
			if ( c==0 ) {
				c=srcbase[ofst-1];
				if ( ( c == 0x0C ) || ( c == 0x0D ) ) flag=0;
				goto tokenloop;
			}
		}
		c=codecnv0080( srcbase, text, &ofst, &textofst ) ;	// 0x0080 - 0x00FE
		if ( c==0 ) goto tokenloop;
		
		c=ex_codecnv( srcbase, text, &ofst, &textofst ) ;	// ext
		if ( c==0 ) goto tokenloop;

	  tokenskip:
		if ( ( 0x20 <= c ) && ( c <= 0x7E ) ) {
			if ( c == 'C' ) { 
				if ( ( text[textofst+1] == 'h' ) && ( text[textofst+2] == 'a' ) && ( text[textofst+3] == 'r' ) ) {
					c=text[textofst+4];
					if ( ( c == '!' ) || ( c == '-') ) {
						srcbase[ofst++] = c;
						textofst+=5;
					}
				} else {
					srcbase[ofst++] = c;
					textofst++;
				}
			} else {
				srcbase[ofst++] = c;
				textofst++;
			}
		} else 
		if ( ( 0xFFFFFFA0 <= c ) && ( c <= 0xFFFFFFDF ) ) {	// kana
				srcbase[ofst++] = 0xFF;
				srcbase[ofst++] = c;
				textofst++;
		} else {	// no token
				srcbase[ofst++] = '*';
				textofst++;
		}
	}
	srcbase[ofst] = 0;
	SetSrcSize( filebase, SrcSize(filebase)+ofst ) ; 	// set new file size
	return ofst;
}

