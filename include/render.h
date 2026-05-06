#pragma once
#include <M5Unified.h>
#include "pet.h"

// ---- common (called by all modes) ----
void render_init();
void render_message(const char *msg);
void render_set_diag(bool a_pressed, bool b_pressed);
void render_present();           // pushes canvas to LCD

// ---- mode renderers ----
//   Each mode draws into the same shared canvas; main.cpp calls render_present()
//   exactly once per frame after picking which mode to render.
void render_splash(uint32_t enter_ms, uint32_t now_ms);

void render_home(const PetState *p, uint32_t f, uint8_t batt, uint8_t connections);

// page = 0..4, page_age_ms = elapsed time on this page (for typewriter effect)
void render_story(uint8_t page, uint32_t page_age_ms, uint32_t f);

// found_count = simulated cohort connections
void render_radar(uint8_t found_count, uint8_t total, uint32_t f,
                  const char *latest_name, uint32_t latest_age_ms,
                  bool unity_unlocked);

// Time capsule countdown screen
void render_capsule(uint32_t boot_ms, bool force_preview);

// Pet mode (owl now, axolotl retained as fallback if cfg::PET_USE_OWL=false)
void render_pet(const PetState *p, uint32_t f, uint8_t batt);
