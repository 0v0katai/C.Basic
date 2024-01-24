//
//
struct st_RTC {             // struct RTC      similar to SH7705
    unsigned char R64CNT;	// C0
    unsigned char t1;
    unsigned char RSECCNT;	// C2
    unsigned char t2;
    unsigned char RMINCNT;	// C4
    unsigned char t3;
    unsigned char RHRCNT;	// C6
    unsigned char t4;
    unsigned char RWKCNT;	// C8
    unsigned char t5;
    unsigned char RDAYCNT;	// CA
    unsigned char t6;
    unsigned char RMONCNT;	// CC
    unsigned char t7;
    unsigned char RYRCONT;	// CE
    unsigned char t8;
    unsigned char RSECAR;	// D0
    unsigned char t9;
    unsigned char RMINAR;	// D2
    unsigned char t10;
    unsigned char RHRCAR;	// D4
    unsigned char t11;
    unsigned char RWKCAR;	// D6
    unsigned char t12;
    unsigned char RDAYAR;	// D8
    unsigned char t13;
    unsigned char RMONAR;	// DA
    unsigned char t14;
    union {                           // struct RCR1   similar to SH7705
        unsigned char BYTE;           // BYTE Access
        struct {                      // Bit  Access
            unsigned long CF  :1;    
            unsigned long     :1;    
            unsigned long     :1;    
            unsigned long CIE :1;    
            unsigned long AIE :1;    
            unsigned long     :1;    
            unsigned long     :1;    
            unsigned long AF  :1;    
            }   BIT;
        }   RCR1;
    unsigned char t15;
	union {                           // struct RCR2   similar to SH7705
        unsigned char BYTE;           // BYTE Access
        struct {                      // Bit  Access
            unsigned long REF   :1;    
            unsigned long RES   :3;    
            unsigned long RTCEN :1;    
            unsigned long ADJ   :1;    
            unsigned long RESET :1;    
            unsigned long START :1;    
            }   BIT;
        }   RCR2;
	unsigned char t16;
   	unsigned short RYRAR;		// E0
   	unsigned short t17;
   	union {                           // struct RCR3   similar to SH7705
        unsigned char BYTE;           // BYTE Access
        struct {                      // Bit  Access
            unsigned long YAEN  :1;    
            unsigned long       :7;    
            }   BIT;
        }   RCR3;
}  ;
                                    //
                                    
#define RTC3 (*(volatile struct st_RTC   *)0xFFFFFEC0)    // RTC Address SH3
#define RTC4 (*(volatile struct st_RTC   *)0xA413FEC0)    // RTC Address SH4A

//--------------------------------------------------------------------------


void SetRtc( int data ) ;
