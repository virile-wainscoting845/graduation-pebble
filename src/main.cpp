// =============================================================================
//  Arete Pebble — main entry / state machine
//
//  6 modes: SPLASH -> HOME -> {STORY, RADAR, CAPSULE, PET}
//
//  Global controls (always live):
//   - BtnA + BtnB held 2s -> M5.Power.powerOff()
//   - In any non-HOME mode: BtnA short -> back to HOME
//
//  Per-mode controls listed in setup_state_machine().
// =============================================================================
#include <M5Unified.h>
#include <esp_sleep.h>
#include "config.h"
#include "demo_data.h"
#include "pet.h"
#include "render.h"
#include "storage.h"

enum AppMode : uint8_t {
    APP_SPLASH = 0,
    APP_HOME,
    APP_STORY,
    APP_RADAR,
    APP_CAPSULE,
    APP_PET,
};

// ---- global state ----
static PetState  g_pet;
static AppMode   g_mode             = APP_SPLASH;
static uint32_t  g_mode_enter_ms    = 0;
static uint32_t  g_last_tick_ms     = 0;
static uint32_t  g_last_save_ms     = 0;
static uint32_t  g_last_shake_ms    = 0;
static uint32_t  g_frame            = 0;

// story state
static uint8_t   g_story_page       = 0;
static uint32_t  g_story_page_ms    = 0;

// radar state
static uint8_t   g_radar_found      = cfg::COHORT_DEMO_START;
static uint32_t  g_radar_last_find  = 0;
static uint8_t   g_radar_b_clicks   = 0;
static uint32_t  g_radar_b_window   = 0;
static const char *g_radar_latest   = nullptr;
static uint32_t  g_radar_latest_ms  = 0;
static bool      g_unity_unlocked   = false;

// capsule state
static bool      g_capsule_preview  = false;

// pet state — triple-click on BtnA to exit
static uint8_t   g_pet_a_clicks     = 0;
static uint32_t  g_pet_a_window     = 0;

// =============================================================================
static void beep(uint32_t hz, uint32_t ms) {
    if (M5.Speaker.isEnabled()) M5.Speaker.tone(hz, ms);
}

static uint8_t safe_battery() {
    int32_t lvl = M5.Power.getBatteryLevel();
    if (lvl < 0)   return 0;
    if (lvl > 100) return 100;
    return (uint8_t)lvl;
}

static void enter_mode(AppMode m) {
    g_mode          = m;
    g_mode_enter_ms = millis();
    if (m == APP_STORY) {
        g_story_page    = 0;
        g_story_page_ms = millis();
    }
    if (m == APP_RADAR) {
        // Reset full radar state — without resetting g_radar_found, re-entry
        // after unity-unlock would freeze at 40/40 with no auto-finds.
        g_radar_b_clicks  = 0;
        g_radar_b_window  = 0;
        g_unity_unlocked  = false;
        g_radar_found     = cfg::COHORT_DEMO_START;
        g_radar_last_find = millis();
        g_radar_latest    = nullptr;
        g_radar_latest_ms = 0;
    }
    if (m == APP_PET) {
        g_pet_a_clicks = 0;
        g_pet_a_window = 0;
    }
    if (m == APP_CAPSULE) {
        g_capsule_preview = false;
    }
}

static bool check_powerof_combo(uint32_t ms) {
    static uint32_t pwr_combo_start = 0;
    if (M5.BtnA.isPressed() && M5.BtnB.isPressed()) {
        if (pwr_combo_start == 0) pwr_combo_start = ms;
        uint32_t held = ms - pwr_combo_start;
        if (held >= 2000) {
            // Toast must be visible BEFORE we block; force a final present.
            render_message("powering off...");
            render_present();
            storage_save(&g_pet);
            beep(220, 200);
            delay(700);
            M5.Power.powerOff();
            esp_deep_sleep_start();
            return true;
        } else if (held >= 250) {
            render_message("hold both = off");
        }
        return true;   // suppress regular button handlers
    }
    pwr_combo_start = 0;
    return false;
}

static void handle_imu_pet() {
    if (!M5.Imu.isEnabled())  return;
    if (!M5.Imu.update())     return;
    auto d = M5.Imu.getImuData();
    float mag2 = d.accel.x * d.accel.x
               + d.accel.y * d.accel.y
               + d.accel.z * d.accel.z;
    if (mag2 > cfg::SHAKE_THRESHOLD_G2 &&
        millis() - g_last_shake_ms > cfg::SHAKE_COOLDOWN_MS) {
        g_last_shake_ms = millis();
        if (g_mode == APP_PET) {
            pet_pet(&g_pet);
            render_message("petted!");
            beep(880, 60);
        } else if (g_mode == APP_HOME) {
            render_message("hello!");
            beep(660, 50);
        }
    }
}

// =============================================================================
//  Per-mode update functions
// =============================================================================

