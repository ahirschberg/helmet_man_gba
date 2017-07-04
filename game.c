#include "myLib.h"
#include "random.h"
#include "main.h"
#include "gfx_helper.h"
#include "entities.h"
#include "game_state.h"
#include "game.h"

#include "tile_scroller.h"
#include "game_over.h"

const ubyte difficulty_inc_len = 7 * 2;
uint score = 0;
bool game_ee_mode = FALSE;
ubyte hud_mode = 0;
int scoreToNext = 0;
ubyte difficulty_inc = 0;

// runner
// FIXME
const ubyte runner_points_inc[] =  {50, 7, 13, 15, 20, 25, 30};
byte scrollerDX = 0;

// shooter
const ubyte shooter_points_inc[] = {10, 13, 15, 17, 20, 25, 30};


void reset() {
    score = 0;
    game_ee_mode = FALSE;
    hud_mode = 0;
    scoreToNext = 0;
    difficulty_inc = 0;
}

// game over
ubyte game_over_anim_frame = 0;
INLINE int difficultyPlusPlus() {
    if (difficulty_inc < difficulty_inc_len - 1) difficulty_inc++;
    return difficulty_inc;
}

void drawFloor(ubyte offset) {
    drawImageFW(GROUND_OFFSET, 16, 16 * offset, tile_scroller);
}

enum GAME_STATE paused_lastState = START_SCREEN;
void setPaused(bool isPaused) {
    if (isPaused) {
        paused_lastState = gameState;
        initState(PAUSED);
    } else {
        gameState = paused_lastState;
    }
}

void initState(const enum GAME_STATE state) {
    PLAYER_ENTITY->f_invuln = 0; // prevent weird things from happening between states

    switch(state) {
        case START_SCREEN:
            reset();
            clearEntities(0);
            addEntity(PLAYER_STAND_TID, 110 + 3, GROUND_OFFSET, PLAYER);
            redrawBG2(0, SCREEN_HEIGHT);
            break;
        case START_SCREEN_NODRAW:
            drawFloor(0);
            break;
        case RUNNER_TRANSITION:
            clearEntities(1);
            scoreToNext+=runner_points_inc[difficultyPlusPlus() / 2];
            break;
        case RUNNER:
            clearEntities(1);

            hud_mode = 1; redrawHUD();

            setRunning(PLAYER_ENTITY, 0);
            setFacing(PLAYER_ENTITY->obj, 1);

            if (difficulty_inc < 3) scrollerDX = -7;
            else if (difficulty_inc < 5) scrollerDX = -8;
            else scrollerDX = -9;

            PLAYER_ENTITY->dx = ((byte) -1) * scrollerDX;
            break;
        case SHOOTER_TRANSITION:
            clearEntities(1);
            hud_mode = 2; redrawHUD();
            scoreToNext+=shooter_points_inc[difficultyPlusPlus() / 2];
            PLAYER_ENTITY->health = 5;
            setWalking(PLAYER_ENTITY, DIR_RIGHT);
            scrollerDX = 0;
            break;
        case SHOOTER:
            PLAYER_ENTITY->state=STANDING;
            PLAYER_ENTITY->dx = 0;
            ENEMY_DATA->spawn_wave_id = 0;
            ENEMY_DATA->ticks_until_next_spawn = 10;
            break;
        case GAME_OVER:
            hud_mode = 0;
            PLAYER_ENTITY->state = STANDING;
            scrollerDX = 0;
            drawRectFW(0, SCREEN_HEIGHT, BYTETOWORD(WHITE));
            break;
        case GAME_OVER_NODRAW:
            ;
            const int sdraw = 20;
            drawString(75, sdraw, "Press START!", BLACK);
            drawString(75 + 20, sdraw, "Programmed by Alex Hirschberg", BLACK);
            drawString(75 + 32, sdraw, "Art by Chris Thompson", BLACK);
            PLAYER_ENTITY->dx = 0;
            break;
        default:
            break;
    }
    gameState = state;
}

void gameOver() {
    if (!game_ee_mode) initState(GAME_OVER);
    //else PUTS("Prevented game from ending.");
}

int rnum = 0x0;
u16 color = BLACK;
ubyte last_player_health = -1;
bool ee_longTitle = FALSE;

INLINE ENTITY* addTallEnemy(int x) {
    register ENTITY* te = addEntity(TALL_ENEMY_STAND_TID, x, GROUND_OFFSET, TALL_ENEMY);
    te->health = 3;
    return te;
}

