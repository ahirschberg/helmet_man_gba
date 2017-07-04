#ifndef GFX_HELPER_H
#define GFX_HELPER_H

extern const unsigned char fontdata_6x8[12288];

// FW functions are much faster because they do direct DMA and can do full word transfers

INLINE void drawRect(int r, int c, int width, int height, volatile const uint16_t* color) {
    for (int i = 0; i < height; i++) {
        DMA_TRANSFER(VIDEOBUFF + OFFSET(r+i, c), color, width, 3, DMA_ENABLE | DMA_SRC_FIXED);
    }
}

INLINE void drawRectFW(int r, int height, volatile const uint32_t dcolor) {
    DMA_TRANSFER(OFFSET(r, VIDEOBUFF), &dcolor, (SCREEN_WIDTH * height) / 2, 3, DMA_ENABLE | DMA_SRC_FIXED | DMA_CS_32);
}

INLINE void drawImage3(int r, int c, int width, int height, const uint16_t* image)
{
    for (int i = 0; i < height; i++) {
        if (r + i < 0) continue; // do not draw out of bounds! fixes flicker near top of screen

        DMA_TRANSFER(VIDEOBUFF + OFFSET(r + i, c), OFFSET3(i, image, width), width, 3, DMA_ENABLE);
    }
}

INLINE void drawImageFW(int r, int height, int imgr, const uint16_t* image) {
    DMA_TRANSFER(OFFSET(r, VIDEOBUFF), OFFSET(imgr, image), (SCREEN_WIDTH * height) / 2, 3, DMA_ENABLE | DMA_CS_32);
}
// is there a better way to inline these??
INLINE void setPixel(int row, int col, uint16_t color)
{
    VIDEOBUFF[OFFSET(row, col)] = color;
}

INLINE void drawChar(int row, int col, char ch, uint16_t color)
{
	for(int r=0; r<8; r++)
	{
		for(int c = 0; c<6; c++)
		{
			if(fontdata_6x8[OFFSET3(r, c, 6)+ch*48])
			{
				setPixel(row+r, col+c, color);
			}
		}
	}
}

INLINE int drawString(int row, int col, char *str, uint16_t color)
{
	while(*str)
	{
        if (*str != ' ') {
            drawChar(row, col, *str, color);
        }

		col += 6;
        str++;
	}
    return col;
}

INLINE void _digits(int num, char* toFill, int fill_len) {
    toFill[fill_len - 1] = '\0';
    for (int i = fill_len - 2; i >= 0; i--) {
        toFill[i] = '0' + (num - (num / 10) * 10);
        num /= 10;
    }
}

INLINE int drawInt(int row, int col, int num, int places, uint16_t color) {
    char toFill[places + 1];
    _digits(num, toFill, places + 1);
    return drawString(row, col, toFill, color);
}

#endif
