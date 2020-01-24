// -------------------------------------------------------------------------------------
// QuickMAN.c -- Main file for the QuickMAN SSE/SSE2-based Mandelbrot Set calculator
// Copyright (C) 2006-2008 Paul Gentieu (paul.gentieu@yahoo.com)
//
// This file is part of QuickMAN.
//
// QuickMAN is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
//
// Project Page: http://sourceforge.net/projects/quickman
//
// Author: Paul Gentieu (main code, ASM iteration cores, palettes)
//
// Contributors:
// Andy Milne
// George Bullis
// Mike Mossey (palettes)
// Ben Valdes (fast "wave" algorithm)
// Luciano Genero (conversion to C)
// Peter Kankowski (general suggestions)
//
// Thanks to Daniele Paccaloni and the other FFFF authors for the zooming
// user interface (duplicated in QuickMAN).
//
// -------------------------------------------------------------------------------------
//
// 1/19/08 - 2/01/08 PG: Started v1.0 from v0.95. Removed previous comments. See v0.95 for these.
//
// Added multithreading for higher performance on multi-core CPUs. Scales pretty well:
// speedup factor (compared to a single core) is about the same as the number of cores.
//
// Removed buddhabrot functionality. This added a lot of code and complexity for minimal
// benefit. May add it back in later when it's in a better state.
//
// Improved thread load sharing by assigning fine-grained stripes to the threads.
//
// Tweaked GUI to be more logical.
//
// Added "pan lock" mode. Press shift key with arrows to lock panning. Will
// keep panning in current direction until shift is pressed again. Fun...
//
// Added pan/zoom rate controls (multi-core PCs really needed them- default rates
// were too fast). The super-slow zooms and pans are nice...
//
// Fixed buggy offset in wave 3 of fast algorithm. Added two extra dummy pixels at
// the end of each iteration data line, as required for off-screen neighboring pixel
// checks.
//
// Finally fixed the fast algorithm corrupt pixel bug. Caused by comparing pixel sums rather
// than values directly. Removed old code that was apparently there to kluge around it.
//
// Cut overhead by tweaking various areas of the code. Simplified iteration functions
// to share the same point structures and use the same initial conditions.
//
// Tried multithreading the palette mapper. Seems to give no benefit on the Pentium D 820 system.
//
// Added logfile commands for auto-zooming, auto-panning, image sizing, setting the
// max iters color, and algorithm/GUI settings. Examples in auto_panzoom.log and
// tune.log.
//
// Now have a quickman.cfg file, so users can set their own default settings. Has the
// same syntax as logfile commands.
//
// Bugs and pseudo-bugs:
//
// For a given image, the total number of iterations increases slightly with increasing
// numbers of threads. This is because each thread has to do some dummy iterations to flush
// its queues. The more threads there are, the more dummy iterations there are.
//
// The number of iterations can vary from run to run with the fast algorithm and more than
// one thread. Apparently this is due to threads racing and looking into each other's
// data areas for neighboring pixel matches. Depending on the order they run in there
// could be a different number of matches. Doesn't seem to cause any visual issues- maybe
// fix in next version.
//
// Saw panning get into a bad state once (partial old image). Only had one thread. Couldn't
// reproduce. Probably some quadrant boundary condition... if anyone can reproduce
// it let me know.

#define STRICT
#define WIN32_LEAN_AND_MEAN

// This probably doesn't run too well on Win98 anymore.

#define _WIN32_WINNT 0x501    // Windows XP
//#define _WIN32_WINNT 0x0410 // Windows 98. Defining this will force use of the slow threading functions

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <mmsystem.h> // for timer functions
#include <commctrl.h>
#include <process.h>  // for threading functions
#include "resource.h"

// Precision used in calculation
#define PRECISION_AUTO        0 // Automatically determined based on magnification
#define PRECISION_SINGLE      1 // 32-bit float
#define PRECISION_DOUBLE      2 // 64-bit double
#define PRECISION_EXTENDED    3 // 80-bit (x87) double or larger

// Available algorithms
#define ALG_FAST_ASM_AMD      0 // Use the "wave" algorithm to guess pixels
#define ALG_EXACT_ASM_AMD     1 // Calculate every pixel (no interpolation or guessing)
#define ALG_FAST_ASM_INTEL    2 // Intel versions
#define ALG_EXACT_ASM_INTEL   3 //
#define ALG_FAST_C            4 // Unoptimized C versions
#define ALG_EXACT_C           5 //
//#define ALG_ERROR           6 // Show error image: exact ^ fast

#define ALG_EXACT             1 // using Exact alg if this bit set (change with above)
#define ALG_INTEL             2 // using Intel alg if this bit set
#define ALG_C                 4 // using C alg if this bit set

#define NUM_ELEM(a) (sizeof(a) / sizeof(a[0]))

// Combo box initialization strings/defines. String order should correspond
// to the above #defines.
static char *precision_strs[] = { "Auto", "Single", "Double", "Extended"};
static char *alg_strs[] =       { "Fast, AMD",   "Exact, AMD",
                                  "Fast, Intel", "Exact, Intel",
                                  "Fast, C",     "Exact, C" };
                                  //"Fast error" };

// Palettes 0, 2, and 11 are marked for replacement (rarely used)
static char *palette_strs[] =   { "0 Ugly", "1 Striped", "2 Loud", "3 Muted", "4 Purple",
                                  "5 Earthy", "6 Smoky", "7 Deco", "8 Flaming", "9 Metallic",
                                  "10 Angry", "11 Murky", "12 Flaming+", "13 Plantlike" };

// Not all of these will be used
static char *num_threads_strs[] = { "1", "2", "4", "8", "16", "32", "64", "128", "256" };

#define MAX_THREADS_IND     5 // Set this to set maximum number of threads (== 2^this)
#define MAX_THREADS        (1 << MAX_THREADS_IND)

//#define USE_PERFORMANCE_COUNTER   // See get_timer()

#ifdef USE_PERFORMANCE_COUNTER
typedef LARGE_INTEGER TIME_UNIT;
#else
typedef DWORD TIME_UNIT;
#endif

#define DEFAULT_PAL      3 // muted, looks good on most images

// Global mandelbrot parameters

static double re;                      // real part of image center coordinate
static double im;                      // imaginary part of image center coordinate
static double mag;                     // magnification
static unsigned max_iters = 128;       // maximum iterations to do per point
unsigned max_iters_last = 0;           // last max iters used in a calculation; used by palette code
static unsigned palette = DEFAULT_PAL; // current palette
static int precision = PRECISION_AUTO; // user-desired precision
static int calc_precision;             // actual precision used in calculation: same as precision when not AUTO
static int alg = ALG_FAST_ASM_AMD;     // algorithm
static int cur_alg;                    // current algorithm in use: can change during panning
static int num_threads_ind = 0;        // number of threads string index
int num_threads = 1;                   // number of calculation threads. Limited to values that make sense: 1, 2, 4, 8, 16...
static int man_xsize;                  // image size; defaults now come from cfg_settings structure
static int man_ysize;                  // or quickman.cfg
static int prev_man_xsize;             // previous sizes, for restoring window
static int prev_man_ysize;
static int iter_data_line_size;        // iteration data array needs 2 extra pixels at the end of each line
static int man_min_dimension;          // smaller of xsize and ysize
static double mouse_re;                // re/im coordinates of the mouse position
static double mouse_im;
static double zoom_start_mag;          // starting magnification, for zoom button
static int image_size;                 // current image size in pixels
static int precision_loss = 0;         // 1 if precision loss detected on most recent calculation
static unsigned num_builtin_palettes;  // number of builtin palettes
static unsigned num_palettes;          // total number of palettes
static char palette_file[256];         // filename of current user palette file
static char logfile[256] = "quickman.log";      // default logfile
char *file_strs[] = {logfile, "auto_panzoom.log", "bmark.log", "zoomtest.log", "fpstest.log" };

#define CFG_FILE "quickman.cfg"         // configuration file containing default settings

// Timing/benchmarking values
static double iter_time = 0.0;          // mandelbrot iteration time only
static double zoom_time;                // time to do 1 zoom, includes above + extra overhead
static double calc_interval_time = 0.0; // mandelbrot time during one interval
static double calc_total_time = 0.0;    // mandelbrot total time
static double interval_time = 0.0;      // pan/zoom time during 1 interval
static double total_time = 0.0;         // total pan/zoom time
static unsigned total_frames = 0;       // total frames
static unsigned interval_frames = 0;    // frames per interval
static double file_tot_time = 0.0;      // total calculation time since file opened
static int all_recalculated = 0;        // flag indicating a recalculation of the whole image happened
static TIME_UNIT zoom_start_time;       // for zoom button benchmarking (helps measure overhead)

// Idicates whether processor supports SSE/SSE2 and CMOV instructions
static int sse_support = 0; // 1 for SSE, 2 for SSE and SSE2

// Navigation modes
#define MODE_ZOOM             1     // zoom in/out 2x or zoom on rectangle, using magnifier
#define MODE_RTZOOM           2     // realtime zoom in/out
#define MODE_PAN              3     // pan around the image

// Mouse position: index 0 is position on initial button press; index 1 is current position
static int mouse_x[2], mouse_y[2];

static int nav_mode = MODE_RTZOOM;
static int do_rtzoom = 0;           // if nonzero, do a realtime zoom in/out
static int prev_do_rtzoom = 0;      // previous state of do_rtzoom

// Bits for do_rtzoom
#define RTZOOM_IN             1     // zoom in
#define RTZOOM_OUT            2     // zoom out
#define RTZOOM_WITH_BUTTON    4     // 1 if zooming with the zoom button, 0 if with the mouse

// Multiply or divide the magnification by this factor when the user
// zooms using a single click.
#define MAG_ZOOM_FACTOR       2.0
#define MAG_MIN               0.02  // minimum magnification

// Zoom steps as a function of slider value. Nonlinear at beginning and end. These
// seem to work pretty well.

static double rtzoom_mag_steps[] =
{
    1.000625,  // 0, super slow
    1.00125, 1.0025, 1.005, 1.010, 1.015, 1.020, 1.025, 1.03, 1.04,
    1.05,      // 10, default. 1.05 was previous value, but no existing benchmarks, so can change it
    1.06, 1.07, 1.08, 1.09, 1.10, 1.11, 1.12, 1.14, 1.17,
    1.20       // 20, fast
};

// Arbitrary units; slider range is determined by array size
#define MAX_ZOOM_RATE         ((sizeof(rtzoom_mag_steps) / sizeof(rtzoom_mag_steps[0])) - 1)
#define DEFAULT_ZOOM_RATE     (MAX_ZOOM_RATE >> 1)

// Pan step scales as a function of slider value.
static double pan_step_scales[] =
{
   0.00125, // 0, very slow
   0.0025, 0.005, 0.01, 0.02, 0.04, 0.08, 0.2, 0.4, 0.6,
   0.8,     // 10, default. With new magic constants, this should be roughly compatible with prev. benchmarks
   1.0, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 2.6,
   2.8      // 20, fast
};

#define MAX_PAN_RATE          ((sizeof(pan_step_scales) / sizeof(pan_step_scales[0])) - 1)
#define DEFAULT_PAN_RATE      (MAX_PAN_RATE >> 1)

// Magnitude squared must exceed this for a point to be considered diverged.
// Values smaller than 16 usually give less aesthetically pleasing results. Maybe
// make this user-settable.

#define DIVERGED_THRESH       16.0
//#define DIVERGED_THRESH     4.0 // this looks cool on some images

// Integer portion of high word in double, corresponding to threshold above
#define DIV_EXP               0x40300000   // for doubles with 11 bit exponent
#define DIV_EXP_FLOAT         0x41800000   // for floats with 8 bit exponent
//#define DIV_EXP             0x40100000

#define MIN_ITERS             2            // allow to go down to min possible, for overhead testing
#define MAX_ITERS             0x08000000   // keep upper 4 bits free in iter array, in case we need them for something

#define MIN_SIZE              32           // min image size dimension

//#define WM_DUMMY   (WM_APP + 100)        // use this form if messages needed

// GUI stuff
HCURSOR mag_cursor, rtzoom_cursor, hopen_cursor, hclosed_cursor, arrow_cursor, wait_cursor;
HCURSOR mag_zoom_cursor;                   // either mag or rtzoom, depending on mode
HWND hwnd_main;                            // handle for the main window
HWND hwnd_dialog = 0;                      // handle for the dialog box
HWND hwnd_info, hwnd_status, hwnd_status2; // handles for the info/status text areas
HWND hwnd_iters, hwnd_thumbnail_frame;     // and other controls
HINSTANCE hinstance = 0;                   // the application
HDC hscreen_dc = NULL;                     // screen device context
int x_border;                              // variables for calculating window sizes and such
int y_border;
int y_thinborder;

// Iteration data for each point in the image. Converted to a bitmap by applying a palette.
static unsigned *iter_data = NULL;
static unsigned *iter_data_start = NULL;   // for dummy line creation: see create_bitmap

static int status = 0;                     // general status value

// Status bits
#define STAT_NEED_RECALC            1      // 1 if image needs to be recalculated (due to resize, etc)
#define STAT_RECALC_FOR_PALETTE     2      // 1 if need recalculation before new palette applied (true after panning)
#define STAT_SHOW_WAIT_CURSOR       4      // 1 if a wait cursor should be shown during calculation

static double *img_re = NULL;              // Arrays for holding the RE, IM coordinates
static double *img_im = NULL;              // of each pixel in the image

// Structures for new quadrant-based panning algorithm:
//
// Each quadrant is the size of the screen (xsize x ysize). Initially, the screen window is
// placed in the upper left (UL) quadrant, at position (0, 0).
//
//
// (0,0) +-------+-------+
//       |       |       |
//       |  UL,  |  UR   |
//       |screen |       |
//       +-------+-------+
//       |       |       |
//       |  LL   |  LR   |
//       |       |       |
//       +-------+-------+ (2 * xsize - 1, 2 * ysize - 1)
//
// If panning causes the screen window to move outside the 4 quadrants, swap quadrants
// and renormalize the screen coordinates.
//
// Example with xsize and ysize = 5. Screen panned to -2, -3 (outside quadrants in both x
// and y directions). Swap UL/LR and UR/LL, add xsize to the screen x coordinate, and
// add ysize to the screen y coordinate.
//
// XXXX = still-valid bitmap data. Blank = new bitmap data that needs to be calculated.
//
// (-2, -3)
//     +-------+
//     |Screen |
//     |  +----+--+-------+         +-------+-------+
//     |  |XXXX|  |       |         |old LR |old LL |
//     +--+----+  |       |         |    +--+----+  |  Screen now at 3, 2 after quadrant swap and renormalization.
//        |  UL   |  UR   |  Swap   |    |  |    |  |  1. Calculate the update rectangles based
//        +-------+-------+  ---->  +----+--+----+--+     on the new screen position
//        |       |       |         |    |  |XXXX|  |  2. Iterate/palette map the update rectangles
//        |  LL   |  LR   |         |    +--+----+  |     (XXXX is not recalculated)
//        |       |       |         |old UR |old UL |  3. Blit all 4 quadrant rectangles to the screen
//        +-------+-------+         +-------+-------+     (all rectangles now have valid bitmap data).
//
// Swappings:
// if (x < 0 || x > xsize) swap(UL, UR); swap(LL, LR);
// if (y < 0 || y > ysize) swap(UL, LL); swap(UR, LR);
//
// The swap operation swaps only memory pointers and handles, not memory itself.

// Structure for rectangles used in panning
typedef struct
{
   int x[2];   // x coordinates; x[0] <= x[1]
   int y[2];   // y coordinates; y[0] <= y[1]
   int valid;  // nonzero if rect is valid, 0 if invalid (should be ignored)
}
rectangle;

// Panning can generate up to 2 update rectangles (1 horizontal, 1 vertical)
static rectangle update_rect[2];

// Structure for quadrants used in panning
typedef struct
{
   int status;          // see below
   HBITMAP handle;      // handle to the bitmap

   rectangle quad_rect; // rectangle coordinates of this quadrant

   // Raw bitmap data. Each 32-bit value is an RGB triplet (in bits 23-0) and one 0 byte
   // (in bits 31-24). Faster to access than a 24-bit bitmap.
   unsigned *bitmap_data;

   // Blitting parameters. All offsets are quadrant-relative (i.e., range from 0 to
   // xsize - 1 and 0 to ysize - 1 inclusive).

   int src_xoffs;       // Source offsets: blitting starts from this point in the bitmap
   int src_yoffs;
   int dest_xoffs;      // Dest offsets: blitting goes to this point on the screen
   int dest_yoffs;
   int blit_xsize;      // Size of area to blit
   int blit_ysize;
}
quadrant;

// The 4 quadrant bitmaps (each of size man_xsize x man_ysize)
static quadrant quad[4];

#define UL 0  // upper left
#define UR 1  // upper right
#define LL 2  // lower left
#define LR 3  // lower right

// Values for the quadrant status word
#define QSTAT_DO_BLIT      1  // if set, blit this quadrant's data to the screen

static int screen_xpos, screen_ypos; // position of screen (in above coordinate system)

// Constants and variables used in the fast "wave" algorithm

// Starting values for x and y. Now seems faster to have these as static globals
static const int wave_ystart[7]   = {3, 1, 3, 1, 0, 1, 0};   // calculates y = 0, needs dummy line at y = -1 (set to 0's)
static const int wave_xstart[7]   = {0, 2, 2, 0, 1, 1, 0};

// X and Y increments for each wave
static const int wave_inc[7]      = {4, 4, 4, 4, 2, 2, 2};

// Offsets from current pixel location. If all 4 pixels there are equal, set the current pixel
// to that value (not used for wave 0). Stored in increasing pointer order

// Old code had a buggy offset in wave 3
static const int wave_xoffs[7][4] = {{ 0, 0, 0,  0}, {-2, 2, -2, 2}, {0, -2, 2, 0}, {0, -2, 2, 0},
                                     {-1, 1, -1, 1}, { 0, -1, 1, 0}, {0, -1, 1, 0}};
static const int wave_yoffs[7][4] = {{ 0,  0, 0, 0}, {-2, -2, 2, 2}, {-2, 0, 0, 2}, {-2, 0, 0, 2},
                                     {-1, -1, 1, 1}, {-1,  0, 0, 1}, {-1, 0, 0, 1}};

static int wave_ptr_offs[7][4];  // Pointer offsets into bitmap, precalculated from above values

// Structures and variables used in iteration

// Structure to hold the state of 4 iterating points (or 8 for SSE). For SSE (single precision),
// 32-bit floats are packed into the fields, otherwise 64-bit doubles. This needs
// to be 16-byte aligned. Project/compiler options can't guarantee this alignment:
// must use syntax below.
//
// Only the first 4/8 values of each array are used (unless otherwise noted). But the
// additional values are still necessary to force each array to occupy its own 64-byte
// cache line (i.e, no sharing). With line sharing there can be conflicts that cost cycles.
//
// May even want to give each 128 bits (xmm reg) its own cache line- change x to x01, x23, etc.
// Initialization and divergence detection would be nastier
//
// Tried expanding x, y, and yy to 16 doubles so ..23 regs could have own cache line: no effect

typedef struct // sps
{
   double x[8];                  // 0   x, y, yy = the state of the iterating points
   double y[8];                  // 64
   double yy[8];                 // 128
   double a[8];                  // 192 Real coordinate of point
   double b[8];                  // 256 Imag coordinate of point
   unsigned exp[16];             // 320 Only 1st 8 used; for comparing magnitude exponents for divergence detection
   unsigned expp[16];            // 384 Magnitude exponents from previous iteration
   double two_d[8];              // 448 Only 1st 2 used; must be set to 2.0. Used in SSE2 routine.
   float two_f[16];              // 512 Only 1st 4 used; must be set to 2.0. Used in SSE routine.
   double rad_d[8];              // 576 Radius^2 for divergence detection; only 1st 2 used; only used in Intel version
   float rad_f[16];              // 640 only 1st 4 used

   // Even though the following fields aren't used in the inner loop, there's a slight decrease in
   // performance if they aren't aligned to a 64-byte cache line.

   unsigned iters[16];           // 704 Current iteration counts; only 1st 4/8 used
   unsigned *iters_ptr[16];      // 768 Pointer into iteration count array; only 1st 4/8 used
   double ab_in[8];              // 832 Main loop sets ab_in to the point to iterate on (ab_in[0] = re, ab_in[1] = im). Others unused.
   unsigned long long iterctr;   // 896 Iterations counter, for benchmarking. M$ 64-bit aligns this, adding (crash-causing) extra padding if not already on a 64-bit boundary...
   unsigned cur_max_iters;       // 904 Max iters to do this loop
   unsigned queue_status;        // status of pointstruct queue (free/full slots)
   unsigned pad[12];             // Pad to make size a multiple of 64. Necessary, otherwise code will crash with 2 or more threads due to array misalignment.
}
mandel_pointstruct;

// Pointers to structure members for asm functions.
// There should be some function to calculate these automatically, but use constants for now.
// Changing structure order can slow things down anyway so it shouldn't be done without good reason.

// Aliases for 4 double-precision points. EBX points to the beginning of the structure.
#define PS4_X01            [ebx + 0]
#define PS4_X23            [ebx + 0 + 16]
#define PS4_Y01            [ebx + 64]
#define PS4_Y23            [ebx + 64 + 16]
#define PS4_YY01           [ebx + 128]
#define PS4_YY23           [ebx + 128 + 16]
#define PS4_A01            [ebx + 192]
#define PS4_A23            [ebx + 192 + 16]
#define PS4_B01            [ebx + 256]
#define PS4_B23            [ebx + 256 + 16]
#define PS4_EXP01          [ebx + 320]
#define PS4_EXP0           [ebx + 320 + 4]      // Locations of magnitude exponent bits in doubles
#define PS4_EXP1           [ebx + 320 + 12]
#define PS4_EXP23          [ebx + 320 + 16]
#define PS4_EXP2           [ebx + 320 + 16 + 4]
#define PS4_EXP3           [ebx + 320 + 16 + 12]
#define PS4_EXPP01         [ebx + 384]
#define PS4_EXPP23         [ebx + 384 + 16]
#define PS4_TWO            [ebx + 448]
#define PS4_RAD            [ebx + 576]
#define PS4_ITERS0         [ebx + 704]
#define PS4_ITERS1         [ebx + 704 + 4]
#define PS4_ITERS2         [ebx + 704 + 8]
#define PS4_ITERS3         [ebx + 704 + 12]
#define PS4_ITERCTR_L      [ebx + 896]
#define PS4_ITERCTR_H      [ebx + 900]
#define PS4_CUR_MAX_ITERS  [ebx + 904]

