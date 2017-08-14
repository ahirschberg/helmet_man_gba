#ifndef ENTITIES_H
#define ENTITIES_H

#include "render/sprites.h"
#include "random.h"
#include "render/gfx_helper.h"

#define PLAYER_HEIGHT       32
#define PLAYER_WIDTH        16
#define OBSTACLE_HEIGHT     16
#define OBSTACLE_WIDTH      16
#define PLAYER_TPF          8
#define TALL_ENEMY_TPF      8
#define SHORT_ENEMY_TPF     4
#define PROJECTILE_TPF      1

// animation lengths
#define PLAYER_WALK_LEN     (10 * PLAYER_TPF)
#define PLAYER_RUN_LEN      (9 * PLAYER_TPF)
#define PLAYER_HURT_LEN     (5 * PLAYER_TPF)
#define OBSTACLES_LEN       (2 * 4)
#define TALL_ENEMY_WALK_LEN (5 * TALL_ENEMY_TPF)
#define TALL_ENEMY_HURT_LEN (3 * TALL_ENEMY_TPF) // Decreased from 5
#define SHORT_ENEMY_WALK_LEN (5 * SHORT_ENEMY_TPF)
#define SHORT_ENEMY_HURT_LEN (3 * SHORT_ENEMY_TPF)
#define SHORT_ENEMY_ATTACK_LEN (6 * SHORT_ENEMY_TPF)

// entity tile positions
#define PLAYER_STAND_TID    512
#define PLAYER_WALK_TID     (PLAYER_STAND_TID + PLAYER_TPF)
#define PLAYER_JUMP_TID     (PLAYER_RUN_TID)
#define PLAYER_RUN_TID      (PLAYER_WALK_TID + PLAYER_WALK_LEN)
#define PLAYER_HURT_TID     (PLAYER_RUN_TID + PLAYER_RUN_LEN)

#define PROJECTILE_TID      (PLAYER_HURT_TID + PLAYER_HURT_LEN)
#define PROJECTILE_LEN      (4 * PROJECTILE_TPF)

#define TALL_ENEMY_STAND_TID        (PROJECTILE_TID + PROJECTILE_LEN)
#define TALL_ENEMY_WALK_TID         (TALL_ENEMY_STAND_TID + TALL_ENEMY_TPF)
#define TALL_ENEMY_HURT_TID         (TALL_ENEMY_WALK_TID + TALL_ENEMY_WALK_LEN)
#define TALL_ENEMY_JUMP_TID         (TALL_ENEMY_STAND_TID)

#define SHORT_ENEMY_STAND_TID        (TALL_ENEMY_HURT_TID + TALL_ENEMY_HURT_LEN)
#define SHORT_ENEMY_JUMP_TID         (SHORT_ENEMY_STAND_TID)
#define SHORT_ENEMY_WALK_TID         (SHORT_ENEMY_STAND_TID + SHORT_ENEMY_TPF)
#define SHORT_ENEMY_HURT_TID         (SHORT_ENEMY_WALK_TID + SHORT_ENEMY_WALK_LEN)
#define SHORT_ENEMY_ATTACK_TID       (SHORT_ENEMY_HURT_TID + SHORT_ENEMY_HURT_LEN)

#define OBSTACLE_ROCK1_TID          (SHORT_ENEMY_ATTACK_TID + SHORT_ENEMY_ATTACK_LEN)
#define OBSTACLE_ROCK2_TID          (OBSTACLE_ROCK1_TID + 4)
#define OBSTACLE_CACTUS_TID         (OBSTACLE_ROCK2_TID + 4)
#define OBSTACLE_SHEET_TID          (OBSTACLE_CACTUS_TID + 8)


#define PLAYER_ENTITY       (allEntities + 0)
#define ENTITIES_LEN        0x80

#define GROUND_OFFSET (SCREEN_HEIGHT - 16)
#define DIR_LEFT            (-1)
#define DIR_RIGHT           (1)
#define ENT_MAX_INVULN_FRAMES 30

#define ENT_X(_ent)         sext9(BF_GET((_ent)->obj->attr1, ATTR1_X))
#define ENT_Y(_ent)         (BF_GET((_ent)->obj->attr0, ATTR0_Y))

enum ENT_STATE {
    STANDING = 0, WALKING, RUNNING, HURT
};

typedef struct ENTITY_ATTRS {
    const uint8_t height;
    const uint8_t width;
    const bool gravityEnabled;
    const uint8_t tile_shape;
    const uint8_t tile_size;
    const uint8_t default_palette;
    const uint8_t invuln_max_frames;
} ENTITY_ATTRS;

enum ENTITY_TYPE {
    PLAYER = 0, PROJECTILE, TALL_ENEMY, SHORT_ENEMY, OBSTACLE_ROCK, OBSTACLE_CACTUS, OBSTACLE_SHEET
};