static void update_splash(uint32_t ms) {
    uint32_t age = ms - g_mode_enter_ms;
    // chiptune timing aligned to splash visual stages:
    //   200 ms  : start of particle convergence -> 4 ascending notes
    //   1300 ms : flash bang -> punctuation note
    //   1700 ms : crest reveal -> closing chord
    static bool played[5] = { false, false, false, false, false };
    if (age >  300 && !played[0]) { beep(523, 100); played[0] = true; }   // C5
    if (age >  600 && !played[1]) { beep(659, 100); played[1] = true; }   // E5
    if (age >  900 && !played[2]) { beep(784, 100); played[2] = true; }   // G5
    if (age > 1300 && !played[3]) { beep(1568, 80); played[3] = true; }   // G6 punctuation
    if (age > 1700 && !played[4]) { beep(1047, 300); played[4] = true; }  // C6 sustain

    if (M5.BtnA.wasClicked() || age >= cfg::SPLASH_DURATION_MS) {
        for (int i = 0; i < 5; i++) played[i] = false;
        beep(1318, 60);
        enter_mode(APP_HOME);
    }
    render_splash(g_mode_enter_ms, ms);
}

static void update_home(uint32_t ms) {
    if (M5.BtnA.wasClicked()) {
        beep(880, 50);
        enter_mode(APP_STORY);
        return;
    }
    if (M5.BtnA.wasHold()) {
        beep(523, 80);
        enter_mode(APP_CAPSULE);
        return;
    }
    if (M5.BtnB.wasClicked()) {
        beep(988, 50);
        enter_mode(APP_RADAR);
        return;
    }
    if (M5.BtnB.wasHold()) {
        beep(660, 80);
        enter_mode(APP_PET);
        return;
    }

    render_home(&g_pet, g_frame, safe_battery(), g_radar_found);
    (void)ms;
}

static void update_story(uint32_t ms) {
    // BtnA: back to home
    if (M5.BtnA.wasClicked()) {
        beep(523, 50);
        enter_mode(APP_HOME);
        return;
    }
    // BtnB: skip to next page
    if (M5.BtnB.wasClicked()) {
        if (g_story_page + 1 < cfg::STORY_PAGE_COUNT) {
            g_story_page++;
            g_story_page_ms = ms;
            beep(784, 40);
        } else {
            // last page -> back home
            beep(1047, 100);
            enter_mode(APP_HOME);
            return;
        }
    }

    // auto-advance
    if (ms - g_story_page_ms > cfg::STORY_PAGE_MS) {
        if (g_story_page + 1 < cfg::STORY_PAGE_COUNT) {
            g_story_page++;
            g_story_page_ms = ms;
            beep(659, 30);
        } else {
            // hold last page until user back
        }
    }

    render_story(g_story_page, ms - g_story_page_ms, g_frame);
}

static void update_radar(uint32_t ms) {
    if (g_unity_unlocked) {
        if (M5.BtnA.wasClicked()) {
            enter_mode(APP_HOME);
            return;
        }
        render_radar(cfg::COHORT_TOTAL, cfg::COHORT_TOTAL, g_frame,
                     nullptr, 9999, true);
        return;
    }

    if (M5.BtnA.wasClicked()) {
        beep(523, 50);
        enter_mode(APP_HOME);
        return;
    }

    // simulate: every 2 seconds find one more cohort (until 39/40)
    if (g_radar_found < cfg::COHORT_TOTAL - 1 &&
        ms - g_radar_last_find > 2200) {
        const char *nm = demo::COHORT_NAMES[g_radar_found % 10];
        g_radar_found++;
        g_radar_last_find  = ms;
        g_radar_latest     = nm;
        g_radar_latest_ms  = ms;
        beep(1500, 30);
    }

    // BtnB short: manually + 1 (lets user reach unity faster) AND counts toward x5
    if (M5.BtnB.wasClicked()) {
        if (ms - g_radar_b_window > 2000) {
            g_radar_b_clicks = 0;
            g_radar_b_window = ms;
        }
        g_radar_b_clicks++;
        if (g_radar_b_clicks >= 5) {
            g_unity_unlocked = true;
            g_radar_found    = cfg::COHORT_TOTAL;
            // Non-blocking fanfare — M5.Speaker mixes channels
            if (M5.Speaker.isEnabled()) {
                M5.Speaker.tone(523,  240, 0);
                M5.Speaker.tone(659,  240, 1);
                M5.Speaker.tone(784,  240, 2);
                M5.Speaker.tone(1047, 480, 3);
            }
            return;
        }
        if (g_radar_found < cfg::COHORT_TOTAL - 1) {
            const char *nm = demo::COHORT_NAMES[g_radar_found % 10];
            g_radar_found++;
            g_radar_latest    = nm;
            g_radar_latest_ms = ms;
            beep(1200, 30);
        }
    }

    render_radar(g_radar_found, cfg::COHORT_TOTAL, g_frame,
                 g_radar_latest,
                 ms - g_radar_latest_ms,
                 false);
}

