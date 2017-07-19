#include "myLib.h"
#include "game_state.h"
#include "difficulty.h"

#define DIFFICULTY_NUM_STATES 2

static const int32_t POINTS_LEN = 4 * DIFFICULTY_NUM_STATES;

static const uint8_t runner_points_increase[]  = {5, 2, 15, 20, 25, 30, 35};
static const uint8_t shooter_points_increase[] = {3, 4, 15, 17, 20, 25, 30};
static const uint8_t* state_points_increase;

int32_t Difficulty_level;
uint8_t Difficulty_points_increase_to_next;

static const struct RUNNER_CONF runner_difficulty_configs[] = {
        { .scroller_dx = -7, .obstacle_spawn_delay_lower = 100, .obstacle_spawn_delay_upper = 200, .max_objs = 10},
        { .scroller_dx = -8, .obstacle_spawn_delay_lower = 100, .obstacle_spawn_delay_upper = 200, .max_objs = 10},
        { .scroller_dx = -8, .obstacle_spawn_delay_lower = 90, .obstacle_spawn_delay_upper = 150, .max_objs = 10},
        { .scroller_dx = -9, .obstacle_spawn_delay_lower = 75, .obstacle_spawn_delay_upper = 100, .max_objs = 100}
};

static const struct RUNNER_CONF default_runner_conf = { .scroller_dx = 0 };
const struct RUNNER_CONF* runner_conf = &default_runner_conf;

void Difficulty_init(const enum GAME_STATE state, const int current_score) {
    Difficulty_points_increase_to_next = current_score + state_points_increase[Difficulty_level / DIFFICULTY_NUM_STATES];
    PUTI(Difficulty_points_increase_to_next);
    runner_conf = &default_runner_conf;
    switch(state) {
        case RUNNER:
            state_points_increase = runner_points_increase;
            runner_conf = &runner_difficulty_configs[Difficulty_level / DIFFICULTY_NUM_STATES];
            break;
        case SHOOTER:
            state_points_increase = shooter_points_increase;
            break;
        default:
            break;
    }
}

void Difficulty_reset() {
    Difficulty_level = 0;
    Difficulty_points_increase_to_next = 0;
}

void Difficulty_next() {
    if (Difficulty_level < POINTS_LEN - 1) Difficulty_level++;
}
