/* dispbios.h */
#define IM_VRAM_WIDTH               128
#define IM_VRAM_HEIGHT              64

#define IM_VRAM_SIZE                1024

#define IM_CHARACTERS_MAX_LINE      21
#define IM_BYTES_MAX_LINE           (IM_CHARACTERS_MAX_LINE*2)

#define SAVEDISP_PAGE1              1
#define SAVEDISP_PAGE2              5
#define SAVEDISP_PAGE3              6

#define MINI_OVER                   0x10
#define MINI_OR                     0x11
#define MINI_REV                    0x12
#define MINI_REVOR                  0x13

#define IM_BIOS_DD_WIDTH            IM_VRAM_WIDTH
#define IM_BIOS_DD_HEIGHT           IM_VRAM_HEIGHT

#define WRITEKIND                   unsigned char
#define IMB_WRITEKIND_OVER          0x01
#define IMB_WRITEKIND_OR            0x02
#define IMB_WRITEKIND_AND           0x03
#define IMB_WRITEKIND_XOR           0x04

#define WRITEMODIFY                 unsigned char
#define IMB_WRITEMODIFY_NORMAL      0x01
#define IMB_WRITEMODIFY_REVERCE     0x02
#define IMB_WRITEMODIFY_MESH        0x03

#define AREAKIND                    unsigned char
#define IMB_AREAKIND_OVER           0x01
#define IMB_AREAKIND_MESH           0x02
#define IMB_AREAKIND_CLR            0x03
#define IMB_AREAKIND_REVERSE        0x04

#define EFFECTWIN                   unsigned char
#define IMB_EFFECTWIN_OK            0x01
#define IMB_EFFECTWIN_NG            0x02

typedef struct tag_DISPBOX {
    int     left;
    int     top;
    int     right;
    int     bottom;
} DISPBOX;

typedef struct tag_GRAPHDATA {
    int             width;
    int             height;
    unsigned char   *pBitmap;
} GRAPHDATA;

typedef struct tag_RECTANGLE {
    DISPBOX         LineArea;
    AREAKIND        AreaKind;
    EFFECTWIN       EffectWin;
} RECTANGLE;

typedef struct tag_DISPGRAPH {
    int             x;
    int             y;
    GRAPHDATA       GraphData;
    WRITEMODIFY     WriteModify;
    WRITEKIND       WriteKind;
} DISPGRAPH;

/* filebios.h */
#define FONTCHARACTER               unsigned short

#define _OPENMODE_READ              0x01
#define _OPENMODE_READ_SHARE        0x80
#define _OPENMODE_WRITE             0x02
#define _OPENMODE_READWRITE         0x03
#define _OPENMODE_READWRITE_SHARE   0x83

#define _CREATEMODE_BINARY          1
#define _CREATEMODE_DIRECTORY       5

typedef enum tag_DEVICE_TYPE {
    DEVICE_MAIN_MEMORY,
    DEVICE_STORAGE,
    DEVICE_SD_CARD,                 // only fx-9860G SD model
} DEVICE_TYPE;


