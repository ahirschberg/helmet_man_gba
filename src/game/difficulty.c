#include "../myLib.h"
#include "game_state.h"
#include "difficulty.h"
#include "difficulty_runner.h"

static const uint8_t shooter_points_increase[] = {3, 4, 15, 17, 20, 25, 30};
static const uint8_t* state_points_increase;

int32_t Difficulty_level;
uint8_t Difficulty_points_increase_to_next;

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
    if (Difficulty_level < DIFFICULTY_POINTS_LEN - 1) Difficulty_level++;
}
