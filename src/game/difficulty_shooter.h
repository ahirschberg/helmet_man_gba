#ifndef DIFFICULTY_SHOOTER_H
#define DIFFICULTY_SHOOTER_H

extern const struct SHOOTER_CONF shooter_difficulty_configs[DIFFICULTY_NUM_LEVELS];
extern const struct SHOOTER_CONF *shooter_conf;
extern const struct SHOOTER_CONF default_shooter_conf;

#define SWAP_SIDE(b) ((b & 1) * (SCREEN_WIDTH - 7) + 2)
#define BRAND() qran_range(0, 2)


#endif
