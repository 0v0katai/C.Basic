/*
*SHDSP Library
*Copyright (c) Hitachi,Ltd. 1998
*Licensed material of Hitachi,Ltd
*/
/***********************************************************
* File:     ensigdsp.h
* Purpose:  Common
* Create:   1998.03.20 Rev. 1.0
***********************************************************/

/*
** Copyright Ensigma Ltd. 1995
**
** Unauthorised copying or distribution of this software is prohibited.
**
** Version 1.0.
**
*/

/*
** ENSIGMA DSP Library
*/

/* Constants */
#ifndef _ENSIGDSP
#define _ENSIGDSP

#define BIQUAD_SIZE     6       /* no. coefficients in an IIR biquad */

#define EDSP_OK         0
#define EDSP_BAD_ARG    1
#define EDSP_NO_HEAP    2

#define EFFTALLSCALE    (-1L)
#define EFFTMIDSCALE    0x55555555L
#define EFFTNOSCALE     0x00000000L

#ifdef __cplusplus            /*A-10-014-01*/
extern"C"{                    /*A-10-014-01*/
#endif                        /*A-10-014-01*/

/*
** Fourier transforms
*/
__attribute__((renesas)) int FftComplex(short op_x[],
           short op_y[],
           const short ip_x[],
           const short ip_y[],
           long size,
           long scale);
__attribute__((renesas)) int FftInComplex(short data_x[],
         short data_y[],
         long size,
         long scale);
__attribute__((renesas)) int FftReal(short op_x[],
        short op_y[],
        const short ip[],
        long size,
        long scale);
       /* int ip_is_x);*/
__attribute__((renesas)) int FftInReal(short data_x[],
          short data_y[],
          long size,
          long scale,
          int not_sure_about_ip_format);
__attribute__((renesas)) int IfftComplex(short op_x[],
        short op_y[],
        const short ip_x[],
        const short ip_y[],
        long size,
        long scale);
__attribute__((renesas)) int IfftInComplex(short data_x[],
          short data_y[],
          long size,
          long scale);
__attribute__((renesas)) int IfftReal(short op_x[],
         short op_y[],
         const short ip_x[],
         const short ip_y[],
         long size,
         long scale,
         int op_is_x_scratch_is_y);
__attribute__((renesas)) int IfftInReal(short data_x[],
           short data_y[],
           long size,
           long scale,
           int not_sure_about_op_format);
__attribute__((renesas)) int InitFft(long max_size);
__attribute__((renesas)) void FreeFft(void);          /*A-10-015-01*/
__attribute__((renesas)) int LogMagnitude(short output[],
         const short ip_x[],
         const short ip_y[],
         long no_elements,
         float fscale);

/*
** These three variables are for the internal use of the library FFT
** functions only. They should not be accessed by user programs.
*/
extern long max_fft_size;
extern short *twtble;

/*
** Window functions
*/
__attribute__((renesas)) int GenBlackman(short data[], long N);
__attribute__((renesas)) int GenHamming(short data[], long N);
__attribute__((renesas)) int GenHanning(short data[], long N);
__attribute__((renesas)) int GenTriangle(short data[], long N);

/*
** Filters. The coefficients must be supplied in X and the workspace in Y.
** If it is straightforward we can support the opposite, ie Xworkspace and
** Ycoeff.
*/
__attribute__((renesas)) int FreeFir(short **Yworkspace, long order);
__attribute__((renesas)) int InitFir(short **Yworkspace, long order);
__attribute__((renesas)) int InitIir(short **Yworkspace, long no_sections);
__attribute__((renesas)) int FreeIir(short **Yworkspace, long no_sections);  /*A-10-015-01*/
__attribute__((renesas)) int InitDIir(long **Yworkspace, long no_sections);
__attribute__((renesas)) int FreeDIir(long **Yworkspace, long no_sections);  /*A-10-015-01*/
__attribute__((renesas)) int InitLms(short **Yworkspace, long order);
__attribute__((renesas)) int FreeLms(short **Yworkspace, long order);        /*A-10-015-01*/
__attribute__((renesas)) int Fir(short op[],
    const short ip[],
    long no_samples,
    const short Xcoeff[],
    long order,
    int res_shift,
    short *Yworkspace);
__attribute__((renesas)) int Fir1(short *op_ptr,
     short input,
     const short Xcoeff[],
     long order,
     int res_shift,
     short *Yworkspace);
__attribute__((renesas)) int Iir(short op[],
    const short ip[],
    long no_samples,
    const short Xcoeff[],
    long no_sections,
    short *Yworkspace);
__attribute__((renesas)) int Iir1(short *op_ptr,
     short input,
     const short Xcoeff[],
     long no_sections,
     short *Yworkspace);
__attribute__((renesas)) int DIir(short op[],
     const short ip[],
     long no_samples,
     const long Xcoeff[],
     long no_sections,
     long *Yworkspace);
__attribute__((renesas)) int DIir1(short *op_ptr,
      const short input, /*98.01.09        C-10-005-01 */ 
/*    short input,    98.01.09             D-10-005-01 */
      const long Xcoeff[],
      long no_sections,
      long *Yworkspace);
__attribute__((renesas)) int Lms(short op[],
    const short ip[],
    const short ref[],
    long no_samples,
    short Xcoeff[],
    long order,
    int res_shift,
    short mu,
    short *Yworkspace);
__attribute__((renesas)) int Lms1(short *op_ptr,
     short input,
     short ref_op,
     short Xcoeff[],
     long order,
     int res_shift,
     short mu,
     short *Yworkspace);

/*
** Convolution and Correlation
*/
__attribute__((renesas)) int ConvComplete(short op[],
         const short ix[],
         const short iy[],
         long ix_size,
         long iy_size,
         int res_shift);
__attribute__((renesas)) int ConvCyclic(short op[],
           const short ix[],
           const short iy[],
           long size,
           int res_shift);
__attribute__((renesas)) int ConvPartial(short op[],
        const short ix[],
        const short iy[],
        long ix_size,
        long iy_size,
        int res_shift);
__attribute__((renesas)) int Correlate(short op[],
          const short ix[],
          const short iy[],
          long ix_size,
          long iy_size,
          long no_corr,
          int x_is_larger,
          int res_shift);
__attribute__((renesas)) int CorrCyclic(short op[],
           const short ix[],
           const short iy[],
           long size,
           int reverse,
           int res_shift);


/*
** Miscellaneous
*/
__attribute__((renesas)) int GenGWnoise(short output[], long no_samples, float variance);

__attribute__((renesas)) int MinI(short **min_ptr, short buffer[], long buff_len, int src_is_x);
__attribute__((renesas)) int MaxI(short **max_ptr, short buffer[], long buff_len, int src_is_x);
__attribute__((renesas)) int PeakI(short **peak_ptr, short buffer[], long buff_len, int src_is_x);

__attribute__((renesas)) int Mean(short *mean_ptr, const short buffer[], long buff_len, int src_is_x);
__attribute__((renesas)) int Variance(long *variance_ptr,
         short *mean_ptr,
         const short buffer[],
         long buff_len, int src_is_x);

__attribute__((renesas)) int VectorMult(short dest[],
           const short matrixX[],
           const short matrixY[],
           long length,
           int res_shift);
__attribute__((renesas)) int MatrixMult(void *matrix3,
           const void *matrixX,
           const void *matrixY,
           long m,
           long n,
           long p,
           int res_shift, int dest_is_x);
__attribute__((renesas)) int MsPower(long *pow_ptr, const short input[], long buff_size, int src_is_x);

__attribute__((renesas)) int CopyXtoY(short y[], const short x[], long n);
__attribute__((renesas)) int CopyYtoX(short x[], const short y[], long n);
__attribute__((renesas)) int CopyToX(short x[], const short src[], long n);
__attribute__((renesas)) int CopyToY(short y[], const short src[], long n);
__attribute__((renesas)) int CopyFromX(short src[], const short x[], long n);
__attribute__((renesas)) int CopyFromY(short src[], const short y[], long n);

__attribute__((renesas)) int Limit(short data[], long no_elements, int data_is_x);

#ifdef __cplusplus           /*A-10-014-01*/
}                            /*A-10-014-01*/
#endif                       /*A-10-014-01*/

#endif
