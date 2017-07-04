extern byte scrollerDX;
extern uint score;
extern bool game_ee_mode;
extern ubyte hud_mode;

extern void initState(const enum GAME_STATE state);
extern void gameOver();
extern void tickGame(const uint frame);
void tickEntities(const uint frame);
extern void setPaused(bool isPaused);

#define SWAP_SIDE(b) ((b & 1) * (SCREEN_WIDTH - 7) + 2)
#define BRAND() qran_range(0, 2)