static void update_capsule(uint32_t ms) {
    if (M5.BtnA.wasClicked()) {
        beep(523, 50);
        enter_mode(APP_HOME);
        return;
    }
    if (M5.BtnB.wasClicked()) {
        g_capsule_preview = !g_capsule_preview;
        beep(g_capsule_preview ? 1047 : 523, 60);
    }
    render_capsule(ms, g_capsule_preview);
}

static void update_pet(uint32_t ms) {
    // Triple-click BtnA -> back to home
    if (M5.BtnA.wasClicked()) {
        if (ms - g_pet_a_window > 1200) {
            g_pet_a_clicks = 0;
            g_pet_a_window = ms;
        }
        g_pet_a_clicks++;
        if (g_pet_a_clicks >= 3) {
            beep(1047, 80);
            g_pet_a_clicks = 0;
            enter_mode(APP_HOME);
            return;
        }
        // first / second click = feed
        pet_feed(&g_pet);
        render_message("nom nom!");
        beep(660, 60);
        storage_save(&g_pet);
    } else if (M5.BtnA.wasHold()) {
        pet_clean(&g_pet);
        render_message("squeaky clean");
        beep(880, 50);
        storage_save(&g_pet);
    }

    if (M5.BtnB.wasClicked()) {
        pet_play(&g_pet);
        render_message("wheee!");
        beep(784, 60);
        storage_save(&g_pet);
    } else if (M5.BtnB.wasHold()) {
        pet_sleep_toggle(&g_pet);
        render_message(g_pet.is_sleeping ? "zzz..." : "good morning!");
        beep(g_pet.is_sleeping ? 330 : 988, 100);
        storage_save(&g_pet);
    }

    handle_imu_pet();

    uint8_t batt = safe_battery();
    render_pet(&g_pet, g_frame, batt);
    (void)ms;
}

// =============================================================================
//  Setup / Loop
// =============================================================================
void setup() {
    auto m5cfg = M5.config();
    M5.begin(m5cfg);

    M5.Display.setRotation(0);
    M5.Display.setBrightness(140);
    M5.Display.fillScreen(0);

    M5.BtnA.setHoldThresh(cfg::HOLD_THRESHOLD_MS);
    M5.BtnB.setHoldThresh(cfg::HOLD_THRESHOLD_MS);

    if (M5.Speaker.isEnabled()) M5.Speaker.setVolume(80);

    render_init();

    if (!storage_load(&g_pet)) {
        pet_init(&g_pet, "Owly");
        storage_save(&g_pet);
    }

    g_last_tick_ms = millis();
    enter_mode(APP_SPLASH);
}

void loop() {
    M5.update();
    const uint32_t ms = millis();

    // Global power-off combo (suppress per-mode handlers when active)
    bool combo_active = check_powerof_combo(ms);

    // also handle IMU shake outside pet mode (for fun feedback)
    if (g_mode != APP_PET) handle_imu_pet();

    if (!combo_active) {
        switch (g_mode) {
            case APP_SPLASH:  update_splash(ms);  break;
            case APP_HOME:    update_home(ms);    break;
            case APP_STORY:   update_story(ms);   break;
            case APP_RADAR:   update_radar(ms);   break;
            case APP_CAPSULE: update_capsule(ms); break;
            case APP_PET:     update_pet(ms);     break;
        }
    } else {
        // still render current mode under the toast
        switch (g_mode) {
            case APP_SPLASH:  render_splash(g_mode_enter_ms, ms); break;
            case APP_HOME:    render_home(&g_pet, g_frame, safe_battery(), g_radar_found); break;
            case APP_STORY:   render_story(g_story_page, ms - g_story_page_ms, g_frame); break;
            case APP_RADAR:   render_radar(g_radar_found, cfg::COHORT_TOTAL, g_frame,
                                           g_radar_latest, ms - g_radar_latest_ms,
                                           g_unity_unlocked); break;
            case APP_CAPSULE: render_capsule(ms, g_capsule_preview); break;
            case APP_PET:     render_pet(&g_pet, g_frame,
                                         safe_battery()); break;
        }
    }

    // 1 Hz pet logic tick
    if (ms - g_last_tick_ms >= cfg::TICK_INTERVAL_MS) {
        uint32_t delta_s = (ms - g_last_tick_ms) / 1000;
        g_last_tick_ms  += delta_s * 1000;
        if (delta_s) pet_tick(&g_pet, delta_s);
    }
    if (ms - g_last_save_ms >= cfg::SAVE_INTERVAL_MS) {
        g_last_save_ms = ms;
        storage_save(&g_pet);
    }

    g_frame++;
    render_set_diag(M5.BtnA.isPressed(), M5.BtnB.isPressed());
    render_present();
    delay(33);
}
