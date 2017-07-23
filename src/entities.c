#include "myLib.h"
#include "entities.h"
#include "game/game_state.h"

ENTITY allEntities[];
uint8_t objs_length = 0;

INLINE void tick_entity_anim(ENTITY* e,
        const int E_STAND_TID,
        const int E_WALK_TID,
        const int E_RUN_TID,
        const int E_HURT_TID,
        const int E_JUMP_TID,
        const int E_WALK_LEN,
        const int E_RUN_LEN,
        const int E_HURT_LEN,
        const int E_TILES_PER_FRAME) {
    if (e->isDead) {
        e->isJumping = FALSE;
        e->state = WALKING;
        /* if (e->lastAnimatedTile >= E_HURT_LEN) return; // prevent hurt anim from looping */
    }

    if (e->state == HURT) {
        if (e->lastAnimatedTile >= E_HURT_LEN) {
            setStanding(e);
        } else {
            BF_SET(e->obj->attr2, E_HURT_TID + e->lastAnimatedTile, ATTR2_ID);
        }
    } else if (e->isJumping) {
        BF_SET(e->obj->attr2, E_JUMP_TID, ATTR2_ID);
    } else switch(e->state) {
        case WALKING:
            if (e->lastAnimatedTile >= E_WALK_LEN) e->lastAnimatedTile = 0;
            BF_SET(e->obj->attr2, E_WALK_TID + e->lastAnimatedTile, ATTR2_ID);
            break;
        case RUNNING:
            if (e->lastAnimatedTile >= E_RUN_LEN) e->lastAnimatedTile = 0;
            BF_SET(e->obj->attr2, E_RUN_TID + e->lastAnimatedTile, ATTR2_ID);
            break;
        case STANDING:
            BF_SET(e->obj->attr2, E_STAND_TID, ATTR2_ID);
            break;
        default:
            break;
    }

    // only update tiles per frame on animated states
    if (e->state == WALKING
            || e->state == RUNNING
            || e->state == HURT) e->lastAnimatedTile+=E_TILES_PER_FRAME;
}
void tick_animations(ENTITY* e) {
    if (e->type == PLAYER) {
        tick_entity_anim(e,
                PLAYER_STAND_TID,
                PLAYER_WALK_TID,
                PLAYER_RUN_TID,
                PLAYER_HURT_TID,
                PLAYER_JUMP_TID,
                PLAYER_WALK_LEN,
                PLAYER_RUN_LEN,
                PLAYER_HURT_LEN,
                PLAYER_TPF);
    } else if (e->type == TALL_ENEMY) {
        tick_entity_anim(e,
                TALL_ENEMY_STAND_TID,
                TALL_ENEMY_WALK_TID,
                -1,
                TALL_ENEMY_HURT_TID,
                TALL_ENEMY_WALK_TID,
                TALL_ENEMY_WALK_LEN,
                -1,
                TALL_ENEMY_HURT_LEN,
                TALL_ENEMY_TPF);
    } else if (e->type == SHORT_ENEMY) {
        tick_entity_anim(e,
                SHORT_ENEMY_STAND_TID,
                SHORT_ENEMY_WALK_TID,
                -1,
                SHORT_ENEMY_HURT_TID,
                -1,
                SHORT_ENEMY_WALK_LEN,
                -1,
                SHORT_ENEMY_HURT_LEN,
                SHORT_ENEMY_TPF);
    } else if (e->type == PROJECTILE) {
        if (e->lastAnimatedTile >= PROJECTILE_LEN) e->lastAnimatedTile = 0;
        BF_SET(e->obj->attr2, PROJECTILE_TID + e->lastAnimatedTile, ATTR2_ID);
        e->lastAnimatedTile += PROJECTILE_TPF;
    }
}

INLINE bool isOffScreen(ENTITY* e) {
    return ENT_Y(e) > SCREEN_HEIGHT
        || ENT_Y(e) < 0
        || ENT_X(e) < 0
        || ENT_X(e) > SCREEN_WIDTH;

}

