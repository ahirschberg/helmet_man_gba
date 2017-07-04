#ifndef SPRITES_H
#define SPRITES_H
// tile 8x8@4bpp: 32bytes; 8 ints
typedef struct { uint32_t data[8];  } TILE, TILE4;
// d-tile: double-sized tile (8bpp)
typedef struct { uint32_t data[16]; } TILE8;
// tile block: 32x16 tiles, 16x16 d-tiles
typedef TILE  CHARBLOCK[512];
typedef TILE8 CHARBLOCK8[256];

typedef struct {uint16_t data[16];} PALETTE;
#define WS_PALETTE 16 // palette word size


#define palette_mem ((volatile PALETTE*)0x05000200)
#define tile_mem    ((volatile TILE*)0x6010000)
#define tile_mem8   ((volatile TILE8*)0x6010000)
/* #define tile_mem    ((CHARBLOCK*)0x06010000) */
/* #define tile_mem8  ((CHARBLOCK8*)0x06010000) */
#define OBJ_ATTR_MEM ((volatile OBJ_ATTR*)0x07000000)
typedef struct tagOBJ_ATTR
{
    uint16_t attr0;
    uint16_t attr1;
    uint16_t attr2;
    int16_t fill;
} ALIGN4 OBJ_ATTR;

// FIXME
#define NULL_OBJ_ATTR   ((volatile OBJ_ATTR){2<<0x8, 0, 0, 0})

// probably never gonna be used.
typedef struct OBJ_AFFINE
{
    uint16_t fill0[3];
    int16_t pa;
    uint16_t fill1[3];
    int16_t pb;
    uint16_t fill2[3];
    int16_t pc;
    uint16_t fill3[3];
    int16_t pd;
} ALIGN4 OBJ_AFFINE;

#define ATTR2_ID_MASK       0x03FF
#define ATTR2_ID_SHIFT      0
#define ATTR2_PALBANK_MASK  0xF000
#define ATTR2_PALBANK_SHIFT 0xC

#define ATTR1_X_MASK        0x01FF // This tricky 9 bits....
#define ATTR1_X_SHIFT       0
#define ATTR1_SIZE_MASK     0xC000
#define ATTR1_SIZE_SHIFT    0xE
#define ATTR1_HFLIP_SHIFT   0xC
#define ATTR1_HFLIP_MASK    (1<<ATTR1_HFLIP_SHIFT)
#define ATTR1_VFLIP_SHIFT   0xD
#define ATTR1_VFLIP_MASK    (1<<ATTR1_VFLIP_SHIFT)

#define ATTR0_Y_MASK        0x00FF
#define ATTR0_Y_SHIFT       0
#define ATTR0_256_COLOR_SHIFT     0xD
#define ATTR0_256_COLOR_MASK        (1<<0xD)
#define ATTR0_SHAPE_MASK    0xC000
#define ATTR0_SHAPE_SHIFT   0xE
#define ATTR0_OBJ_MODE_SHIFT    0x8
#define ATTR0_OBJ_MODE_MASK     (3<<ATTR0_OBJ_MODE_SHIFT)
#define ATTR0_OBJ_MODE_NORMAL   0
#define ATTR0_OBJ_MODE_NODRAW   2

#define SPRITE_SQUARE       0
#define SPRITE_WIDE         1
#define SPRITE_TALL         2

#define SPRITE_SIZE_SM      0 
#define SPRITE_SIZE_MD      1
#define SPRITE_SIZE_LG      2
#define SPRITE_SIZE_XL      3

// hack to get x coordinate's signedness to work properly
INLINE signed short sext9(unsigned short num) {
    signed short ret = num << 7;
    return ret >> 7;
}

// bit field set and get routines
#define BF_PREP(x, name)         ( ((x)<<name##_SHIFT)& name##_MASK  )
#define BF_GET(x, name)          ( ((x) & name##_MASK)>> name##_SHIFT )
#define BF_SET(y, x, name)       (y = ((y)&~name##_MASK) | BF_PREP(x,name) )

#define OBJ_DX(attr1, dx)   (BF_SET(attr1, \
                                    BF_GET(attr1, ATTR1_X) + (dx), \
                                    ATTR1_X))

#define OBJ_DY(obj_attr, dy)   (BF_SET((obj_attr).attr0, \
                                    BF_GET((obj_attr).attr0, ATTR0_Y) + (dy), \
                                    ATTR0_Y))
#define PLAYER_OBJ_PTR         (OBJ_ATTR_MEM)

extern void loadSpriteData4(uint16_t id, const unsigned int* tile_data, const int tile_length);
extern void loadPaletteData4(uint16_t id, const unsigned short* palette_data, const int palette_length);

#endif