// Aliases for 8 single precision points
#define PS8_X03            PS4_X01
#define PS8_X47            PS4_X23
#define PS8_Y03            PS4_Y01
#define PS8_Y47            PS4_Y23
#define PS8_YY03           PS4_YY01
#define PS8_YY47           PS4_YY23
#define PS8_A03            PS4_A01
#define PS8_A47            PS4_A23
#define PS8_B03            PS4_B01
#define PS8_B47            PS4_B23
#define PS8_EXP03          [ebx + 320]       // Location of magnitude exponent bits. These
#define PS8_EXP0           [ebx + 320]       // differ from the locations for doubles.
#define PS8_EXP1           [ebx + 320 + 4]
#define PS8_EXP2           [ebx + 320 + 8]
#define PS8_EXP3           [ebx + 320 + 12]
#define PS8_EXP47          [ebx + 320 + 16]
#define PS8_EXP4           [ebx + 320 + 16]
#define PS8_EXP5           [ebx + 320 + 20]
#define PS8_EXP6           [ebx + 320 + 24]
#define PS8_EXP7           [ebx + 320 + 28]
#define PS8_EXPP03         PS4_EXPP01
#define PS8_EXPP47         PS4_EXPP23
#define PS8_TWO            [ebx + 512]
#define PS8_RAD            [ebx + 640]
#define PS8_ITERS0         [ebx + 704]       // Iteration counters for 8 points
#define PS8_ITERS1         [ebx + 704 + 4]
#define PS8_ITERS2         [ebx + 704 + 8]
#define PS8_ITERS3         [ebx + 704 + 12]
#define PS8_ITERS4         [ebx + 704 + 16]
#define PS8_ITERS5         [ebx + 704 + 20]
#define PS8_ITERS6         [ebx + 704 + 24]
#define PS8_ITERS7         [ebx + 704 + 28]
#define PS8_ITERCTR_L      PS4_ITERCTR_L
#define PS8_ITERCTR_H      PS4_ITERCTR_H
#define PS8_CUR_MAX_ITERS  PS4_CUR_MAX_ITERS

// Microsoft syntax for forcing 64-byte alignment (use for cache line optimization).
// Minimum required is 16. Remove static to see address in map file

static __declspec(align(64)) mandel_pointstruct pointstruct_array[MAX_THREADS];

// A stripe for the thread function to calculate. See man_calculate().
typedef struct
{
   int xstart;
   int xend;
   int ystart;
   int yend;
}
stripe;

// Maximum number of stripes per image to give each thread. See man_calculate().
#define MAX_STRIPES        8

// Thread state structure
typedef struct
{
   int thread_num;               // thread number
   mandel_pointstruct *ps_ptr;   // pointer to this thread's iterating point structure (from array above)
   stripe stripes[MAX_STRIPES];  // list of stripes for this thread to calculate
   int num_stripes;
   HANDLE done_event;            // event set by thread when calculation is finished

   // Nonessential variables (for profiling, load balance testing, etc)
   unsigned long long total_iters;  // iters value that keeps accumulating until reset (before next zoom, etc)
   unsigned points_guessed;         // points guessed in fast algorithm
}
thread_state;

static thread_state thread_states[MAX_THREADS];
static HANDLE thread_done_events[MAX_THREADS]; // the event handles for each thread

// Prototypes
void do_man_calculate(int recalc_all);
int set_slider_pos(int dlg_item, int pos);
int get_pan_steps(int *xstep, int *ystep, int set_pan_key);
// From palettes.c
extern int init_palettes(void);
extern int load_palette(FILE *fp);
extern int get_palette_rgb_val(int ind, char *str, int length, unsigned *rgb);
extern void apply_palette(unsigned *dest, unsigned *src, unsigned max_iters,
                          unsigned iter_line_size, unsigned bmp_line_size,
                          unsigned xsize, unsigned ysize, unsigned palette_num,
                          int max_iters_color);

// ----------------------- File/misc functions -----------------------------------

// Generic setting structure. Some flags are encoded by upper or lowercase
// letters in the name (see below). The name is not case sensitive in files.
typedef struct
{
   char *name;       // Name of the setting
   int val;          // Value of the setting
   int default_val;  // Default value for the setting
   int min;          // Its min and max limits, to prevent bad data
   int max;          // in files from messing things up
}
setting;

#define LOWERCASE 0x20

// Autoreset flag is set by having the first letter of the name uppercase.
// If the autoreset flag is set, the setting resets to default_val before each new image.
#define SETTING_AUTORESET(s) (!((s)->name[0] & LOWERCASE))

// Global configuration and algorithm settings. All fields must be setting structs.
typedef struct
{
   setting pan_rate;        // Value for the GUI pan rate slider
   setting pan_key;         // If nonzero, this image should be auto-panned (contains key code)
   setting zoom_rate;       // Value for the GUI zoom rate slider
   setting zoom_in_out;     // 0 = no zoom, 1 = zoom in, 2 = zoom out (to be implemented)
   setting xsize;           // X image size. 0 = maximize window (to be implemented)
   setting ysize;           // Y image size  0 = restore window (to be implemented)
   setting max_iters_color; // the color for points with max iters- for making lakes and such. Cool...

   // Stripes per thread (bitfield; 4 bits per num_threads index). For example, bits 7-4 give
   // the number of stripes per thread for two threads, 11-8 are for four threads, etc.

   setting stripes_per_thread;
   setting blit_stripe_thickness;  // thickness of stripes used in striped_blit
   setting pfcmin;                 // pan filter constant min and max
   setting pfcmax;                 // 10000 times the real value for these
}
settings;

// Default for stripes per thread bitfield.
// 32 threads: 2; 16 threads: 3; 8 threads: 4; 4 threads: 4; 2 threads: 7; 1 thread: 1
#define SPT_DEFAULT  0x234471

// Initialize the settings struct. The name field is what you give in the logfile to set the
// setting (not case sensitive). Only the val field can be modified from logfiles. Here,
// it should be set the same as the default_val.

// Names should not have numbers in them, unless the numbers are preceded by a '/' (comment).
// This is so that if they're later deleted, they won't be interpreted as image parameters
// when reading legacy logfiles.

static settings cfg_settings = // scfs
{
   // name, val, default, min, max
   {"panrate", DEFAULT_PAN_RATE, DEFAULT_PAN_RATE, 0, MAX_PAN_RATE},
   {"Pan", 0, 0, 0, 0xFFFF},               // bitfield; max doesn't matter; autoreset
   {"zoomrate", DEFAULT_ZOOM_RATE, DEFAULT_ZOOM_RATE, 0, MAX_ZOOM_RATE},
   {"Zoom", 0, 0, 0, 0xFFFF},              // future bitfield; max doesn't matter; autoreset
   {"xsize", 700, 700, MIN_SIZE, 0xFFFF},  // window functions automatically clip maxes for these
   {"ysize", 700, 700, MIN_SIZE, 0xFFFF},
   {"Maxiters_color", 0, 0, 0, 0xFFFFFF},  // max doesn't really matter, but this only uses 24 bits; autoreset
   {"spt", SPT_DEFAULT, SPT_DEFAULT, 0, 0xFFFFFF}, // bitfield; have to do an external min/max check on this one
   {"bst", 16, 16, 1, 0xFFFFFF},           // blit stripe thickness; max doesn't matter
   {"pfcmin", 150, 150, 1, 10000},         // 10000 * real value
   {"pfcmax", 300, 300, 1, 10000},         // 10000 * real value
};

// A log entry structure. It can have its own set of settings. The settings fields are all
// initialized to -1, then set to any values in the logfile that are found for this entry.
// Any setting that's >= 0 will be copied to the global config settings before the image
// is displayed.

typedef struct
{
   double re;
   double im;
   double mag;
   unsigned max_iters;
   unsigned palette;

   // Kinda wasteful to have the entire settings struct here when all we need are the
   // val fields, but it's easier this way
   settings log_settings;
}
log_entry;

static log_entry *log_entries = NULL;
static int log_pos = 0;
static int log_count = 0;

// Copy any settings fields that have changed (i.e., are >= 0) from src to dest.
void copy_changed_settings(settings *dest, settings *src)
{
   int i;
   setting *s, *d;

   s = (setting *) src;   // treat structs as arrays
   d = (setting *) dest;
   for (i = 0; i < sizeof(cfg_settings) / sizeof(setting); i++)
      if (s[i].val >= 0)
      {
         // char m[256];
         // sprintf_s(m, sizeof(m), "Setting %d changed to 0x%X (%d)", i, s[i].val, s[i].val);
         // MessageBox(NULL, m, "Warning", MB_OK | MB_ICONWARNING | MB_TASKMODAL);

         d[i].val = s[i].val;
      }
}

// Autoreset settings fields to defaults, if so configured. Call before every image recalculation.
// Only should be called with global cfg_settings (change to not take parm?)
void autoreset_settings(settings *dest)
{
   int i;
   setting *d;
   d = (setting *) dest; // treat struct as array
   for (i = 0; i < sizeof(cfg_settings) / sizeof(setting); i++)
      if (SETTING_AUTORESET(&d[i]))
         d[i].val = d[i].default_val;
}

// Set all val fields to -1, which invalidates all settings.
void invalidate_settings(settings *dest)
{
   int i;
   setting *d;
   d = (setting *) dest; // treat struct as array
   for (i = 0; i < sizeof(cfg_settings) / sizeof(setting); i++)
      d[i].val = -1;
}

// Holds the most recent set of settings read from a file. Copied to the log entry if the
// entry was valid. Copied to the global config settings if it was read out of quickman.cfg.
settings cur_file_settings;

// Read a set of mandelbrot parms into a log entry (if not NULL). This has now evolved
// to do a lot more (read in optional config settings and user commands)
int log_read_entry(log_entry *entry, FILE *fp)
{
   double vals[5];
   int i, j, n, ind, val;
   setting *s, *f;
   unsigned char strs[5][256], *str, c;

   // Initialize cur file settings structure to all invalid (no change)
   invalidate_settings(&cur_file_settings);

   // Read re, im, mag, iters, pal, and optional commands. To support legacy logfiles,
   // the five main fields don't need any leading items (they can be just numbers). Any
   // leading item before a number will be ignored, unless it's a recognized setting.
   for (i = 0; i < 5;)
   {
      if (feof(fp))
         return 0;
      if (fgets((char *) &strs[i][0], sizeof(strs[0]), fp) == NULL)
         return 0;

      str = &strs[i][0];

      // Skip any leading whitespace
      ind = -1;
      do
         c = str[++ind];
      while (c == ' ' || c == '\t'); // null will terminate loop

      // Look for any optional commands or settings. This should stay reasonably fast
      // even with large logfiles.

      s = (setting *) &cfg_settings;      // treat structs as arrays
      f = (setting *) &cur_file_settings;
      for (j = 0; j < sizeof(cfg_settings) / sizeof(setting); j++)
      {
         if (!_strnicmp(&str[ind], s->name, (size_t) n = strlen(s->name))) // not case sensitive
         {
            // Can use the function for reading a palette RGB value here (it will read
            // normal integers too). As a side effect any 24 bit value can be specified
            // as three individual bytes if desired...
            // Some settings are palette values.

            get_palette_rgb_val(ind + n, str, sizeof(strs[0]), &val);

            if (val >= s->min && val <= s->max)  // Set value if it's within legal range
               f->val = val;
            c = 0;                               // Found a setting; skip the stuff below
            break;
         }
         s++;
         f++;
      }
      if (!c)
         continue;

      // Might have an image parameter here (a number with or without leading items)
      for (j = ind; j < sizeof(strs[0]); j++)
      {
         c = str[j]; // strip out any leading non-numeric/non-quote chars
         if ( (c >= '0' && c <= '9') || c == '-' || c == '.' || c == '\"' || !c)
            break;
         if (c == '/') // '/' starts a comment
         {
            c = 0;
            break;
         }
      }
      if (c)
      {
         // Got something that looks like a number or a " if we get here. Any bad
         // values will be set to 0.0 (ok). J is long lived (see below)
         vals[i] = atof(&str[j]);
         i++; // look for next entry
      }
   }

   // All values good: update mandelbrot parms
   if (entry != NULL)
   {
      // Fill in the entry, including optional fields (if they're still at -1, nothing will happen later).
      entry->re = vals[0];
      entry->im = vals[1];
      entry->mag = vals[2];
      entry->max_iters = (unsigned) vals[3];
      entry->palette = (unsigned) vals[4];

      entry->log_settings = cur_file_settings; // Copy any settings found above

      // For user palette files (palette starts with " in logfile), use the position in the
      // dropdown list. Assumes dropdown list is already populated. As a side effect this also
      // allows the user to specify an exact builtin palette name, e.g. "3 Muted"
      if (str[j] == '\"')
      {
         for (i = j + 1; i < sizeof(strs[0]); i++) // Replace any trailing " with a null
         {
            if (str[i] == '\"')
               str[i] = 0;
            if (!str[i])
               break;
         }

         // Get palette from dropdown list. If not found, set to default palette.
         i = (int) SendDlgItemMessage(hwnd_dialog, IDC_PALETTE, CB_FINDSTRINGEXACT,
                                      num_builtin_palettes - 1, (LPARAM) &str[j + 1]);
         entry->palette = (i != CB_ERR) ? i : DEFAULT_PAL;
      }
   }
   return 1;
}