INLINE ENTITY* addShortEnemy(int x) {
    register ENTITY* se = addEntity(SHORT_ENEMY_STAND_TID, x, GROUND_OFFSET, SHORT_ENEMY);
    se->health = 1;
    return se;
}

void spawnEnemies(int roomLevel, int spawn_wave) {
    // Uncomment for a debug fight
//    if (spawn_wave == 1) {
//        for (int i = 0; i < 10; ++i) {
//            addShortEnemy(0);
//        }
//    }
//    return;
    switch (roomLevel) {
        case 0:
            if (spawn_wave < 3) addTallEnemy(0);
            else if (spawn_wave == 4) addShortEnemy(1);
            else if (spawn_wave < 5) addTallEnemy(SWAP_SIDE(spawn_wave >> 1));
            else addTallEnemy(SWAP_SIDE(BRAND()));
            break;
        case 1:
            if (spawn_wave & 3) {
                addShortEnemy(SWAP_SIDE(BRAND()));
            } else if (spawn_wave & 1){
                addTallEnemy(SWAP_SIDE(BRAND()));
            }
            break;
        case 2:
            if (spawn_wave & 1) {
                for (int i = 0; i < 2; i++) addShortEnemy(0);
                addTallEnemy(SWAP_SIDE(1));
            } else {
                for (int i = 0; i < 3; i++) addShortEnemy(SWAP_SIDE(i));
            }
            break;
        default: // randomize for default
            ;
            const int num_enemies = qran_range(1, 5);
            for (int i = 0; i < num_enemies; i++) {
                BRAND() ? addShortEnemy(SWAP_SIDE(BRAND())) : addTallEnemy(SWAP_SIDE(BRAND()));
            }
            break;
    }
}

ubyte rock_lobster = 0; // FIXME move to game_state with descriptive name.
void tickGame(const uint frame) {
    const ubyte frame4s = frame & 0xFF;
    switch(gameState) {
        case START_SCREEN_NODRAW:
            if (ee_longTitle && frame4s == rnum) {
                const int action_sel = qran_range(0, 13);
                int movement_dir = qran_range(0, 2);
                if (movement_dir == 0) movement_dir = -1;
                if (action_sel < 4) setWalking(PLAYER_ENTITY, movement_dir);
                else if (action_sel < 10) setRunning(PLAYER_ENTITY, movement_dir);
                else setJumping(PLAYER_ENTITY, 20);

                if (frame > 60 * 60) {
                    BF_SET(PLAYER_ENTITY->obj->attr2, 0x1, ATTR2_PALBANK);
                }

                if (qran_range(0, 4) == 0) {
                    setJumping(PLAYER_ENTITY, 10);
                }
                rnum = qran_range(0, 0xF);
            } else if (!ee_longTitle && frame4s == rnum) {
                setJumping(PLAYER_ENTITY, frame / (60 * 5) + 5);
                if (frame > 60 * 5) {
                    ee_longTitle = TRUE;
                }
            }
            break;
        case RUNNER_TRANSITION:
            ;
            const byte dir = TRIBOOL(10 - ENT_X(PLAYER_ENTITY));
            setRunning(PLAYER_ENTITY, dir); // player walks towards col 10

            if (IABS(ENT_X(PLAYER_ENTITY) - 10) < 4) {
                BF_SET(PLAYER_ENTITY->obj->attr1, 10, ATTR1_X);
                initState(RUNNER);
            }
            break;
        case RUNNER:
            if (((signed int)score) - scoreToNext >= 0) {
                initState(SHOOTER_TRANSITION);
            } else {
                if ((frame & 0xF) == 0) {
                    drawFloor(0);
                } else if ((frame & 0xF) == 8) {
                    drawFloor(1);
                }
                if (--rnum <= 0) {
                    if (objs_length < 4) {
                        const int ran = qran_range(0, 3);
                        switch(ran) {
                            case 0: ;
                                const uint rock_tid = (rock_lobster++ & 1) ? OBSTACLE_ROCK1_TID : OBSTACLE_ROCK2_TID;
                                addEntity(rock_tid, SCREEN_WIDTH, GROUND_OFFSET, OBSTACLE_ROCK);
                                break;
                            case 1:
                                addEntity(OBSTACLE_CACTUS_TID, SCREEN_WIDTH, GROUND_OFFSET, OBSTACLE_CACTUS);
                                break;
                            case 2:
                                addEntity(OBSTACLE_SHEET_TID, SCREEN_WIDTH, GROUND_OFFSET, OBSTACLE_SHEET);
                                break;
                            default:
                                addEntity(PLAYER_HURT_TID, SCREEN_WIDTH, GROUND_OFFSET, OBSTACLE_ROCK);
                        }
                    }
                    // TODO: make this more tuned to difficulty
                    rnum = qran_range(100 - difficulty_inc * 2, 150 - difficulty_inc * 3);
                }
            }
            break;
        case SHOOTER_TRANSITION:
            if (BF_GET(PLAYER_ENTITY->obj->attr1, ATTR1_X) > SCREEN_WIDTH / 2 - 10) {
                initState(SHOOTER);
            }
            break;
        case SHOOTER:
            if (((signed int)score) - scoreToNext >= 0) {
                initState(RUNNER_TRANSITION);
            } else {
                if ((frame & 3) == 0 && PLAYER_ENTITY->health != last_player_health) {
                    redrawHUD();
                }
                if ((frame & 0x7) == 4 && --ENEMY_DATA->ticks_until_next_spawn <= 0 && objs_length < 8) {
                    ENEMY_DATA->ticks_until_next_spawn = (int)(1.0f / (difficulty_inc + 1) * 30);
                    spawnEnemies(difficulty_inc / 2, ENEMY_DATA->spawn_wave_id++);
                }
            }
            break;
        case GAME_OVER:
            PLAYER_ENTITY->dx = 0;
            if ((frame & 3) == 0) {
                if (game_over_anim_frame < 20) {
                    drawImageFW(50, 20, (game_over_anim_frame++) * 20, game_over);
                } else {
                    game_over_anim_frame = 0;
                    initState(GAME_OVER_NODRAW);
                }
            }
            break;
        default:
            break;
    }
    tickEntities(frame);
}

