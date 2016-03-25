#define STBI_NO_STDIO
#define STBI_NO_WRITE
#define STBI_NO_HDR
/* stb_image - v2.05 - public domain image loader - http://nothings.org/stb_image.h
                                     no warranty implied; use at your own risk

   Do this:
      #define STB_IMAGE_IMPLEMENTATION
   before you include this file in *one* C or C++ file to create the implementation.

   // i.e. it should look like this:
   #include ...
   #include ...
   #include ...
   #define STB_IMAGE_IMPLEMENTATION
   #include "stb_image.h"

   You can #define STBI_ASSERT(x) before the #include to avoid using assert.h.
   And #define STBI_MALLOC, STBI_REALLOC, and STBI_FREE to avoid using malloc,realloc,free


   QUICK NOTES:
      Primarily of interest to game developers and other people who can
          avoid problematic images and only need the trivial interface

      JPEG baseline & progressive (12 bpc/arithmetic not supported, same as stock IJG lib)
      PNG 1/2/4/8-bit-per-channel (16 bpc not supported)

      TGA (not sure what subset, if a subset)
      BMP non-1bpp, non-RLE
      PSD (composited view only, no extra channels)

      GIF (*comp always reports as 4-channel)
      HDR (radiance rgbE format)
      PIC (Softimage PIC)
      PNM (PPM and PGM binary only)

      - decode from memory or through FILE (define STBI_NO_STDIO to remove code)
      - decode from arbitrary I/O callbacks
      - SIMD acceleration on x86/x64 (SSE2) and ARM (NEON)

   Full documentation under "DOCUMENTATION" below.


   Revision 2.00 release notes:

      - Progressive JPEG is now supported.

      - PPM and PGM binary formats are now supported, thanks to Ken Miller.

      - x86 platforms now make use of SSE2 SIMD instructions for
        JPEG decoding, and ARM platforms can use NEON SIMD if requested.
        This work was done by Fabian "ryg" Giesen. SSE2 is used by
        default, but NEON must be enabled explicitly; see docs.

        With other JPEG optimizations included in this version, we see
        2x speedup on a JPEG on an x86 machine, and a 1.5x speedup
        on a JPEG on an ARM machine, relative to previous versions of this
        library. The same results will not obtain for all JPGs and for all
        x86/ARM machines. (Note that progressive JPEGs are significantly
        slower to decode than regular JPEGs.) This doesn't mean that this
        is the fastest JPEG decoder in the land; rather, it brings it
        closer to parity with standard libraries. If you want the fastest
        decode, look elsewhere. (See "Philosophy" section of docs below.)

        See final bullet items below for more info on SIMD.

      - Added STBI_MALLOC, STBI_REALLOC, and STBI_FREE macros for replacing
        the memory allocator. Unlike other STBI libraries, these macros don't
        support a context parameter, so if you need to pass a context in to
        the allocator, you'll have to store it in a global or a thread-local
        variable.

      - Split existing STBI_NO_HDR flag into two flags, STBI_NO_HDR and
        STBI_NO_LINEAR.
            STBI_NO_HDR:     suppress implementation of .hdr reader format
            STBI_NO_LINEAR:  suppress high-dynamic-range light-linear float API

      - You can suppress implementation of any of the decoders to reduce
        your code footprint by #defining one or more of the following
        symbols before creating the implementation.

            STBI_NO_JPEG
            STBI_NO_PNG
            STBI_NO_BMP
            STBI_NO_PSD
            STBI_NO_TGA
            STBI_NO_GIF
            STBI_NO_HDR
            STBI_NO_PIC
            STBI_NO_PNM   (.ppm and .pgm)

      - You can request *only* certain decoders and suppress all other ones
        (this will be more forward-compatible, as addition of new decoders
        doesn't require you to disable them explicitly):

            STBI_ONLY_JPEG
            STBI_ONLY_PNG
            STBI_ONLY_BMP
            STBI_ONLY_PSD
            STBI_ONLY_TGA
            STBI_ONLY_GIF
            STBI_ONLY_HDR
            STBI_ONLY_PIC
            STBI_ONLY_PNM   (.ppm and .pgm)

         Note that you can define multiples of these, and you will get all
         of them ("only x" and "only y" is interpreted to mean "only x&y").

       - If you use STBI_NO_PNG (or _ONLY_ without PNG), and you still
         want the zlib decoder to be available, #define STBI_SUPPORT_ZLIB

      - Compilation of all SIMD code can be suppressed with
            #define STBI_NO_SIMD
        It should not be necessary to disable SIMD unless you have issues
        compiling (e.g. using an x86 compiler which doesn't support SSE
        intrinsics or that doesn't support the method used to detect
        SSE2 support at run-time), and even those can be reported as
        bugs so I can refine the built-in compile-time checking to be
        smarter.

      - The old STBI_SIMD system which allowed installing a user-defined
        IDCT etc. has been removed. If you need this, don't upgrade. My
        assumption is that almost nobody was doing this, and those who
        were will find the built-in SIMD more satisfactory anyway.

      - RGB values computed for JPEG images are slightly different from
        previous versions of stb_image. (This is due to using less
        integer precision in SIMD.) The C code has been adjusted so
        that the same RGB values will be computed regardless of whether
        SIMD support is available, so your app should always produce
        consistent results. But these results are slightly different from
        previous versions. (Specifically, about 3% of available YCbCr values
        will compute different RGB results from pre-1.49 versions by +-1;
        most of the deviating values are one smaller in the G channel.)

      - If you must produce consistent results with previous versions of
        stb_image, #define STBI_JPEG_OLD and you will get the same results
        you used to; however, you will not get the SIMD speedups for
        the YCbCr-to-RGB conversion step (although you should still see
        significant JPEG speedup from the other changes).

        Please note that STBI_JPEG_OLD is a temporary feature; it will be
        removed in future versions of the library. It is only intended for
        near-term back-compatibility use.


   Latest revision history:
      2.05  (2015-04-19) fix bug in progressive JPEG handling, fix warning
      2.04  (2015-04-15) try to re-enable SIMD on MinGW 64-bit
      2.03  (2015-04-12) additional corruption checking
                         stbi_set_flip_vertically_on_load
                         fix NEON support; fix mingw support
      2.02  (2015-01-19) fix incorrect assert, fix warning
      2.01  (2015-01-17) fix various warnings
      2.00b (2014-12-25) fix STBI_MALLOC in progressive JPEG
      2.00  (2014-12-25) optimize JPEG, including x86 SSE2 & ARM NEON SIMD
                         progressive JPEG
                         PGM/PPM support
                         STBI_MALLOC,STBI_REALLOC,STBI_FREE
                         STBI_NO_*, STBI_ONLY_*
                         GIF bugfix
      1.48  (2014-12-14) fix incorrectly-named assert()
      1.47  (2014-12-14) 1/2/4-bit PNG support (both grayscale and paletted)
                         optimize PNG
                         fix bug in interlaced PNG with user-specified channel count

   See end of file for full revision history.


 ============================    Contributors    =========================

 Image formats                                Bug fixes & warning fixes
    Sean Barrett (jpeg, png, bmp)                Marc LeBlanc
    Nicolas Schulz (hdr, psd)                    Christpher Lloyd
    Jonathan Dummer (tga)                        Dave Moore
    Jean-Marc Lienher (gif)                      Won Chun
    Tom Seddon (pic)                             the Horde3D community
    Thatcher Ulrich (psd)                        Janez Zemva
    Ken Miller (pgm, ppm)                        Jonathan Blow
                                                 Laurent Gomila
                                                 Aruelien Pocheville
 Extensions, features                            Ryamond Barbiero
    Jetro Lauha (stbi_info)                      David Woo
    Martin "SpartanJ" Golini (stbi_info)         Martin Golini
    James "moose2000" Brown (iPhone PNG)         Roy Eltham
    Ben "Disch" Wenger (io callbacks)            Luke Graham
    Omar Cornut (1/2/4-bit PNG)                  Thomas Ruf
    Nicolas Guillemot (vertical flip)            John Bartholomew
                                                 Ken Hamada
 Optimizations & bugfixes                        Cort Stratton
    Fabian "ryg" Giesen                          Blazej Dariusz Roszkowski
    Arseny Kapoulkine                            Thibault Reuille
                                                 Paul Du Bois
                                                 Guillaume George
  If your name should be here but                Jerry Jansson
  isn't, let Sean know.                          Hayaki Saito
                                                 Johan Duparc
                                                 Ronny Chevalier
                                                 Michal Cichon
                                                 Tero Hanninen
                                                 Sergio Gonzalez
                                                 Cass Everitt
                                                 Engin Manap
                                                 Martins Mozeiko
                                                 Joseph Thomson
                                                 Phil Jordan

License:
   This software is in the public domain. Where that dedication is not
   recognized, you are granted a perpetual, irrevocable license to copy
   and modify this file however you want.

*/

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STBI_INCLUDE_STB_IMAGE_H

