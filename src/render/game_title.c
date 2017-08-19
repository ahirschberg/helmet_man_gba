#include "gfx_helper.h"
#include "game_title.h"
#include "../assets/game_title.h"
#include "../assets/background_img.h"

#define HSTART_OFFSET 55

void GameTitle_add_to_background() {
    drawImageFW(HSTART_OFFSET, 15, 0, game_title);
}

void GameTitle_remove_from_background() {
    drawImageFW(HSTART_OFFSET, 15, HSTART_OFFSET, background_img);
}
