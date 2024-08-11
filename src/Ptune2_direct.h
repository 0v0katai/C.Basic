/*
===============================================================================

 Ptune2 is SH7305 CPG&BSC tuning utility for PRIZM fx-CG10/20  v1.10

 copyright(c)2014,2015,2016 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/

//---------------------------------------------------------------------------------------------
#define FLF_default  900
#define FLFMAX      2048-1

#define PLL_64x 63		//0b111111 
#define PLL_36x 35		//0b100011 
#define PLL_32x 31		//0b011111 
#define PLL_26x 25		//0b011001 
#define PLL_16x 15		//0b001111 
#define PLLMAX  64-1

#define DIV_2 0 		//0b0000	// 1/2
#define DIV_4 1 		//0b0001	// 1/4
#define DIV_8 2 		//0b0010	// 1/8
#define DIV16 3 		//0b0011	// 1/16
#define DIV32 4 		//0b0100	// 1/32
#define DIV64 5 		//0b0101	// 1/64

#define WAIT_0 0 		//0b0000
#define WAIT_1 1 		//0b0001
#define WAIT_2 2 		//0b0010	// cs2wcr default 
#define WAIT_3 3 		//0b0011
#define WAIT_4 4 		//0b0100
#define WAIT_5 5 		//0b0101
#define WAIT_6 6 		//0b0110
#define WAIT_8 7 		//0b0111
#define WAIT10 8 		//0b1000
#define WAIT12 9 		//0b1001
#define WAIT14 10		//0b1010
#define WAIT18 11		//0b1011	// cs0wcr default 
#define WAIT24 12		//0b1100

#define WAITW0 1		//0b0001
#define WAITW1 2		//0b0010
#define WAITW2 3		//0b0011
#define WAITW3 4		//0b0100
#define WAITW4 5		//0b0101
#define WAITW5 6		//0b0110
#define WAITW6 7		//0b0111

#define FLLFRQ_default	0x00004384
#define FRQCRA_default	0x0F102203

#define CS0BCR_default	0x24920400
#define CS2BCR_default	0x24923400
#define CS3BCR_default  0x24924400
#define CS4BCR_default  0x36DB0400
#define CS5aBCR_default 0x15140400
#define CS5bBCR_default 0x15140400
#define CS6aBCR_default 0x34D30200
#define CS6bBCR_default 0x34D30200

#define CS0WCR_default	0x000001C0
#define CS2WCR_default	0x00000140
#define CS3WCR_default  0x000024D0
#define CS4WCR_default  0x00000540
#define CS5aWCR_default 0x00010240
#define CS5bWCR_default 0x00010240
#define CS6aWCR_default 0x000302C0
#define CS6bWCR_default 0x000302C0

//---------------------------------------------------------------------------------------------
void LoadDataF0();	//  default    -> CPG Register
void LoadDataF1();	//  backup     -> CPG Register
void LoadDataF2();	//  F2 setting -> CPG Register
void LoadDataF3();	//  F3 setting -> CPG Register
void LoadDataF4();	//  F4 setting -> CPG Register
void LoadDataF5();	//  F5 setting -> CPG Register

void SaveDataF1();	// CPG Register -> F1 setting ( backup )
void SaveDataF2();	// CPG Register -> F2 setting
void SaveDataF3();	// CPG Register -> F3 setting
void SaveDataF4();	// CPG Register -> F4 setting
void SaveDataF5();	// CPG Register -> F5 setting