// DOCUMENTATION
//
// Limitations:
//    - no 16-bit-per-channel PNG
//    - no 12-bit-per-channel JPEG
//    - no JPEGs with arithmetic coding
//    - no 1-bit BMP
//    - GIF always returns *comp=4
//
// Basic usage (see HDR discussion below for HDR usage):
//    int x,y,n;
//    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
//    // ... process data if not NULL ...
//    // ... x = width, y = height, n = # 8-bit components per pixel ...
//    // ... replace '0' with '1'..'4' to force that many components per pixel
//    // ... but 'n' will always be the number that it would have been if you said 0
//    stbi_image_free(data)
//
// Standard parameters:
//    int *x       -- outputs image width in pixels
//    int *y       -- outputs image height in pixels
//    int *comp    -- outputs # of image components in image file
//    int req_comp -- if non-zero, # of image components requested in result
//
// The return value from an image loader is an 'unsigned char *' which points
// to the pixel data, or NULL on an allocation failure or if the image is
// corrupt or invalid. The pixel data consists of *y scanlines of *x pixels,
// with each pixel consisting of N interleaved 8-bit components; the first
// pixel pointed to is top-left-most in the image. There is no padding between
// image scanlines or between pixels, regardless of format. The number of
// components N is 'req_comp' if req_comp is non-zero, or *comp otherwise.
// If req_comp is non-zero, *comp has the number of components that _would_
// have been output otherwise. E.g. if you set req_comp to 4, you will always
// get RGBA output, but you can check *comp to see if it's trivially opaque
// because e.g. there were only 3 channels in the source image.
//
// An output image with N components has the following components interleaved
// in this order in each pixel:
//
//     N=#comp     components
//       1           grey
//       2           grey, alpha
//       3           red, green, blue
//       4           red, green, blue, alpha
//
// If image loading fails for any reason, the return value will be NULL,
// and *x, *y, *comp will be unchanged. The function stbi_failure_reason()
// can be queried for an extremely brief, end-user unfriendly explanation
// of why the load failed. Define STBI_NO_FAILURE_STRINGS to avoid
// compiling these strings at all, and STBI_FAILURE_USERMSG to get slightly
// more user-friendly ones.
//
// Paletted PNG, BMP, GIF, and PIC images are automatically depalettized.
//
// ===========================================================================
//
// Philosophy
//
// stb libraries are designed with the following priorities:
//
//    1. easy to use
//    2. easy to maintain
//    3. good performance
//
// Sometimes I let "good performance" creep up in priority over "easy to maintain",
// and for best performance I may provide less-easy-to-use APIs that give higher
// performance, in addition to the easy to use ones. Nevertheless, it's important
// to keep in mind that from the standpoint of you, a client of this library,
// all you care about is #1 and #3, and stb libraries do not emphasize #3 above all.
//
// Some secondary priorities arise directly from the first two, some of which
// make more explicit reasons why performance can't be emphasized.
//
//    - Portable ("ease of use")
//    - Small footprint ("easy to maintain")
//    - No dependencies ("ease of use")
//
// ===========================================================================
//
// I/O callbacks
//
// I/O callbacks allow you to read from arbitrary sources, like packaged
// files or some other source. Data read from callbacks are processed
// through a small internal buffer (currently 128 bytes) to try to reduce
// overhead.
//
// The three functions you must define are "read" (reads some bytes of data),
// "skip" (skips some bytes of data), "eof" (reports if the stream is at the end).
//
// ===========================================================================
//
// SIMD support
//
// The JPEG decoder will try to automatically use SIMD kernels on x86 when
// supported by the compiler. For ARM Neon support, you must explicitly
// request it.
//
// (The old do-it-yourself SIMD API is no longer supported in the current
// code.)
//
// On x86, SSE2 will automatically be used when available based on a run-time
// test; if not, the generic C versions are used as a fall-back. On ARM targets,
// the typical path is to have separate builds for NEON and non-NEON devices
// (at least this is true for iOS and Android). Therefore, the NEON support is
// toggled by a build flag: define STBI_NEON to get NEON loops.
//
// The output of the JPEG decoder is slightly different from versions where
// SIMD support was introduced (that is, for versions before 1.49). The
// difference is only +-1 in the 8-bit RGB channels, and only on a small
// fraction of pixels. You can force the pre-1.49 behavior by defining
// STBI_JPEG_OLD, but this will disable some of the SIMD decoding path
// and hence cost some performance.
//
// If for some reason you do not want to use any of SIMD code, or if
// you have issues compiling it, you can disable it entirely by
// defining STBI_NO_SIMD.
//
// ===========================================================================
//
// HDR image support   (disable by defining STBI_NO_HDR)
//
// stb_image now supports loading HDR images in general, and currently
// the Radiance .HDR file format, although the support is provided
// generically. You can still load any file through the existing interface;
// if you attempt to load an HDR file, it will be automatically remapped to
// LDR, assuming gamma 2.2 and an arbitrary scale factor defaulting to 1;
// both of these constants can be reconfigured through this interface:
//
//     stbi_hdr_to_ldr_gamma(2.2f);
//     stbi_hdr_to_ldr_scale(1.0f);
//
// (note, do not use _inverse_ constants; stbi_image will invert them
// appropriately).
//
// Additionally, there is a new, parallel interface for loading files as
// (linear) floats to preserve the full dynamic range:
//
//    float *data = stbi_loadf(filename, &x, &y, &n, 0);
//
// If you load LDR images through this interface, those images will
// be promoted to floating point values, run through the inverse of
// constants corresponding to the above:
//
//     stbi_ldr_to_hdr_scale(1.0f);
//     stbi_ldr_to_hdr_gamma(2.2f);
//
// Finally, given a filename (or an open file or memory block--see header
// file for details) containing image data, you can query for the "most
// appropriate" interface to use (that is, whether the image is HDR or
// not), using:
//
//     stbi_is_hdr(char *filename);
//
// ===========================================================================
//
// iPhone PNG support:
//
// By default we convert iphone-formatted PNGs back to RGB, even though
// they are internally encoded differently. You can disable this conversion
// by by calling stbi_convert_iphone_png_to_rgb(0), in which case
// you will always just get the native iphone "format" through (which
// is BGR stored in RGB).
//
// Call stbi_set_unpremultiply_on_load(1) as well to force a divide per
// pixel to remove any premultiplied alpha *only* if the image file explicitly
// says there's premultiplied data (currently only happens in iPhone images,
// and only if iPhone convert-to-rgb processing is on).
//


