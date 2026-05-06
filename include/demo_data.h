// =============================================================================
//  Demo personalization data for Arete Pebble — Phase 0 build.
//
//  Owner : 蔡秀吉 (Hsiu-Chi Tsai)
//  Cohort: Arete Honors Program (百川學士學位學程) 115 級
//
//  In Phase 1 this header gets replaced per-device by a Python tool that reads
//  each student's form submission and writes a per-device user_data.bin. For
//  the demo build, we hardcode one student so all features show real content.
// =============================================================================
#pragma once
#include <stdint.h>

namespace demo {

// ---- identity ----
constexpr const char* OWNER_NAME_TW  = "蔡秀吉";
constexpr const char* OWNER_NAME_EN  = "Hsiu-Chi Tsai";
constexpr const char* OWNER_ID       = "110950008";
constexpr const char* COHORT_LABEL   = "Arete 115";
constexpr const char* PROGRAM_TW     = "百川學士學位學程";
constexpr const char* PROGRAM_EN     = "Arete Honors Program";

// ---- key dates (epoch seconds, hardcoded for demo) ----
//   Enroll : 2022-09-01 00:00 UTC+8
//   Today  : 2026-05-06 (boot reference)
//   Grad   : 2026-06-13 13:00 UTC+8
//   Capsule unlock: 2031-06-13 13:00 UTC+8 (5 years after grad)
constexpr uint32_t ENROLL_DAYS_BEFORE_BOOT = 1343;   // ~3.7 years until 2026-05-06
constexpr uint32_t GRAD_DAYS_AFTER_BOOT    = 38;     // 2026-06-13
constexpr uint32_t CAPSULE_DAYS_AFTER_BOOT = 38 + 365 * 5 + 1;   // 5 yrs after grad

// ---- 5 keywords (hashtag cloud) ----
constexpr const char* KEYWORDS[5] = {
    "#量子計算",
    "#永續設計",
    "#社會企業",
    "#獨立研究",
    "#百川入海",
};

// ---- 4-year story pages + 1 closing ----
struct StoryPage {
    const char* title_tw;       // e.g. "大一 · 秋"
    const char* year_label;     // e.g. "Year 1"
    const char* date_str;       // e.g. "2022.09"
    const char* caption;        // 1-line user-written caption
    uint16_t    accent_color;   // dominant tone for this page
};

constexpr StoryPage STORY[5] = {
    { "大一  秋", "Year 1",
      "2022.09",
      "初來百川，不知所措但充滿期待。",
      0xFC68 /* AUTUMN */ },
    { "大二  夏", "Year 2",
      "2023.07",
      "找到第一個跨域興趣，開始逃離舒適圈。",
      0x6F6D /* SUMMER */ },
    { "大三  春", "Year 3",
      "2024.04",
      "在實驗室和文學營之間，學會擁抱矛盾。",
      0xFB9A /* SPRING */ },
    { "大四  冬", "Year 4",
      "2026.01",
      "回望四年，原來路是自己走出來的。",
      0x9999 /* WINTER */ },
    { "致 4 年前的自己", "Closing",
      "2026.06.13",
      "謝謝你選了這條沒人走過的路。",
      0xFE60 /* GOLD */ },
};

// ---- Time capsule (locked until 2031-06-13) ----
constexpr const char* CAPSULE_PREVIEW_TW = "給 5 年後的你...";
constexpr const char* CAPSULE_PREVIEW_EN = "To my future self...";
constexpr const char* CAPSULE_UNLOCK_DATE = "2031-06-13";

// ---- Cohort name list for radar simulation (10 sample names) ----
//   In production: each device's NVS holds the actual cohort roster.
//   Demo: rotates through these as "discovered" cohort members.
constexpr const char* COHORT_NAMES[10] = {
    "林思辰", "張柏宇", "王芷涵", "陳冠廷", "黃語婕",
    "李宥成", "劉沛瑤", "吳秉謙", "周妍希", "鄭子翔",
};

}  // namespace demo