static const ENTITY_ATTRS entity_attrs[] = {
    {PLAYER_HEIGHT, PLAYER_WIDTH, TRUE, SPRITE_TALL, SPRITE_SIZE_LG, .default_palette = 0, .invuln_max_frames = 30},
    {8, 8, FALSE, SPRITE_SQUARE, SPRITE_SIZE_SM, .default_palette = 1},
    {PLAYER_HEIGHT, PLAYER_WIDTH, TRUE, SPRITE_TALL, SPRITE_SIZE_LG, .default_palette = 2, .invuln_max_frames = 1},
    {16, 16, TRUE, SPRITE_SQUARE, SPRITE_SIZE_MD, .default_palette = 3, .invuln_max_frames = 1},
    {16, 16, TRUE, SPRITE_SQUARE, SPRITE_SIZE_MD, .default_palette = 4},
    {PLAYER_HEIGHT, 16, TRUE, SPRITE_TALL, SPRITE_SIZE_LG, .default_palette = 5},
    {16, 64, TRUE, SPRITE_WIDE, SPRITE_SIZE_LG, .default_palette = 6},
};

#define attrs(e)    (entity_attrs[(e)->type])

typedef struct ENTITY {
    volatile OBJ_ATTR* obj;
    enum ENTITY_TYPE type;
    int8_t dy;
    int8_t dx;
    enum ENT_STATE state;
    bool isJumping;
    bool isDead;
    uint8_t lastAnimatedTile;
    uint8_t health;
    uint8_t f_invuln;
} ENTITY;

ENTITY* addEntity(const uint32_t id, uint8_t x, uint8_t y, enum ENTITY_TYPE type);
void clearEntities(int firstIdx);
ENTITY removeEntity(int i);
void gravity(ENTITY* e);
void tickEntityAnimations();
void addProjectileFrom(ENTITY* e);

INLINE bool isOffScreenX(ENTITY* e) {
    return ENT_X(e) < 0 || ENT_X(e) > SCREEN_WIDTH;
}

INLINE int getFacing(volatile OBJ_ATTR* obj) {
    const bool flipped = BF_GET(obj->attr1, ATTR1_HFLIP);
    return flipped ? -1 : 1;
}

INLINE void setFacing(volatile OBJ_ATTR* obj, int8_t dir) {
    const bool toFlip = (dir == -1) ? TRUE : FALSE;
    BF_SET(obj->attr1, toFlip, ATTR1_HFLIP);
}

INLINE void setEntityState(ENTITY* e, enum ENT_STATE newState) {
    if (e->state != newState) {
        e->state = newState;
        e->lastAnimatedTile = 0;
    } // else do nothing
}

INLINE void setWalking(ENTITY* e, tribool dir) {
    setEntityState(e, WALKING);
    setFacing(e->obj, dir);
    e->dx = 3 * dir;
}

// set walking, but only if the player is not in a higher state (ie hurt)
INLINE void setWalkingWeak(ENTITY* e, tribool dir) {
    if (e->state < WALKING) setWalking(e, dir);
    else { // duplicate from setWalking
        setFacing(e->obj, dir);
        e->dx = 3 * dir;
    }
}
INLINE void setRunning(ENTITY* e, tribool dir) {
    setEntityState(e, RUNNING);
    setFacing(e->obj, dir);
    e->dx = 7 * dir;
}
INLINE void setJumping(ENTITY* e, tribool dy) {
    e->isJumping = TRUE;
    e->dy = dy;
}
INLINE void setStanding(ENTITY* e) {
    setEntityState(e, STANDING);
    e->dx = 0;
}

#define HURT_DX 4

INLINE void setHurt(ENTITY* e, tribool dir) {
    setEntityState(e, HURT);
    e->dx = dir * HURT_DX;
    e->dy = 2;
}
INLINE void setDead(ENTITY* e) {
    e->lastAnimatedTile = 0;
    e->isJumping = FALSE;
    e->isDead = TRUE;
    e->dx = 0;
    e->dy = 10;
    // draw blood
    const int x = ENT_X(e) + 8;
    for (int i = 0; i < 10; i+=qran_range(1,4)) {
        setPixel(GROUND_OFFSET, x + i, COLOR(0, 20, 0));
    }

    // vertically flip sprite
    /* BF_SET(e->obj->attr1, TRUE, ATTR1_VFLIP); */
}

INLINE int groundDist(ENTITY* e) {
    const int y = BF_GET(e->obj->attr0, ATTR0_Y);
    return GROUND_OFFSET - (y + entity_attrs[e->type].height);
}

ENTITY allEntities[ENTITIES_LEN];
extern uint8_t objs_length;

extern OBJ_ATTR _setupObj(const uint32_t id, uint8_t x, uint8_t y, enum ENTITY_TYPE type);

#endif
