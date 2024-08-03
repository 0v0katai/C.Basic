//
struct st_rtc {             // struct RTC      similar to SH7705
    unsigned char R64CNT;
    unsigned char t1;
    unsigned char RSECONT;
    unsigned char t2;
    unsigned char RMIMCONT;
    unsigned char t3;
    unsigned char RHRCONT;
    unsigned char t4;
    unsigned char RWKCONT;
    unsigned char t5;
    unsigned char RDAYCONT;
    unsigned char t6;
    unsigned char RMONCONT;
    unsigned char t7;
    unsigned char RYRCONT;
    unsigned char t8;
    unsigned char RSECAR;
    unsigned char t9;
    unsigned char RMINAR;
    unsigned char t10;
    unsigned char RHRCAR;
    unsigned char t11;
    unsigned char RWKCAR;
    unsigned char t12;
    unsigned char RDAYAR;
    unsigned char t13;
    unsigned char RMONAR;
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
    unsigned char t16;
	union {                           // struct RCR1   similar to SH7705
        unsigned char BYTE;           // BYTE Access
        struct {                      // Bit  Access
            unsigned long REF   :1;    
            unsigned long RES2  :1;    
            unsigned long RES1  :1;    
            unsigned long RES0  :1;    
            unsigned long RTCEN :1;    
            unsigned long ADJ   :1;    
            unsigned long RESET :1;    
            unsigned long START :1;    
            }   BIT;
        }   RCR2;
	unsigned char t17;
   	unsigned short RYRAR;
   	unsigned short t18;
   	union {                           // struct RCR1   similar to SH7705
        unsigned char BYTE;           // BYTE Access
        struct {                      // Bit  Access
            unsigned long YAEN  :1;    
            unsigned long       :7;    
            }   BIT;
        }   RCR3;
};
                                    //              
#define RTCSH3 (*(volatile struct st_rtc   *)0xFFFFFEC0)    // RTC Address
#define RTCSH4 (*(volatile struct st_rtc   *)0xA413FEC0)    // RTC Address

//--------------------------------------------------------------------------

 