// Open a file for reading
FILE *open_file(char *file, char *msg)
{
   char s[256];
   FILE *fp;

   if (fopen_s(&fp, file, "rt"))
   {
      if (msg != NULL)
      {
         sprintf_s(s, sizeof(s), "Could not open '%s' for read.%s", file, msg);
         MessageBox(NULL, s, "Warning", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
      }
      return NULL;
   }
   return fp;
}

// Scan the logfile, dynamically allocate an array for the entries, and fill it in
// from the logfile. If init_pos is nonzero, initializes the position to the beginning
int log_read(char *file, char *msg, int init_pos)
{
   int i, count;
   FILE *fp;

   // Kind of inefficient: scan once to get length, then scan again to fill in array
   if ((fp = open_file(file, msg)) == NULL)
      return 0;

   for (count = 0; log_read_entry(NULL, fp); count++)
      ;

   fclose(fp);

   if (!count)
      return 0; // normal cfg files will return here

   if (log_entries != NULL)   // Allocate the array and fill it in
      free(log_entries);
   if ((log_entries = (log_entry *) malloc(count * sizeof(log_entry))) == NULL)
      return 0;

   if ((fp = open_file(file, "")) == NULL)
      return 0;
   for (i = 0; i < count; i++)
      log_read_entry(&log_entries[i], fp);

   fclose(fp);

   log_count = count;
   if (init_pos)
   {
      log_pos = -1;
      file_tot_time = 0.0; // for benchmarking
   }
   return 1;
}

// Open the logfile for appending and add the current image. Reset position if reset_pos is 1.
int log_update(char *file, int reset_pos)
{
   char s[512], p[256];
   FILE *fp;

   if (fopen_s(&fp, file, "at")) // open for append
   {
      sprintf_s(s, sizeof(s), "Could not open '%s' for write.", file);

      MessageBox(NULL, s, NULL, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
      return 0;
   }

   // For palette, use either number (for builtin palette), or "file" for user file
   if (palette < num_builtin_palettes)
      sprintf_s(p, sizeof(p), "%d", palette);
   else
      sprintf_s(p, sizeof(p), "\"%s\"", palette_file);

   // Logfile read function ignores any leading items: may later want to make it require these,
   // to make sure corrupted files don't get us out of sync
   sprintf_s(s, sizeof(s),
              "\n"
              "Real     %-16.16lf\n"
              "Imag     %-16.16lf\n"
              "Mag      %-16lf\n"
              "Iters    %d\n"
              "Palette  %s\n",
              re, im, mag, max_iters, p);

   fputs(s, fp);
   fclose(fp);

   // Now reread the logfile (need to reallocate array) - a bit inefficient but who cares...
   // Keep current position
   return log_read(file, "", reset_pos);
}

// Get the next or prev entry from the log entry array. Returns the entry.
log_entry *log_get(int next_prevn)
{
   log_entry *e;

   if (log_entries == NULL)
      return NULL;

   if (next_prevn)
   {
      if (++log_pos > log_count - 1)
         // log_pos = log_count - 1; // stop at end
         log_pos = 0;                // wrap to beginning
   }
   else
      if (--log_pos < 0)
         // log_pos = 0;            // stop at beginning
         log_pos = log_count - 1;   // wrap to end

   e = &log_entries[log_pos];

   re = e->re;
   im = e->im;
   mag = e->mag;
   max_iters = e->max_iters;
   palette = e->palette;

   return e;
}

// Call this early, before the main window is initialized- could change window size.
void read_cfg_file(void)
{
   // The cfg file is just another logfile, but it shouldn't have any images in it.
   // If it does, the settings will be reset after each image.
   //
   // This file will update the values in the cfg_settings structure. If it's missing,
   // the default settings in the structure will be used.

   invalidate_settings(&cur_file_settings); // initialize all to "no change"

   log_read(CFG_FILE, NULL, 1); // NULL = no error message on failure to read

   copy_changed_settings(&cfg_settings, &cur_file_settings);

   // maybe eliminate these separate variables later
   man_xsize = prev_man_xsize = cfg_settings.xsize.val;
   man_ysize = prev_man_ysize = cfg_settings.ysize.val;
}

// Find all files ending in .pal and add them to the palette dropdown list.
// If calling this more than once (for instance every time the user accesses the
// palette menu), should delete old files from the combo box first.

void add_user_palettes(void)
{
   SendDlgItemMessage(hwnd_dialog, IDC_PALETTE, CB_DIR, DDL_READONLY | DDL_READWRITE, (LPARAM) "*.pal");
}

// Timer function: returns current time in TIME_UNITs (changed from previous versions).
// Can use two different methods based on #define: QueryPerformanceCounter is more
// precise, but has a bug when running on dual-core CPUs. Randomly selects one or
// the other core to read the number from, and gets bogus results if it reads the wrong one.

TIME_UNIT get_timer(void)
{
   #ifdef USE_PERFORMANCE_COUNTER
   TIME_UNIT t;
   QueryPerformanceCounter(&t);
   return t;
   #else
   return timeGetTime();
   #endif
}

// Get the number of seconds elapsed since start_time.
double get_seconds_elapsed(TIME_UNIT start_time)
{
   TIME_UNIT t;
   #ifdef USE_PERFORMANCE_COUNTER
   QueryPerformanceFrequency(&f);
   QueryPerformanceCounter(&t);
   t.QuadPart -= start_time.QuadPart;
   if (f.QuadPart)
      return (double) t.QuadPart / (double) f.QuadPart;
   return 1e10;
   #else
   // Need to use TIME_UNIT (dword) to avoid wrapping issues with timeGetTime().
   // Can subtract in DWORD domain, but not double.
   t = timeGetTime() - start_time;
   return 1e-3 * (double) t;
   #endif
}

// For GetAsyncKeyState: if this bit is set in return value, key is down (MSB of SHORT)
#define KEYDOWN_BIT     0x8000
#define KEY_LEFT        1
#define KEY_RIGHT       2
#define KEY_UP          4
#define KEY_DOWN        8
#define KEY_CTRL        16
#define KEY_ESC         32
#define KEY_SHIFT       64

// Return a bitfield indicating the key(s) pressed. Added ASDW as alternate arrow keys.
int get_keys_pressed(void)
{
   int i, key = 0;
   static SHORT vkeys[] = {VK_LEFT, 'A', VK_RIGHT, 'D', VK_UP, 'W', VK_DOWN, 'S', VK_CONTROL, VK_SHIFT};
   static int keybits[] = {KEY_LEFT, KEY_LEFT, KEY_RIGHT, KEY_RIGHT, KEY_UP, KEY_UP,
                           KEY_DOWN, KEY_DOWN, KEY_CTRL, KEY_SHIFT};

   for (i = 0; i < NUM_ELEM(vkeys); i++)
      if (GetAsyncKeyState(vkeys[i]) & KEYDOWN_BIT)
         key |= keybits[i];

   return key;
}

// Reset thread iters accumulators, used to measure thread load balance
void reset_thread_load_counters(void)
{
   int i;
   for (i = 0; i < MAX_THREADS; i++)
      thread_states[i].total_iters = 0;
}

// Reset frames/sec timing values
void reset_fps_values(void)
{
   total_frames = 0;
   interval_frames = 0;
   calc_interval_time = 0.0;
   calc_total_time = 0.0;
   interval_time = 0.0;
   total_time = 0.0;
}

// Get the real or imaginary coordinate delta based on the pixel delta from
// image center (offs), the image smaller dimension (size), and the magnification.
// This is done as a macro rather than a function, to allow using the full
// 80-bit x87 result where possible.

#define RE_IM_DELTA(offs, size, mag) ((((double) offs * 4.0) / (double) size) / mag)

// Update the image center coordinates based on xoffs and yoffs (pixels from current center).
void update_re_im(int xoffs, int yoffs)
{
   // Maintain precision as long as possible
   re += RE_IM_DELTA(xoffs, man_min_dimension, mag);
   im -= RE_IM_DELTA(yoffs, man_min_dimension, mag);
}

// Set the new point and magnification based on x0, x1, y0, y1. If zoom_box is 0,
// multiplies/divides the magnification by a fixed value. If zoom_box is 1, calculates
// the new zoom from the ratio of the zoom box size (defined by x0, x1, y0, y1)
// to the window size. Recenters the image on x0, y0 (if no zoom box) or the center
// of the zoom box.

void update_re_im_mag(int zoom_box, int in_outn, int x0, int y0, int x1, int y1)
{
   double tmp_mag, xz, yz;
   int x, y;

   tmp_mag = mag;

   if (!zoom_box) // just zoom in or out
   {
      x = x0;     // point center
      y = y0;
      if (in_outn)
         tmp_mag *= MAG_ZOOM_FACTOR;
      else
         tmp_mag *= (1.0 / MAG_ZOOM_FACTOR);
   }
   // Zoom based on box. For zooming in, uses the box size and center to set the
   // new point and magnification
   else
   {
      x = abs(x0 - x1);
      y = abs(y0 - y1);

      // Get smaller of xzoom and yzoom
      xz = (double) man_xsize / (double) x;
      yz = (double) man_ysize / (double) y;

      if (xz < yz)
         tmp_mag *= xz;
      else
         tmp_mag *= yz;

      x = (x0 + x1) >> 1; // new point center:
      y = (y0 + y1) >> 1; // center of zoom box
   }

   // Get new point center, based on x, y

   //if (in_outn) // uncomment to not center when zooming out
   update_re_im(x - (man_xsize >> 1), y - (man_ysize >> 1));

   // Update mag
   if (tmp_mag >= MAG_MIN) // preserve closest min, to allow
      mag = tmp_mag;       // zooming back to original mag
}

// ----------------------- Iteration functions -----------------------------------

// Use this for overhead-sensitive functions (but nothing else, as it can
// hog registers). Seems to have a small effect.
#define FASTCALL __fastcall

// Set these function pointers based on the algorithm in use

// Point queueing function (C/SSE/SSE2/x87)
static void (FASTCALL *queue_point)(mandel_pointstruct *ps_ptr, unsigned *iters_ptr);

// Iteration function (C/SSE/SSE2/x87, AMD/Intel). Returns the number of iterations done per point.
static unsigned (*mandel_iterate)(mandel_pointstruct *ps_ptr);

// Lame unoptimized C iteration function. Just does one point at a time, using point 0
// in the point structure.

static unsigned iterate_c(mandel_pointstruct *ps_ptr)
{
   double a, b, x, y, xx, yy, rad;
   unsigned iters, iter_ct;

   iters = 0;
   iter_ct = ps_ptr->cur_max_iters;

   a = ps_ptr->ab_in[0];
   b = ps_ptr->ab_in[1];
   rad = DIVERGED_THRESH;
   x = y = xx = yy = 0.0;

   do
   {
      y = (x + x) * y + b;
      x = xx - yy + a;
      yy = y * y;
      xx = x * x;
      iters++;
      if ((xx + yy) >= rad)
         break;
   }
   while (--iter_ct);

   ps_ptr->iterctr += iters;
   return iters;
}

// Queue a point to be iterated, for the C iteration function
static void FASTCALL queue_point_c(mandel_pointstruct *ps_ptr, unsigned *iters_ptr)
{
   unsigned iters;

   iters = mandel_iterate(ps_ptr); // No queuing- just iterate 1 point

   // Do this just to match iteration offset of ASM versions
   if (iters != ps_ptr->cur_max_iters)
   {
      iters++;
      ps_ptr->iterctr++;
   }
   *iters_ptr = iters;
}

// Code for ASM iteration functions.

// XMM registers for the SSE2 algorithms
#define xmm_x01   xmm0
#define xmm_y01   xmm1
#define xmm_yy01  xmm2
#define xmm_mag   xmm3
#define xmm_x23   xmm4
#define xmm_y23   xmm5
#define xmm_yy23  xmm6
#define xmm_two   xmm7

// XMM registers for the SSE algorithms
#define xmm_x03   xmm0
#define xmm_y03   xmm1
#define xmm_yy03  xmm2
#define xmm_x47   xmm4
#define xmm_y47   xmm5
#define xmm_yy47  xmm6

// Optimized SSE2 (double precision) ASM algorithm that iterates 4 points at a time. The point
// 2,3 calculations are about a half-iteration behind the point 0,1 calculations, to allow
// improved interleaving of multiplies and adds. Additionally, the loop is unrolled twice and
// divergence detection is done only every 2nd iteration. After the loop finishes, the code
// backs out to check if the points actually diverged during the previous iteration.
//
// The divergence detection is done using integer comparisons on the magnitude exponents
// (the divergence threshold should be a power of two).
//
// 8 total iterations (4 points * 2x unroll) are done per loop. It consists
// of independent, interleaved iteration blocks:
//
// do
// {
// // Points 0,1     Points 2,3
//    y += b;        y *= x;
//    mag += yy;     x *= x;
//    x += a;        y *= 2;
//    yy = y;        mag = x;
//    yy *= yy;      x -= yy;
//    y *= x;        y += b;
//    x *= x;        mag += yy;
//    y *= 2;        x += a;
//    mag = x;       yy = y;
//    x -= yy;       yy *= yy;
//
//    [2nd iter: repeat above blocks]
//
//    if (any 2nd iter mag diverged)
//       break;
// }
// while (--cur_max_iters);
//
// Initial conditions: x = y = yy = 0.0
//
// Note: The mags in the first half of the loop are not actually calculated in
// the loop, but are determined afterwards in the backout calculations from
// stored intermediate values.
//
// From timing measurements, the 53-instruction loop takes ~41 clocks on an Athlon 64 4000+ 2.4 GHz.
//
// Very slow on a Pentium 4 (~100 clocks) due to blocked store fowarding; see Intel code.

static unsigned iterate_amd_sse2(mandel_pointstruct *ps_ptr) // sip4
{
   __asm
   {
   // Tried getting rid of the xmm save/restore by having queue_point load directly into
   // the xmm registers (see qhold_load_xmm.c): overhead reduction is negligible.

   mov      ebx,           ps_ptr      // Get pointstruct pointer. PS4_ macros below reference [ebx + offset]
   movapd   xmm_x23,       PS4_X23     // Restore point states
   movapd   xmm_x01,       PS4_X01
   movapd   xmm_two,       PS4_TWO
   movapd   xmm_y23,       PS4_Y23
   movapd   xmm_y01,       PS4_Y01
   movapd   xmm_yy23,      PS4_YY23

   mov      edx,           DIV_EXP           // Exp for magnitude exponent comparison. Slower to compare to const directly
   mov      ecx,           PS4_CUR_MAX_ITERS // max iters to do this call; always even
   mov      eax,           0                 // iteration counter (for each of the 4 points)
   jmp      skip_top                         // Jump past 1st 2 movapds for loop entry, eliminating
   nop                                       // need to restore yy01- lower overhead
   nop
   nop                                       // Achieve the magic alignment (see below)
   nop
   nop
   nop

   // Found that it's important for the end-of-loop branch to be on a 16-byte boundary
   // (code is slower if not). Choose instructions above to cause this.
   //
   // v1.0 update: The above no longer holds. Now the magic alignment seems random...
   // the number of nops above must be determined by trial and error.

iter_loop:
   movapd   PS4_YY01,      xmm_yy01    // save yy01 for mag backout checking
   movapd   PS4_X01,       xmm_x01     // save x01 for mag backout checking; contains xx01 - yy01 here
skip_top:
   addpd    xmm_y01,       PS4_B01     // y01 += b01; faster at top of loop. Initial y01 = 0
   mulpd    xmm_y23,       xmm_x23     // y23 *= x23; faster at top of loop.
   add      eax,           2           // update iteration counter; faster here than 2 insts below
   mulpd    xmm_x23,       xmm_x23     // x23 *= x23
   addpd    xmm_x01,       PS4_A01     // x01 += a01
   addpd    xmm_y23,       xmm_y23     // y23 *= 2; faster here than mulpd xmm_y23, xmm_two
   movapd   xmm_yy01,      xmm_y01     // yy01 = y01
   movapd   PS4_X23,       xmm_x23     // save xx23 for magnitude backout checking
   mulpd    xmm_yy01,      xmm_yy01    // yy01 *= yy01
   subpd    xmm_x23,       xmm_yy23    // x23 -= yy23
   mulpd    xmm_y01,       xmm_x01     // y01 *= x01
   addpd    xmm_y23,       PS4_B23     // y23 += b23
   mulpd    xmm_x01,       xmm_x01     // x01 *= x01
   movapd   PS4_YY23,      xmm_yy23    // save yy23 for magnitude backout checking
   mulpd    xmm_y01,       xmm_two     // y01 *= 2; add slower here; bb stall
   addpd    xmm_x23,       PS4_A23     // x23 += a23
   movapd   xmm_mag,       xmm_x01     // mag01 = x01
   movapd   xmm_yy23,      xmm_y23     // yy23 = y23
   subpd    xmm_x01,       xmm_yy01    // x01 -= yy01
   mulpd    xmm_yy23,      xmm_yy23    // yy23 *= yy23
   addpd    xmm_y01,       PS4_B01     // y01 += b01
   mulpd    xmm_y23,       xmm_x23     // y23 *= x23
   // ----- Start of 2nd iteration block ------
   addpd    xmm_mag,       xmm_yy01
   mulpd    xmm_x23,       xmm_x23
   addpd    xmm_x01,       PS4_A01
   movapd   xmm_yy01,      xmm_y01     // these 2 instrs: faster in this order than reversed (fixes y23 dep?)
   mulpd    xmm_y23,       xmm_two     // (yy01 is apparently just "marked" to get y01 here; doesn't cause a dep delay)
   movapd   PS4_EXP01,     xmm_mag     // save point 0,1 magnitudes for comparison
   movapd   xmm_mag,       xmm_x23
   mulpd    xmm_yy01,      xmm_yy01
   subpd    xmm_x23,       xmm_yy23
   mulpd    xmm_y01,       xmm_x01
   addpd    xmm_y23,       PS4_B23
   mulpd    xmm_x01,       xmm_x01
   addpd    xmm_mag,       xmm_yy23
   movapd   PS4_EXP23,     xmm_mag     // Save point 2,3 magnitudes. Best here, despite dep
   cmp      PS4_EXP0,      edx         // Compare the magnitude exponents of points 0 and 1
   cmovge   ecx,           eax         // to the divergence threshold. AMD doesn't seem to mind
   cmp      PS4_EXP1,      edx         // the store fowarding issue, but Intel does.
   cmovge   ecx,           eax         // Conditional moves set ecx to eax on divergence,
   mulpd    xmm_y01,       xmm_two     // breaking the loop.
   addpd    xmm_x23,       PS4_A23     // add y, y and mul y, two seem equal speed here
   movapd   xmm_yy23,      xmm_y23
   subpd    xmm_x01,       xmm_yy01
   mulpd    xmm_yy23,      xmm_yy23
   cmp      PS4_EXP2,      edx         // Compare the magnitude exponents of points 2 and 3.
   cmovge   ecx,           eax
   cmp      PS4_EXP3,      edx
   cmovge   ecx,           eax
   cmp      ecx,           eax         // Continue iterating until max iters reached for this call,
   jne      iter_loop                  // or one of the points diverged.

   // Exited loop: save iterating point states, and update point iteration counts. Because
   // divergence detection is only done every 2 iterations, need to "back out" and see if
   // a point diverged the previous iteration. Calculate previous magnitudes from stored
   // values and save in expp. Caller can then use DIVERGED and DIVERGED_PREV macros
   // to detect if/when the point diverged.

   // Structure contents here: PS4.x = xx01 - yy01;  PS4.yy = yy01; PS4.x + 16 = xx23; PS4.yy + 16 = yy23
   // Order here seems fastest, despite dependencies
   // Really only need to calculate prev mags for points that diverged... could save overhead

   movapd   PS4_Y01,       xmm_y01     // save y01 state
   movapd   PS4_Y23,       xmm_y23     // save y23 state

   mulpd    xmm_two,       PS4_YY01    // Use xmm_two for tmp var; tmp1 = 2 * yy01
   movapd   xmm_mag,       PS4_X23     // tmp2 = xx23
   addpd    xmm_two,       PS4_X01     // get mag01 = xx01 - yy01 + 2 * yy01 = xx01 + yy01
   addpd    xmm_mag,       PS4_YY23    // get mag23 = xx23 + yy23
   movapd   PS4_EXPP01,    xmm_two     // store prev_mag 01
   movapd   PS4_EXPP23,    xmm_mag     // store prev_mag 23

   xor      ecx,           ecx         // Get a 0
   add      PS4_ITERCTR_L, eax         // Update iteration counter. Multiply by 36 to get effective flops.
   adc      PS4_ITERCTR_H, ecx         // Update iterctr high dword

   movapd   PS4_YY01,      xmm_yy01    // save yy01 state
   movapd   PS4_X23,       xmm_x23     // save x23 state
   movapd   PS4_X01,       xmm_x01     // save x01 state
   movapd   PS4_YY23,      xmm_yy23    // save yy23 state

   add      PS4_ITERS0,    eax         // update point iteration counts
   add      PS4_ITERS1,    eax
   add      PS4_ITERS2,    eax
   add      PS4_ITERS3,    eax

   // return value (iters done per point) is in eax

   // 0xF3 prefix for AMD single byte return fix: does seem to make a difference
   // Don't put any code between here and ret. Watch for compiler-inserted pops if
   // using extra registers.

   //pop      ebx        // compiler pushes ebx, ebp on entry
   //pop      ebp        // accessing ebp gives compiler warning
   //__emit(0xF3);
   //ret
   }
}

// Intel version. Basically the same as the AMD version, but doesn't do magnitude
// comparison in the INT domain because this causes a blocked store-forwarding
// penalty (from XMM store to INT load) of 40-50 clock cycles. Uses cmppd/movmskpd instead.
//
// Despite Intel's high memory and mulpd latency, the CPU seems to do a good job of
// reordering operations to hide it (dependent instructions are too close together
// here but this doesn't cause much delay). This code is faster than an older version
// that preloads values from memory into a tmp XMM register (see old.txt)

// Changed to use same initial conditions as AMD code. No consistent speed difference
// vs. old version. Appears to be about 0.3% slower on the home benchmark, but
// 0.5% faster on bmark.log.

// From timing measurements, the loop takes 50 clocks on a Pentium D 820 2.8 GHz.

static unsigned iterate_intel_sse2(mandel_pointstruct *ps_ptr) // sip4
{
   __asm
   {
   mov      ebx,           ps_ptr      // Get pointstruct pointer
   movapd   xmm_yy01,      PS4_YY01    // Restore point states
   movapd   xmm_y01,       PS4_Y01
   movapd   xmm_x23,       PS4_X23
   movapd   xmm_x01,       PS4_X01
   movapd   xmm_two,       PS4_TWO
   movapd   xmm_y23,       PS4_Y23
   movapd   xmm_yy23,      PS4_YY23
   addpd    xmm_y01,       PS4_B01     // pre-add y01 to get correct initial condition

   mov      eax,           2           // iteration counter (for each of the 4 points)
   jmp      skip_top

iter_loop:                             // alignment doesn't seem to matter on Intel
   movapd   PS4_YY01,      xmm_yy01    // save yy01 for mag backout checking
   movapd   PS4_X01,       xmm_x01     // save x01 for mag backout checking; contains xx01 - yy01 here
   add      eax,           2           // update iteration counter
skip_top:
   mulpd    xmm_x23,       xmm_x23     // x23 *= x23
   addpd    xmm_x01,       PS4_A01     // x01 += a01
   addpd    xmm_y23,       xmm_y23     // y23 *= 2; faster here than mulpd xmm_y23, xmm_two
   movapd   xmm_yy01,      xmm_y01     // yy01 = y01
   movapd   PS4_X23,       xmm_x23     // save xx23 for magnitude backout checking
   mulpd    xmm_yy01,      xmm_yy01    // yy01 *= yy01
   subpd    xmm_x23,       xmm_yy23    // x23 -= yy23
   mulpd    xmm_y01,       xmm_x01     // y01 *= x01
   addpd    xmm_y23,       PS4_B23     // y23 += b23
   mulpd    xmm_x01,       xmm_x01     // x01 *= x01
   movapd   PS4_YY23,      xmm_yy23    // save yy23 for magnitude backout checking
   mulpd    xmm_y01,       xmm_two     // y01 *= 2; add slower here
   addpd    xmm_x23,       PS4_A23     // x23 += a23
   movapd   xmm_mag,       xmm_x01     // mag01 = x01
   movapd   xmm_yy23,      xmm_y23     // yy23 = y23
   subpd    xmm_x01,       xmm_yy01    // x01 -= yy01
   mulpd    xmm_yy23,      xmm_yy23    // yy23 *= yy23
   addpd    xmm_y01,       PS4_B01     // y01 += b01
   mulpd    xmm_y23,       xmm_x23     // y23 *= x23
   // ----- Start of 2nd iteration block ------
   addpd    xmm_mag,       xmm_yy01
   mulpd    xmm_x23,       xmm_x23
   addpd    xmm_x01,       PS4_A01
   mulpd    xmm_y23,       xmm_two
   movapd   xmm_yy01,      xmm_y01
   cmpnltpd xmm_mag,       PS4_RAD     // compare point 0, 1 magnitudes (mag >= rad): let cpu reorder these
   movmskpd edx,           xmm_mag     // save result in edx
   movapd   xmm_mag,       xmm_x23
   mulpd    xmm_yy01,      xmm_yy01
   subpd    xmm_x23,       xmm_yy23
   mulpd    xmm_y01,       xmm_x01
   addpd    xmm_y23,       PS4_B23
   mulpd    xmm_x01,       xmm_x01
   addpd    xmm_mag,       xmm_yy23
   mulpd    xmm_y01,       xmm_two
   addpd    xmm_x23,       PS4_A23
   add      edx,           edx         // shift point 01 mag compare results left 2
   add      edx,           edx
   movapd   xmm_yy23,      xmm_y23
   subpd    xmm_x01,       xmm_yy01
   cmpnltpd xmm_mag,       PS4_RAD     // compare point 2, 3 magnitudes
   mulpd    xmm_yy23,      xmm_yy23
   addpd    xmm_y01,       PS4_B01
   movmskpd ecx,           xmm_mag
   mulpd    xmm_y23,       xmm_x23
   or       ecx,           edx         // Continue iterating until max iters reached for this call,
   jnz      done                       // or one of the points diverged.
   cmp      PS4_CUR_MAX_ITERS, eax     // No penalty for comparing from memory vs. register here
   jne      iter_loop

done:
   subpd    xmm_y01,       PS4_B01     // subtract out pre-add (see loop top)
   movapd   PS4_Y01,       xmm_y01     // save y01 state
   movapd   PS4_Y23,       xmm_y23     // save y23 state

   // Get previous magnitudes. See AMD code
   mulpd    xmm_two,       PS4_YY01    // Use xmm_two for tmp var; tmp1 = 2 * yy01
   movapd   xmm_mag,       PS4_X23     // tmp2 = xx23
   addpd    xmm_two,       PS4_X01     // get mag01 = xx01 - yy01 + 2 * yy01 = xx01 + yy01
   addpd    xmm_mag,       PS4_YY23    // get mag23 = xx23 + yy23
   movapd   PS4_EXPP01,    xmm_two     // store prev_mag 01
   movapd   PS4_EXPP23,    xmm_mag     // store prev_mag 23

   xor      edx,           edx         // Get a 0
   add      PS4_ITERCTR_L, eax         // Update iteration counter. Multiply by 36 to get effective flops.
   adc      PS4_ITERCTR_H, edx         // Update iterctr high dword

   movapd   PS4_YY01,      xmm_yy01    // save yy01 state
   movapd   PS4_X23,       xmm_x23     // save x23 state
   movapd   PS4_X01,       xmm_x01     // save x01 state
   movapd   PS4_YY23,      xmm_yy23    // save yy23 state

   add      PS4_ITERS0,    eax         // update point iteration counts
   add      PS4_ITERS1,    eax
   add      PS4_ITERS2,    eax
   add      PS4_ITERS3,    eax

   // return value (iterations done per point) is in eax - don't modify.

   // Create diverged flags compatible with AMD version: store diverged exponent
   // threshold for each point that diverged, else a 0 exponent.
   // ECX contains the 4 diverged bits here (max value = 15)

   mov      edx,           DIV_EXP        // Exp for magnitude exponent comparison
   test     ecx,           8              // point 1
   cmovz    edx,           ecx            // cmov better here than unpredictable branches
   mov      PS4_EXP1,      edx

   mov      edx,           DIV_EXP
   test     ecx,           4              // point 0
   cmovz    edx,           ecx
   mov      PS4_EXP0,      edx

   mov      edx,           DIV_EXP
   test     ecx,           2              // point 3
   cmovz    edx,           ecx
   mov      PS4_EXP3,      edx

   mov      edx,           DIV_EXP
   test     ecx,           1              // point 2
   cmovz    edx,           ecx
   mov      PS4_EXP2,      edx
   }
}

// SSE (single precision) algorithm for AMD; iterates 8 points at a time,
// or 16 iterations per loop. Based on AMD SSE2 algorithm.

// Loop appears to take 42.25 clocks- the 8 extra int compare/cmov
// instructions add 2 extra clocks per loop vs. the SSE2 algorithm.
// Tried rearranging them but current order seems best.

static unsigned iterate_amd_sse(mandel_pointstruct *ps_ptr) // sip8
{
   __asm
   {
   mov      ebx,           ps_ptr            // Get pointstruct pointer
   movaps   xmm_x47,       PS8_X47           // Restore point states
   movaps   xmm_x03,       PS8_X03
   movaps   xmm_two,       PS8_TWO
   movaps   xmm_y47,       PS8_Y47
   movaps   xmm_y03,       PS8_Y03
   movaps   xmm_yy47,      PS8_YY47

   mov      edx,           DIV_EXP_FLOAT     // Exp for magnitude exponent comparison. Slower to compare to const directly
   mov      ecx,           PS8_CUR_MAX_ITERS // max iters to do this call; always even
   mov      eax,           0                 // Iteration counter (for each of the 4 points)
   jmp      skip_top                         // Allows removing yy03 restore above
   nop
   nop                                       // Achieve the magic alignment...
   nop
   nop
   nop
   nop
   nop
   nop
   nop

iter_loop:
   movaps   PS8_YY03,      xmm_yy03    // save yy03 for mag backout checking
   movaps   PS8_X03,       xmm_x03     // save x03 for mag backout checking; contains xx03 - yy03 here
skip_top:
   addps    xmm_y03,       PS8_B03     // y03 += b03; faster at top of loop. Initial y03 = 0
   mulps    xmm_y47,       xmm_x47     // y47 *= x47; faster at top of loop.
   add      eax,           2           // update iteration counter; faster here than 2 insts below
   mulps    xmm_x47,       xmm_x47     // x47 *= x47
   addps    xmm_x03,       PS8_A03     // x03 += a03
   addps    xmm_y47,       xmm_y47     // y47 *= 2; faster here than mulps xmm_y47, xmm_two
   movaps   xmm_yy03,      xmm_y03     // yy03 = y03
   movaps   PS8_X47,       xmm_x47     // save xx47 for magnitude backout checking
   mulps    xmm_yy03,      xmm_yy03    // yy03 *= yy03
   subps    xmm_x47,       xmm_yy47    // x47 -= yy47
   mulps    xmm_y03,       xmm_x03     // y03 *= x03
   addps    xmm_y47,       PS8_B47     // y47 += b47
   mulps    xmm_x03,       xmm_x03     // x03 *= x03
   movaps   PS8_YY47,      xmm_yy47    // save yy47 for magnitude backout checking
   mulps    xmm_y03,       xmm_two     // y03 *= 2; add slower here; bb stall
   addps    xmm_x47,       PS8_A47     // x47 += a47
   movaps   xmm_mag,       xmm_x03     // mag03 = x03
   movaps   xmm_yy47,      xmm_y47     // yy47 = y47
   subps    xmm_x03,       xmm_yy03    // x03 -= yy03
   mulps    xmm_yy47,      xmm_yy47    // yy47 *= yy47
   addps    xmm_y03,       PS8_B03     // y03 += b03
   mulps    xmm_y47,       xmm_x47     // y47 *= x47
   // ----- Start of 2nd iteration block ------
   addps    xmm_mag,       xmm_yy03
   mulps    xmm_x47,       xmm_x47
   addps    xmm_x03,       PS8_A03
   movaps   xmm_yy03,      xmm_y03     // these 2 instrs: faster in this order than reversed (fixes y47 dep?)
   mulps    xmm_y47,       xmm_two     // (yy03 is apparently just "marked" to get y03 here; doesn't cause a dep delay)
   movaps   PS8_EXP03,     xmm_mag     // save point 0-3 magnitudes for comparison
   movaps   xmm_mag,       xmm_x47
   mulps    xmm_yy03,      xmm_yy03
   subps    xmm_x47,       xmm_yy47
   mulps    xmm_y03,       xmm_x03
   addps    xmm_y47,       PS8_B47
   mulps    xmm_x03,       xmm_x03
   addps    xmm_mag,       xmm_yy47
   movaps   PS8_EXP47,     xmm_mag     // Save point 4-7 magnitudes. Best here, despite dep
   cmp      PS8_EXP0,      edx         // Compare the magnitude exponents of points 0-3
   cmovge   ecx,           eax         // to the divergence threshold. AMD doesn't seem to mind
   cmp      PS8_EXP1,      edx         // the store fowarding issue, but Intel does.
   cmovge   ecx,           eax         // Conditional moves set ecx to eax on divergence,
   cmp      PS8_EXP2,      edx         // breaking the loop.
   cmovge   ecx,           eax
   cmp      PS8_EXP3,      edx
   cmovge   ecx,           eax
   mulps    xmm_y03,       xmm_two     // add y, y and mul y, two seem equal speed here
   addps    xmm_x47,       PS8_A47
   movaps   xmm_yy47,      xmm_y47
   subps    xmm_x03,       xmm_yy03
   mulps    xmm_yy47,      xmm_yy47
   cmp      PS8_EXP4,      edx         // Compare the magnitude exponents of points 4-7.
   cmovge   ecx,           eax
   cmp      PS8_EXP5,      edx
   cmovge   ecx,           eax
   cmp      PS8_EXP6,      edx
   cmovge   ecx,           eax
   cmp      PS8_EXP7,      edx
   //cmovge   ecx,           eax       // jge done seems a hair faster. When changing instrs, don't forget
   jge      done                       // to adjust nops to put jne iter_loop on a 16-byte boundary.
   cmp      ecx,           eax         // Continue iterating until max iters reached for this call,
   jne      iter_loop                  // or one of the points diverged.

done:
   // Get previous magnitudes. See AMD SSE2 code
   movaps   PS8_Y03,       xmm_y03     // save y03 state
   movaps   PS8_Y47,       xmm_y47     // save y47 state

   mulps    xmm_two,       PS8_YY03    // Use xmm_two for tmp var; tmp1 = 2 * yy03
   movaps   xmm_mag,       PS8_X47     // tmp2 = xx47
   addps    xmm_two,       PS8_X03     // get mag03 = xx03 - yy03 + 2 * yy03 = xx03 + yy03
   addps    xmm_mag,       PS8_YY47    // get mag47 = xx47 + yy47
   movaps   PS8_EXPP03,    xmm_two     // store prev_mag 03
   movaps   PS8_EXPP47,    xmm_mag     // store prev_mag 47

   xor      ecx,           ecx         // Get a 0
   add      PS8_ITERCTR_L, eax         // Update iteration counter. Multiply by 72 to get effective flops.
   adc      PS8_ITERCTR_H, ecx         // Update iterctr high dword

   movaps   PS8_YY03,      xmm_yy03    // save yy03 state
   movaps   PS8_X47,       xmm_x47     // save x47 state
   movaps   PS8_X03,       xmm_x03     // save x03 state
   movaps   PS8_YY47,      xmm_yy47    // save yy47 state

   add      PS8_ITERS0,    eax         // update point iteration counts
   add      PS8_ITERS1,    eax
   add      PS8_ITERS2,    eax
   add      PS8_ITERS3,    eax
   add      PS8_ITERS4,    eax
   add      PS8_ITERS5,    eax
   add      PS8_ITERS6,    eax
   add      PS8_ITERS7,    eax

   // return value (iterations done per point) is in eax

   // 0xF3 prefix for AMD single byte return fix: does seem to make a difference
   // Don't put any code between here and ret
   // pop ebx
   // pop ebp
   // __emit(0xF3);
   // ret
   }
}

// SSE (single precision) algorithm for Intel; iterates 8 points at a time,
// or 16 iterations per loop. Based on Intel SSE2 algorithm.
static unsigned iterate_intel_sse(mandel_pointstruct *ps_ptr) // sip8
{
   __asm
   {
   mov      ebx,           ps_ptr      // Get pointstruct pointer
   movaps   xmm_yy03,      PS8_YY03    // Restore point states
   movaps   xmm_x47,       PS8_X47
   movaps   xmm_x03,       PS8_X03
   movaps   xmm_two,       PS8_TWO
   movaps   xmm_y47,       PS8_Y47
   movaps   xmm_y03,       PS8_Y03
   movaps   xmm_yy47,      PS8_YY47
   addps    xmm_y03,       PS8_B03     // pre-add y03 to get correct initial condition

   mov      eax,           2           // iteration counter (for each of the 4 points)
   jmp      skip_top

iter_loop:                             // alignment doesn't seem to matter on Intel
   movaps   PS8_YY03,      xmm_yy03    // save yy03 for mag backout checking
   movaps   PS8_X03,       xmm_x03     // save x03 for mag backout checking; contains xx03 - yy03 here
   add      eax,           2           // update iteration counter
skip_top:
   mulps    xmm_x47,       xmm_x47     // x47 *= x47
   addps    xmm_x03,       PS8_A03     // x03 += a03
   addps    xmm_y47,       xmm_y47     // y47 *= 2; faster here than mulps xmm_y47, xmm_two
   movaps   xmm_yy03,      xmm_y03     // yy03 = y03
   movaps   PS8_X47,       xmm_x47     // save xx47 for magnitude backout checking
   mulps    xmm_yy03,      xmm_yy03    // yy03 *= yy03
   subps    xmm_x47,       xmm_yy47    // x47 -= yy47
   mulps    xmm_y03,       xmm_x03     // y03 *= x03
   addps    xmm_y47,       PS8_B47     // y47 += b47
   mulps    xmm_x03,       xmm_x03     // x03 *= x03
   movaps   PS8_YY47,      xmm_yy47    // save yy47 for magnitude backout checking
   mulps    xmm_y03,       xmm_two     // y03 *= 2; add slower here
   addps    xmm_x47,       PS8_A47     // x47 += a47
   movaps   xmm_mag,       xmm_x03     // mag03 = x03
   movaps   xmm_yy47,      xmm_y47     // yy47 = y47
   subps    xmm_x03,       xmm_yy03    // x03 -= yy03
   mulps    xmm_yy47,      xmm_yy47    // yy47 *= yy47
   addps    xmm_y03,       PS8_B03     // y03 += b03
   mulps    xmm_y47,       xmm_x47     // y47 *= x47
   // ----- Start of 2nd iteration block ------
   addps    xmm_mag,       xmm_yy03
   mulps    xmm_x47,       xmm_x47
   addps    xmm_x03,       PS8_A03
   mulps    xmm_y47,       xmm_two
   movaps   xmm_yy03,      xmm_y03
   cmpnltps xmm_mag,       PS8_RAD     // compare point 0-3 magnitudes (mag >= rad): let cpu reorder these
   movmskps edx,           xmm_mag     // save result in edx
   movaps   xmm_mag,       xmm_x47
   mulps    xmm_yy03,      xmm_yy03
   subps    xmm_x47,       xmm_yy47
   mulps    xmm_y03,       xmm_x03
   addps    xmm_y47,       PS8_B47
   mulps    xmm_x03,       xmm_x03
   addps    xmm_mag,       xmm_yy47
   mulps    xmm_y03,       xmm_two
   addps    xmm_x47,       PS8_A47
   shl      edx,           4           // shift point 0-3 mag compare results left 4
   movaps   xmm_yy47,      xmm_y47
   subps    xmm_x03,       xmm_yy03
   cmpnltps xmm_mag,       PS8_RAD     // compare point 4-7 magnitudes
   mulps    xmm_yy47,      xmm_yy47
   addps    xmm_y03,       PS8_B03
   movmskps ecx,           xmm_mag
   mulps    xmm_y47,       xmm_x47
   or       ecx,           edx         // Continue iterating until max iters reached for this call,
   jnz      done                       // or one of the points diverged.
   cmp      PS8_CUR_MAX_ITERS, eax     // No penalty for comparing from memory vs. register here
   jne      iter_loop

 done:
   subps    xmm_y03,       PS8_B03     // subtract out pre-add (see loop top)
   movaps   PS8_Y03,       xmm_y03     // save y03 state
   movaps   PS8_Y47,       xmm_y47     // save y47 state

   // Get previous magnitudes. See AMD SSE2 code
   mulps    xmm_two,       PS8_YY03    // Use xmm_two for tmp var; tmp1 = 2 * yy03
   movaps   xmm_mag,       PS8_X47     // tmp2 = xx47
   addps    xmm_two,       PS8_X03     // get mag03 = xx03 - yy03 + 2 * yy03 = xx03 + yy03
   addps    xmm_mag,       PS8_YY47    // get mag47 = xx47 + yy47
   movaps   PS8_EXPP03,    xmm_two     // store prev_mag 03
   movaps   PS8_EXPP47,    xmm_mag     // store prev_mag 47

   xor      edx,           edx         // Get a 0
   add      PS8_ITERCTR_L, eax         // Update iteration counter. Multiply by 72 to get effective flops.
   adc      PS8_ITERCTR_H, edx         // Update iterctr high dword

   movaps   PS8_YY03,      xmm_yy03    // save yy03 state
   movaps   PS8_X47,       xmm_x47     // save x47 state
   movaps   PS8_X03,       xmm_x03     // save x03 state
   movaps   PS8_YY47,      xmm_yy47    // save yy47 state

   add      PS8_ITERS0,    eax         // update point iteration counts
   add      PS8_ITERS1,    eax
   add      PS8_ITERS2,    eax
   add      PS8_ITERS3,    eax
   add      PS8_ITERS4,    eax
   add      PS8_ITERS5,    eax
   add      PS8_ITERS6,    eax
   add      PS8_ITERS7,    eax

   // Create diverged flags compatible with AMD version: store diverged exponent
   // threshold for each point that diverged, else a 0 exponent.
   // Making a separate version of queue_point for Intel to eliminate the need for
   // this doesn't result in any measurable overhead reduction.

   // ECX contains the 8 diverged bits here (max value = 255)

   mov      edx,           DIV_EXP_FLOAT  // Exp for magnitude exponent comparison
   test     ecx,           128            // point 3
   cmovz    edx,           ecx            // cmov better here than unpredictable branches
   mov      PS8_EXP3,      edx

   mov      edx,           DIV_EXP_FLOAT
   test     ecx,           64             // point 2
   cmovz    edx,           ecx
   mov      PS8_EXP2,      edx

   mov      edx,           DIV_EXP_FLOAT
   test     ecx,           32             // point 1
   cmovz    edx,           ecx
   mov      PS8_EXP1,      edx

   mov      edx,           DIV_EXP_FLOAT
   test     ecx,           16             // point 0
   cmovz    edx,           ecx
   mov      PS8_EXP0,      edx

   mov      edx,           DIV_EXP_FLOAT
   test     ecx,           8              // point 7
   cmovz    edx,           ecx
   mov      PS8_EXP7,      edx

   mov      edx,           DIV_EXP_FLOAT
   test     ecx,           4              // point 6
   cmovz    edx,           ecx
   mov      PS8_EXP6,      edx

   mov      edx,           DIV_EXP_FLOAT
   test     ecx,           2              // point 5
   cmovz    edx,           ecx
   mov      PS8_EXP5,      edx

   mov      edx,           DIV_EXP_FLOAT
   test     ecx,           1              // point 4
   cmovz    edx,           ecx
   mov      PS8_EXP4,      edx
   }
}

// Queuing functions

// The queue_status field of the thread_state structure keeps track of which point
// queue slots are free. Each 3 bits gives a free slot.
// Push: shift left 3, OR in free slot number.
// Pop: slot number = low 3 bits, shift right 3.
// Queue is full when queue_status == QUEUE_FULL.
// Initialize with 3, 2, 1, 0 in bits 11-0, QUEUE_FULL in bits 15-12
// Also used for single precision algorithm, to track up to 8 queue slots

#define QUEUE_FULL   0xF

// Condition indicating point[ind] diverged
#define DIVERGED(p, ind)         (p->exp[1 + (ind << 1)] >= DIV_EXP)

// Condition indicating point[ind] diverged on the previous iteration
#define DIVERGED_PREV(p, ind)    (p->expp[1 + (ind << 1)] >= DIV_EXP)

// For single-precision (SSE) version
#define DIVERGED_S(p, ind)       (p->exp[ind] >= DIV_EXP_FLOAT)
#define DIVERGED_PREV_S(p, ind)  (p->expp[ind] >= DIV_EXP_FLOAT)

// Queue a point for iteration using the 4-point SSE2 algorithm. On entry, ps_ptr->ab_in
// should contain the real and imaginary parts of the point to iterate on, and
// iters_ptr should be the address of the point in the iteration count array.
//
// Can't assume all 4 point b's will be the same (pixels on the previous line may
// still be iterating).

// Rewriting this in ASM would probably save a lot of overhead (important for
// realtime zooming and panning)

static void FASTCALL queue_4point_sse2(mandel_pointstruct *ps_ptr, unsigned *iters_ptr) // sqp
{
   unsigned i, iters, max;
   unsigned queue_status;

   queue_status = ps_ptr->queue_status;

   if (queue_status == QUEUE_FULL) // If all points in use, iterate to clear at least one point first
   {
      mandel_iterate(ps_ptr);    // Returns (iters done) if any point hit max iterations, or diverged
      max = 0;
      for (i = 0; i < 4; i++) // compiler fully unrolls this
      {
         // Find which point(s) are done and retire them (store iteration count to array).
         // Iteration counts will later be mapped to colors using the current palette.
         // Timing test: removing array stores results in NO time savings on bmark.log.

         iters = ps_ptr->iters[i];
         if (DIVERGED(ps_ptr, i))
         {
            // If actually diverged on previous iteration, dec iters. This form avoids an
            // (unpredictable) conditional branch
            *ps_ptr->iters_ptr[i] = iters - DIVERGED_PREV(ps_ptr, i);

            // Push free slot. Use this form to allow compiler to use the lea instruction
            queue_status = queue_status * 8 + i;
         }
         // Gets here most often. See if this point has the most accumulated iterations.
         // Also check if point reached max iters and retire if so. Definite overhead
         // improvement to combine the max iters check with the max check- measurable with
         // small max_iters (e.g., when realtime zooming)
         else
            if (iters >= max)
               if (iters == max_iters)
               {
                  *ps_ptr->iters_ptr[i] = iters;
                  queue_status = queue_status * 8 + i;  // Push free slot
               }
               else
                  max = iters;

      }
      // Set the maximum iterations to do next loop: max iters - iters already done.
      // The next loop must break if the point with the most accumulated iterations (max)
      // reaches max_iters.
      ps_ptr->cur_max_iters = max_iters - max;

      // Most common case: comes here with one point free. Retiring multiple points only
      // happens about 1-5% of the time for complex images. For images with vast areas
      // of a single color, can go to 50%.
   }

   i = queue_status & 3;                     // Get next free slot
   ps_ptr->queue_status = queue_status >> 3; // Pop free slot

   // Initialize pointstruct fields
   ps_ptr->a[i] = ps_ptr->ab_in[0]; // Set input point
   ps_ptr->b[i] = ps_ptr->ab_in[1];
   ps_ptr->y[i] = 0.0;              // Set initial conditions
   ps_ptr->x[i] = 0.0;
   ps_ptr->yy[i] = 0.0;
   ps_ptr->iters[i] = 0;
   ps_ptr->iters_ptr[i] = iters_ptr;
}

// Similar queuing function for the 8-point SSE algorithm
static void FASTCALL queue_8point_sse(mandel_pointstruct *ps_ptr, unsigned *iters_ptr) // sqp8
{
   unsigned i, iters, max;
   unsigned queue_status;

   queue_status = ps_ptr->queue_status;

   if (queue_status == QUEUE_FULL)
   {
      mandel_iterate(ps_ptr);

      max = 0;
      for (i = 0; i < 8; i++)
      {
         iters = ps_ptr->iters[i];
         if (DIVERGED_S(ps_ptr, i))
         {
            *ps_ptr->iters_ptr[i] = iters - DIVERGED_PREV_S(ps_ptr, i);
            queue_status = queue_status * 8 + i;
         }
         else
            if (iters >= max)
               if (iters == max_iters)
               {
                  *ps_ptr->iters_ptr[i] = iters;
                  queue_status = queue_status * 8 + i;
               }
               else
                  max = iters;
      }
      ps_ptr->cur_max_iters = max_iters - max;
   }

   i = queue_status & 7;
   ps_ptr->queue_status = queue_status >> 3;

   // Initialize pointstruct fields as packed 32-bit floats
   ((float *) ps_ptr->a)[i] = (float) ps_ptr->ab_in[0];  // Set input point- convert from doubles
   ((float *) ps_ptr->b)[i] = (float) ps_ptr->ab_in[1];  // generated by the main loop
   ((float *) ps_ptr->y)[i] = 0.0;                       // Set initial conditions
   ((float *) ps_ptr->x)[i] = 0.0;
   ((float *) ps_ptr->yy)[i] = 0.0;
   ps_ptr->iters[i] = 0;
   ps_ptr->iters_ptr[i] = iters_ptr;
}

// Calculate the image, using the currently set precision and algorithm. Calculations in here are
// always done in double (or extended) precision, regardless of the iteration algorithm's precision.

// Now called from multiple threads. Calculates a list of stripes from the thread state structure
// (passed in PARAM). See man_calculate().

unsigned __stdcall man_calculate_threaded(LPVOID param) // smc
{
   int i, n, x, y, xstart, xend, ystart, yend, line_size;
   unsigned *iters_ptr, dummy, points_guessed;
   mandel_pointstruct *ps_ptr;
   thread_state *t;
   stripe *s;

   t = (thread_state *) param;
   s = t->stripes;
   n = t->num_stripes;
   ps_ptr = t->ps_ptr;

   line_size = iter_data_line_size;
   points_guessed = 0;

   // Calculate all the stripes. Needs to handle num_stripes == 0
   for (i = 0; i < n; i++)
   {
      // Use these for benchmarking thread creation/execution overhead
      // return 0;                  // for CreateThread method
      // SetEvent(t->done_event);   // for QueueUserWorkItem method
      // return 0;

      xstart = s->xstart;
      xend = s->xend;
      ystart = s->ystart;
      yend = s->yend;

      // Optimization for panning: set alg to exact mode for very thin regions. Due to the
      // fast algorithm's 4x4 cell size it often computes more pixels than exact for these
      // regions. Effect is most apparent with high iter count images.

      #define FE_SWITCHOVER_THRESH  2 // only do it for 1-pixel wide regions for now. best value TBD...

      cur_alg = alg;
      if ((xend - xstart) < FE_SWITCHOVER_THRESH ||
          (yend - ystart) < FE_SWITCHOVER_THRESH )
         cur_alg |= ALG_EXACT;

      // Main loop. Queue each point in the image for iteration. Queue_point will return
      // immediately if its queue isn't full (needs 4 points for the asm version), otherwise
      // it will iterate on all the points in the queue.

      if (cur_alg & ALG_EXACT) // Exact algorithm: calculates every pixel
      {
         y = ystart;
         do
         {
            x = xstart;
            ps_ptr->ab_in[1] = img_im[y];    // Load IM coordinate from the array
            iters_ptr = iter_data + y * line_size + x;
            do
            {
               ps_ptr->ab_in[0] = img_re[x]; // Load RE coordinate from the array
               queue_point(ps_ptr, iters_ptr++);
            }
            while (++x <= xend);
         }
         while (++y <= yend);
      }
      else // Fast "wave" algorithm from old code: guesses pixels.
      {
         int wave, xoffs, inc, p0, p1, p2, p3, offs0, offs1, offs2, offs3;

         // Doing the full calculation (all waves) on horizontal chunks to improve cache locality
         // gives no speedup (tested before realtime zooming was implemented- maybe should test again).

         for (wave = 0; wave < 7; wave++)
         {
            inc = wave_inc[wave];
            y = wave_ystart[wave] + ystart;

            // Special case for wave 0 (always calculates all pixels). Makes realtime
            // zooming measurably faster. X starts at xstart for wave 0, so can use do-while.
            // For Y, need to calculate all waves even if out of range, because subsequent
            // waves look forward to pixels calculated in previous waves (wave 0 starts at y = 3)

            if (!wave) // it's faster with the special case inside the wave loop than outside
            {
               do
               {
                  x = xstart;
                  ps_ptr->ab_in[1] = img_im[y];    // Load IM coordinate from the array
                  iters_ptr = iter_data + y * line_size + x; // adding a line to the ptr every y loop is slower
                  do
                  {
                     ps_ptr->ab_in[0] = img_re[x]; // Load RE coordinate from the array
                     queue_point(ps_ptr, iters_ptr);
                     iters_ptr += inc;
                     x += inc;
                  }
                  while (x <= xend);
               }
               while ((y += inc) <= yend);
            }
            else  // waves 1-6 check neighboring pixels
            {
               offs0 = wave_ptr_offs[wave][0]; // pointer offsets of neighboring pixels
               offs1 = wave_ptr_offs[wave][1];
               offs2 = wave_ptr_offs[wave][2];
               offs3 = wave_ptr_offs[wave][3];

               xoffs = wave_xstart[wave] + xstart;

               do
               {
                  x = xoffs;
                  ps_ptr->ab_in[1] = img_im[y];
                  iters_ptr = iter_data + y * line_size + x;

                  // No faster to have a special case for waves 1 and 4 that loads only 2 pixels/loop
                  while (x <= xend)
                  {
                     // If all 4 neighboring pixels (p0 - p3) are the same, set this pixel to
                     // their value, else iterate.

                     p0 = iters_ptr[offs0];
                     p1 = iters_ptr[offs1];
                     p2 = iters_ptr[offs2];
                     p3 = iters_ptr[offs3];

                     if (p0 == p1 && p0 == p2 && p0 == p3) // can't use sum compares here (causes corrupted pixels)
                     {
                        // aargh... compiler (or AMD CPU) generates different performance on
                        // zoomtest depending on which point is stored here. They're all the same...
                        // p3: 18.5s  p2: 18.3s  p1: 18.7s  p0: 19.2s  (+/- 0.1s repeatability)

                        *iters_ptr = p2;
                        points_guessed++;
                     }
                     else
                     {
                        ps_ptr->ab_in[0] = img_re[x]; // Load RE coordinate from the array
                        queue_point(ps_ptr, iters_ptr);
                     }
                     iters_ptr += inc;
                     x += inc;
                  }
               }
               while ((y += inc) <= yend);
            }
            // really should flush at the end of each wave, but any errors should have no visual effect
         }
      }
      s++;  // go to next stripe
   }        // end of stripe loop

   t->total_iters += ps_ptr->iterctr;   // accumulate iters, for thread load balance measurement
   t->points_guessed = points_guessed;

   // Up to 4 points could be left in the queue (or 8 for SSE). Queue non-diverging dummy points
   // to flush them out. This is tricky. Be careful changing it... can cause corrupted pixel bugs.
   // Turns out that 4 more points (8 for SSE) must always be queued. They could be stored
   // to the dummy value if all points left in the queue still have max_iters remaining.

   ps_ptr->ab_in[0] = 0.0;
   ps_ptr->ab_in[1] = 0.0;

   // Add some extra logic here to get the exact iteration count (i.e, exclude dummy iterations).
   // It's actually pretty tough to calculate
   for (i = calc_precision == PRECISION_SINGLE ? 8 : 4; i--;)
      queue_point(ps_ptr, &dummy);

   // Thread 0 always runs in the master thread, so doesn't need to signal. Save overhead.
   if (t->thread_num)
      SetEvent(t->done_event); // For other threads, signal master thread that we're done

   return 0;
}

// Check for precision loss- occurs if the two doubles (or converted floats)
// in ptest are equal to each other. Returns PLOSS_FLOAT for float loss,
// PLOSS_DOUBLE for double loss, etc. or 0 for no loss.

// New more conservative version demands that bits beyond the lsb should also
// differ. If only the lsb differs, bound to get degradation during iteration.

#define PLOSS_DOUBLE    2
#define PLOSS_FLOAT     1

int check_precision_loss(double *ptest)
{
   float f[2];
   int i0[2], i1[2];

   // Check double loss
   i0[0] = *((int *) &ptest[0]) & ~1;  // get low dword of 1st double; mask off lsb
   i0[1] = *((int *) &ptest[0] + 1);   // get high dword

   i1[0] = *((int *) &ptest[1]) & ~1;  // get low dword of 2nd double; mask off lsb
   i1[1] = *((int *) &ptest[1] + 1);   // get high dword

   if ((i0[1] == i1[1]) && (i0[0] == i1[0]))
      return PLOSS_DOUBLE | PLOSS_FLOAT; // double loss is also float loss

   // Check float loss
   f[0] = (float) ptest[0];            // convert doubles to floats
   f[1] = (float) ptest[1];

   i0[0] = *((int *) &f[0]) & ~1;      // get 1st float; mask off lsb
   i1[0] = *((int *) &f[1]) & ~1;      // get 2nd float; mask off lsb

   if (i0[0] == i1[0])
      return PLOSS_FLOAT;

   return 0;
}

// Calculate the real and imaginary arrays for the current rectangle, set
// precision/algorithm, and do other misc setup operations. Call before
// starting mandelbrot calculation.

void man_setup(int xstart, int xend, int ystart, int yend) // sms
{
   double pinc;
   int i, x, y, xsize, ysize, ploss, step;
   unsigned queue_init;
   mandel_pointstruct *ps_ptr;

   // Make max iters even (required by optimized algorithm) - later change combo box to reflect this
   max_iters &= ~1;

   xsize = man_xsize;
   ysize = man_ysize;

   // Really only need to recalc. these arrays if the mag or image size changes.

   // Get pixel increment - based on smaller of xsize and ysize, so pixels are "square"
   pinc = (4.0 / ((double) man_min_dimension)) / mag; // convert so code maintains 1/mag

   // Make re/im arrays, to avoid doing xsize * ysize flops in the main loop. Recalculating
   // each point (rather than incrementing by pinc) maintains precision as long as possible.
   // Also check for precision loss (two consecutive values equal or differing only in the lsb)

   // Cut overhead by only going from start to end - not whole image size
   // Need to do more than 1 point to detect precision loss... otherwise auto mode won't work.
   // To be safe use at least 4 (allocate 4 extra).

   ploss = 0;
   step = -(xsize >> 1) + xstart;
   x = xstart;
   do
   {
      img_re[x] = re + pinc * (double) step++;
      if (x > xstart)
         ploss |= check_precision_loss(&img_re[x - 1]);
   }
   while (++x <= xend + 4);

   step = -(ysize >> 1) + ystart;
   y = ystart;
   do
   {
      img_im[y] = im - pinc * (double) step++;
      if (y > ystart)
         ploss |= check_precision_loss(&img_im[y - 1]);
   }
   while (++y <= yend + 4);

   precision_loss = 0;
   calc_precision = precision;

   // Set precision loss flag. If in auto precision mode, set single or double calculation
   // precision based on loss detection.
   switch (calc_precision)
   {
      case PRECISION_AUTO:
         calc_precision = PRECISION_SINGLE;
         if (ploss & PLOSS_FLOAT)
            calc_precision = PRECISION_DOUBLE; // deliberate fallthrough
      case PRECISION_DOUBLE:
         if (ploss & PLOSS_DOUBLE)
            precision_loss = 1;
         break;
      case PRECISION_SINGLE:
         if (ploss & PLOSS_FLOAT)
            precision_loss = 1;
         break;
      default: // should never get here (x87 is suppressed until implemented)
         break;
   }

   // Set iteration and queue_point function pointers and initialize queues

   // Alg will always be C if no sse support.
   // Should change algorithm in dialog box if it's reset to C here.
   if (((alg & ALG_C) || (sse_support < 2 && calc_precision == PRECISION_DOUBLE)))
   {
      queue_point = queue_point_c;
      mandel_iterate = iterate_c; // Unoptimized C algorithm
   }
   else
   {
      if (calc_precision == PRECISION_DOUBLE)
      {
         queue_init = (QUEUE_FULL << 12) | (3 << 9) | (2 << 6) | (1 << 3) | 0;
         queue_point = queue_4point_sse2;
         mandel_iterate = (alg & ALG_INTEL) ? iterate_intel_sse2 : iterate_amd_sse2;
      }
      else
      {
         queue_init = (QUEUE_FULL << 24) | (7 << 21) | (6 << 18) | (5 << 15) |
                                           (4 << 12) | (3 << 9) | (2 << 6) | (1 << 3) | 0;
         queue_point = queue_8point_sse;
         mandel_iterate = (alg & ALG_INTEL) ? iterate_intel_sse : iterate_amd_sse;
      }
   }

   // Set pointstruct initial values
   for (i = 0; i < num_threads; i++)
   {
      ps_ptr = thread_states[i].ps_ptr;
      ps_ptr->queue_status = queue_init;
      ps_ptr->cur_max_iters = max_iters;
      ps_ptr->iterctr = 0;
   }
}

// Man_calculate() splits the calculation up into multiple threads, each calling
// the man_calculate_threaded() function.
//
// This should behave intelligently: it should consider the rectangle size and
// display mode when deciding how to split up the calculation.
//
// When zooming, it should "learn" from the previous image's load balance
// and adjust accordingly. The new image is likely to be similar to the previous one.
//
// Current alg: divide the calculation rectangle into N stripes per thread (N
// depends on the number of threads). More stripes help load balancing by making it
// unlikely that the stripes will have wildly different iteration counts. But too
// many stripes cause a slowdown due to excess overhead.
//
// Example, image rectangle and stripes for two threads:
//
//       1 Stripe            2 Stripes         etc...
//
//  +----------------+   +----------------+
//  |                |   |   Thread 0     |
//  |   Thread 0     |   |----------------|
//  |                |   |   Thread 1     |
//  +----------------|   |----------------|
//  |                |   |   Thread 0     |
//  |   Thread 1     |   |----------------|
//  |                |   |   Thread 1     |
//  +----------------+   +----------------+
//
//  The rectangle is sometimes divided horizontally:
//
//  +-------------------------------+
//  |  T0   |  T1   |  T0   |  T1   |
//  +-------------------------------+
//
// This is necessary so that 1-pixel high rectangles (as often found in panning) can
// still be divided. Horizontal stripes (vertical division) would be preferred because
// x is done in the inner loop. Also memory access is better on horizontal stripes.
// Arbitrarily decide to do horizontal division only if the stripe height is < some
// constant (say 8, = 2x fast alg cell size).
//
// Returns the time taken to do the calculation.

double man_calculate(int xstart, int xend, int ystart, int yend) // smc
{
   TIME_UNIT start_time;
   double iteration_time;
   int i, xsize, ysize, step, thread_ind, stripe_ind, num_stripes, frac, frac_step, this_step;
   stripe *s;

   all_recalculated = 0;
   if (status & STAT_NEED_RECALC) // if need recalculation, recalculate all
   {
      xstart = 0;                 // reset rectangle to full screen
      xend = man_xsize - 1;
      ystart = 0;
      yend = man_ysize - 1;
      status = status & ~STAT_NEED_RECALC;
      all_recalculated = 1;
   }

   man_setup(xstart, xend, ystart, yend);

   xsize = xend - xstart + 1;
   ysize = yend - ystart + 1;

   // Get the number of stripes per thread based on number of threads (extract bits 3-0
   // for 1 thread, 7-4 for two threads, etc).
   num_stripes = (cfg_settings.stripes_per_thread.val >> (num_threads_ind << 2)) & 0xF;

   // Need to check min/max here (couldn't be checked automatically by log-reading function)
   if (num_stripes < 1)
      num_stripes = 1;
   if (num_stripes > MAX_STRIPES)
      num_stripes = MAX_STRIPES;

   // Now multiply by num_threads to get total number of stripes for the image.
   num_stripes <<= num_threads_ind;

   // With pathologically small images, some threads may not calculate anything.
   for (i = 0; i < num_threads; i++)
      thread_states[i].num_stripes = 0;

   // Start at the last thread, so that thread 0 gets any leftovers at the end. Thread 0 is
   // the master and doesn't suffer the overhead of being spawned, so he should get the extra work.

   thread_ind = num_threads - 1;
   stripe_ind = 0;

   // Divide along the y axis if stripe height is >= 8 (see above), or ysize is larger than xsize

   if ((ysize >= (num_stripes << 3)) || (ysize >= xsize))
   {
      if (!(step = ysize / num_stripes))  // step size (height of each stripe)
      {
         num_stripes = ysize;             // if more stripes than pixels of height,
         step = 1;                        // limit stripes and threads
      }

      // Use fractional steps to get the threads as evenly balanced as possible. For each
      // thread, the stripe height could either be step or step + 1 (they all get the
      // same step as a group).
      //
      // Dual Opteron 280, Double, Fast, 4 threads, 4 stripes/thread, tune.log:
      // With fractional steps      :  4.788s, efficiency 97.8%
      // Without fractional steps   :  4.980s, efficiency 94.1%

      frac = frac_step = ysize - (num_stripes * step);
      this_step = step;

      for (i = 0; i < num_stripes; i++)
      {
         thread_states[thread_ind].num_stripes++;
         s = &thread_states[thread_ind].stripes[stripe_ind];
         s->xstart = xstart;
         s->xend = xend;
         s->ystart = ystart;
         s->yend = ystart + this_step - 1;
         ystart += this_step;

         // Next stripe goes to next thread. If it wraps, reset and increment each thread's stripe index.
         if (--thread_ind < 0)
         {
            thread_ind = num_threads - 1;
            stripe_ind++;

            // Now that each thread has a stripe, update the fraction, and if it wraps
            // increase the stripe height for the next group by 1.
            this_step = step;
            if ((frac += frac_step) >= num_stripes)
            {
               frac -= num_stripes;
               this_step++;   // comment this out to compare efficiencies without fractional steps
            }
         }
      }
      // Give thread 0 any leftovers.
      s->yend = yend;
   }
   else // Similar code to above for dividing along the x axis
   {
      if (!(step = xsize / num_stripes))
      {
         num_stripes = xsize;
         step = 1;
      }
      frac = frac_step = xsize - (num_stripes * step);
      this_step = step;

      for (i = 0; i < num_stripes; i++)
      {
         thread_states[thread_ind].num_stripes++;
         s = &thread_states[thread_ind].stripes[stripe_ind];
         s->xstart = xstart;
         s->xend = xstart + this_step - 1;
         s->ystart = ystart;
         s->yend = yend;
         xstart += this_step;

         if (--thread_ind < 0)
         {
            thread_ind = num_threads - 1;
            stripe_ind++;
            this_step = step;
            if ((frac += frac_step) >= num_stripes)
            {
               frac -= num_stripes;
               this_step++;
            }
         }
      }
      s->xend = xend;
   }

   // Run the threads on the stripes calculated above.
   // These threading functions are slow. Benchmarks on an Athlon 64 4000+ 2.4 GHz:
   //                                                                                   Equivalent SSE2
   // Functions (tested with 4 threads)                             uS   Clock Cycles   iters (per core)
   // --------------------------------------------------------------------------------------------------
   // 4 CreateThread + WaitForMultipleObjects + 4 CloseHandle     173.0     415200      83040
   // 4 _beginthreadex + WaitForMultipleObjects + 4 CloseHandle   224.0     537600      107520
   // 4 QueueUserWorkItem + 4 SetEvent + WaitForMultipleObjects     7.6      18240      3648
   // 4 SetEvent                                                    1.0       2400      480
   // 4 Null (loop overhead + mandel function call only)            0.01        24      4
   //
   // The QueueUserWorkItem method is probably fast enough. The first two are usually on par with
   // the iteration time while panning, negating any multi-core advantage (for panning).

   // Don't call C library routines in threads. 4K stack size is more than enough

   start_time = get_timer();

   #if (_WIN32_WINNT >= 0x500) // WinXP or above. Requires kernel32.dll (annoying)
   {
      // Fast method. Using WT_EXECUTEINPERSISTENTTHREAD is 50% slower than without.
      // Leaving out WT_EXECUTELONGFUNCTION makes the initial run twice as slow, then same speed.

      // Use the master thread (here) to do some of the work. Queue any other threads. Saves some
      // overhead, and doesn't spawn any new threads at all if there's only one thread.

      for (i = 1; i < num_threads; i++)
         QueueUserWorkItem(man_calculate_threaded, &thread_states[i],
                           WT_EXECUTELONGFUNCTION | ((num_threads - 1) << 16));

      man_calculate_threaded(&thread_states[0]);

      if (num_threads > 1)
         WaitForMultipleObjects(num_threads - 1, &thread_done_events[1], TRUE, INFINITE); // wait till all threads are done
   }
   #else // Slow method. Use this if compiling for Win98
   {
      HANDLE hthreads[MAX_THREADS];
      for (i = 1; i < num_threads; i++)
         hthreads[i] = CreateThread(NULL, 4096, man_calculate_threaded, &thread_states[i], 0, NULL);

      man_calculate_threaded(&thread_states[0]);

      if (num_threads > 1)
         WaitForMultipleObjects(num_threads - 1, &hthreads[1], TRUE, INFINITE); // wait till all threads are done

      for (i = 1; i < num_threads; i++)
         CloseHandle(hthreads[i]);
   }
   #endif

   iteration_time = get_seconds_elapsed(start_time);
   file_tot_time += iteration_time;
   return iteration_time;
}

// Get the real/imag coordinates of the current image extents (left, right, top, and bottom)
void get_image_extents(double *re_left, double *im_top, double *re_right, double *im_bot, double pinc)
{
   int xsize_d2;
   int ysize_d2;

   xsize_d2 = man_xsize >> 1;
   ysize_d2 = man_ysize >> 1;

   *re_left = re + pinc * (double) (-xsize_d2);
   *re_right = re + pinc * (double) (-xsize_d2 + man_xsize - 1);
   *im_top = im - pinc * (double) (-ysize_d2);
   *im_bot = im - pinc * (double) (-ysize_d2 + man_ysize - 1);
}

// ----------------------- Quadrant/panning functions -----------------------------------

// Swap the memory pointers and handles of two quadrants (e.g., upper left and upper right).
// Doesn't modify other fields as these will all be changed afterwards.

void swap_quadrants(quadrant *q1, quadrant *q2)
{
   unsigned *tmp_data;
   HBITMAP tmp_handle;

   tmp_data = q1->bitmap_data;
   tmp_handle = q1->handle;

   q1->bitmap_data = q2->bitmap_data;
   q1->handle = q2->handle;

   q2->bitmap_data = tmp_data;
   q2->handle = tmp_handle;
}

// Reset the quadrants to the initial state: put the screen in the UL quadrant, set
// the blit size to the screen size, and set one update rectangle to the UL quadrant.
// Set all other quadrants inactive. Calling this will cause recalculation of the whole
// image. Call if this is desired, or if the screen size changes.

void reset_quadrants(void)
{
   int xsize, ysize;
   quadrant *q;

   xsize = man_xsize;
   ysize = man_ysize;

   q = &quad[UL];                         // upper left
   q->status = QSTAT_DO_BLIT;             // needs blit
   q->src_xoffs = 0;                      // src and dest offsets all 0
   q->src_yoffs = 0;
   q->dest_xoffs = 0;
   q->dest_yoffs = 0;
   q->blit_xsize = xsize;                 // blit sizes = screen size
   q->blit_ysize = ysize;
   q->quad_rect.x[0] = 0;                 // rectangle coordinates
   q->quad_rect.y[0] = 0;
   q->quad_rect.x[1] = xsize - 1;
   q->quad_rect.y[1] = ysize - 1;

   q = &quad[UR];                         // upper right
   q->status = 0;                         // inactive
   q->quad_rect.x[0] = xsize;             // rectangle coordinates
   q->quad_rect.y[0] = 0;
   q->quad_rect.x[1] = (xsize << 1) - 1;
   q->quad_rect.y[1] = ysize - 1;

   q = &quad[LL];                         // lower left
   q->status = 0;                         // inactive
   q->quad_rect.x[0] = 0;                 // rectangle coordinates
   q->quad_rect.y[0] = ysize;
   q->quad_rect.x[1] = xsize - 1;
   q->quad_rect.y[1] = (ysize << 1) - 1;

   q = &quad[LR];                         // lower right
   q->status = 0;                         // inactive
   q->quad_rect.x[0] = xsize;             // rectangle coordinates
   q->quad_rect.y[0] = ysize;
   q->quad_rect.x[1] = (xsize << 1) - 1;
   q->quad_rect.y[1] = (ysize << 1) - 1;

   // Update rectangles
   update_rect[0].valid = 1;              // 1st update rect valid; equals whole quadrant
   update_rect[0].x[0] = 0;
   update_rect[0].x[1] = xsize - 1;
   update_rect[0].y[0] = 0;
   update_rect[0].y[1] = ysize - 1;
   update_rect[1].valid = 0;              // 2nd update rect invalid

   screen_xpos = 0;                       // reset screen window to cover UL quadrant only
   screen_ypos = 0;
}

// Calculate the intersection of two rectangles. If they intersect, sets rdest to
// the intersection and returns 1, else returns 0.
// For each rectangle, the coord at index 0 is <= the coord at index 1.
int intersect_rect(rectangle *rdest, rectangle *r1, rectangle *r2)
{
   // Check if one lies outside the bounds of the other; return 0 if so
   if (r1->x[0] > r2->x[1] || r1->x[1] < r2->x[0] ||
       r1->y[0] > r2->y[1] || r1->y[1] < r2->y[0])
      return 0;

   // Get max/min coordinates to get intersection
   rdest->x[0] = r1->x[0] > r2->x[0] ? r1->x[0] : r2->x[0];
   rdest->x[1] = r1->x[1] < r2->x[1] ? r1->x[1] : r2->x[1];
   rdest->y[0] = r1->y[0] > r2->y[0] ? r1->y[0] : r2->y[0];
   rdest->y[1] = r1->y[1] < r2->y[1] ? r1->y[1] : r2->y[1];
   return 1;
}

// Iterate on the update rectangles, and palette-map the iteration data
// to the quadrants.
void man_calculate_quadrants(void) // smq
{
   rectangle r;
   int i, j, x, y;
   unsigned *bmp_ptr, *iters_ptr;

   iter_time = 0.0;

   // First calculate the update rectangles (up to 2).
   for (i = 0; i < 2; i++)
      if (update_rect[i].valid)
      {
         // To get position in (screen-mapped) image, subtract screen upper left coordinates,
         // Rectangles will be at one of the screen edges (left, right, top, or bottom).
         // Could simplify this: determined solely by pan offs_x and offs_y

         // Iterate on the update rectangles
         iter_time += man_calculate( update_rect[i].x[0] - screen_xpos,  // xstart
                                     update_rect[i].x[1] - screen_xpos,  // xend
                                     update_rect[i].y[0] - screen_ypos,  // ystart
                                     update_rect[i].y[1] - screen_ypos); // yend
      }

   // Now palette-map the update rectangles into their quadrants. Each rectangle can
   // occupy 1-4 quadrants.

   for (i = 0; i < 4; i++)
      for (j = 0; j < 2; j++)
         if (update_rect[j].valid)
            if (intersect_rect(&r, &quad[i].quad_rect, &update_rect[j]))
            {
               // Subtract upper left coordinates of this quadrant from upper left
               // coords of intersection rect to get the x, y offset of the
               // bitmap data in this quadrant

               x = r.x[0] - quad[i].quad_rect.x[0];
               y = r.y[0] - quad[i].quad_rect.y[0];
               bmp_ptr = quad[i].bitmap_data + y * man_xsize + x; // get pointer to bitmap data

               // Subtract upper left coords of screen pos from upper left coords
               // of intersection rect to get iter data offset

               x = r.x[0] - screen_xpos;
               y = r.y[0] - screen_ypos;
               iters_ptr = iter_data + y * iter_data_line_size + x; // get pointer to iter data to be mapped

               apply_palette( bmp_ptr, iters_ptr, max_iters,
                              iter_data_line_size, man_xsize,           // line sizes for iter data and bitmap
                              r.x[1] - r.x[0] + 1, r.y[1] - r.y[0] + 1, // rectangle edge lengths
                              palette, cfg_settings.max_iters_color.val);
            }
}

// Pan the image by offs_x and offs_y. Sets iter_time (from iteration functions)
// to the time to do the iteration only.
void pan_image(int offs_x, int offs_y)
{
   int xsize, ysize, swap_x, swap_y, tmp;
   quadrant *q;
   rectangle *u;

   if (offs_x | offs_y) // Recalculate only if the image moved
   {
      // Update image center coordinate and recalculate
      update_re_im(-offs_x, -offs_y); // Maybe invert offs_x, offs_y signs later

      xsize = man_xsize;
      ysize = man_ysize;

      // See algorithm explanation above
      screen_xpos -= offs_x;  // update screen pos
      screen_ypos -= offs_y;

      // Renormalize screen coordinates and swap quadrants if necessary
      swap_x = swap_y = 0;
      if (screen_xpos < 0)
      {
         screen_xpos += xsize;
         swap_x = 1;
      }
      if (screen_xpos > xsize)
      {
         screen_xpos -= xsize;
         swap_x = 1;
      }
      if (screen_ypos < 0)
      {
         screen_ypos += ysize;
         swap_y = 1;
      }
      if (screen_ypos > ysize)
      {
         screen_ypos -= ysize;
         swap_y = 1;
      }
      if (swap_x)
      {
         swap_quadrants(&quad[UL], &quad[UR]);
         swap_quadrants(&quad[LL], &quad[LR]);
      }
      if (swap_y)
      {
         swap_quadrants(&quad[UL], &quad[LL]);
         swap_quadrants(&quad[UR], &quad[LR]);
      }

      // Get the update rectangles (there can be either 1 or 2): determined by difference
      // between screen pos and previous screen pos (offs_x and offs_y). Use quadrant
      // coordinates (0,0 to 2 * xsize - 1, 2 * ysize - 1). Rectangles will be split
      // at quadrant boundaries during palette mapping. If either of the sizes is 0,
      // the rectangle will be ignored (not calculated or palette mapped).

      // Check for vertical rectangles (have an x offset)
      u = &update_rect[1];
      u->valid = offs_x;                  // valid if any nonzero offset
      u->y[0] = screen_ypos;              // vertical rectangles are
      u->y[1] = screen_ypos + ysize - 1;  // full height of screen

      if (offs_x > 0)
      {  // vertical rect at left
         u->x[0] = screen_xpos;
         u->x[1] = screen_xpos + offs_x - 1;
      }
      if (offs_x < 0)
      {  // vertical rect at right
         u->x[0] = screen_xpos + xsize + offs_x;
         u->x[1] = screen_xpos + xsize - 1;
      }

      // Check for horizontal rectangles (have a y offset).
      // Optimize out the corner intersection with any vertical rectangles so we don't
      // calculate it twice. Clip it off the vertical rect. The intersection could
      // be a large area for drag pans.

      u = &update_rect[0];
      u->valid = offs_y;                  // valid if any nonzero offset
      u->x[0] = screen_xpos;              // horizontal rectangles are
      u->x[1] = screen_xpos + xsize - 1;  // full width of screen

      if (offs_y > 0)
      {  // horizontal rect at top
         u->y[0] = screen_ypos;
         u->y[1] = tmp = screen_ypos + offs_y - 1;
         u[1].y[0] = tmp + 1;    // clip off corner intersection from any vertical rect
      }
      if (offs_y < 0)
      {  // horizontal rect at bottom
         u->y[0] = tmp = screen_ypos + ysize + offs_y;
         u->y[1] = screen_ypos + ysize - 1;
         u[1].y[1] = tmp - 1;    // clip off corner intersection from any vertical rect
      }

      // Get the blit rectangles from the screen position (screen_xpos, screen_ypos = screen upper
      // left corner). Screen coordinates always range from 0 to xsize and 0 to ysize inclusive here.
      // Refer to diagram.

      quad[UL].status = 0; // Default: all inactive
      quad[UR].status = 0;
      quad[LL].status = 0;
      quad[LR].status = 0;

      if (screen_xpos < xsize && screen_ypos < ysize) // Check if UL has a blit rectangle
      {
         q = &quad[UL];
         q->status = QSTAT_DO_BLIT; // need a blit
         q->dest_xoffs = 0;         // always blits to screen upper left corner
         q->dest_yoffs = 0;
         q->src_xoffs = screen_xpos;
         q->src_yoffs = screen_ypos;
         q->blit_xsize = xsize - screen_xpos;
         q->blit_ysize = ysize - screen_ypos;
      }
      if (screen_xpos > 0 && screen_ypos < ysize)     // Check if UR has a blit rectangle
      {
         q = &quad[UR];
         q->status = QSTAT_DO_BLIT; // need a blit
         q->dest_xoffs = xsize - screen_xpos;
         q->dest_yoffs = 0;         // always blits to screen upper edge
         q->src_xoffs = 0;          // always blits from bitmap left edge
         q->src_yoffs = screen_ypos;
         q->blit_xsize = screen_xpos;
         q->blit_ysize = ysize - screen_ypos;
      }
      if (screen_xpos < xsize && screen_ypos > 0)     // Check if LL has a blit rectangle
      {
         q = &quad[LL];
         q->status = QSTAT_DO_BLIT; // need a blit
         q->dest_xoffs = 0;         // always blits to screen left edge
         q->dest_yoffs = ysize - screen_ypos;
         q->src_xoffs = screen_xpos;
         q->src_yoffs = 0;          // always blits from bitmap top edge
         q->blit_xsize = xsize - screen_xpos;
         q->blit_ysize = screen_ypos;
      }
      if (screen_xpos > 0 && screen_ypos > 0)         // Check if LR has a blit rectangle
      {
         q = &quad[LR];
         q->status = QSTAT_DO_BLIT; // need a blit
         q->dest_xoffs = xsize - screen_xpos;
         q->dest_yoffs = ysize - screen_ypos;
         q->src_xoffs = 0;          // always blits from bitmap upper left corner
         q->src_yoffs = 0;
         q->blit_xsize = screen_xpos;
         q->blit_ysize = screen_ypos;
      }

      status |= STAT_RECALC_FOR_PALETTE;

      // Calculate and palette map the new update rectangles
      do_man_calculate(0);
   }
}

// Create the text in the INFO area of the main dialog
// If update_iters_sec is 0, won't update iters/sec and gflops (use during
// panning, when calculations will be inaccurate due to short calculation times)

char *get_image_info(int update_iters_sec)
{
   static char s[1024 + 32 * MAX_THREADS];
   static char iters_str[256];
   static unsigned long long ictr = 0;
   static double guessed_pct = 0.0;
   static double miters_s = 0.0;             // mega iterations/sec
   static double avg_iters = 0.0;            // average iterations per pixel

   unsigned long long ictr_raw;
   unsigned long long ictr_total_raw;
   double cur_pct, max_cur_pct, tot_pct, max_tot_pct;
   int i, points_guessed;
   thread_state *t;
   char tmp[256];

   ictr_raw = 0;
   ictr_total_raw = 0;
   points_guessed = 0;
   for (i = 0; i < num_threads; i++)
   {
      t = &thread_states[i];
      points_guessed += t->points_guessed;
      ictr_raw += t->ps_ptr->iterctr;         // N iterations per tick
      ictr_total_raw += t->total_iters;
   }

   if (update_iters_sec)
   {
      // For the C versions, each tick is 1 iteration.
      // For the SSE2 ASM versions, each tick is 4 iterations.
      // For the SSE ASM versions, each tick is 8 iterations.

      ictr = ictr_raw;
      if (!(alg & ALG_C))
      {
         if (calc_precision == PRECISION_DOUBLE && sse_support >= 2)
            ictr <<= 2;
         if (calc_precision == PRECISION_SINGLE && sse_support >= 1)
            ictr <<= 3;
      }

      if (iter_time < 0.001)  // Prevent division by 0. If the time is in this neighborhood
         iter_time = 0.001;   // the iters/sec won't be accurate anyway.
      miters_s = (double) ictr * 1e-6 / iter_time;
      avg_iters = (double) ictr / (double) image_size;

      guessed_pct = 100.0 * (double) points_guessed / (double) image_size;

      // Since one flop is optimized out per 18 flops in the ASM versions,
      // factor should really be 8.5 for those. But actually does 9 "effective" flops per iteration

      sprintf_s(iters_str, sizeof(iters_str), "%-4.4gM (%-.2f GFlops)", miters_s,
                miters_s * 9.0 * 1e-3);
   }

   sprintf_s(s, sizeof(s),   // Microsoft wants secure version
              "Real\t%-16.16lf\r\n"
              "Imag\t%-16.16lf\r\n"
              "Mag\t%-16lf\r\n"
              "\r\n"
              "Size\t%u x %u\r\n"
              "Time\t%-3.3fs\r\n"
              "Iters/s\t%s\r\n"

              "\r\n" // Everything here and below is "hidden" - must scroll down to see

              "Avg iters/point\t%-.1lf\r\n"
              "Points guessed\t%-.1lf%%\r\n"
              "Total iters\t%-.0lf\r\n",

              re, im, mag,
              man_xsize, man_ysize,
              iter_time, iters_str,  // Miters/s string created above
              avg_iters, guessed_pct, (double) ictr
              );

   // Get each thread's percentage of the total load, to check balance.

   sprintf_s(tmp, sizeof(tmp), "\r\nThread load %%\tCur    Total\r\n");
   strcat_s(s, sizeof(s), tmp);

   max_cur_pct = 0.0;
   max_tot_pct = 0.0;
   for (i = 0; i < num_threads; i++)
   {
      t = &thread_states[i];
      sprintf_s(tmp, sizeof(tmp),
               "Thread %d\t%#3.3g   %#3.3g\r\n", i,
               cur_pct = (double) t->ps_ptr->iterctr / (double) ictr_raw * 100.0,
               tot_pct = (double) t->total_iters / (double) ictr_total_raw * 100.0);
      strcat_s(s, sizeof(s), tmp);

      if (cur_pct > max_cur_pct)
         max_cur_pct = cur_pct;
      if (tot_pct > max_tot_pct)
         max_tot_pct = tot_pct;
   }

   // Figure of merit: percentage of best possible speed, which occurs with
   // perfect thread load balancing.
   sprintf_s(tmp, sizeof(tmp),
              "Efficiency %%\t%#3.3g   %#3.3g\r\n"
              "\r\nTotal calc time\t%-.3lfs\r\n" // resets whenever new file is opened
              "\r\n(C) 2006-2008 Paul Gentieu",
              100.0 * 100.0 / (num_threads * max_cur_pct),
              100.0 * 100.0 / (num_threads * max_tot_pct),
              file_tot_time);
   strcat_s(s, sizeof(s), tmp);

   return s;
}

// Print frames/sec info and current algorithm indicator. Current algorithm can
// temporarily change from Fast to Exact while panning, to improve performance.
// See man_calculate_threaded.

void print_fps_status_line(HWND hwnd_status, double fps, double avg_fps, double eff)
{
   char s[256];
   // Interval and average frames/sec. Removed "AVG" so large frame rates don't get cut off
   sprintf_s(s, sizeof(s), "%c Fps %3.0f/%-3.0f", cur_alg & ALG_EXACT ? 'E' : 'F',
             fps, avg_fps);
   SetWindowText(hwnd_status, s);

   // Iteration percentage: mandelbrot calculation time / total time
   sprintf_s(s, sizeof(s), "Iter %2.0f%%", eff);
   SetWindowText(hwnd_status2, s);
}

// Get frames/sec during one interval, average frames per sec, and iteration time
// percentage (mandelbrot calculation time / pan or zoom time). Assumes iter_time is set
// to the mandelbrot calculation time. Op_time should be the total time for
// the pan or zoom operation. Call once per frame.

void update_benchmarks(double op_time, int update_iters_sec)
{
   #define UPDATE_INTERVAL_TIME  0.25 // update about 4 times per sec

   interval_frames++;
   total_frames++;
   interval_time += op_time;        // Update interval and total operation times
   total_time += op_time;
   calc_total_time += iter_time;    // Update time spent only on mandelbrot calculation
   calc_interval_time += iter_time; // Update time spent only on mandelbrot calculation this interval

   if (interval_time >= UPDATE_INTERVAL_TIME) // Update status line every interval
   {
      print_fps_status_line(hwnd_status,
                           (double) interval_frames / interval_time,
                           (double) total_frames / total_time,
                           100.0 * (double) calc_interval_time / interval_time); // interval iteration %
                           //100.0 * (double) calc_total_time / total_time);     // average iteration %

      SetWindowText(hwnd_info, get_image_info(update_iters_sec)); // Update image info
      interval_frames = 0;
      interval_time = 0.0;
      calc_interval_time = 0.0;
   }
}

// Some magic constants
#define PAN_STEP_DIV       150000.0
//#define PAN_STEP_DIV       225000.0
#define OVERHEAD_FACTOR    100000
//#define OVERHEAD_FACTOR    1000000

// These adjust the pan filter constant based on image size (filter constant now comes
// from config file- adds acceleration and deceleration to movements).
#define PFC_SLOPE_FACTOR   (1600.0 * 1140.0 - 700.0 * 700.0)
#define PFC_OFFS_FACTOR    (700.0 * 700.0)

static double cur_pan_xstep = 0.0, cur_pan_ystep = 0.0;
static double pan_xstep_accum = 0.0, pan_ystep_accum = 0.0;

#define PAN_KEY (KEY_RIGHT | KEY_LEFT | KEY_UP | KEY_DOWN) // any pan key

// Reset the pan filter state and accumulators
void reset_pan_state(void)
{
   cur_pan_xstep = 0.0;
   cur_pan_ystep = 0.0;
   pan_xstep_accum = 0.0;
   pan_ystep_accum = 0.0;
}

// This calculates the x and y steps for panning based on keys pressed.
// Returns 0 if it didn't do anything (due to loss of focus or whatever),
// else 1.

// With very slow panning, the steps will be 0 most of the time, which will
// cause the main code to automatically give up CPU time.

// Later make a more sophisticated algorithm that adjusts in real time,
// based on the image characteristics and CPU speed.

// Returns a 1-clock pulse on a transition from active to stopped (keys haven't
// been pressed for awhile, and all filters are cleared out).

int get_pan_steps(int *xstep, int *ystep, int set_pan_key)
{
   int key, pkey, xstep_int, ystep_int, pulse;
   static int key_lock = 0, wait_release;
   double pan_step, xs, ys, pan_step_scale, pan_filter_const, tmp;
   double pfcmin, pfcmax, pfc_slope, pfc_offs;
   static int stopped_counter = 0;
   static int stopped = 0;

#define STOPPED_COUNTER_MAX 10

   if (xstep == NULL || ystep == NULL) // null pointers mean set pan key
   {
      key_lock = set_pan_key;
      return 0;
   }

   // Without this, will pan even if other applications have the focus... still does
   // if it recaptures focus due to the cursor moving over the main window.

   if (GetFocus() != hwnd_main)
   {
      // Stay active even if we don't have the focus, but don't accept any keys.
      // Necessary for clearing/updating the pan filters, and for keeping pan lock
      // going. Maybe go into low-priority mode too?
      key = pkey = 0;
   }
   else
      key = pkey = get_keys_pressed();

   // No need to do all the code below if we're stopped and no pan keys were pressed.
   if (!(key & (PAN_KEY | KEY_CTRL)) && !key_lock)
   {
      if (stopped)
         return 0;
   }
   else // a pan key was pressed
      stopped = 0;

   // Do pan lock with the shift key. If pan is locked, keep panning in the
   // current direction. CTRL can toggle speed. Fun...
   if (key_lock)
   {
      if (pkey & (PAN_KEY | KEY_CTRL))
      {
         if (!wait_release)
            if (pkey & KEY_CTRL)
            {
               key_lock ^= KEY_CTRL;
               wait_release = 1;
            }
            else
               key_lock = (key_lock & ~PAN_KEY) | (pkey & PAN_KEY); // preserve CTRL (fast/slow) state
      }
      else
      {
         if (pkey & KEY_SHIFT) // leave lock mode if shift pressed without a pan key
            key_lock = 0;
         wait_release = 0;
      }
      key = key_lock;
   }
   else // not in pan lock mode: see if we need to go into it
      if ((pkey & KEY_SHIFT) && (pkey & PAN_KEY))
      {
         key_lock = pkey & (PAN_KEY | KEY_CTRL);
         wait_release = 1;
      }

   // Get the step scale from the pan rate slider
   pan_step_scale = pan_step_scales[cfg_settings.pan_rate.val];

   // Adjust the step based on the image size, to try to compensate for size-based variable frame rates
   pan_step = pan_step_scale * (double) (image_size + OVERHEAD_FACTOR) * (1.0 / PAN_STEP_DIV);

   // Pan filter settings now come from config file. The 0.0001 scales large config file
   // integers to the values needed here.
   pfcmin = 0.0001 * (double) cfg_settings.pfcmin.val;
   pfcmax = 0.0001 * (double) cfg_settings.pfcmax.val;
   pfc_slope = (pfcmax - pfcmin) * (1.0 / PFC_SLOPE_FACTOR);
   pfc_offs = pfcmin - pfc_slope * PFC_OFFS_FACTOR;

   pan_filter_const = (double) image_size * pfc_slope + pfc_offs;
   if (pan_filter_const < pfcmin)
      pan_filter_const = pfcmin;

   // Go to fast mode while CTRL key is held down
   if (key & KEY_CTRL)
      pan_step *= 4.0;  // arbitrary speedup factor: 4 seems to work pretty well

   // Get x and y steps
   xs = key & KEY_RIGHT ? -pan_step : key & KEY_LEFT ? pan_step : 0.0;
   ys = key & KEY_DOWN  ? -pan_step : key & KEY_UP   ? pan_step : 0.0;

   // Filter the pan movements
   cur_pan_xstep = xs * pan_filter_const + (tmp = 1.0 - pan_filter_const) * cur_pan_xstep;
   cur_pan_ystep = ys * pan_filter_const + tmp * cur_pan_ystep;

   // Accumulate fractional steps
   pan_xstep_accum += cur_pan_xstep;
   pan_ystep_accum += cur_pan_ystep;

   // Round up/down based on sign; convert to int
   xstep_int = (int) (pan_xstep_accum + ((pan_xstep_accum < 0.0) ? -0.5 : 0.5));
   ystep_int = (int) (pan_ystep_accum + ((pan_ystep_accum < 0.0) ? -0.5 : 0.5));

   // Subtract integer part
   pan_xstep_accum -= (double) xstep_int;
   pan_ystep_accum -= (double) ystep_int;

   // Set integer steps
   *xstep = xstep_int;
   *ystep = ystep_int;

   // Detect when a pan stopped
   pulse = 0;
   if (key | xstep_int | ystep_int)  // Reset the stopped counter on any activity
      stopped_counter = STOPPED_COUNTER_MAX;
   else
      if (stopped_counter)           // else decrement and pulse on transition to 0
         if (!--stopped_counter)
         {
            stopped = 1;
            reset_pan_state();       // Clear out old data. Without this, there can be
            pulse = 1;               // little artifacts when restarting.
         }
   return pulse;
}

// Pan the image using the keyboard. Super cool...
//
// Returns 1 if it did a pan, else 0 (if idle).

int do_panning(void)
{
   int xstep, ystep;
   static TIME_UNIT start_time;
   static double pan_time = -1.0;

   // Update coords, xstep, and ystep based on keys pressed. Returns a 1-clock pulse when
   // panning transitioned from active to stopped.

   if (get_pan_steps(&xstep, &ystep, 0))
   {
      SetWindowText(hwnd_info, get_image_info(0)); // update image info
      pan_time = -1.0;                             // restart timing next time
      return 0;
   }

   if (xstep | ystep)
   {
      if (pan_time < 0.0) // if < 0, we need to restart timer
         start_time = get_timer();

      pan_image(xstep, ystep);

      pan_time = get_seconds_elapsed(start_time);  // get time since last screen update

      // skip update if the whole image was recalculated (due to size change, etc). messes
      // up the averages.
      if (!all_recalculated)
         update_benchmarks(pan_time, 0);           // update the fps, average fps, and iteration %
      start_time = get_timer();                    // restart timer for next update

      return 1;
   }
   return 0;
}

// Get re/im coordinates at the mouse position (mx, my), for realtime zoom
void get_mouse_re_im(int mx, int my)
{
   mx -= (man_xsize >> 1); // Get offset from image center
   my -= (man_ysize >> 1);

   mouse_re = re + RE_IM_DELTA(mx, man_min_dimension, mag);
   mouse_im = im - RE_IM_DELTA(my, man_min_dimension, mag);
}

// Do realtime zooming. Has two modes:

// Mode #1: mouse-controlled zooming. left button = zoom in, right button = zoom out.
// Keeps the point under the mouse at a constant position on the screen.

// Mode #2: do a realtime zoom to the current image. This is done when the zoom
// button is clicked.

int do_zooming(void)
{
   TIME_UNIT start_time;
   int mx, my, done = 0;
   double step;

   // If a panning key is pressed, temporarily exit to do the pan, then resume any zooming
   // (but abort zooming started with the zoom button). Need to improve this...
   // Problem is that zoom frame rate is vastly lower than pan rate

   if (get_keys_pressed() & PAN_KEY)
   {
      if (do_rtzoom)
         if (!(do_rtzoom & RTZOOM_WITH_BUTTON))
            prev_do_rtzoom = do_rtzoom;
         else
            prev_do_rtzoom = 0;
      do_rtzoom = 0;
   }
   else if (prev_do_rtzoom)
   {
      get_mouse_re_im(mouse_x[1], mouse_y[1]);  // update mouse coords after any pan
      reset_pan_state();                        // reset pan so we don't get extra movement
      do_rtzoom = prev_do_rtzoom;               // after stopping zoom
   }

   if (!do_rtzoom) // Return if not zooming
      return 0;

   step = rtzoom_mag_steps[cfg_settings.zoom_rate.val];

   start_time = get_timer();

   if (do_rtzoom & RTZOOM_IN)
      mag *= step;
   else
   {
      mag /= step;
      if (mag < MAG_MIN)
         mag = MAG_MIN;
   }
   if (!(do_rtzoom & RTZOOM_WITH_BUTTON)) // if zooming using the mouse
   {
      // Set the new image center re/im to keep the position at mouse[1]
      // at the same point on the screen.

      mx = mouse_x[1] - (man_xsize >> 1); // Get offset from image center
      my = mouse_y[1] - (man_ysize >> 1);

      re = mouse_re - RE_IM_DELTA(mx, man_min_dimension, mag);
      im = mouse_im + RE_IM_DELTA(my, man_min_dimension, mag);
   }
   else // if zooming using the button, stop when we hit the start mag
      if (mag > zoom_start_mag)
      {
         mag = zoom_start_mag;
         done = 1; // setting do_rtzoom 0 here wipes out fps numbers after button zoom is done
      }

   do_man_calculate(1);

   update_benchmarks(get_seconds_elapsed(start_time), 1);

   if (done) // If we just finished button zoom, update info
   {
      do_rtzoom = 0;
      file_tot_time = get_seconds_elapsed(zoom_start_time); // use for benchmarking
      SetWindowText(hwnd_info, get_image_info(1)); // Update all info
   }
   return 1;
}

// Initialize values that never change. Call once at the beginning of the program.
void init_man(void)
{
   int i;
   mandel_pointstruct *ps_ptr;
   HANDLE e;

   for (i = 0; i < MAX_THREADS; i++)
   {
      thread_states[i].thread_num = i;

      // Create an auto-reset done event for each thread. The thread sets it when done with a calculation
      e = CreateEvent(NULL, FALSE, FALSE, NULL);
      thread_states[i].done_event = e;
      thread_done_events[i] = e;

      // Init each thread's point structure
      thread_states[i].ps_ptr = ps_ptr = &pointstruct_array[i];

      // Init 64-bit double and 32-bit float fields with divergence radius and constant 2.0
      ps_ptr->two_d[1] = ps_ptr->two_d[0] = 2.0;
      ps_ptr->two_f[3] = ps_ptr->two_f[2] = ps_ptr->two_f[1] = ps_ptr->two_f[0] = 2.0;

      ps_ptr->rad_d[1] = ps_ptr->rad_d[0] = DIVERGED_THRESH;
      ps_ptr->rad_f[3] = ps_ptr->rad_f[2] = ps_ptr->rad_f[1] = ps_ptr->rad_f[0] = DIVERGED_THRESH;
   }
}

// ----------------------- GUI / misc functions -----------------------------------

// Detect whether the CPU supports SSE2 and conditional move instructions; used to
// set algorithms. Also detect the number of cores

#define FEATURE_SSE     0x02000000
#define FEATURE_SSE2    0x04000000
#define FEATURE_CMOV    0x00008000

void get_cpu_info(void)
{
   unsigned vendor[4];
   unsigned features;
   SYSTEM_INFO info;

   vendor[3] = 0;

   __asm
   {
      // If the (ancient) CPU doesn't support the CPUID instruction, we would never get here... 

      xor eax, eax   // get vendor
      cpuid
      mov vendor,     ebx
      mov vendor + 4, edx
      mov vendor + 8, ecx
      mov eax, 1     // get features
      cpuid
      mov features, edx
   }

   // Use vendor to select default algorithm
   if (!strcmp((const char *) vendor, "AuthenticAMD"))
      alg = ALG_FAST_ASM_AMD;
   else
      alg = ALG_FAST_ASM_INTEL;

   sse_support = 0;
   if ((features & (FEATURE_SSE | FEATURE_CMOV)) == (FEATURE_SSE | FEATURE_CMOV))
      sse_support = 1;
   if ((features & (FEATURE_SSE2 | FEATURE_CMOV)) == (FEATURE_SSE2 | FEATURE_CMOV))
      sse_support = 2;

   if (sse_support < 2)
   {
      MessageBox(NULL, "Your (obsolete) CPU does not support SSE2 instructions.\r\n"
                       "Performance will be suboptimal.",  "Warning", MB_OK | MB_ICONSTOP | MB_TASKMODAL);

      // Ok to stay in auto precision mode with only sse support- will switch
      // to C algorithm for double. If no sse support, no choice but to use C
      if (!sse_support)
         alg = ALG_FAST_C;
   }

   // Set the default number of threads to the number of cores. Does this count a hyperthreading
   // single core as more than one core? Should ignore these as hyperthreading won't help.
   GetSystemInfo(&info);
   num_threads = info.dwNumberOfProcessors;

   // Convert number of threads (cores) to a selection index for the dropdown box

   // Get log2(num_threads)
   for (num_threads_ind = 0; num_threads_ind <= MAX_THREADS_IND; num_threads_ind++)
      if ((1 << num_threads_ind) >= num_threads)
         break;

   num_threads = 1 << num_threads_ind;
}

// Delete any existing quadrant bitmaps, iter_data, and image coordinate arrays
void free_mem(void)
{
   int i;
   if (iter_data_start != NULL)
   {
      for (i = 0; i < 4; i++)
         DeleteObject(quad[i].handle);
      free(iter_data_start);
      free(img_re);
      free(img_im);
   }
}

// Rename this; now does a lot more than create a bitmap
int create_bitmap(int width, int height)
{
   BITMAPINFO bmi;
   BITMAPINFOHEADER *h;
   int i, j, err;
   int bmihsize = sizeof(BITMAPINFOHEADER);
   static int prev_width = 0;
   static int prev_height = 0;

   // If size didn't change, return
   if (prev_width == width && prev_height == height)
      return 0;

   free_mem(); // free any existing arrays/bitmaps

   memset(&bmi, 0, bmihsize);
   h = &bmi.bmiHeader;
   h->biSize         = bmihsize;
   h->biWidth        = width;
   h->biHeight       = -height;
   h->biPlanes       = 1;
   h->biBitCount     = 32;
   h->biCompression  = BI_RGB;

   // Create the 4 quadrant bitmaps
   err = 0;
   for (i = 0; i < 4; i++)
   {
      quad[i].handle = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&quad[i].bitmap_data, NULL, 0);
      if (!quad[i].handle)
         err = 1;
   }

   // Because the fast algorithm checks offsets from the current pixel location, iter_data needs dummy
   // lines to accomodate off-screen checks. Needs one line at y = -1, and 6 at y = ysize. Also needs
   // two dummy pixels at the end of each line.

   // Need separate pointer to be able to free later

   iter_data_line_size = width + 2;

   iter_data_start = (unsigned *) malloc(iter_data_line_size * (height + 7) * sizeof(iter_data_start[0]));
   memset(iter_data_start, 0, iter_data_line_size * (height + 7) * sizeof(iter_data_start[0]));

   iter_data = iter_data_start + iter_data_line_size; // create dummy lines at y = -1 for fast alg

   // These two need 4 extra dummy values
   img_re = (double *) malloc((width + 4) * sizeof(img_re[0]));
   img_im = (double *) malloc((height + 4) * sizeof(img_im[0]));

   if (!hscreen_dc || err || (iter_data_start == NULL) || (img_re == NULL) || (img_im == NULL))
      MessageBox(NULL, "Error allocating storage arrays.", NULL, MB_OK | MB_ICONSTOP | MB_TASKMODAL);

   status |= STAT_NEED_RECALC | STAT_SHOW_WAIT_CURSOR;   // resized; need to recalculate entire image
   prev_width = width;
   prev_height = height;
   image_size = width * height;                          // new image size
   man_min_dimension = (width < height) ? width: height; // set smaller dimension

   // Precalculate pointer offsets of neighboring pixels for the fast "wave" algorithm
   // (these only change when image width changes)
   for (j = 1; j < 7; j++)
      for (i = 0; i < 4; i++)
         wave_ptr_offs[j][i] = wave_yoffs[j][i] * iter_data_line_size + wave_xoffs[j][i];

   reset_quadrants();   // reset to recalculate all
   reset_fps_values();  // reset frames/sec timing values
   reset_pan_state();   // reset pan filters and movement state

   return 1;
}

void init_combo_box(HWND hwnd, int dlg_item, char **strs, int n, int default_selection)
{
   int i;
   // Initialize all the strings
   for (i = 0; i < n; i++)
      SendDlgItemMessage(hwnd, dlg_item, CB_ADDSTRING, 0, (LPARAM) strs[i]);

   // Set the default selection
   SendDlgItemMessage(hwnd, dlg_item, CB_SETCURSEL, default_selection, 0);
}

void get_system_metrics(void)
{
   // Need to calculate all this stuff to accomodate strange window borders and such- see below
   x_border = 2 * GetSystemMetrics(SM_CXSIZEFRAME);
   y_border = 2 * (y_thinborder = GetSystemMetrics(SM_CYSIZEFRAME)) + GetSystemMetrics(SM_CYCAPTION);
}

// Move dialog to edge of mandelbrot window
void move_dialog(HWND hwnd_dialog)
{
   // Original dialog box coords and sizes, for adjustment
   #define ORIG_YBORDER         34
   #define ORIG_DIALOG_HEIGHT   (700 + ORIG_YBORDER)
   #define ORIG_DIALOG_WIDTH    173
   #define ORIG_TFRAME_WIDTH    155
   #define ORIG_TFRAME_HEIGHT   218
   #define ORIG_STATUS_X        8
   #define ORIG_STATUS2_X       100
   #define ORIG_STATUS_Y        694
   #define YBORDER_ADJUSTMENT   (y_border - ORIG_YBORDER)

   HWND hwnd_desktop;
   RECT rc_owner, rc_dialog, rc_desktop;
   int right, top, overhang;

   get_system_metrics();
   hwnd_desktop = GetDesktopWindow();

   GetWindowRect(hwnd_main, &rc_owner);
   GetWindowRect(hwnd_dialog, &rc_dialog);
   GetWindowRect(hwnd_desktop, &rc_desktop);

   // Offset the dialog box rectangle so that the right and bottom values
   // represent the width and height

   OffsetRect(&rc_dialog, -rc_dialog.left, -rc_dialog.top);

   right = rc_owner.right;
   top = rc_owner.top;

   // Clip so dialog doesn't go off the right end of the desktop. Also
   // move down so minimize/maximize buttons are still visible
   overhang = right + rc_dialog.right - rc_desktop.right;
   if (overhang > 0)
   {
      right -= overhang;
      top += y_border - y_thinborder;
   }

   // What a pain...
   SetWindowPos(hwnd_dialog, HWND_TOP, right, top, ORIG_DIALOG_WIDTH,
                ORIG_DIALOG_HEIGHT + YBORDER_ADJUSTMENT, 0);
   SetWindowPos(hwnd_thumbnail_frame, HWND_TOP, 0, 0, ORIG_TFRAME_WIDTH,
                ORIG_TFRAME_HEIGHT + YBORDER_ADJUSTMENT, SWP_NOMOVE);
   SetWindowPos(hwnd_status, HWND_TOP, ORIG_STATUS_X, ORIG_STATUS_Y + YBORDER_ADJUSTMENT,
                0, 0, SWP_NOSIZE);
   SetWindowPos(hwnd_status2, HWND_TOP, ORIG_STATUS2_X, ORIG_STATUS_Y + YBORDER_ADJUSTMENT,
                0, 0, SWP_NOSIZE);
}

// Find a string in a strings array and return its index, or -1 if not found
int get_string_index(char *str, char **strs, int num_strs)
{
   int i;
   for (i = 0; i < num_strs; i++)
      if (!strcmp(str, strs[i]))
         return i;
   return -1;
}

// Returns 1 if palette is one of the builtins, else 0
int get_builtin_palette(void)
{
   int tmp;
   char str[256];

   GetDlgItemText(hwnd_dialog, IDC_PALETTE, str, sizeof(str));

   if ((tmp = get_string_index(str, palette_strs, NUM_ELEM(palette_strs))) >= 0)
   {
      palette = tmp;
      return 1;
   }
   return 0;
}

// Try loading a user palette from a file. Always reload so user can edit files
// on the fly. If file is missing or bad, palette doesn't change.

void get_user_palette(void)
{
   FILE *fp;
   unsigned tmp;

   GetDlgItemText(hwnd_dialog, IDC_PALETTE, palette_file, sizeof(palette_file));

   if ((fp = open_file(palette_file, "")) != NULL)
   {
      if ((tmp = load_palette(fp))) // load_palette assigns a nonzero number to the
         palette = tmp;             // palette if valid, which can then be used
      fclose(fp);                   // with apply_palette().
   }
}

void get_precision(void)
{
   int tmp;
   char str[256];

   GetDlgItemText(hwnd_dialog, IDC_PRECISION, str, sizeof(str));
   if ((tmp = get_string_index(str, precision_strs, NUM_ELEM(precision_strs))) >= 0)
         precision = tmp;
}

void get_alg(void)
{
   int tmp;
   char str[256];

   GetDlgItemText(hwnd_dialog, IDC_ALGORITHM, str, sizeof(str));
   if ((tmp = get_string_index(str, alg_strs, NUM_ELEM(alg_strs))) >= 0)
      alg = tmp;
}

void get_num_threads(void)
{
   int tmp;
   char str[256];

   GetDlgItemText(hwnd_dialog, IDC_THREADS, str, sizeof(str));
   if ((tmp = get_string_index(str, num_threads_strs, NUM_ELEM(num_threads_strs))) >= 0)
   {
      num_threads_ind = tmp;
      num_threads = 1 << num_threads_ind;
   }
}

// Increase, decrease, or just clip the max iterations
void update_iters(int up, int down)
{
   if (up)
      max_iters <<= 1;
   if (down)
      max_iters >>= 1;
   if (max_iters < MIN_ITERS)
      max_iters = MIN_ITERS;
   if (max_iters > MAX_ITERS)
      max_iters = MAX_ITERS;

   SetDlgItemInt(hwnd_dialog, IDC_ITERS, max_iters, FALSE);
}

// Reset mandelbrot parameters to the home image
void set_home_image(void)
{
   re = -0.7;
   im = 0.0;
   mag = 1.35;
   max_iters = 128;     // Better to reset the iters here. Don't want large #
   update_iters(0, 0);  // from previous image
}

// Get all fields from the dialog
void get_dialog_fields(void)
{
   max_iters = GetDlgItemInt(hwnd_dialog, IDC_ITERS, NULL, FALSE);
   update_iters(0, 0); // clip

   // Doing these only in the dialog box handler mean they don't take effect until
   // user actually clicks them, whereas getting them here mean they take effect as soon as
   // the user moves the dropdown selection.

   // It's useful to be able to set these on the fly
   get_alg();
   get_precision();
   if (precision == PRECISION_EXTENDED) // don't allow on-the-fly change to this (unimplemented)
      precision = PRECISION_DOUBLE;

   get_builtin_palette();

   // This not so much
   // get_num_threads();
}

// Update the slider position and get the new value
int set_slider_pos(int dlg_item, int pos)
{
   // This clips any bad values
   SendDlgItemMessage(hwnd_dialog, dlg_item, TBM_SETPOS, TRUE, (LONG)pos);
   return (int) SendDlgItemMessage(hwnd_dialog, dlg_item, TBM_GETPOS, 0, 0);
}

void setup_controls(void)
{
   cfg_settings.pan_rate.val = set_slider_pos(IDC_PAN_RATE, cfg_settings.pan_rate.val);
   cfg_settings.zoom_rate.val = set_slider_pos(IDC_ZOOM_RATE, cfg_settings.zoom_rate.val);
}

// Print status line at bottom of dialog with current precision, logfile image position,
// and logfile image total.

void print_status_line(HWND hwnd_status, HWND hwnd_status2, int calc)
{
   char s[256], *str1;
   if (calc)
      str1 = "Calculating...";
   else
      if (precision_loss)
         str1 = "Ready [prec loss]"; // precision loss
      else
         str1 = "Ready";

   SetWindowText(hwnd_status, str1);

   sprintf_s(s, sizeof(s), "%d/%d  %c", log_pos + 1, log_count,
             calc_precision == PRECISION_SINGLE ? 'S' : calc_precision == PRECISION_DOUBLE ? 'D' : 'E');
   SetWindowText(hwnd_status2, s);
}

void not_implemented_yet(void)
{
   MessageBox(NULL, "This feature is not implemented yet.", NULL, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
}

void unsupported_alg_prec(void)
{
   MessageBox(NULL, "Your (obsolete) CPU cannot run this algorithm/precision combination.\n"
                    "Using C algorithm.", NULL, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
}

// See if the CPU supports the selected algorithm/precision combination
void check_alg_and_precision(HWND hwnd)
{
   get_precision();
   get_alg();
   if (precision == PRECISION_EXTENDED)
   {
      not_implemented_yet();
      SendDlgItemMessage(hwnd, IDC_PRECISION, CB_SETCURSEL, precision = PRECISION_DOUBLE, 0);
   }
   else if (precision == PRECISION_DOUBLE)
   {
      if (sse_support < 2 && !(alg & ALG_C)) // If CPU doesn't support SSE2, can only run C version
      {
         unsupported_alg_prec();
         SendDlgItemMessage(hwnd, IDC_ALGORITHM, CB_SETCURSEL, alg = ALG_FAST_C, 0);
      }
   }
   else
   {
      if (!sse_support && !(alg & ALG_C)) // If CPU doesn't support SSE, can only run C version
      {
         unsupported_alg_prec();
         SendDlgItemMessage(hwnd, IDC_ALGORITHM, CB_SETCURSEL, alg = ALG_FAST_C, 0);
      }
   }
}

// Calculate all or a portion of the set. If recalc_all is nonzero, recalculate entire image,
// update image info/status line and set a wait cursor during calculation. Otherwise only
// recalculate the update rectangles and don't update info.
// Resets the STAT_SHOW_WAIT_CURSOR flag in status on return.

void do_man_calculate(int recalc_all)
{
   HCURSOR cursor;

   // Make max iters even (required by optimized algorithm) - later change combo box to reflect this
   max_iters &= ~1;

   if (max_iters != max_iters_last) // Need to recalculate all if max iters changed
      status |= STAT_NEED_RECALC;
   if (status & STAT_NEED_RECALC)   // If need recalculation,
      recalc_all = 1;               // force info update and wait cursor
   if (recalc_all)
   {
      reset_quadrants();      // Recalculate all
      if (!do_rtzoom)         // If not realtime zooming, reset frames/sec timing values
      {                       // and print status line
         print_status_line(hwnd_status, hwnd_status2, 1);
         reset_fps_values();
      }
      // Do this always, so we can change max_iters while zooming, etc.
      get_dialog_fields();
   }
   if (status & STAT_SHOW_WAIT_CURSOR)
   {
      cursor = GetCursor();   // Save current cursor and set wait cursor
      SetCursor(wait_cursor);
   }

   man_calculate_quadrants();
   max_iters_last = max_iters;  // last iters actually calculated, for palette code

   InvalidateRect(hwnd_main, NULL, 0); // Cause repaint with image data
   UpdateWindow(hwnd_main);

   // Don't update this stuff if realtime zooming. Will be done at intervals
   if (recalc_all && !do_rtzoom)
   {
      SetWindowText(hwnd_info, get_image_info(1)); // Update time, GFlops
      print_status_line(hwnd_status, hwnd_status2, 0);
   }
   if (status & STAT_SHOW_WAIT_CURSOR)
      SetCursor(cursor);   // Restore old cursor
   status &= ~STAT_SHOW_WAIT_CURSOR;
}

// Handler for the control dialog box
INT_PTR CALLBACK man_dialog_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   static int adj_iters_prev = 0, adj_threads_prev = 0;
   static int new_file_entered = 0, new_file_selected = 0;
   int tab_spacing = 26; // dialog box units
   log_entry *e;

   switch (uMsg)
   {
      case WM_INITDIALOG:

         SetDlgItemInt(hwnd, IDC_ITERS, max_iters, FALSE);
         SendDlgItemMessage(hwnd, IDC_PAN_RATE, TBM_SETRANGE, TRUE, MAKELONG(0, MAX_PAN_RATE));
         SendDlgItemMessage(hwnd, IDC_ZOOM_RATE, TBM_SETRANGE, TRUE, MAKELONG(0, MAX_ZOOM_RATE));
         init_combo_box(hwnd, IDC_PRECISION, precision_strs, NUM_ELEM(precision_strs), precision);
         init_combo_box(hwnd, IDC_PALETTE, palette_strs, NUM_ELEM(palette_strs), palette);
         init_combo_box(hwnd, IDC_ALGORITHM, alg_strs, NUM_ELEM(alg_strs), alg);
         init_combo_box(hwnd, IDC_THREADS, num_threads_strs, MAX_THREADS_IND + 1, num_threads_ind);
         init_combo_box(hwnd, IDC_LOGFILE, file_strs, NUM_ELEM(file_strs), 0);

         // Set tab stops for INFO window
         SendDlgItemMessage(hwnd, IDC_INFO, EM_SETTABSTOPS, 1, (LPARAM) &tab_spacing);

         // if any of these are null, the system has major problems
         hwnd_iters = GetDlgItem(hwnd, IDC_ITERS);
         hwnd_info = GetDlgItem(hwnd, IDC_INFO);
         hwnd_status = GetDlgItem(hwnd, IDC_STATUS);
         hwnd_status2 = GetDlgItem(hwnd, IDC_STATUS2);
         hwnd_thumbnail_frame = GetDlgItem(hwnd, IDC_THUMBNAIL_FRAME); // we'll probably use this later

         SetWindowText(hwnd_info, get_image_info(1));
         print_status_line(hwnd_status, hwnd_status2, 0);
         move_dialog(hwnd);
         break;

      case WM_VSCROLL: // Update iterations from spin control
         if ((HWND) lParam == GetDlgItem(hwnd, IDC_ADJUST_ITERS))
         {
            max_iters = GetDlgItemInt(hwnd, IDC_ITERS, NULL, FALSE); // Get value user may have edited
            switch (LOWORD(wParam))
            {
               case SB_THUMBPOSITION:
                  // No change (0) or decreasing value means up, increasing value means down
                  if (HIWORD(wParam) > adj_iters_prev)
                     update_iters(0, 1);
                  else
                     update_iters(1, 0);
                  adj_iters_prev = HIWORD(wParam);
                  break;
             }
         }
         return TRUE;

      case WM_HSCROLL: // Slider values are being updated. Moving these around during zooming
                       // annoyingly creates about a 1% slowdown, even with no processing.
         if ((HWND) lParam == GetDlgItem(hwnd, IDC_PAN_RATE))
            cfg_settings.pan_rate.val = (int) SendDlgItemMessage(hwnd, IDC_PAN_RATE, TBM_GETPOS, 0, 0);
         if ((HWND) lParam == GetDlgItem(hwnd, IDC_ZOOM_RATE))
            cfg_settings.zoom_rate.val = (int) SendDlgItemMessage(hwnd, IDC_ZOOM_RATE, TBM_GETPOS, 0, 0);
         reset_fps_values(); // reset frames/sec timing values when pan or zoom rate changes
         return TRUE;

      case WM_COMMAND:

         switch (LOWORD(wParam))
         {
            case IDC_LOGFILE:
               if (HIWORD(wParam) == CBN_EDITCHANGE)
               {
                  // Mark that user entered a new filename: will be added to box when a log func is used.
                  // Problem: user has to use next or prev buttons the first time to load settings. Then
                  // can load just by selecting.
                  new_file_entered = 1;
                  new_file_selected = 0;
               }
               // Read log immediately on sel change, if the user wants to load a config "logfile"
               if (HIWORD(wParam) == CBN_SELCHANGE)
               {
                  // Microsoft code to get the new selection. A bit obfuscated
                  SendMessage((HANDLE) LOWORD(lParam), CB_GETLBTEXT, (WPARAM) SendMessage((HANDLE)
                               LOWORD(lParam), CB_GETCURSEL, (WPARAM) 0, 0), (LPARAM) logfile);

                  log_read(logfile, "", 1);     // read logfile into array
                  reset_thread_load_counters(); // for testing load-balancing alg
                  new_file_entered = 0;
                  new_file_selected = 1;
               }
               return TRUE;

            case IDC_PALETTE:
               if (HIWORD(wParam) == CBN_SELCHANGE)
               {
                  // If palette is not one of the builtins, try loading from file
                  if (!get_builtin_palette())
                     get_user_palette();

                  // Recalculate all first if we need to
                  if (status & STAT_RECALC_FOR_PALETTE || max_iters != max_iters_last)
                  {
                     status |= STAT_SHOW_WAIT_CURSOR;
                     do_man_calculate(1);
                     status &= ~STAT_RECALC_FOR_PALETTE;
                  }

                  // Apply palette to the whole image (in UL quadrant here)
                  apply_palette(quad[UL].bitmap_data, iter_data, max_iters,
                                iter_data_line_size, man_xsize, man_xsize, man_ysize, palette,
                                cfg_settings.max_iters_color.val);
                  InvalidateRect(hwnd_main, NULL, 0); // cause repaint
                  UpdateWindow(hwnd_main);
               }
               return TRUE;

            case IDC_ALGORITHM:
            case IDC_PRECISION:
               if (HIWORD(wParam) == CBN_SELCHANGE)
               {
                  get_alg();
                  get_precision();
                  check_alg_and_precision(hwnd); // make sure CPU supports alg/precision combination
               }
               return TRUE;

            case IDC_THREADS:
               if (HIWORD(wParam) == CBN_SELCHANGE)
                  get_num_threads();
               return TRUE;

            case ID_HOME:
               set_home_image();   // Reset to base image coordinates; deliberate fallthru

            case ID_CALCULATE:
               do_rtzoom = prev_do_rtzoom = 0;     // stop any rt zoom in progress
               autoreset_settings(&cfg_settings);  // this may not be necessary
               reset_pan_state();                  // reset pan filters and movement state
               get_pan_steps(NULL, NULL, 0);       // reset any pan lock

               // No longer need to recalc for palette; show wait cursor
               status = (status & ~STAT_RECALC_FOR_PALETTE) | STAT_SHOW_WAIT_CURSOR;

               do_man_calculate(1);  // calculate all pixels, update image info
               SetFocus(hwnd_main);  // allow arrow keys to work immediately for panning
               return TRUE;          // set focus AFTER calculating, or you get an annoying blink

            case ID_LOG_IMAGE:
            case ID_LOG_PREV:
            case ID_LOG_NEXT:

               // Get the current filename
               GetDlgItemText(hwnd, IDC_LOGFILE, logfile, sizeof(logfile));

               // If it was a newly entered filename, read it and add it to the list.
               if (new_file_entered)
               {
                  if (LOWORD(wParam) != ID_LOG_IMAGE)
                  {
                     log_read(logfile, "", 1);     // read logfile into array
                     reset_thread_load_counters(); // for testing load-balancing alg
                  }
                  SendDlgItemMessage(hwnd, IDC_LOGFILE, CB_ADDSTRING, 0, (LPARAM) logfile);
               }
               if (LOWORD(wParam) == ID_LOG_IMAGE)
               {
                  log_update(logfile, new_file_entered | new_file_selected); // reset pos if new file
                  SetWindowText(hwnd_status, "Logged"); // Ok if this stays till next calculation
               }
               new_file_entered = 0;
               new_file_selected = 0;

               // If user wants a new log image...
               if (LOWORD(wParam) == ID_LOG_NEXT || LOWORD(wParam == ID_LOG_PREV))
               {
                  autoreset_settings(&cfg_settings);          // Autoreset any previous settings that need it
                  e = log_get(LOWORD(wParam) == ID_LOG_NEXT); // get next/prev image from logfile array

                  // Update any new settings
                  copy_changed_settings(&cfg_settings, &e->log_settings);

                  // Update sliders, etc.
                  setup_controls();

                  // Update info box, iters, and palette
                  update_iters(0, 0);
                  UpdateWindow(hwnd_iters);
                  SendDlgItemMessage(hwnd, IDC_PALETTE, CB_SETCURSEL, palette, 0);

                  do_rtzoom = prev_do_rtzoom = 0; // stop any rt zoom in progress
                  reset_pan_state();              // reset pan filters and movement state
                  get_pan_steps(NULL, NULL, cfg_settings.pan_key.val); // set pan lock (or turn it off if pan_key is 0)

                  // If palette is a user palette, read it in
                  if (palette >= num_builtin_palettes)
                     get_user_palette();

                  // Resize window if necessary. These values are already clipped to min sizes
                  if (cfg_settings.xsize.val != prev_man_xsize || cfg_settings.ysize.val != prev_man_ysize)
                  {
                     // Without this, you can change the size of a maximized window, which puts
                     // it into a bad state (can't resize)
                     ShowWindow(hwnd_main, SW_RESTORE);
                     SetWindowPos(hwnd_main, HWND_TOP, 0, 0, cfg_settings.xsize.val + x_border,
                                  cfg_settings.ysize.val + y_border, SWP_NOMOVE | SWP_NOCOPYBITS);
                     UpdateWindow(hwnd_main); // not sure why this is necessary. Sizes get out of sync?

                     prev_man_xsize = cfg_settings.xsize.val; // Save previous size
                     prev_man_ysize = cfg_settings.ysize.val; // Can't do this in create_bitmap
                  }

                  if (!cfg_settings.zoom_in_out.val) // If not zooming, just calculate
                  {
                     status = (status & ~STAT_RECALC_FOR_PALETTE) | STAT_SHOW_WAIT_CURSOR;
                     do_man_calculate(1);  // calculate all pixels, update image info
                     SetFocus(hwnd_main);
                     return TRUE;
                  }
                  else
                  {
                     // Else fallthrough to do realtime zoom in (later need zoom out also)
                  }
               }
               else
                  return TRUE;

            case ID_ZOOM: // Do a realtime zoom in to the current image
               reset_fps_values();
               reset_thread_load_counters();
               zoom_start_time = get_timer();
               zoom_start_mag = mag;
               mag = MAG_MIN;
               do_rtzoom = RTZOOM_IN | RTZOOM_WITH_BUTTON;
               return TRUE;

            // This didn't work well in practice
            // case ID_PAN_LOCK:
            //    pan_lock ^= 1;
            //    return TRUE;

            case ID_SAVE_BMP:
            case ID_FULLSCREEN:
            // case ID_SLIDESHOW:
            // case ID_OPTIONS:
               not_implemented_yet();
               return TRUE;

            default:
               return FALSE;
         }
         break;

      case WM_CLOSE:    // Never close or destroy this dialog
         break;
      case WM_DESTROY:
         break;
   }
   return FALSE; // return FALSE if we didn't process the message (exceptions for INITDIALOG) otherwise TRUE
}

// Blits thin horizontal stripes from quadrant bitmaps (each stripe potentially
// coming from 2 quadrants), so pixels are copied in upper-left to lower-right order.
// Emulates blitting a single bitmap to avoid visual artifacts. Will be an exact
// emulation (with a lot of overhead) if STRIPE_THICKNESS is 1.

void striped_blit(quadrant *ql, quadrant *qr, HDC hdc, HDC hscreen_dc)
{
   int src_yoffs, dest_yoffs, ysize, this_y, y_done;

   // Thickness of the stripes: the thinner the better, but thinner
   // stripes cause more overhead. 8 gives no measureable overhead on
   // the Athlon 4000+ system, but significant overhead on the Pentium
   // D 820 system. Probably hugely dependent on the video driver.

   // No artifacts visible with either 8 or 16 (except those that are present
   // with a full bitmap also- tearing, CPU cycle stealing by other applications,
   // and frame rate aliasing with the screen refresh rate).

   //#define STRIPE_THICKNESS 16 // this now comes from global config setting

   // Return if no data in these quadrants
   if (!(ql->status & QSTAT_DO_BLIT) && !(qr->status & QSTAT_DO_BLIT))
      return;

   // The src_yoffs, dest_yoffs, and blit_ysize fields of the left and right
   // quadrants will always be the same.

   // Get from left quad if it has a blit rectangle, else get from right
   if (ql->status & QSTAT_DO_BLIT)
   {
      src_yoffs = ql->src_yoffs;
      dest_yoffs = ql->dest_yoffs;
      ysize = ql->blit_ysize;
   }
   else
   {
      src_yoffs = qr->src_yoffs;
      dest_yoffs = qr->dest_yoffs;
      ysize = qr->blit_ysize;
   }

   this_y = cfg_settings.blit_stripe_thickness.val; // STRIPE_THICKNESS;
   y_done = 0;
   do
   {
      if (y_done + this_y > ysize)
         this_y = ysize - y_done;

      if (ql->status & QSTAT_DO_BLIT) // Blit stripe left half from left quadrant
      {
         SelectObject(hscreen_dc, ql->handle);
         BitBlt(hdc, ql->dest_xoffs, dest_yoffs, ql->blit_xsize, this_y,
                hscreen_dc, ql->src_xoffs, src_yoffs, SRCCOPY);
      }
      if (qr->status & QSTAT_DO_BLIT)  // Blit stripe right half from right quadrant
      {
         SelectObject(hscreen_dc, qr->handle);
         BitBlt(hdc, qr->dest_xoffs, dest_yoffs, qr->blit_xsize, this_y,
                hscreen_dc, qr->src_xoffs, src_yoffs, SRCCOPY);
      }

      src_yoffs += this_y;
      dest_yoffs += this_y;
      y_done += this_y;
   }
   while (y_done != ysize);
}

// Microsoft code for confining the mouse cursor to the main window.
void confine_mouse_cursor(void)
{
   RECT rc;             // working rectangle
   POINT ptClientUL;    // client upper left corner
   POINT ptClientLR;    // client lower right corner

   GetClientRect(hwnd_main, &rc);  // Retrieve the screen coordinates of the client area,
   ptClientUL.x = rc.left;         // and convert them into client coordinates.
   ptClientUL.y = rc.top;
   ptClientLR.x = rc.right + 1;             // Add one to the right and bottom sides, because the
   ptClientLR.y = rc.bottom + 1;            // coordinates retrieved by GetClientRect do not
   ClientToScreen(hwnd_main, &ptClientUL);  // include the far left and lowermost pixels.
   ClientToScreen(hwnd_main, &ptClientLR);

   SetRect(&rc, ptClientUL.x, ptClientUL.y, // Copy the client coordinates of the client area
           ptClientLR.x, ptClientLR.y);     // to the rcClient structure.

   SetCapture(hwnd_main);  // capture mouse input
   ClipCursor(&rc);        // confine the mouse cursor to the client area
}

// The window function for the main window
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   RECT rc;        // working rectangle
   HDC hdc;        // a device context used for drawing
   PAINTSTRUCT ps; // also used during window drawing
   quadrant *q;

   static HPEN hpen;    // For drawing the zoom box
   static int prev_mouse_x = -1, prev_mouse_y;
   static int dragging = 0, have_box = 0;
   static int allow_mode_change = 1;  // Flag indicating whether zoom/pan mode change is allowed
   static int zoom_mode_pending = 0;  // Flag indicating need a change back to zoom mode
   static int prev_nav_mode = MODE_RTZOOM;

   switch (nMsg)
   {
      case WM_CREATE: // The window is being created

         hscreen_dc = CreateCompatibleDC(NULL); // screen device context
         hwnd_dialog = CreateDialog(hinstance, MAKEINTRESOURCE(IDD_MAN_DIALOG), hwnd, man_dialog_proc);

         // Create a pen for the zoom rectangle
         hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0)); // easier to see than PS_DOT
         // Deliberate fallthrough to WM_PAINT to set up coordinates

      case WM_PAINT: // The window needs to be painted (redrawn).

         hdc = BeginPaint(hwnd, &ps);
         GetClientRect(hwnd, &rc);

         // Calculate mandelbrot image size.
         man_xsize = rc.right - rc.left;
         man_ysize = rc.bottom - rc.top;

         // Clip min size
         if (man_xsize < MIN_SIZE)
            man_xsize = MIN_SIZE;
         if (man_ysize < MIN_SIZE)
            man_ysize = MIN_SIZE;

         // Create arrays and bitmaps used in calculations (will deallocate/resize as needed-
         // returns > 0 if resized). Doesn't do anything if no resize

         // Rename this- now does a lot more than create a bitmap
         if (create_bitmap(man_xsize, man_ysize))
            if (hwnd_info)
               SetWindowText(hwnd_info, get_image_info(0)); // update size info if resized

         // Blit the mandelbrot bitmap. Could take rectangular regions from 1 to 4 quadrants.
         // Blits thin horizontal stripes. See comments at striped_blit().
         // Do upper quadrants (UL and UR), then lower (LL and LR).

         // Also optimize for the case where we've recalculated the whole image (only UL
         // is valid). In this case do a normal blit.
         q = &quad[UL];
         if (q->blit_xsize == man_xsize && q->blit_ysize == man_ysize)
         {
            SelectObject(hscreen_dc, q->handle);
            BitBlt(hdc, 0, 0, q->blit_xsize, q->blit_ysize, hscreen_dc, 0, 0, SRCCOPY);
         }
         else
         {
            striped_blit(&quad[UL], &quad[UR], hdc, hscreen_dc);
            striped_blit(&quad[LL], &quad[LR], hdc, hscreen_dc);
         }
         EndPaint(hwnd, &ps);
         return FALSE;

      case WM_LBUTTONDOWN:

         // Save the coordinates of the mouse cursor.
         mouse_x[0] = LOWORD(lParam);
         mouse_y[0] = HIWORD(lParam);
         mouse_x[1] = LOWORD(lParam); // Init this for switch from zoom to pan also
         mouse_y[1] = HIWORD(lParam);

         get_mouse_re_im(mouse_x[0], mouse_y[0]);  // Get re/im coords at the mouse position, for realtime zoom
         prev_mouse_x = -1;
         dragging = 1; // dragging either rectangle (for zoom) or image (for pan)

         confine_mouse_cursor();

         if (nav_mode == MODE_PAN)
            SetCursor(hclosed_cursor);  // set closed hand
         if (nav_mode == MODE_RTZOOM)   // if in realtime zoom mode,
            do_rtzoom = RTZOOM_IN;      // set global flag for do_zooming()
         else
            allow_mode_change = 0;      // don't allow mode change while button is down if not in rtzoom mode

         return FALSE;

      case WM_LBUTTONUP: // Zoom in

         mouse_x[1] = LOWORD(lParam);
         mouse_y[1] = HIWORD(lParam);
         allow_mode_change = 1;  // Allow mode change again after button released

         if (dragging)
         {
            dragging = 0;
            if (nav_mode == MODE_ZOOM)
            {
               // Update mandelbrot parms from rectangle and recalculate
               status |= STAT_SHOW_WAIT_CURSOR;
               update_re_im_mag(have_box, 1, mouse_x[0], mouse_y[0], mouse_x[1], mouse_y[1]);
               do_man_calculate(1);
            }
            else // Update image info (excluding iters/sec) after pan or realtime zoom
               SetWindowText(hwnd_info, get_image_info(0));
         }
         have_box = 0;

         if (zoom_mode_pending) // go back to zoom mode if change was pending
         {
            nav_mode = prev_nav_mode;
            if (GetCursor() != arrow_cursor) // Only if space released in client area (kluge)
               SetCursor(mag_zoom_cursor);
            zoom_mode_pending = 0;
         }
         // allow dragging during zooming. Possibly some bug here, but I think I fixed it
         if (nav_mode != MODE_PAN)
            do_rtzoom = prev_do_rtzoom = 0;  // clear realtime zoom flag for do_zooming()

         ClipCursor(NULL); // release mouse cursor and capture
         ReleaseCapture();

         return FALSE;

      case WM_MOUSEMOVE:

         mouse_x[1] = LOWORD(lParam);
         mouse_y[1] = HIWORD(lParam);
         get_mouse_re_im(mouse_x[1], mouse_y[1]);  // Get re/im coords at the mouse position, for realtime zoom

         // If user is dragging, draw the zoom rectangle (zoom mode) or pan the image (pan mode)

         if (nav_mode == MODE_PAN) // panning mode- move the image
         {
            if (wParam & (MK_LBUTTON | MK_RBUTTON)) // allow panning using right button drag also
            {
               int pan_offs_x, pan_offs_y;

               // Get difference from previous mouse location; use as pan offset
               pan_offs_x = mouse_x[1] - mouse_x[0];
               pan_offs_y = mouse_y[1] - mouse_y[0];

               mouse_x[0] = mouse_x[1];            // update previous mouse location
               mouse_y[0] = mouse_y[1];

               pan_image(pan_offs_x, pan_offs_y);  // do the pan
            }
         }
         else if (nav_mode == MODE_ZOOM)
         {
            if ((wParam & MK_LBUTTON) && dragging) // zoom rectangle
            {
               hdc = GetDC(hwnd);
               SelectObject(hdc, hpen);

               SetROP2(hdc, R2_NOTXORPEN); // not ideal- can be tough to see at times

               // erase previous rectangle, if it exists
               if (prev_mouse_x >= 0 && prev_mouse_x != mouse_x[0])
               {
                  Rectangle(hdc, mouse_x[0], mouse_y[0], prev_mouse_x, prev_mouse_y);
                  have_box = 1;
               }

               // draw new rectangle
               Rectangle(hdc, mouse_x[0], mouse_y[0],
                         prev_mouse_x = mouse_x[1], prev_mouse_y = mouse_y[1]);
               ReleaseDC(hwnd, hdc);
            }
         }
         return FALSE;

      case WM_RBUTTONDOWN: // Zoom out

         mouse_x[0] = LOWORD(lParam);
         mouse_y[0] = HIWORD(lParam);
         mouse_x[1] = LOWORD(lParam); // Init this for switch from zoom to pan also
         mouse_y[1] = HIWORD(lParam);

         get_mouse_re_im(mouse_x[0], mouse_y[0]);  // Get re/im coordinates at the mouse position, for realtime zoom
         if (nav_mode == MODE_RTZOOM)  // if in realtime zoom mode,
            do_rtzoom = RTZOOM_OUT;    // set global flag for do_zooming()
         else
            allow_mode_change = 0;     // else don't allow mode change while button is down

         if (nav_mode == MODE_PAN)
            SetCursor(hclosed_cursor); // set closed hand

         confine_mouse_cursor();       // Also need to confine here, for realtime zoom
         return FALSE;

      case WM_RBUTTONUP: // Zoom out

         mouse_x[1] = LOWORD(lParam);
         mouse_y[1] = HIWORD(lParam);
         allow_mode_change = 1;  // Allow mode change again after button released

         // Zoom out from current point, and recenter
         if (nav_mode == MODE_ZOOM)
         {
            status |= STAT_SHOW_WAIT_CURSOR;
            update_re_im_mag(0, 0, mouse_x[0], mouse_y[0], mouse_x[1], mouse_y[1]);
            do_man_calculate(1);
         }
         do_rtzoom = prev_do_rtzoom = 0;  // clear realtime zoom flag for do_zooming()
         ClipCursor(NULL);                // release mouse cursor and capture
         ReleaseCapture();
         return FALSE;

      case WM_MOUSEWHEEL: // Use mousewheel to adjust iterations. (Maybe palette too, if button down?)

         if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
            update_iters(1, 0);
         else
            update_iters(0, 1);
         SetDlgItemInt(hwnd_dialog, IDC_ITERS, max_iters, FALSE);
         return FALSE;

      case WM_KEYDOWN:  // Go to pan mode while space is held down (if allowed)

         #define PREV_KEYDOWN (1 << 30)

         // Too easy to accidentally hit this and lose an image

         // Center image on x axis
         // if (wParam == '0')
         // {
         //    im = 0.0;
         //    do_man_calculate(1);
         //    return TRUE;
         // }

         if (lParam & PREV_KEYDOWN) // aargh... ignore key autorepeats. Were wiping out prev_do_rtzoom.
            return TRUE;

         if (allow_mode_change)
         {
            if (wParam == 'Z') // toggle zoom mode
            {
               mag_zoom_cursor = (mag_zoom_cursor == mag_cursor) ? rtzoom_cursor : mag_cursor;
               nav_mode = prev_nav_mode = (mag_zoom_cursor == mag_cursor) ? MODE_ZOOM : MODE_RTZOOM;
               if (GetCursor() != arrow_cursor) // Change cursor only if released in client area (kluge)
                  SetCursor(mag_zoom_cursor);
            }
            nav_mode = (wParam == VK_SPACE) ? MODE_PAN : prev_nav_mode;
            if (nav_mode == MODE_PAN)
            {
               mouse_x[0] = mouse_x[1];            // Reset mouse position for pan
               mouse_y[0] = mouse_y[1];
               prev_do_rtzoom = do_rtzoom;         // save realtime zoom state
               do_rtzoom = 0;                      // stop any realtime zoom
               if (GetCursor() == mag_zoom_cursor) // Fix toggling between hand and arrow in non-client area (kluge)
                  SetCursor(prev_do_rtzoom ? hclosed_cursor : hopen_cursor);
            }
         }
         return TRUE;

      case WM_KEYUP: // Go back to zoom mode if space released, if allowed.

         if (nav_mode == MODE_PAN)
            if (allow_mode_change)
            {
               if (GetCursor() != arrow_cursor) // Change cursor only if space released in client area (kluge)
                  SetCursor(mag_zoom_cursor);
               nav_mode = prev_nav_mode;        // Restore old nav mode and
               do_rtzoom = prev_do_rtzoom;      // zooming status
            }
            else
               zoom_mode_pending = 1;  // Else pending: do as soon as mouse released
         return TRUE;

      case WM_SETCURSOR: // We get this message whenever the cursor moves in this window.

         // Set cursor (hand or zoom) based on nav mode. Also set keyboard focus to this
         // window for key detection. Eliminates need to click window first to set focus
         SetFocus(hwnd);
         if (LOWORD(lParam) == HTCLIENT) // only set zoom/hand cursor in client area
         {
            SetCursor(nav_mode == MODE_PAN ? hopen_cursor : mag_zoom_cursor);
            return TRUE;
         }
         break; // let system set cursor outside client area (resize, arrow, etc)

      // case WM_NCMOUSEMOVE: // mouse move in non-client area
      //    break;

      case WM_WINDOWPOSCHANGED: // Move dialog box along with main window
         if (hwnd_dialog)
            move_dialog(hwnd_dialog);
         return FALSE;

      case WM_COMMAND:
         return FALSE;

      case WM_DESTROY:  // The window is being destroyed, close the application
         PostQuitMessage(0);
         return FALSE;
   }
   // If we don't handle a message completely we hand it to the system-provided default window function.
   return DefWindowProc(hwnd, nMsg, wParam, lParam);
}

