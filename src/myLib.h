#ifndef MYLIB_H
#define MYLIB_H


typedef unsigned char bool;
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned int uint32_t;
typedef int int32_t;

typedef unsigned short uint16_t;
typedef signed short int16_t;

typedef volatile uint16_t vu16;
typedef volatile int16_t vs16;
typedef volatile uint32_t vu32;

typedef int8_t tribool;

typedef void (*fnptr)(void);

#include "tonc_lib/memmap.h"

#define NULL  (void*)0
#define FALSE 0
#define TRUE  1

#define INLINE static inline
#define ALIGN4 __attribute__((aligned(4)))

// DMA (from http://www.coranac.com/tonc/text/dma.htm)
typedef struct DMA_REC
{
    const void *src;
    void *dst;
    uint32_t cnt;
} DMA_REC;
// surround in do while for semicolon protection
#define DMA_TRANSFER(_dst, _src, count, ch, mode) \
    do {\
        REG_DMA[ch].cnt = 0;\
        REG_DMA[ch].src = (const void*)(_src);\
        REG_DMA[ch].dst = (void*)(_dst);\
        REG_DMA[ch].cnt = (count) | (mode);\
    } while(0)
#define DMA_ENABLE      (1<<0x1F)
#define DMA_REPEAT      (1<<0x19)
#define DMA_SRC_FIXED   (2<<0x17)
#define DMA_AT_VBLANK   (1<<0x1C)
#define DMA_AT_HBLANK   (2<<0x1C)
#define DMA_DST_RELOAD  (3<<0x15)
#define DMA_CS_16       (0<<0x1A)
#define DMA_CS_32       (1<<0x1A)


// video
#define MODE0 0
#define MODE3 3
#define BG2_ENABLE  (1<<0xA)
#define DCNT_OBJ_1D (1<<0x6)
#define DCNT_OBJ    (1<<0xC)
#define OFFSET(r,c) ((r)*240+(c))
#define OFFSET3(r,c,w) ((r)*(w)+(c))
#define COLOR(r,g,b) ((r) | (g) << 5 | (b) << 10)
#define RED COLOR(31,0,0)
#define WHITE COLOR(31,31,31)
#define BLUE  COLOR(0, 0, 31)
#define BLACK 0
#define SCREEN_HEIGHT 160
#define SCREEN_WIDTH 240

#define IABS(a) ((uint32_t) ((a) < 0 ? ~(a)+1 : (a) ))
#define TRIBOOL(a) (a == 0 ? a : (a < 0 ? -1 : 1))
#define BYTETOWORD(val16) ((val16) << 16 | (val16))

#include "render/gfx_helper.h"

#define PUTI(i) do {\
    drawRectFW(20, 8, COLOR(10, 10, 10));\
    drawInt(20, 20, (i), 6, WHITE);\
} while(0)

#define PUTS(s) do {\
    drawRectFW(20, 8, COLOR(10, 10, 10));\
    drawString(20, 20, (s), WHITE);\
} while(0)


#endif