#ifndef STBI_NO_STDIO
#include <stdio.h>
#endif // STBI_NO_STDIO

#define STBI_VERSION 1

enum
{
   STBI_default = 0, // only used for req_comp

   STBI_grey       = 1,
   STBI_grey_alpha = 2,
   STBI_rgb        = 3,
   STBI_rgb_alpha  = 4
};

typedef unsigned char stbi_uc;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef STB_IMAGE_STATIC
#define STBIDEF static
#else
#define STBIDEF extern
#endif

//////////////////////////////////////////////////////////////////////////////
//
// PRIMARY API - works on images of any type
//

//
// load image by filename, open file, or memory buffer
//

typedef struct
{
   int      (*read)  (void *user,char *data,int size);   // fill 'data' with 'size' bytes.  return number of bytes actually read
   void     (*skip)  (void *user,int n);                 // skip the next 'n' bytes, or 'unget' the last -n bytes if negative
   int      (*eof)   (void *user);                       // returns nonzero if we are at end of file/data
} stbi_io_callbacks;

STBIDEF stbi_uc *stbi_load               (char              const *filename,           int *x, int *y, int *comp, int req_comp);
STBIDEF stbi_uc *stbi_load_from_memory   (stbi_uc           const *buffer, int len   , int *x, int *y, int *comp, int req_comp);
STBIDEF stbi_uc *stbi_load_from_callbacks(stbi_io_callbacks const *clbk  , void *user, int *x, int *y, int *comp, int req_comp);

