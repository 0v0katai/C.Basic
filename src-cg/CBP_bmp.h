#define WORD	short
#define DWORD	int
#define LONG	int

typedef struct tagBITMAPFILEHEADER { 
  WORD    bfType; 
  DWORD   bfSize; 
  WORD    bfReserved1; 
  WORD    bfReserved2; 
  DWORD   bfOffBits; 
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPCOREHEADER {
  DWORD   bcSize; 
  WORD    bcWidth; 
  WORD    bcHeight; 
  WORD    bcPlanes; 
  WORD    bcBitCount; 
} BITMAPCOREHEADER, *PBITMAPCOREHEADER;

typedef struct tagBITMAPINFOHEADER{
  DWORD  biSize; 
  LONG   biWidth; 
  LONG   biHeight; 
  WORD   biPlanes; 
  WORD   biBitCount; 
  DWORD  biCompression; 
  DWORD  biSizeImage; 
  LONG   biXPelsPerMeter; 
  LONG   biYPelsPerMeter; 
  DWORD  biClrUsed; 
  DWORD  biClrImportant; 
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagRGBTRIPLE { 
  BYTE rgbtBlue; 
  BYTE rgbtGreen; 
  BYTE rgbtRed; 
} RGBTRIPLE;

typedef struct tagRGBQUAD {
  BYTE    rgbBlue; 
  BYTE    rgbGreen; 
  BYTE    rgbRed; 
  BYTE    rgbReserved; 
} RGBQUAD;

