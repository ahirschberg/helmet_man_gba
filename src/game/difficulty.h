#ifndef DIFFICULTY_H
#define DIFFICULTY_H


#define Difficulty_shouldTransition(score) ((int) score >= (int) Difficulty_points_increase_to_next)

#define DIFFICULTY_NUM_STATES 2
#define DIFFICULTY_NUM_LEVELS 4
#define DIFFICULTY_POINTS_LEN (DIFFICULTY_NUM_STATES * DIFFICULTY_NUM_LEVELS)

extern void Difficulty_reset();
extern void Difficulty_init(const enum GAME_STATE state, const int current_score);
extern uint32_t Difficulty_points_increase_to_next;
extern void Difficulty_next();

struct RUNNER_CONF {
    int32_t scroller_dx;
    int obstacle_spawn_delay_lower;
    int obstacle_spawn_delay_upper;
    int8_t max_objs;
    uint8_t pts_to_next;
};

struct SHOOTER_CONF {
    void (*level_cnt)(int spawn_wave_i);
    uint16_t max_enemies;
    uint8_t pts_to_next;
};

extern const struct RUNNER_CONF* runner_conf;
extern const struct SHOOTER_CONF* shooter_conf;
extern void tick_shooter_level();


#endif