INLINE void runner_checkPlayerCollisions() {
    const ubyte player_x = (const ubyte) ENT_X(PLAYER_ENTITY);
    const ubyte player_y = (const ubyte) groundDist(PLAYER_ENTITY);
    const int player_width = attrs(PLAYER_ENTITY).width;

    int i = 1;
    while (i < objs_length) {
        const short ent_x = ENT_X(allEntities+i);

        const ENTITY_ATTRS obstacle_attrs = attrs(allEntities + i);
        const byte fix_sprite_height_a_little = (allEntities[i].type == OBSTACLE_SHEET) ? 0 : 4;
        if (player_y < obstacle_attrs.height - fix_sprite_height_a_little && player_x < ent_x + obstacle_attrs.width / 2 // why?? 
                && player_x + player_width - 5 > ent_x) {
            gameOver();
        }
        if (ent_x + obstacle_attrs.width <= 0) {
            score++;
            removeEntity(i--);
        }
        i++;
    }
}

bool hurtEntity(ENTITY* e, byte damage, byte dir) {
    setHurt(e, dir);
    if (((signed char)e->health) - damage <= 0) {
        e->health = 0;
        return TRUE;
    }
    e->health -= damage;
    e->f_invuln = ENT_MAX_INVULN_FRAMES;
    return FALSE;
}

int decrementInvulnFrames(ENTITY* e) {
    if (e->f_invuln > 0) {
        e->f_invuln--;
        if (e->f_invuln == 0) {// && IABS(e->dx) == HURT_DX) { // hack to prevent dx being set to 0 when it probably shouldn't
            e->dx = 0;
        }
    }
    return e->f_invuln;
}

