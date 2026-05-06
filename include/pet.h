#pragma once
#include <stdint.h>
#include <stddef.h>

enum PetMood : uint8_t {
    MOOD_IDLE,
    MOOD_HAPPY,
    MOOD_SAD,
    MOOD_HUNGRY,
    MOOD_SLEEPING,
    MOOD_DIRTY,
    MOOD_SICK,
    MOOD_DEAD,
};

enum PetStage : uint8_t {
    STAGE_EGG,
    STAGE_BABY,
    STAGE_TEEN,
    STAGE_ADULT,
};

struct PetState {
    uint32_t magic;             // cfg::STATE_MAGIC
    char     name[16];
    uint32_t alive_seconds;     // total awake lifetime, persisted
    uint8_t  fullness;          // 0..100, higher = full
    uint8_t  happiness;
    uint8_t  energy;
    uint8_t  cleanliness;
    uint8_t  health;
    uint8_t  stage;             // PetStage
    uint8_t  is_sleeping;
    uint8_t  is_dead;
    uint8_t  reserved[8];
    uint32_t crc32;
};
static_assert(sizeof(PetState) <= 64, "Keep state small for NVS");

void        pet_init(PetState *p, const char *name);
void        pet_tick(PetState *p, uint32_t elapsed_seconds);
void        pet_feed(PetState *p);
void        pet_play(PetState *p);
void        pet_clean(PetState *p);
void        pet_sleep_toggle(PetState *p);
void        pet_pet(PetState *p);
PetMood     pet_current_mood(const PetState *p);
const char* pet_stage_name(PetStage s);
const char* pet_mood_name(PetMood m);
uint32_t    pet_compute_crc(const PetState *p);