void tickEntityAnimations() {
    for (volatile int i = 0; i < objs_length; ++i) {
        if (allEntities[i].isDead && isOffScreen(allEntities + i)) {
            removeEntity(i--);
        }
        tick_animations(allEntities + i);
    }
}

void gravity(ENTITY* e) {
    volatile OBJ_ATTR* obj = e->obj;
    const int8_t g = 1;

    if (e->isDead) {
        e->dy = e->dy - g;
        OBJ_DY(*obj, ~(e->dy)+1);
        return;
    }

    short ground_dist = groundDist(e);
    if (ground_dist > 0) {
        e->dy = e->dy - g;
    }
    if (((ground_dist + e->dy) <= 0 && (e->dy) <= 0)) {
        BF_SET(obj->attr0, (GROUND_OFFSET - entity_attrs[e->type].height), ATTR0_Y);
        e->isJumping = FALSE;
    } else {
        OBJ_DY(*obj, ~(e->dy)+1);
    }
}
OBJ_ATTR _setupObj(const uint32_t id, uint8_t x, uint8_t y, enum ENTITY_TYPE type) {
    volatile OBJ_ATTR obj;
    obj.attr0 = BF_PREP(y, ATTR0_Y) | BF_PREP(entity_attrs[type].tile_shape, ATTR0_SHAPE);
    obj.attr1 = BF_PREP(x, ATTR1_X) | BF_PREP(entity_attrs[type].tile_size, ATTR1_SIZE);
    obj.attr2 = BF_PREP(id, ATTR2_ID) | BF_PREP(entity_attrs[type].default_palette, ATTR2_PALBANK);
    return obj;
}

ENTITY* _setupEntity(OBJ_ATTR obj, enum ENTITY_TYPE type) {
    volatile OBJ_ATTR* ptr = OBJ_ATTR_MEM + objs_length;
    *ptr = obj;
    ENTITY ent = {.obj = ptr, .type = type, .dy = 0, .dx = 0, .health = 2, .f_invuln = 0};
    allEntities[objs_length] = ent;
    objs_length++;
    return allEntities + (objs_length - 1);
}

ENTITY* addEntity(const uint32_t id, uint8_t x, uint8_t y, enum ENTITY_TYPE type) {
    return _setupEntity(_setupObj(id, x, y - entity_attrs[type].height, type), type);
}

ENTITY removeEntity(const int index) {
    if (index == 0) {
        PUTS("WARN: POS 0 (player) removed.");
    }
    ENTITY removed = allEntities[index];
    if (removed.type == PROJECTILE) {
        PLAYER_DATA->projectile_count--;
    }

    OBJ_ATTR_MEM[index] = OBJ_ATTR_MEM[objs_length - 1];
    allEntities[index] = allEntities[objs_length - 1];
    allEntities[index].obj = OBJ_ATTR_MEM + index;
    OBJ_ATTR_MEM[objs_length - 1] = NULL_OBJ_ATTR;
    objs_length--;
    return removed;
}

void clearEntities(int first_idx) {
    for (int i = first_idx; i < objs_length; i++) {
        OBJ_ATTR_MEM[i] = NULL_OBJ_ATTR;
    }
    PLAYER_DATA->projectile_count = 0;
    objs_length = first_idx;
}

void addProjectileFrom(ENTITY* e) {
    if (PLAYER_DATA->projectile_count > 2) return; // limit fire rate
    else {
        const int8_t facing = getFacing(e->obj);
        ENTITY* proj = addEntity(PROJECTILE_TID, facing * 8 + ENT_X(e) + attrs(e).width / 2, ENT_Y(e) + attrs(e).height/2 + 8, PROJECTILE);
        setFacing(proj->obj, facing);
        proj->dx = facing * 5;
        PLAYER_DATA->projectile_count++;
    }
}
