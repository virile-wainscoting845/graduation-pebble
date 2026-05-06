// All tunables live here so balance changes don't touch logic.
#pragma once
#include <stdint.h>

namespace cfg {

// ---- timing ----
constexpr uint32_t TICK_INTERVAL_MS    = 1000;
constexpr uint32_t SAVE_INTERVAL_MS    = 60000;
constexpr uint32_t MSG_DURATION_MS     = 800;
constexpr uint32_t HOLD_THRESHOLD_MS   = 700;

// ---- decay (real seconds per -1 stat point while awake) ----
constexpr uint32_t DECAY_FULLNESS_S    = 60;   // empties in ~100 min
constexpr uint32_t DECAY_HAPPINESS_S   = 90;
constexpr uint32_t DECAY_ENERGY_S      = 120;
constexpr uint32_t DECAY_CLEANLINESS_S = 180;

// ---- regen while sleeping (seconds per +1 energy) ----
constexpr uint32_t REGEN_ENERGY_S      = 30;

// ---- action effects ----
constexpr int8_t   FEED_FULLNESS_GAIN  = 25;
constexpr int8_t   FEED_CLEAN_LOSS     = 8;
constexpr int8_t   PLAY_HAPPY_GAIN     = 22;
constexpr int8_t   PLAY_ENERGY_LOSS    = 15;
constexpr int8_t   CLEAN_TARGET        = 100;
constexpr int8_t   PET_HAPPY_GAIN      = 3;

// ---- health derivation ----
constexpr uint8_t  LOW_THRESH          = 25;
constexpr uint8_t  SICK_THRESH         = 15;
constexpr uint32_t HEALTH_BLEED_DIV    = 600;  // larger = slower bleed
constexpr uint32_t HEALTH_REGEN_S      = 40;

// ---- evolution (real seconds, only counted while powered on) ----
constexpr uint32_t AGE_BABY_S          = 60;          //  1 min  -> hatch
constexpr uint32_t AGE_TEEN_S          = 60 * 30;     // 30 min  -> teen
constexpr uint32_t AGE_ADULT_S         = 60 * 120;    //  2 h    -> adult

// ---- IMU shake-to-pet ----
constexpr float    SHAKE_THRESHOLD_G2  = 1.8f * 1.8f; // squared, |a| > 1.8 g
constexpr uint32_t SHAKE_COOLDOWN_MS   = 800;

// ---- storage ----
constexpr uint32_t    STATE_MAGIC      = 0xAFB17A1Eu; // "AXOL"
constexpr const char* NVS_NAMESPACE    = "axie";
constexpr const char* NVS_STATE_KEY    = "state";

// ---- screen ----
constexpr uint16_t SCREEN_W            = 135;
constexpr uint16_t SCREEN_H            = 240;

// ---- Buddy compatibility (extracted from cli.js v2.1.89 species UG8) ----
//   Original eye set is Unicode: { '·','✦','×','◉','@','°' }
//   Font0 only does ASCII, so we use these visual substitutes:
//     '·' -> 'o' (default — matches "default cuteness")
//     '✦' -> '*'
//     '×' -> 'x'
//     '◉' -> 'O'
//     '@' -> '@'
//     '°' -> 'o'
constexpr char BUDDY_EYE        = 'o';
//   Hat overlay (12 chars wide, occupies line 0). nullptr = bareheaded.
//   Verbatim hats from cli.js xRY object (you can swap any of them in):
//     "   \\^^^/    "  (crown)
//     "   [___]    "  (tophat)
//     "    -+-     "  (propeller)
//     "   (   )    "  (halo)
//     "    /^\\     "  (wizard)
//     "   (___)    "  (beanie)
//     "    ,>      "  (tinyduck)
constexpr const char* BUDDY_HAT = nullptr;

// ---- Arete brand palette (matches official 百川 logo: navy + white + gold) ----
constexpr uint16_t COL_NAVY            = 0x1186;  // logo background navy ~#102C70
constexpr uint16_t COL_NAVY_DEEP       = 0x0843;  // shadow / deeper
constexpr uint16_t COL_NAVY_LIGHT      = 0x2A2D;  // accent / panel
constexpr uint16_t COL_GOLD            = 0xFE60;  // honor gold (typewriter accent)
constexpr uint16_t COL_GOLD_DIM        = 0xCD20;
constexpr uint16_t COL_BRAND_WHITE     = 0xFFFF;

// ---- palette (RGB565) — original axolotl palette retained for pet mode ----
constexpr uint16_t COL_BG              = COL_NAVY;        // unified to brand
constexpr uint16_t COL_BG_DEEP         = COL_NAVY_DEEP;
constexpr uint16_t COL_BAR             = 0x0822;
constexpr uint16_t COL_TEXT            = 0xFFFF;
constexpr uint16_t COL_TEXT_DIM        = 0xC618;

// ---- 4-season palette for story pages ----
constexpr uint16_t COL_SPRING          = 0xFB9A;  // pale pink (大三 spring)
constexpr uint16_t COL_SUMMER          = 0x6F6D;  // sage green (大二 summer)
constexpr uint16_t COL_AUTUMN          = 0xFC68;  // warm orange (大一 autumn)
constexpr uint16_t COL_WINTER          = 0x9999;  // dusk purple-grey (大四 winter)

// ---- Demo / class constants (Arete 115 cohort) ----
constexpr uint8_t  COHORT_TOTAL        = 40;      // 全班 N 人 (demo: assumed 40)
constexpr uint8_t  COHORT_DEMO_START   = 7;       // demo radar starts at 7 found
constexpr uint32_t SPLASH_DURATION_MS  = 4500;   // 5-stage cinematic intro
constexpr uint32_t STORY_PAGE_MS       = 5000;   // 5 sec per page
constexpr uint8_t  STORY_PAGE_COUNT    = 5;       // 4 years + 1 closing
// 3-tier axolotl palette (deep -> mid -> light) — high contrast at arm length
constexpr uint16_t COL_AXO_DEEP        = 0xF308;  // hot magenta — gill outer + shadow
constexpr uint16_t COL_AXO_MID         = 0xFC8A;  // salmon — body main
constexpr uint16_t COL_AXO_LIGHT       = 0xFEDC;  // cream pink — belly + gill tip
constexpr uint16_t COL_PINK            = COL_AXO_MID;     // legacy alias
constexpr uint16_t COL_PINK_LIGHT      = COL_AXO_LIGHT;
constexpr uint16_t COL_PINK_HOT        = COL_AXO_DEEP;
constexpr uint16_t COL_EYE_BLACK       = 0x18C3;  // soft-black (pure 0 looks too hard on tiny LCD)
constexpr uint16_t COL_BLACK           = COL_EYE_BLACK;
constexpr uint16_t COL_WHITE           = 0xFFFF;
constexpr uint16_t COL_BLUSH           = 0xFA12;
constexpr uint16_t COL_GOOD            = 0x07E0;
constexpr uint16_t COL_WARN            = 0xFFE0;
constexpr uint16_t COL_BAD             = 0xF800;
constexpr uint16_t COL_HEART           = 0xF9A8;
constexpr uint16_t COL_SICK            = 0x07E0;
constexpr uint16_t COL_DIRT            = 0x6B40;

}  // namespace cfg
