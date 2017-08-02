#ifndef GAME_H
#define GAME_H

extern int8_t scrollerDX;
extern uint32_t score;
extern bool game_ee_mode;
extern uint8_t hud_mode;

extern void initState(const enum GAME_STATE state);
extern void gameOver();
extern void tickGame(const uint32_t frame);
void tickEntities(const uint32_t frame);
extern void setPaused(bool isPaused);

#endif
