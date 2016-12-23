/*
 * Exported with nin10kit v1.3
 * Invocation command was nin10kit -mode=sprites -for_bitmap=1 -bpp=4 nin10kitprojectile assets/projectile_sprites.png 
 * Time-stamp: Thursday 12/22/2016, 23:07:58
 * 
 * Image Information
 * -----------------
 * assets/projectile_sprites.png 8@32
 * 
 * Quote/Fortune of the Day!
 * -------------------------
 * 
 * All bug reports / feature requests are to be sent to Brandon (bwhitehead0308@gmail.com)
 */

#ifndef NIN10KITPROJECTILE_H
#define NIN10KITPROJECTILE_H

#define NIN10KITPROJECTILE_PALETTE_TYPE (0 << 13)
#define NIN10KITPROJECTILE_DIMENSION_TYPE (1 << 6)

extern const unsigned short nin10kitprojectile_palette[256];
#define NIN10KITPROJECTILE_PALETTE_SIZE 256

extern const unsigned short nin10kitprojectile[64];
#define NIN10KITPROJECTILE_SIZE 64

#define PROJECTILE_SPRITES_PALETTE (0 << 12)
#define PROJECTILE_SPRITES_SPRITE_SHAPE (2 << 14)
#define PROJECTILE_SPRITES_SPRITE_SIZE (1 << 14)
#define PROJECTILE_SPRITES_ID 512

#endif