#ifndef STBI_NO_STDIO
STBIDEF stbi_uc *stbi_load_from_file  (FILE *f,                  int *x, int *y, int *comp, int req_comp);
// for stbi_load_from_file, file pointer is left pointing immediately after image
#endif

#ifndef STBI_NO_LINEAR
   STBIDEF float *stbi_loadf                 (char const *filename,           int *x, int *y, int *comp, int req_comp);
   STBIDEF float *stbi_loadf_from_memory     (stbi_uc const *buffer, int len, int *x, int *y, int *comp, int req_comp);
   STBIDEF float *stbi_loadf_from_callbacks  (stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *comp, int req_comp);

   #ifndef STBI_NO_STDIO
   STBIDEF float *stbi_loadf_from_file  (FILE *f,                int *x, int *y, int *comp, int req_comp);
   #endif
#endif

#ifndef STBI_NO_HDR
   STBIDEF void   stbi_hdr_to_ldr_gamma(float gamma);
   STBIDEF void   stbi_hdr_to_ldr_scale(float scale);
#endif

#ifndef STBI_NO_LINEAR
   STBIDEF void   stbi_ldr_to_hdr_gamma(float gamma);
   STBIDEF void   stbi_ldr_to_hdr_scale(float scale);
#endif // STBI_NO_HDR

