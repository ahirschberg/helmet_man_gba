#include "game_state.h"

enum GAME_STATE gameState;
// singleton struct containing all player state info
struct PLAYER_DATA* PLAYER_DATA = &(struct PLAYER_DATA) {
    .projectile_count = 0
};

struct ENEMY_DATA* ENEMY_DATA = &(struct ENEMY_DATA) {
    .num_enemies_alive = 0,
    .ticks_until_next_spawn = 0,
};
