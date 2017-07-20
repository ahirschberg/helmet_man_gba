// credit tonc: http://www.coranac.com/tonc/text/keys.htm 
extern uint16_t __key_curr, __key_prev;

#define REG_KEYINPUT *(volatile uint32_t*)0x4000130

#define KEY_A        0x0001
#define KEY_B        0x0002
#define KEY_SELECT   0x0004
#define KEY_START    0x0008
#define KEY_RIGHT    0x0010
#define KEY_LEFT     0x0020
#define KEY_UP       0x0040
#define KEY_DOWN     0x0080
#define KEY_R        0x0100
#define KEY_L        0x0200

#define KEY_MASK     0x03FF

// Polling function
INLINE void key_poll()
{
    __key_prev= __key_curr;
    __key_curr= ~REG_KEYINPUT & KEY_MASK;
}

// Basic state checks
INLINE uint32_t key_curr_state()         {   return __key_curr;          }
INLINE uint32_t key_prev_state()         {   return __key_prev;          }
INLINE uint32_t key_is_down(uint32_t key)     {   return  __key_curr & key;   }
INLINE uint32_t key_is_up(uint32_t key)       {   return ~__key_curr & key;   }
INLINE uint32_t key_was_down(uint32_t key)    {   return  __key_prev & key;   }
INLINE uint32_t key_was_up(uint32_t key)      {   return ~__key_prev & key;   }
// Transitional state checks.

// Key is changing state.
INLINE uint32_t key_transit(uint32_t key)
{   return ( __key_curr ^  __key_prev) & key;   }

// Key is held (down now and before).
INLINE uint32_t key_held(uint32_t key)
{   return ( __key_curr &  __key_prev) & key;  }

// Key is being hit (down now, but not before).
INLINE uint32_t key_hit(uint32_t key)
{   return ( __key_curr &~ __key_prev) & key;  }

//Key is being released (up now but down before)
INLINE uint32_t key_released(uint32_t key)
{   return (~__key_curr &  __key_prev) & key;  }

// === (tonc_core.h) ==================================================
// tribool: 1 if {plus} on, -1 if {minus} on, 0 if {plus}=={minus}
INLINE int bit_tribool(uint32_t x, int plus, int minus)
{   return ((x>>plus)&1) - ((x>>minus)&1);  }
// === (tonc_input.h) =================================================
enum eKeyIndex
{
    KI_A=0, KI_B, KI_SELECT, KI_START, 
    KI_RIGHT, KI_LEFT, KI_UP, KI_DOWN,
    KI_R, KI_L, KI_MAX
};

// --- TRISTATES ---
INLINE tribool key_tri_horz()       // right/left : +/-
{   return bit_tribool(__key_curr, KI_RIGHT, KI_LEFT);  }

INLINE tribool key_tri_vert()       // down/up : +/-
{   return bit_tribool(__key_curr, KI_DOWN, KI_UP);     }

INLINE tribool key_tri_shoulder()   // R/L : +/-
{   return bit_tribool(__key_curr, KI_R, KI_L);         }

INLINE tribool key_tri_fire()       // B/A : -/+
{   return bit_tribool(__key_curr, KI_A, KI_B);         }