void fancy_intro(void)
{
   #define MAG_STEP 1.09
   #define MAG_START 0.3

   set_home_image();
   max_iters = 64;
   mag = MAG_START;
   do
   {
      do_man_calculate(0);
      mag *= MAG_STEP;
   }
   while (mag <= 1.35);

   set_home_image();
   do_man_calculate(1);
   file_tot_time = 0.0; // don't count intro time in file total time
}

int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
   MSG msg;
   WNDCLASSEX wndclass;
   char *classname = "ManWin";

   hinstance = hInst;

   // Read any default settings from config file. Do this early, because it can change almost
   // anything used below.
   read_cfg_file();

   get_cpu_info();
   init_man();
   if (!(num_builtin_palettes = init_palettes()))
      return 0;

   memset(&wndclass, 0, sizeof(WNDCLASSEX)); // Create a window class for our main window.
   wndclass.lpszClassName = classname;
   wndclass.cbSize = sizeof(WNDCLASSEX);
   wndclass.style = CS_HREDRAW | CS_VREDRAW;
   wndclass.lpfnWndProc = MainWndProc;
   wndclass.hInstance = hInst;
   wndclass.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MAN));       // Use mini mandelbrot icon
   wndclass.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MAN));
   wndclass.hCursor = arrow_cursor = LoadCursor(NULL, IDC_ARROW);    // We set other cursors based on nav mode
   wndclass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);    // Black background
   RegisterClassEx(&wndclass);

   // Load cursors - wait, zoom, hand open, and hand closed
   wait_cursor = LoadCursor(NULL, IDC_WAIT);
   mag_cursor = LoadCursor(hInst, MAKEINTRESOURCE(IDC_MAG));
   rtzoom_cursor = mag_zoom_cursor = LoadCursor(hInst, MAKEINTRESOURCE(IDC_RTZOOM));
   hopen_cursor = LoadCursor(hInst, MAKEINTRESOURCE(IDC_HAND_OPEN));
   hclosed_cursor = LoadCursor(hInst, MAKEINTRESOURCE(IDC_HAND_CLOSED));

   get_system_metrics();

   hwnd_main = CreateWindow(  // Create our main window
      classname,
      "QuickMAN v1.0  | z: mode | buttons: zoom in/out/rect | space: drag | arrows: pan (+shift: lock)",       // Caption (put program version here)
      WS_OVERLAPPEDWINDOW,    // Style
      CW_USEDEFAULT,          // Initial x (use default)
      CW_USEDEFAULT,          // Initial y (use default)
      man_xsize + x_border,   // Size
      man_ysize + y_border,
      NULL,                   // No parent window
      NULL,                   // No menu
      hInst,                  // This program instance
      NULL                    // Creation parameters
      );

   // Init common controls; required for < WinXP, and apparently for some Pentium 4 systems
   // (dialog box wasn't showing up on them)
   InitCommonControls();

   #ifndef USE_PERFORMANCE_COUNTER
   // Set timer resolution to 1ms (if supported). Seems to take awhile to take effect.
   // timegettime() will return inaccurate results if it's called too soon after this...
   timeBeginPeriod(1); // fancy_intro will absorb initialization time
   #endif

   ShowWindow(hwnd_main, nShow);
   //AnimateWindow(hwnd_main, 100, nShow | AW_BLEND); // a little fancy
   //UpdateWindow(hwnd_main);                   // Not necessary?
   UpdateWindow(hwnd_dialog);                   // Must update dialog window first, or it starts
   ShowWindow(hwnd_dialog, nShow);              // at the edge of the screen

   setup_controls();                            // Setup sliders, etc
   add_user_palettes();                         // Add user palettes to palette dropdown menu
   log_read(logfile, "\nDid you extract all the files from the QuickMAN .zip archive?", 1);                        // Read logfile- add_user_palettes() must be called before this
   fancy_intro();                               // Zoom in to home image

   while(1)                                     // Main loop
   {
      if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
      {
         if (msg.message == WM_QUIT)
            break;
         if (!IsDialogMessage(hwnd_dialog, &msg)) // only process messages not for dialog box
         {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
         }
      }
      else
      {
         // Do heavy computation here

         if (!do_zooming() && !do_panning())
            Sleep(1); // don't use 100% of CPU when idle
      }
   }

   #ifndef USE_PERFORMANCE_COUNTER
   timeEndPeriod(1);
   #endif

   free_mem();

   return (int) msg.wParam;
}