INLINE void shooter_checkPlayerCollisions() {
    byte playerHurt_tribool = 0;
    PUTI(PLAYER_ENTITY->f_invuln);

    int engager_i = 0;
    bool player_has_hurt_enemy = FALSE;
    while (engager_i < objs_length) {
        if (allEntities[engager_i].type != PROJECTILE
                    && allEntities[engager_i].type != PLAYER) {
            engager_i++;
            continue;
        }
        ENTITY* engager = allEntities + engager_i;
        const enum ENTITY_TYPE engager_type = engager->type;
        const ubyte engager_x = ENT_X(engager);
        const ubyte engager_y = groundDist(engager);
        const ubyte engager_width = attrs(engager).width;
        const ubyte engager_height = attrs(engager).height;

        int i = 1;
        while (i < objs_length) {
            if (allEntities[i].isDead || (allEntities[i].type == PROJECTILE) || allEntities[i].type == PLAYER) {
                i++;
                continue;
            }

            const ubyte ent_x = ENT_X(allEntities+i);
            const ubyte ent_y = groundDist(allEntities + i);
            const ENTITY_ATTRS attrs = attrs(allEntities + i);
            // check bounding boxes
            const ubyte eheight = attrs.height;
            const ubyte ewidth  = attrs.width;

            if (engager_type == PLAYER) {
                const uint wider = (ewidth > engager_width) ? ewidth : engager_width;
                const uint taller = (eheight > engager_height) ? eheight : engager_height;

                if (engager_y - ent_y > engager_width - 8
                        && IABS(engager_x - ent_x) <= wider + 5  // make the player head jumping more forgiving
                        && IABS(engager_y - ent_y) <= PLAYER_HEIGHT) {

                    // prevent the player from jumping on multiple enemies in the same frame, but still give them the
                    // jump boost
                    if (!player_has_hurt_enemy) {
                        setDead(allEntities + i);
                        score++;
                        player_has_hurt_enemy = TRUE;
                    }
                    if (allEntities[i].type == SHORT_ENEMY) engager->dy = 6;
                    else engager->dy = 4;
                } else if (IABS(engager_x - ent_x) < wider
                        && IABS(engager_y - ent_y) < taller) {

                    playerHurt_tribool = TRIBOOL(engager_x - ent_x);
                    if (playerHurt_tribool == 0) playerHurt_tribool = 1; // guarantee tribool not 0;
                }
            } else if (engager_type == PROJECTILE) {
                if (IABS(engager_x - ent_x) < ewidth
                        && IABS(engager_y - ent_y) < eheight) {

                    if (allEntities[i].type == SHORT_ENEMY) { // projectiles bounce off short enemies
                        engager->dy=5;
                        engager->isDead = TRUE;
                    } else {
                        if (allEntities[i].f_invuln == 0) {
                            const bool killed = hurtEntity(allEntities + i, 1, TRIBOOL(engager_x - ent_x));
                            if (killed) {
                                setDead(allEntities + i);
                                score++;
                            }
                        }
                        removeEntity(engager_i--);
                    }
                }
            }
            i++;
        }
        engager_i++;
    }

    if (playerHurt_tribool && PLAYER_ENTITY->f_invuln == 0) { // the player is newly hurt in this frame
        if (hurtEntity(PLAYER_ENTITY, 1, playerHurt_tribool)) {
            gameOver();
        }
        PLAYER_ENTITY->dx = playerHurt_tribool * 3;
    }
}

void tickEntities(const uint count) {
    if (!(count & 3)) {
        for (int i = 0; i < objs_length; i++) {
            ENTITY* curr = allEntities + i;
            if (entity_attrs[curr->type].gravityEnabled) gravity(curr);
            else OBJ_DY(*curr->obj, ~(curr->dy)+1);

            decrementInvulnFrames(curr);
            if (gameState == SHOOTER) {
                bool stoppedWraparound = FALSE;
                if (ENT_X(curr) + (curr)->dx > SCREEN_WIDTH - attrs(curr).width) { // wrap around
                    BF_SET(curr->obj->attr1, SCREEN_WIDTH - attrs(curr).width, ATTR1_X);
                    stoppedWraparound = TRUE;
                } else if (ENT_X(curr) + curr->dx < 0) { // wrap around
                    BF_SET(curr->obj->attr1, 0, ATTR1_X);
                    stoppedWraparound = TRUE;
                } else {
                    OBJ_DX(curr->obj->attr1, ((curr)->dx + scrollerDX));
                }

                // instead of stopping wraparound on projectiles, just remove them
                if (stoppedWraparound && curr->type == PROJECTILE) {
                    removeEntity(i--);
                }
            } else {
                OBJ_DX(curr->obj->attr1, ((curr)->dx + scrollerDX));
            }

            byte ent_action_rand = qran_range(0,20);
            if (!curr->isDead) {
                switch (curr->type) {
                    case TALL_ENEMY:
                        if (ent_action_rand == 3 && !curr->isJumping) setJumping(curr, 6);
                        // FALLTHROUGH TO SHORT ENEMY
                    case SHORT_ENEMY:
                        if(!curr->isJumping && ent_action_rand > 16) { // chance to find player again
                            setWalking(curr, TRIBOOL(ENT_X(PLAYER_ENTITY) - ENT_X(curr) ));
                        }
                    default:
                        break;
                }
            }
        }

    }
    switch (gameState) {
        case RUNNER:
            if ((count & 0x3)==2) {
                runner_checkPlayerCollisions();
            }
            break;
        case SHOOTER:
            if ((count & 0x7)==2) {
                shooter_checkPlayerCollisions(count);
            }
            break;
        default:
            break;
    }
}

