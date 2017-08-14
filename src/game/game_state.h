#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "../myLib.h"

enum GAME_STATE {
    START_SCREEN, START_SCREEN_NODRAW, PAUSED, RUNNER_TRANSITION, RUNNER, SHOOTER_TRANSITION, SHOOTER, GAME_OVER, GAME_OVER_NODRAW
};

struct PLAYER_DATA {
    int projectile_count;
};

struct ENEMY_DATA {
    int num_enemies_alive;
    int32_t ticks_until_next_spawn;
};

extern enum GAME_STATE gameState;
extern struct PLAYER_DATA* PLAYER_DATA;
extern struct ENEMY_DATA* ENEMY_DATA;

#endif
