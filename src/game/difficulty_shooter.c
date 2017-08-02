#include "../myLib.h"
#include "../entities.h"

#include "game_state.h"
#include "difficulty.h"
#include "difficulty_shooter.h"
#include "game.h"

INLINE ENTITY* addTallEnemy(uint8_t x) {
    register ENTITY* te = addEntity(TALL_ENEMY_STAND_TID, x, GROUND_OFFSET, TALL_ENEMY);
    te->health = 3;
    return te;
}

INLINE ENTITY* addShortEnemy(uint8_t x) {
    register ENTITY* se = addEntity(SHORT_ENEMY_STAND_TID, x, GROUND_OFFSET, SHORT_ENEMY);
    se->health = 1;
    return se;
}

static void level0(int wave) {
    addTallEnemy(SWAP_SIDE(wave & 1));
}

/*static void legacy(int roomLevel, int spawn_wave) {
    switch (roomLevel) {
        case 0:
            if (spawn_wave < 3) addTallEnemy(0);
            else if (spawn_wave == 4) addShortEnemy(1);
            else if (spawn_wave < 5) addTallEnemy(SWAP_SIDE(spawn_wave >> 1));
            else addTallEnemy(SWAP_SIDE(BRAND()));
            break;
        case 1:
            if (spawn_wave & 3) {
                addShortEnemy(SWAP_SIDE(BRAND()));
            } else if (spawn_wave & 1){
                addTallEnemy(SWAP_SIDE(BRAND()));
            }
            break;
        case 2:
            if (spawn_wave & 1) {
                for (int i = 0; i < 2; i++) addShortEnemy(0);
                addTallEnemy(SWAP_SIDE(1));
            } else {
                for (int i = 0; i < 3; i++) addShortEnemy(SWAP_SIDE(i));
            }
            break;
        default: // randomize for default
            ;
            const int num_enemies = qran_range(1, 5);
            for (int i = 0; i < num_enemies; i++) {
                BRAND() ? addShortEnemy(SWAP_SIDE(BRAND())) : addTallEnemy(SWAP_SIDE(BRAND()));
            }
            break;
    }
} */

static void debug(int spawn_wave) {
    if (spawn_wave == 0) {
        for (int i = 0; i < 80; ++i) {
            addShortEnemy(SWAP_SIDE(BRAND()));
        }
    }
    return;
}

const struct SHOOTER_CONF shooter_difficulty_configs[] = {
    { .level_cnt = level0, 5 },
    { debug, 3 },
    { .level_cnt = level0, 5 },
    { debug, 50 }
};

const struct SHOOTER_CONF *shooter_conf = &shooter_difficulty_configs[0];
const struct SHOOTER_CONF default_shooter_conf;
