#include "player.h"

// singleton struct containing all player state info
struct PLAYER_DATA* PLAYER_DATA = &(struct PLAYER_DATA) {
    .projectile_count = 0
};
