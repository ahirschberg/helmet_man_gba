#include "myLib.h"
#include "input.h"
#include "entities.h"
#include "game.h"
#include "random.h"

#include "player_sprites.h"
#include "obstacle_sprites.h"
#include "tall_enemy_sprites.h"
#include "short_enemy_sprites.h"
#include "projectile_sprites.h"
#include "background_img.h"

#include "gfx_helper.h"
#include "main.h"

/*
 * vblank occurs at 160
 * used for changing screen buffer w/o tearing or double buffer
 */
void waitForVblank() {
    while(SCANLINECOUNTER > 160);
    while(SCANLINECOUNTER < 160);
}

volatile u32 bgColor = BYTETOWORD(WHITE);

void redrawHUDFill(u32 color) {
     drawRectFW(0, 10, color);
}
void redrawBG2(int start, int height) {
    drawImageFW(start, height, start, background_img);
}
void redrawHUD() {
    if (hud_mode) {
        redrawHUDFill(bgColor);
        const int lastCol = drawString(1, 10, "Score:", BLACK);
        drawInt(1, lastCol, score, 4, BLUE);
        if (hud_mode > 1) {
            const ubyte phealth = PLAYER_ENTITY->health;
            for (int i = 0; i < phealth; i++) {
                drawChar(1, 200 - i * 6, 3, RED);
            }
        }
    }
}

unsigned int lastScore = -1;
void tick(const int frameCounter) {
    if ((frameCounter & 0x7) == 6) {
        tickEntityAnimations();
    }
    tickGame(frameCounter);

    if (score != lastScore) {
        lastScore = score;
        redrawHUD();
    }
}


int ee_next = 0;
int key_last = 0;
const unsigned int konami_ee[] = {KEY_UP, KEY_UP, KEY_DOWN, KEY_DOWN, 
                                  KEY_LEFT, KEY_RIGHT, KEY_LEFT, KEY_RIGHT, 
                                  KEY_B, KEY_A, KEY_START};
void draw()
{
    uint frameCounter = 0;
    loadPaletteData4(0, player_spritesPal, 1);
    loadSpriteData4(PLAYER_STAND_TID, (uint*) player_spritesTiles, player_spritesTilesLen);

    loadPaletteData4(1, projectile_spritesPal, 1);
    loadSpriteData4(PROJECTILE_TID, projectile_spritesTiles, projectile_spritesTilesLen);

    loadPaletteData4(2, tall_enemy_spritesPal, 1);
    loadSpriteData4(TALL_ENEMY_STAND_TID, tall_enemy_spritesTiles, tall_enemy_spritesTilesLen);

    loadPaletteData4(3, tall_enemy_spritesPal, 1);
    loadSpriteData4(SHORT_ENEMY_STAND_TID, short_enemy_spritesTiles, short_enemy_spritesTilesLen);

    loadPaletteData4(4, obstacle_rocksPal, 1);
    loadSpriteData4(OBSTACLE_ROCK1_TID, obstacle_rocksTiles, obstacle_rocksTilesLen);

    loadPaletteData4(5, obstacle_tallPal, 1);
    loadSpriteData4(OBSTACLE_CACTUS_TID, obstacle_tallTiles, obstacle_tallTilesLen);

    loadPaletteData4(6, obstacle_widePal, 1);
    loadSpriteData4(OBSTACLE_SHEET_TID, obstacle_wideTiles, obstacle_wideTilesLen);

    ENTITY* playerEntity = PLAYER_ENTITY;
    BF_SET(playerEntity->obj->attr2, 0, ATTR2_PALBANK);
    initState(START_SCREEN);
    redrawBG2(0, SCREEN_HEIGHT);
    do {
        key_poll();
        switch(gameState) {
            case START_SCREEN:
                initState(START_SCREEN_NODRAW);
                break;
            case START_SCREEN_NODRAW:
                if ((frameCounter & 0x7F) == 0) {
                    redrawBG2(70, 8);
                } else if ((frameCounter & 0x7F) == 64) {
                    drawString(70, 90, "Press Start", WHITE);
                }
                if (key_hit(konami_ee[ee_next])) {
                    key_last = konami_ee[ee_next];
                    ee_next++;
                } else if (key_prev_state() != 0 && key_prev_state() != konami_ee[ee_next - 1]){
                    ee_next = 0;
                    key_last = 0;
                }
                if (key_hit(KEY_START)) {
                    if (ee_next > 10) {
                        game_ee_mode = TRUE;
                        PUTS("Welcome to god mode.");
                        BF_SET(playerEntity->obj->attr2, 0xF, ATTR2_PALBANK); 
                    }
                    sqran(frameCounter);
                    redrawBG2(70, 8);

                    /* initState(RUNNER_TRANSITION); */
                    initState(SHOOTER_TRANSITION);
                }
                break;
            case RUNNER_TRANSITION:
                break;
            case RUNNER:
                if (key_hit(KEY_UP) && !playerEntity->isJumping) {
                    setJumping(playerEntity, 8);
                }
                if (key_hit(KEY_DOWN)) {
                    playerEntity->dy = playerEntity->dy=(-6);
                }
                break;
            case SHOOTER_TRANSITION:
                break;
            case SHOOTER:
                if (key_hit(KEY_A)) {
                    addProjectileFrom(playerEntity);
                }
                if (key_hit(KEY_UP) && !playerEntity->isJumping) {
                    setJumping(playerEntity, 8);
                }
                const int horiz_input = key_tri_horz();
                    if (horiz_input != 0) {
                        setWalkingWeak(playerEntity, horiz_input);
                    } else {
                        if (playerEntity->state <= WALKING) {
                            setStanding(playerEntity);
                        }
                    }
                break;
            default:
                break;
        }
        waitForVblank();
        tick(frameCounter++);
    } while(1);
}

int main()
{
    REG_DISPCTL = MODE3 | BG2_ENABLE | DCNT_OBJ | DCNT_OBJ_1D;
    draw();
}
