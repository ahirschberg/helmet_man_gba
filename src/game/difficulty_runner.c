#include "../myLib.h"
#include "game_state.h"
#include "difficulty.h"
#include "difficulty_runner.h"

const struct RUNNER_CONF runner_difficulty_configs[] = {
        { .scroller_dx = -7, .obstacle_spawn_delay_lower = 100, .obstacle_spawn_delay_upper = 200, .max_objs = 10},
        { -8, 100, 200, 10},
        { -8, 90, 150, 10},
        { -9, 75, 100, 100}
};

const uint8_t runner_points_increase[]  = {5, 2, 15, 20};

const struct RUNNER_CONF default_runner_conf = { .scroller_dx = 0 };
const struct RUNNER_CONF* runner_conf = &default_runner_conf;