// File system standard error code
#define IML_FILEERR_NOERROR             0
#define IML_FILEERR_ENTRYNOTFOUND       -1
#define IML_FILEERR_ILLEGALPARAM        -2
#define IML_FILEERR_ILLEGALPATH         -3
#define IML_FILEERR_DEVICEFULL          -4
#define IML_FILEERR_ILLEGALDEVICE       -5
#define IML_FILEERR_ILLEGALFILESYS      -6
#define IML_FILEERR_ILLEGALSYSTEM       -7
#define IML_FILEERR_ACCESSDENYED        -8
#define IML_FILEERR_ALREADYLOCKED       -9
#define IML_FILEERR_ILLEGALTASKID       -10
#define IML_FILEERR_PERMISSIONERROR     -11
#define IML_FILEERR_ENTRYFULL           -12
#define IML_FILEERR_ALREADYEXISTENTRY   -13
#define IML_FILEERR_READONLYFILE        -14
#define IML_FILEERR_ILLEGALFILTER       -15
#define IML_FILEERR_ENUMRATEEND         -16
#define IML_FILEERR_DEVICECHANGED       -17
//#define IML_FILEERR_NOTRECORDFILE     -18     // Not used
#define IML_FILEERR_ILLEGALSEEKPOS      -19
#define IML_FILEERR_ILLEGALBLOCKFILE    -20
//#define IML_FILEERR_DEVICENOTEXIST    -21     // Not used
//#define IML_FILEERR_ENDOFFILE         -22     // Not used
#define IML_FILEERR_NOTMOUNTDEVICE      -23
#define IML_FILEERR_NOTUNMOUNTDEVICE    -24
#define IML_FILEERR_CANNOTLOCKSYSTEM    -25
#define IML_FILEERR_RECORDNOTFOUND      -26
//#define IML_FILEERR_NOTDUALRECORDFILE -27     // Not used
#define IML_FILEERR_NOTALARMSUPPORT     -28
#define IML_FILEERR_CANNOTADDALARM      -29
#define IML_FILEERR_FILEFINDUSED        -30
#define IML_FILEERR_DEVICEERROR         -31
#define IML_FILEERR_SYSTEMNOTLOCKED     -32
#define IML_FILEERR_DEVICENOTFOUND      -33
#define IML_FILEERR_FILETYPEMISMATCH    -34
#define IML_FILEERR_NOTEMPTY            -35
#define IML_FILEERR_BROKENSYSTEMDATA    -36
#define IML_FILEERR_MEDIANOTREADY       -37
#define IML_FILEERR_TOOMANYALARMITEM    -38
#define IML_FILEERR_SAMEALARMEXIST      -39
#define IML_FILEERR_ACCESSSWAPAREA      -40
#define IML_FILEERR_MULTIMEDIACARD      -41
#define IML_FILEERR_COPYPROTECTION      -42
#define IML_FILEERR_ILLEGALFILEDATA     -43

// FILE_INFO.type
#define DT_DIRECTORY            0x0000      // Directory
#define DT_FILE                 0x0001      // File
#define DT_ADDIN_APP            0x0002      // Add-In application
#define DT_EACT                 0x0003      // eActivity
#define DT_LANGUAGE             0x0004      // Language
#define DT_BITMAP               0x0005      // Bitmap
#define DT_MAINMEM              0x0006      // Main Memory data
#define DT_TEMP                 0x0007      // Temporary data
#define DT_DOT                  0x0008      // .  (Current directory)
#define DT_DOTDOT               0x0009      // .. (Parent directory)
#define DT_VOLUME               0x000A      // Volume label

// typedef struct tag_FILE_INFO
// {
//     unsigned short  id;
//     unsigned short  type;
//     unsigned long   fsize;                  // File size
//     unsigned long   dsize;                  // Data size
//     unsigned int    property;               // The file has not been completed, except when property is 0.
//     unsigned long   address;
// } FILE_INFO;

/* math.h */
#ifndef ERANGE
#define ERANGE  1100
#endif

#ifndef EDOM
#define EDOM    1101
#endif

#ifndef ENUM
#define ENUM    1208
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern volatile int _errno;
extern const double _HUGE_VAL;
#define HUGE_VAL  _HUGE_VAL

extern double frexp(double, int *);
extern double ldexp(double, int );
extern double modf(double, double *);
extern double ceil(double);

#ifdef _SH4
#ifdef _FPS
#define  fabs  _builtin_fabsf
#else
#define  fabs  _builtin_fabs
#endif
#else
#if defined(_SH2E)|defined(_SH3E)
#ifdef _FLT
#define  fabs  _builtin_fabsf
#else
extern double fabs(double);
#endif
#else
extern double fabs(double);
#endif
#endif

extern double floor(double);
extern double fmod(double, double);

extern double acos(double);
extern double asin(double);
extern double atan(double);
extern double atan2(double,double);
extern double cos(double);
extern double sin(double);
extern double tan(double);
extern double cosh(double);
extern double tanh(double);
extern double sinh(double);
extern double exp(double);
extern double log(double);
extern double log10(double);
extern double pow(double,double);

#ifdef _COMPLEX_
extern double sqrt(double);
#else
#ifdef _SH4
#ifdef _FPS
#define  sqrt  _builtin_sqrtf
#else
#define  sqrt  _builtin_sqrt
#endif
#else
#ifdef _SH3E
#ifdef _FLT
#define  sqrt  _builtin_sqrtf
#else
extern double sqrt(double);
#endif
#else
extern double sqrt(double);
#endif
#endif
#endif

#ifdef __cplusplus
}
#endif