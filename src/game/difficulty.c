#include "../myLib.h"
#include "game_state.h"
#include "difficulty.h"
#include "difficulty_runner.h"
#include "difficulty_shooter.h"

int32_t Difficulty_level;
uint32_t Difficulty_points_increase_to_next;

void Difficulty_init(const enum GAME_STATE state, const int current_score) {
    runner_conf = &default_runner_conf;
    int difficulty_pts_increase = 0;
    switch(state) {
        case RUNNER:
            runner_conf = &runner_difficulty_configs[Difficulty_level / DIFFICULTY_NUM_STATES];
            difficulty_pts_increase = runner_conf->pts_to_next;
            break;
        case SHOOTER:
            shooter_conf = &shooter_difficulty_configs[Difficulty_level / DIFFICULTY_NUM_STATES];
            difficulty_pts_increase = shooter_conf->pts_to_next;
            DifficultyShooter_reset();
            break;
        default:
            break;
    }
    Difficulty_points_increase_to_next = current_score + difficulty_pts_increase;
    //PUTI(Difficulty_level);
}

void Difficulty_reset() {
    Difficulty_level = 0;
    Difficulty_points_increase_to_next = 0;
}

void Difficulty_next() {
    if (Difficulty_level < DIFFICULTY_POINTS_LEN - 1) Difficulty_level++;
}

