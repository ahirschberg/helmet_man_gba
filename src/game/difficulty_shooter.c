#include "../myLib.h"
#include "../entities.h"

#include "game_state.h"
#include "difficulty.h"
#include "difficulty_shooter.h"
#include "game.h"

static uint32_t spawn_wave_id;

static ENTITY* addTallEnemy(uint8_t x) {
    register ENTITY* te = addEntity(TALL_ENEMY_STAND_TID, x, GROUND_OFFSET, TALL_ENEMY);
    te->health = 3;
    ENEMY_DATA->num_enemies_alive++;
    return te;
}

static ENTITY* addShortEnemy(uint8_t x) {
    register ENTITY* se = addEntity(SHORT_ENEMY_STAND_TID, x, GROUND_OFFSET, SHORT_ENEMY);
    se->health = 1;
    ENEMY_DATA->num_enemies_alive++;
    return se;
}

void DifficultyShooter_reset() {
    spawn_wave_id = 0;
}
void tick_shooter_level() {
    if (ENEMY_DATA->num_enemies_alive < shooter_conf->max_enemies) {
        shooter_conf->level_cnt(spawn_wave_id++);
    }
    // Debug statement for wave_id and enemies alive count
    //PUTI(spawn_wave_id * 100 + ENEMY_DATA->num_enemies_alive);
}

void set_enemy_dead(ENTITY* e) {
    ENEMY_DATA->num_enemies_alive--;
    setEntityState(e, HURT);
    setDead(e);
}

static inline void set_spawn_delay(int32_t delay) {
    ENEMY_DATA->ticks_until_next_spawn = delay;
}

static void level0(int wave) {
    addTallEnemy(SWAP_SIDE(wave & 1));
    set_spawn_delay(15);
}

static void level1(int wave) {
    if ((wave & 3) == 0) addShortEnemy(SWAP_SIDE(BRAND()));
    else addTallEnemy(SWAP_SIDE(BRAND()));

    if (wave < shooter_conf->pts_to_next - 1) set_spawn_delay(10);
    else set_spawn_delay(int32_t_MAX);
}

/*static void debug(int spawn_wave) {
    if (spawn_wave == 0) {
        for (int i = 0; i < 80; ++i) {
            addShortEnemy(SWAP_SIDE(BRAND()));
        }
    }
    return;
}*/

const struct SHOOTER_CONF shooter_difficulty_configs[] = {
    { .level_cnt = level0, .max_enemies = 1, .pts_to_next = 5 },
    { level0, 3, 10 },
    { level1, 2, 10 },
    { level1, 7, 7 },
};

const struct SHOOTER_CONF *shooter_conf = &shooter_difficulty_configs[0];
const struct SHOOTER_CONF default_shooter_conf;