// stbi_is_hdr is always defined, but always returns false if STBI_NO_HDR
STBIDEF int    stbi_is_hdr_from_callbacks(stbi_io_callbacks const *clbk, void *user);
STBIDEF int    stbi_is_hdr_from_memory(stbi_uc const *buffer, int len);
#ifndef STBI_NO_STDIO
STBIDEF int      stbi_is_hdr          (char const *filename);
STBIDEF int      stbi_is_hdr_from_file(FILE *f);
#endif // STBI_NO_STDIO


// get a VERY brief reason for failure
// NOT THREADSAFE
STBIDEF const char *stbi_failure_reason  (void);

// free the loaded image -- this is just free()
STBIDEF void     stbi_image_free      (void *retval_from_stbi_load);

// get image dimensions & components without fully decoding
STBIDEF int      stbi_info_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp);
STBIDEF int      stbi_info_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *comp);

#ifndef STBI_NO_STDIO
STBIDEF int      stbi_info            (char const *filename,     int *x, int *y, int *comp);
STBIDEF int      stbi_info_from_file  (FILE *f,                  int *x, int *y, int *comp);

#endif



// for image formats that explicitly notate that they have premultiplied alpha,
// we just return the colors as stored in the file. set this flag to force
// unpremultiplication. results are undefined if the unpremultiply overflow.
STBIDEF void stbi_set_unpremultiply_on_load(int flag_true_if_should_unpremultiply);

// indicate whether we should process iphone images back to canonical format,
// or just pass them through "as-is"
STBIDEF void stbi_convert_iphone_png_to_rgb(int flag_true_if_should_convert);

// flip the image vertically, so the first pixel in the output array is the bottom left
STBIDEF void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);

// ZLIB client - used by PNG, available for other purposes

STBIDEF char *stbi_zlib_decode_malloc_guesssize(const char *buffer, int len, int initial_size, int *outlen);
STBIDEF char *stbi_zlib_decode_malloc_guesssize_headerflag(const char *buffer, int len, int initial_size, int *outlen, int parse_header);
STBIDEF char *stbi_zlib_decode_malloc(const char *buffer, int len, int *outlen);
STBIDEF int   stbi_zlib_decode_buffer(char *obuffer, int olen, const char *ibuffer, int ilen);

STBIDEF char *stbi_zlib_decode_noheader_malloc(const char *buffer, int len, int *outlen);
STBIDEF int   stbi_zlib_decode_noheader_buffer(char *obuffer, int olen, const char *ibuffer, int ilen);


#ifdef __cplusplus
}
#endif

//
//
////   end header file   /////////////////////////////////////////////////////
#endif // STBI_INCLUDE_STB_IMAGE_H