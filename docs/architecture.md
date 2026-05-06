# Architecture

## State machine

Six application modes, switched at runtime via `enter_mode()` in `src/main.cpp`:

```
                    ┌──────────────────┐
                    │   APP_SPLASH     │  cinematic boot, ~4.5 s
                    │   (auto-advance) │
                    └────────┬─────────┘
                             │ A press / timeout
                             v
                    ┌──────────────────┐
       ┌────────────│    APP_HOME      │ ◄────── A short (back) ──┐
       │            │   (idle screen)  │                          │
       │            └────────┬─────────┘                          │
       │                     │                                    │
       │                     │ A short → STORY                    │
       │                     │ A hold  → CAPSULE                  │
       │                     │ B short → RADAR                    │
       │                     │ B hold  → PET                      │
       │                     │                                    │
       │                     v                                    │
       │     ┌──────────┬──────────┬──────────┬──────────┐         │
       │     │  STORY   │  RADAR   │ CAPSULE  │   PET    │─────────┘
       │     │ 5 pages  │ + unity  │ countdown│  (owl)   │
       │     │ autoplay │  egg     │   lock   │tamagotchi│
       │     └──────────┴──────────┴──────────┴──────────┘
       │
       │ A+B held 2s — global, suppresses per-mode handlers
       v
   ┌──────────────┐
   │  power off   │  M5.Power.powerOff() + esp_deep_sleep_start()
   └──────────────┘
```

## Source tree

```
src/
├── main.cpp        State machine, button routing, IMU shake handler,
│                   PIN setup, the global update/render loop @ ~30 fps.
├── render.cpp      All visual rendering — splash, home, story, radar,
│                   capsule, pet. Procedural drawing of:
│                     - Greek-temple-and-olive-wreath crest
│                     - Owl mascot (Buddy IG8 lineage)
│                     - Brand bar, hint footer, toast
│                   Uses M5Canvas (PSRAM-backed full-screen sprite).
├── pet.cpp         Pet mechanics: stat decay/regen tick logic, mood
│                   resolution, life-stage progression. Pure logic,
│                   no rendering.
└── storage.cpp     NVS persistence for PetState (Preferences API +
                    CRC32 magic). 64-byte struct, written at most every
                    60 s.

include/
├── config.h        Tunables: timing, decay rates, palette (RGB565),
│                   thresholds, screen dimensions.
├── demo_data.h     Hardcoded cohort data (蔡秀吉 / NYCU 115 reference).
├── pet.h
├── render.h
└── storage.h
```

## Rendering pipeline

Every frame at ~30 fps:

1. `M5.update()` polls buttons + IMU.
2. Global combo check (`A+B` for 2 s → power off).
3. Per-mode `update_*()` handles input + state transitions.
4. Per-mode `render_*()` draws into the shared M5Canvas sprite.
5. `render_present()` overlays toast, then `pushSprite(0, 0)` blits to LCD.

Tear-free because the sprite lives in PSRAM and pushSprite is DMA-driven.

## Font strategy

- **`fonts::Font0`** (M5GFX bundled) — ASCII-only 6×8 bitmap. Used for English labels, numbers, hints.
- **`fonts::efontTW_16`** (M5GFX bundled) — Traditional Chinese 16×16 + ASCII 8×16. Used for all Han text.
- Helpers `use_ascii(int size)` / `use_cjk(int size)` switch fonts; widths via `M5Canvas::textWidth()`.

Why no auto font fallback? The M5GFX font registration system requires explicit fallback chains via `setFont()`. Phase 4 may add a wrapper that auto-selects font based on byte ranges in the input string.

## Persistence

Pet state is the only thing persisted. NVS via `Preferences::putBytes()`, namespace `axie`, key `state`. The struct includes a magic word + CRC32 to detect corruption / firmware-version mismatch (corruption → re-init to fresh egg state).

Time capsule countdown is **not** currently persisted (Phase 3 will fix this with M5PM1 RTC). Today's countdown derives from `millis() / 86400000`, which resets on every reboot.

## Memory budget

- Firmware: ~1.13 MB Flash / 8 MB available (14%)
- RAM: ~24 KB / 320 KB internal (7%)
- PSRAM: 64 KB used by the canvas sprite / 8 MB available (<1%)

Plenty of room for Phase 1+ features (per-student avatars, voice clips, longer story captions).
