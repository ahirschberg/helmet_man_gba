#include "myLib.h"
#include "main.h"
#include "sprites.h"

// 8x8 tile, 64 pixels * 4bits/pix = 256. 256 / 8 bytes = 32 bytes. 32bytes / 4 (b/int) = 8 ints / tile
void loadSpriteData4(uint16_t id, const unsigned int* tile_data, const int tile_length) {
    volatile TILE* tile = tile_mem;
    tile += id;
    DMA_TRANSFER(tile, tile_data, tile_length, 3, DMA_CS_32 | DMA_ENABLE);
}

void loadPaletteData4(uint16_t id, const unsigned short* palette_data, const int num_palettes) {
    volatile PALETTE* toLoad = (volatile PALETTE*) palette_data;
    volatile PALETTE* seek_palette_mem = palette_mem + id;
    DMA_TRANSFER(seek_palette_mem, toLoad, num_palettes * WS_PALETTE / 2, 3, DMA_CS_32 | DMA_ENABLE);
}
