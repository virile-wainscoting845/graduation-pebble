# graduation-pebble 🦉

> **An open-source, hackable, Tamagotchi-style graduation memento firmware for ESP32-S3 (M5StickS3 K150).**
> A keepsake that *lives*: counts down to graduation day, lets your cohort find each other via radar, replays a 4-year story, and locks a 5-year letter to your future self.

[![PlatformIO](https://img.shields.io/badge/PlatformIO-6.x-orange)](https://platformio.org/)
[![M5Stack](https://img.shields.io/badge/M5Stack-StickS3-red)](https://shop.m5stack.com/products/m5sticks3-esp32s3-mini-iot-dev-kit)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Made for graduates](https://img.shields.io/badge/Made%20for-graduates-blueviolet)](#)

> **中文** ➜ [README.zh-TW.md](README.zh-TW.md)

---

## What is this?

Most graduation gifts (mugs, photo books, plaques) are **dead** — you read them once, then they collect dust. `graduation-pebble` is different: it **changes every day**. It greets you with how many days until your ceremony, reminds you to find missing cohort-mates via short-range radar, replays your four years as you press a button, and seals a 5-year letter that *your future self* will unlock in 2031.

Designed first for the **NYCU Arete Honors Program 115th cohort** (2026), this firmware is **forkable for any graduating class, in any program, in any country**. Drop in your school's logo, your cohort's data, your students' photos, and ship.

---

## Features

| Mode | Description |
|---|---|
| **Splash** | Cinematic 5-stage boot animation: black ▸ particle convergence ▸ flash ▸ crest reveal ▸ pulsing rings + chiptune. |
| **Home** | Always-alive dashboard: name + ID + avatar + **N-day countdown** to ceremony + classmate-radar progress (`N / total connected`). Auto-switches to "graduation today!" then "graduated N days ago" after the date. |
| **Story** | 4 seasonal year-recap pages + closing message. UTF-8 typewriter captions, season art (autumn leaf / summer sun / spring sakura / winter stars), 5 paginator dots. |
| **Radar** | Spinning-sweep proximity scanner. Finds classmates over **IR / ESP-NOW** (Phase 2). Detect 5 quick presses to unlock the **百川合一 / Cohort Unity** easter egg. |
| **Capsule** | Time-locked countdown to a future date (default: graduation + 5 years). Lets you sneak-peek the letter; locks it firmly until the unlock day. |
| **Pet** | Owl tamagotchi (homage to Athena's owl + Claude Buddy IG8). Feed, play, sleep, shake to pet. Full state persistence in NVS. |

**Power-off**: hold A + B for 2 seconds (PMIC-level shutdown via M5PM1).

---

## Hardware

| Component | Purpose |
|---|---|
| [M5StickS3 K150](https://shop.m5stack.com/products/m5sticks3-esp32s3-mini-iot-dev-kit) | ESP32-S3-PICO-1-N8R8 · 8 MB Flash · 8 MB OPI PSRAM · 1.14" 135×240 ST7789P3 · BMI270 IMU · ES8311 audio · M5PM1 PMIC · IR TX/RX · 250 mAh Li-Po. |
| USB-C cable | For flashing. |

You can adapt this to other M5Stack ESP32-S3 family devices (StickC PLUS2, AtomS3, etc.) — see [`docs/hardware-guide.md`](docs/hardware-guide.md).

---

## Quick start

```bash
# 1. Install PlatformIO (or use VS Code + PlatformIO extension)
pip install --user platformio

# 2. Clone and build
git clone https://github.com/thc1006/graduation-pebble.git
cd graduation-pebble
pio run -e m5sticks3

# 3. Put your M5StickS3 into download mode
#    (long-press side button ~2 sec until the green LED blinks)

# 4. Flash
pio run -t upload -e m5sticks3
```

Default demo cohort is **NYCU Arete 115** (蔡秀吉). To customize:

```bash
# Edit your cohort data
$EDITOR include/demo_data.h

# Edit theme (palette, crest)
$EDITOR src/render.cpp           # see draw_arete_crest() and palette in include/config.h

# Re-flash
pio run -t upload -e m5sticks3
```

Detailed customization: [`docs/adapting-for-your-school.md`](docs/adapting-for-your-school.md).

---

## Project structure

```
graduation-pebble/
├── src/                  Generic firmware (state machine + rendering)
├── include/              Headers + demo cohort data
├── themes/               Visual identities (palette + crest drawing)
│   └── nycu-arete/       Reference theme (navy + gold + Greek temple)
├── cohorts/              Per-cohort student data
│   └── nycu-arete-115/   Reference cohort (NYCU 2026)
├── tools/                Phase 1 batch tooling (Python)
├── docs/                 Architecture + customization guides
└── platformio.ini        Build config
```

---

## Roadmap

| Phase | Status | Goal |
|---|---|---|
| **0 — Demo** | ✅ done (2026-05-06) | Single-device demo with all 6 modes working on hardware. |
| **1 — Personalization tooling** | 🚧 in progress | Python CLI to bake per-student `personalization.h` from a CSV + photo set. Companion data-collection web form: [`graduation-pebble-form`](https://github.com/thc1006/graduation-pebble-form). |
| **2 — ESP-NOW radar** | ⏳ planned | Real cohort discovery via 2.4 GHz beacons (replaces simulation). |
| **3 — Real RTC** | ⏳ planned | Capsule countdown survives reboots via M5PM1 RTC + optional NTP. |
| **4 — Multi-theme & i18n** | ⏳ planned | Theme abstraction; first non-NYCU example theme; English/中文/日/한 strings. |

---

## Contributing

PRs welcome — especially:

- **New themes** for your institution → see [`docs/creating-a-theme.md`](docs/creating-a-theme.md).
- **New cohort instances** to share your class's keepsake design → [`docs/creating-a-cohort.md`](docs/creating-a-cohort.md).
- **Bug fixes** and **hardware ports** (other ESP32-S3 boards).

Read [`CONTRIBUTING.md`](CONTRIBUTING.md) before opening a PR.

---

## Inspirations

- **Anthropic Claude Code Buddy** (April Fools 2026) — owl species `IG8` ([writeup](docs/inspirations.md)).
- **Athena's owl** — symbol of wisdom, of the NYCU Arete crest's Greek temple motif.
- **百川入海** ("hundred rivers flow into the sea") — the metaphor that every cohort member's path is unique yet converges.
- **Tamagotchi (1996)** — for proving keepsakes can have heartbeats.

---

## License

MIT — see [`LICENSE`](LICENSE) and [`NOTICE`](NOTICE).

The 百川 / Arete crest rendered in `themes/nycu-arete/` remains the intellectual property of NYCU. Please replace it with your own institution's identity when forking.
