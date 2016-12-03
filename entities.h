#include "sprites.h"

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
#define TALL_ENEMY_HURT_LEN (5 * TALL_ENEMY_TPF)
#define SHORT_ENEMY_WALK_LEN (5 * SHORT_ENEMY_TPF)
#define SHORT_ENEMY_HURT_LEN (5 * SHORT_ENEMY_TPF)

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
#define SHORT_ENEMY_WALK_TID         (SHORT_ENEMY_STAND_TID + SHORT_ENEMY_TPF)
#define SHORT_ENEMY_HURT_TID         (SHORT_ENEMY_WALK_TID + SHORT_ENEMY_WALK_LEN)
#define SHORT_ENEMY_JUMP_TID         (SHORT_ENEMY_STAND_TID)

#define OBSTACLE_ROCK1_TID          (SHORT_ENEMY_HURT_TID + SHORT_ENEMY_HURT_LEN)
#define OBSTACLE_ROCK2_TID          (OBSTACLE_ROCK1_TID + 4)
#define OBSTACLE_CACTUS_TID         (OBSTACLE_ROCK2_TID + 4)
#define OBSTACLE_SHEET_TID          (OBSTACLE_CACTUS_TID + 8)


#define PLAYER_ENTITY       (allEntities + 0)
#define ENTITIES_LEN        0x80

#define GROUND_OFFSET (SCREEN_HEIGHT - 16)
#define DIR_LEFT            (-1)
#define DIR_RIGHT           (1)

#define ENT_X(_ent)         (BF_GET((_ent)->obj->attr1, ATTR1_X))
#define ENT_Y(_ent)         (BF_GET((_ent)->obj->attr0, ATTR0_Y))

enum ENT_STATE {
    STANDING = 0, WALKING, RUNNING, HURT
};

typedef struct ENTITY_ATTRS {
    const ubyte height;
    const ubyte width;
    const bool gravityEnabled;
    const ubyte tile_shape;
    const ubyte tile_size;
    const ubyte default_palette;
} ENTITY_ATTRS;

enum ENTITY_TYPE {
    PLAYER = 0, PROJECTILE, TALL_ENEMY, SHORT_ENEMY, OBSTACLE_ROCK, OBSTACLE_CACTUS, OBSTACLE_SHEET
};

static const ENTITY_ATTRS entity_attrs[] = { 
    {PLAYER_HEIGHT, PLAYER_WIDTH, TRUE, SPRITE_TALL, SPRITE_SIZE_LG, 0},
    {8, 8, FALSE, SPRITE_SQUARE, SPRITE_SIZE_SM, 1},
    {PLAYER_HEIGHT, PLAYER_WIDTH, TRUE, SPRITE_TALL, SPRITE_SIZE_LG, 2},
    {16, 16, TRUE, SPRITE_SQUARE, SPRITE_SIZE_MD, 3},
    {16, 16, TRUE, SPRITE_SQUARE, SPRITE_SIZE_MD, 4},
    {PLAYER_HEIGHT, 16, TRUE, SPRITE_TALL, SPRITE_SIZE_LG, 5},
    {16, 64, TRUE, SPRITE_WIDE, SPRITE_SIZE_LG, 6},
};

#define attrs(e)    (entity_attrs[(e)->type])

typedef struct ENTITY {
    volatile OBJ_ATTR* obj;
    enum ENTITY_TYPE type;
    byte dy;
    byte dx;
    enum ENT_STATE state;
    bool isJumping;
    bool isDead;
    ubyte lastAnimatedTile;
    ubyte health;
    ubyte f_invuln;
} ENTITY;

extern ENTITY* addEntity(const uint id, ubyte x, ubyte y, enum ENTITY_TYPE type);
extern void clearEntities();
extern ENTITY removeEntity(int i);
extern void gravity(ENTITY* e);
extern void tickEntityAnimations();
extern void addProjectileFrom(ENTITY* e);

INLINE bool isOffScreenX(ENTITY* e) {
    return ENT_X(e) < 0 || ENT_X(e) > SCREEN_WIDTH;
}

INLINE int getFacing(volatile OBJ_ATTR* obj) {
    const bool flipped = BF_GET(obj->attr1, ATTR1_HFLIP);
    return flipped ? -1 : 1;
}

INLINE void setFacing(volatile OBJ_ATTR* obj, byte dir) {
    const bool toFlip = (dir == -1) ? TRUE : FALSE;
    BF_SET(obj->attr1, toFlip, ATTR1_HFLIP);
}

INLINE void setEntityState(ENTITY* e, enum ENT_STATE newState) {
    if (e->state != newState) {
        e->state = newState;
        e->lastAnimatedTile = 0;
    } // else do nothing
}

INLINE void setWalking(ENTITY* e, byte dir) {
    setEntityState(e, WALKING);
    setFacing(e->obj, dir);
    e->dx = 3 * dir;
}

// set walking, but only if the player is not in a higher state (ie hurt)
INLINE void setWalkingWeak(ENTITY* e, byte dir) {
    if (e->state < WALKING) setWalking(e, dir);
    else { // duplicate from setWalking
        setFacing(e->obj, dir);
        e->dx = 3 * dir;
    }
}
INLINE void setRunning(ENTITY* e, byte dir) {
    setEntityState(e, RUNNING);
    setFacing(e->obj, dir);
    e->dx = 7 * dir;
}
INLINE void setJumping(ENTITY* e, byte dy) {
    e->isJumping = TRUE;
    e->dy = dy;
}
INLINE void setStanding(ENTITY* e) {
    setEntityState(e, STANDING);
    e->dx = 0;
}

#define HURT_DX 4

INLINE void setHurt(ENTITY* e, byte dir) {
    setEntityState(e, HURT);
    e->dx = dir * HURT_DX;
    e->dy = HURT_DX;
}
INLINE void setDead(ENTITY* e) {
    e->lastAnimatedTile = 0;
    e->isDead = TRUE;
    e->dx = 0;
    e->dy = 10;
}

INLINE short groundDist(ENTITY* e) {
    const int y = BF_GET(e->obj->attr0, ATTR0_Y);
    return GROUND_OFFSET - (y + entity_attrs[e->type].height);
}

ENTITY allEntities[ENTITIES_LEN];
extern ubyte objs_length;
extern ubyte gravityAffected_length;

extern OBJ_ATTR _setupObj(const uint id, ubyte x, ubyte y, enum ENTITY_TYPE type);
