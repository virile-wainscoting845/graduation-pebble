#include "pet.h"
#include "config.h"
#include <string.h>

static inline uint8_t clampu8(int v) {
    if (v < 0)   return 0;
    if (v > 100) return 100;
    return (uint8_t)v;
}

static void decay(uint8_t *s, uint32_t per_pt_s, uint32_t elapsed_s) {
    if (*s == 0 || per_pt_s == 0) return;
    uint32_t drops = elapsed_s / per_pt_s;
    if (!drops) return;
    *s = clampu8((int)*s - (int)drops);
}

static void regen(uint8_t *s, uint32_t per_pt_s, uint32_t elapsed_s) {
    if (*s >= 100 || per_pt_s == 0) return;
    uint32_t gains = elapsed_s / per_pt_s;
    if (!gains) return;
    *s = clampu8((int)*s + (int)gains);
}

static PetStage stage_for_age(uint32_t age_s) {
    if (age_s < cfg::AGE_BABY_S)  return STAGE_EGG;
    if (age_s < cfg::AGE_TEEN_S)  return STAGE_BABY;
    if (age_s < cfg::AGE_ADULT_S) return STAGE_TEEN;
    return STAGE_ADULT;
}

void pet_init(PetState *p, const char *name) {
    memset(p, 0, sizeof(*p));
    p->magic         = cfg::STATE_MAGIC;
    strncpy(p->name, name, sizeof(p->name) - 1);
    p->fullness      = 70;
    p->happiness     = 70;
    p->energy        = 80;
    p->cleanliness   = 100;
    p->health        = 100;
    p->stage         = STAGE_EGG;
    p->is_sleeping   = 0;
    p->is_dead       = 0;
    p->alive_seconds = 0;
    p->crc32         = pet_compute_crc(p);
}

void pet_tick(PetState *p, uint32_t elapsed_seconds) {
    if (p->is_dead || elapsed_seconds == 0) return;

    p->alive_seconds += elapsed_seconds;

    if (p->is_sleeping) {
        regen(&p->energy,      cfg::REGEN_ENERGY_S,      elapsed_seconds);
        decay(&p->fullness,    cfg::DECAY_FULLNESS_S * 2, elapsed_seconds);
        decay(&p->cleanliness, cfg::DECAY_CLEANLINESS_S, elapsed_seconds);
        if (p->energy >= 100) p->is_sleeping = 0;
    } else {
        decay(&p->fullness,    cfg::DECAY_FULLNESS_S,    elapsed_seconds);
        decay(&p->happiness,   cfg::DECAY_HAPPINESS_S,   elapsed_seconds);
        decay(&p->energy,      cfg::DECAY_ENERGY_S,      elapsed_seconds);
        decay(&p->cleanliness, cfg::DECAY_CLEANLINESS_S, elapsed_seconds);
    }

    int penalty = 0;
    if (p->fullness    < cfg::LOW_THRESH) penalty += (cfg::LOW_THRESH - p->fullness);
    if (p->cleanliness < cfg::LOW_THRESH) penalty += (cfg::LOW_THRESH - p->cleanliness);
    if (p->happiness   < cfg::LOW_THRESH) penalty += (cfg::LOW_THRESH - p->happiness) / 2;

    if (penalty > 0) {
        uint32_t drops = ((uint32_t)penalty * elapsed_seconds) / cfg::HEALTH_BLEED_DIV;
        if (drops) p->health = clampu8((int)p->health - (int)drops);
    } else if (p->health < 100) {
        regen(&p->health, cfg::HEALTH_REGEN_S, elapsed_seconds);
    }

    if (p->health == 0) p->is_dead = 1;

    PetStage ns = stage_for_age(p->alive_seconds);
    if (ns > p->stage) p->stage = ns;
}

void pet_feed(PetState *p) {
    if (p->is_dead) return;
    p->fullness    = clampu8((int)p->fullness    + cfg::FEED_FULLNESS_GAIN);
    p->cleanliness = clampu8((int)p->cleanliness - cfg::FEED_CLEAN_LOSS);
}

void pet_play(PetState *p) {
    if (p->is_dead || p->is_sleeping) return;
    if (p->energy < 10) return;
    p->happiness = clampu8((int)p->happiness + cfg::PLAY_HAPPY_GAIN);
    p->energy    = clampu8((int)p->energy    - cfg::PLAY_ENERGY_LOSS);
}

void pet_clean(PetState *p) {
    if (p->is_dead) return;
    p->cleanliness = cfg::CLEAN_TARGET;
}

void pet_sleep_toggle(PetState *p) {
    if (p->is_dead) return;
    p->is_sleeping = !p->is_sleeping;
}

void pet_pet(PetState *p) {
    if (p->is_dead || p->is_sleeping) return;
    p->happiness = clampu8((int)p->happiness + cfg::PET_HAPPY_GAIN);
}

PetMood pet_current_mood(const PetState *p) {
    if (p->is_dead)                          return MOOD_DEAD;
    if (p->is_sleeping)                      return MOOD_SLEEPING;
    if (p->health      < cfg::SICK_THRESH)   return MOOD_SICK;
    if (p->fullness    < cfg::LOW_THRESH)    return MOOD_HUNGRY;
    if (p->cleanliness < cfg::LOW_THRESH)    return MOOD_DIRTY;
    if (p->happiness   < cfg::LOW_THRESH)    return MOOD_SAD;
    if (p->happiness   > 75 && p->fullness > 50) return MOOD_HAPPY;
    return MOOD_IDLE;
}

const char* pet_stage_name(PetStage s) {
    switch (s) {
        case STAGE_EGG:   return "egg";
        case STAGE_BABY:  return "baby";
        case STAGE_TEEN:  return "teen";
        case STAGE_ADULT: return "adult";
    }
    return "?";
}

const char* pet_mood_name(PetMood m) {
    switch (m) {
        case MOOD_IDLE:     return "idle";
        case MOOD_HAPPY:    return "happy";
        case MOOD_SAD:      return "sad";
        case MOOD_HUNGRY:   return "hungry";
        case MOOD_SLEEPING: return "asleep";
        case MOOD_DIRTY:    return "dirty";
        case MOOD_SICK:     return "sick";
        case MOOD_DEAD:     return "RIP";
    }
    return "?";
}

// CRC-32 (IEEE 802.3) over the struct with crc32 field zeroed.
uint32_t pet_compute_crc(const PetState *p) {
    PetState t = *p;
    t.crc32 = 0;
    const uint8_t *b = (const uint8_t*)&t;
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < sizeof(t); i++) {
        crc ^= b[i];
        for (int k = 0; k < 8; k++) {
            uint32_t mask = -(int32_t)(crc & 1u);
            crc = (crc >> 1) ^ (0xEDB88320u & mask);
        }
    }
    return ~crc;
}
