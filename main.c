#include "myLib.h"
#include "memmap.h"
#include "interrupts.h"
#include "input.h"
#include "random.h"
#include "gfx_helper.h"
#include "entities.h"
#include "game.h"

#include "player_sprites.h"
#include "obstacle_sprites.h"
#include "tall_enemy_sprites.h"
#include "short_enemy_sprites.h"
#include "projectile_sprites.h"
#include "background_img.h"

#include "main.h"
#include "bios.h"

uint frameCounter = 0;
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

void isr_main() {
    REG_IF = IRQ_VBLANK;
    REG_IFBIOS = IRQ_VBLANK;
}


int ee_next = 0;
int key_last = 0;
const unsigned int konami_ee[] = {KEY_UP, KEY_UP, KEY_DOWN, KEY_DOWN,
    KEY_LEFT, KEY_RIGHT, KEY_LEFT, KEY_RIGHT,
    KEY_B, KEY_A, KEY_START};

INLINE void loadAssets() {
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
}

INLINE void draw()
{
    ENTITY* playerEntity = PLAYER_ENTITY;
    BF_SET(playerEntity->obj->attr2, 0, ATTR2_PALBANK);
    bool isPaused = FALSE;
    initState(START_SCREEN);
    do {
        key_poll();

        if (key_hit(KEY_SELECT)) {
            isPaused = TRUE;
            setPaused(isPaused);
            drawRectFW(30, 50, BYTETOWORD(BLACK));
            drawString(40, 85, "Game paused.", WHITE);
            drawString(42 + 8, 50, "Press SELECT to unpause,", WHITE);
            drawString(42 + 8*2, 50, "or START to reset the game.", WHITE);
        } else {
            while (isPaused) {
                key_poll();

                if (key_hit(KEY_SELECT)) {
                    redrawBG2(30, 50);
                    isPaused = FALSE;
                    setPaused(isPaused);
                } else if (key_hit(KEY_START)) {
                    initState(START_SCREEN);
                    isPaused = FALSE;
                }
            }
        }

        switch(gameState) {
            case START_SCREEN:
                frameCounter = 0;
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
                if (key_hit(KEY_START)){ // || key_hit(KEY_A) || key_hit(KEY_B) || key_hit(KEY_SELECT)) {
                    if (ee_next > 10) {
                        game_ee_mode = TRUE;
                        PUTS("ENTER GOD MODE");
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
                if ((key_hit(KEY_UP) || key_hit(KEY_A)) && !playerEntity->isJumping) {
                    setJumping(playerEntity, 8);
                }
                if (key_hit(KEY_DOWN)) {
                    playerEntity->dy = playerEntity->dy=(-6);
                }
                break;
            case SHOOTER_TRANSITION:
                break;
            case SHOOTER:
                if (key_hit(KEY_B)) {
                    addProjectileFrom(playerEntity);
                }
                if ((key_hit(KEY_UP) || key_hit(KEY_A)) && !playerEntity->isJumping) {
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
            case GAME_OVER_NODRAW:
                if (key_hit(KEY_START)) {
                    initState(START_SCREEN);
                }
            default:
                break;
        }

        // hack fix stutter in VBlankIntrWait
        // how the heck does this fix the stuttering??
        // how the heck does this NOT cause tearing
        // Alex 7-2-17: still no idea, but it's a feature now!
        if (frameCounter & 1) VBlankIntrWait();

        tick(frameCounter++);
    } while(1);
}

int main()
{
    REG_DISPCTL = 3 | BG2_ENABLE | DCNT_OBJ | DCNT_OBJ_1D;

    // setup interrupts
    REG_ISR_MAIN = isr_main; // tell gba what function to call
    REG_DISPSTAT |= (1<<3); //1<<3 vblank??
    REG_IE |= IRQ_VBLANK;
    REG_IME = 1;

    loadAssets();
    draw();
}
