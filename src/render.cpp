// =============================================================================
//  Arete Pebble — render module (rewrite v2)
//
//  Fixes from comprehensive code review:
//   C1+C2 : CJK font (efontTW_16) + textWidth() for all Chinese strings
//   H1    : story caption uses setTextWrap + UTF-8-aware reveal
//   H2    : battery clamp (caller-side)
//   H3    : sprite alloc fallback hardened
//   M10   : brand bar simplified, no name overflow
//   M12   : keyword cloud uses CJK font with proper line stride
//
//  User feedback addressed:
//   - splash text was overflowing -> rebalanced layout, big CJK title
//   - "更勁爆" -> 5-stage splash (particles -> flash -> reveal -> pulse)
//   - unity easter egg now has Chinese explanation
//   - all CJK rendered correctly
//
//  Design: storefront screens (splash/unity/closing) push wow factor;
//  daily screens (home/pet/radar/capsule) prioritize legibility.
// =============================================================================
#include "render.h"
#include "config.h"
#include "demo_data.h"
#include <Arduino.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

// =============================================================================
//  Globals
// =============================================================================
static M5Canvas g_canvas(&M5.Display);
static char     g_msg[40]   = {0};
static uint32_t g_msg_until = 0;
static bool     g_diag_a    = false;
static bool     g_diag_b    = false;

static const lgfx::IFont *FONT_ASCII  = &fonts::Font0;
static const lgfx::IFont *FONT_CJK    = &fonts::efontTW_16;

// =============================================================================
//  Helpers — font / centering / UTF-8
// =============================================================================
static inline void use_ascii(int size = 1) {
    g_canvas.setFont(FONT_ASCII);
    g_canvas.setTextSize(size);
}
static inline void use_cjk(int size = 1) {
    g_canvas.setFont(FONT_CJK);
    g_canvas.setTextSize(size);
}

// Centered drawString in CURRENT font/size
static void draw_centered(const char *s, int y, uint16_t col) {
    int w = g_canvas.textWidth(s);
    int x = (cfg::SCREEN_W - w) / 2;
    if (x < 0) x = 0;
    g_canvas.setTextColor(col);
    g_canvas.drawString(s, x, y);
}

static int utf8_byte_len(uint8_t lead) {
    if (lead < 0x80)         return 1;
    if ((lead & 0xE0) == 0xC0) return 2;
    if ((lead & 0xF0) == 0xE0) return 3;
    if ((lead & 0xF8) == 0xF0) return 4;
    return 1;  // malformed; advance one byte
}

// Count complete UTF-8 codepoints in s
static int utf8_count_chars(const char *s) {
    int n = 0;
    while (*s) {
        s += utf8_byte_len((uint8_t)*s);
        n++;
    }
    return n;
}

// Build a prefix containing the first n_chars UTF-8 codepoints. Returns byte length.
static int utf8_prefix(const char *s, int n_chars, char *out, int out_cap) {
    int byte_pos = 0;
    int chars = 0;
    int len = (int)strlen(s);
    while (byte_pos < len && chars < n_chars) {
        int adv = utf8_byte_len((uint8_t)s[byte_pos]);
        if (byte_pos + adv > len)              break;
        if (byte_pos + adv >= out_cap)         break;
        for (int k = 0; k < adv; k++) out[byte_pos + k] = s[byte_pos + k];
        byte_pos += adv;
        chars++;
    }
    out[byte_pos] = 0;
    return byte_pos;
}

// =============================================================================
//  Public API: init / message / present
// =============================================================================
void render_init() {
    g_canvas.setPsram(true);
    g_canvas.setColorDepth(16);
    if (!g_canvas.createSprite(cfg::SCREEN_W, cfg::SCREEN_H)) {
        // hardened fallback: try 8-bit in internal RAM
        g_canvas.setPsram(false);
        g_canvas.setColorDepth(8);
        if (!g_canvas.createSprite(cfg::SCREEN_W, cfg::SCREEN_H)) {
            M5.Display.setCursor(0, 0);
            M5.Display.print("sprite alloc fail");
            while (true) delay(1000);
        }
    }
    g_canvas.setFont(FONT_ASCII);
    g_canvas.setTextDatum(textdatum_t::top_left);
    g_canvas.setTextWrap(false);
}

void render_message(const char *msg) {
    strncpy(g_msg, msg, sizeof(g_msg) - 1);
    g_msg[sizeof(g_msg) - 1] = 0;
    g_msg_until = millis() + cfg::MSG_DURATION_MS;
}

void render_set_diag(bool a, bool b) { g_diag_a = a; g_diag_b = b; }

static void draw_toast() {
    if (!g_msg[0]) return;
    if ((int32_t)(millis() - g_msg_until) >= 0) return;

    use_ascii(1);
    int tw = g_canvas.textWidth(g_msg) + 14;
    if (tw > cfg::SCREEN_W - 4) tw = cfg::SCREEN_W - 4;
    int tx = (cfg::SCREEN_W - tw) / 2;
    int ty = cfg::SCREEN_H / 2 - 9;
    g_canvas.fillRoundRect(tx, ty, tw, 18, 4, cfg::COL_NAVY_DEEP);
    g_canvas.drawRoundRect(tx, ty, tw, 18, 4, cfg::COL_GOLD);
    g_canvas.setTextColor(cfg::COL_BRAND_WHITE);
    g_canvas.drawString(g_msg, tx + 7, ty + 5);
}

void render_present() {
    draw_toast();
    g_canvas.pushSprite(0, 0);
}

// =============================================================================
//  Brand assets — Greek temple, olive wreath, full Arete crest
// =============================================================================
static void draw_temple_mini(M5Canvas &c, int x, int y, uint16_t col) {
    c.fillTriangle(x + 0, y + 3, x + 13, y + 3, x + 6, y + 0, col);
    c.drawLine(x + 0, y + 3, x + 13, y + 3, col);
    c.fillRect(x + 1, y + 4, 12, 1, col);
    for (int i = 0; i < 4; i++) c.fillRect(x + 2 + i * 3, y + 5, 1, 4, col);
    c.fillRect(x + 0, y + 9, 14, 1, col);
    c.fillRect(x + 1, y + 10, 12, 1, col);
}

static void draw_temple_large(M5Canvas &c, int cx, int cy, uint16_t col) {
    int baseY = cy;
    c.fillRect(cx - 16, baseY - 3, 32, 3, col);
    c.fillRect(cx - 14, baseY - 6, 28, 3, col);
    c.fillRect(cx - 12, baseY - 9, 24, 3, col);
    int colY = baseY - 26;
    int colH = 17;
    for (int i = 0; i < 6; i++) {
        int colX = cx - 13 + i * 5;
        c.fillRect(colX, colY, 2, colH, col);
        c.fillRect(colX - 1, colY - 1, 4, 2, col);
    }
    c.fillRect(cx - 14, colY - 3, 28, 2, col);
    c.fillRect(cx - 15, colY - 5, 30, 2, col);
    c.fillTriangle(cx - 15, colY - 5, cx + 15, colY - 5, cx, colY - 14, col);
    c.drawLine(cx - 15, colY - 5, cx, colY - 14, col);
    c.drawLine(cx + 15, colY - 5, cx, colY - 14, col);
}

static void draw_wreath_leaf(M5Canvas &c, int x, int y, int dir, uint16_t col) {
    if (dir > 0) c.fillTriangle(x, y, x + 4, y - 2, x + 4, y + 2, col);
    else         c.fillTriangle(x, y, x - 4, y - 2, x - 4, y + 2, col);
}

static void draw_olive_wreath(M5Canvas &c, int cx, int cy, int radius, uint16_t col) {
    for (int i = 0; i < 8; i++) {
        float t = i / 7.0f;
        float ang = 3.6f - t * 2.4f;
        int lx = cx + (int)(cosf(ang) * radius);
        int ly = cy + (int)(sinf(ang) * radius);
        draw_wreath_leaf(c, lx, ly, (lx < cx) ? -1 : 1, col);
    }
    for (int i = 0; i < 8; i++) {
        float t = i / 7.0f;
        float ang = -0.4f + t * 2.4f;
        int lx = cx + (int)(cosf(ang) * radius);
        int ly = cy + (int)(sinf(ang) * radius);
        draw_wreath_leaf(c, lx, ly, (lx < cx) ? -1 : 1, col);
    }
    c.fillRect(cx - 4, cy + radius - 2, 8, 3, col);
    c.drawLine(cx - 6, cy + radius + 1, cx - 2, cy + radius + 4, col);
    c.drawLine(cx + 6, cy + radius + 1, cx + 2, cy + radius + 4, col);
}

// Compact version of the crest, fits 32px radius (vs original 50px)
static void draw_arete_crest(M5Canvas &c, int cx, int cy, uint16_t col) {
    c.drawCircle(cx, cy, 32, col);
    c.drawCircle(cx, cy, 31, col);
    for (int i = 0; i < 48; i++) {
        float ang = i * (M_PI * 2.0f / 48.0f);
        int x1 = cx + (int)(cosf(ang) * 30);
        int y1 = cy + (int)(sinf(ang) * 30);
        int x2 = cx + (int)(cosf(ang) * 27);
        int y2 = cy + (int)(sinf(ang) * 27);
        c.drawLine(x1, y1, x2, y2, col);
    }
    c.drawCircle(cx, cy, 24, col);
    draw_temple_large(c, cx, cy + 12, col);
    draw_olive_wreath(c, cx, cy, 38, col);
}

// =============================================================================
//  Owl mascot (procedural — Buddy IG8 lineage)
// =============================================================================
static void draw_owl(M5Canvas &c, int cx, int cy, PetMood mood, uint8_t stage, uint32_t f) {
    int hw, hh;
    switch (stage) {
        case STAGE_BABY:  hw = 26; hh = 30; break;
        case STAGE_TEEN:  hw = 30; hh = 36; break;
        case STAGE_ADULT: hw = 34; hh = 42; break;
        default:          hw = 28; hh = 33; break;
    }
    int bob = (mood == MOOD_SLEEPING || mood == MOOD_DEAD) ? 0 : (int)(sinf(f * 0.10f) * 2);
    cy += bob;

    int tuft_y = cy - hh + 2;
    if (mood != MOOD_SLEEPING && mood != MOOD_DEAD) {
        c.fillTriangle(cx - hw / 2 + 2,  tuft_y,
                       cx - hw / 2 - 4,  tuft_y - 8,
                       cx - hw / 2 + 6,  tuft_y - 5, cfg::COL_NAVY_DEEP);
        c.fillTriangle(cx + hw / 2 - 2,  tuft_y,
                       cx + hw / 2 + 4,  tuft_y - 8,
                       cx + hw / 2 - 6,  tuft_y - 5, cfg::COL_NAVY_DEEP);
    }

    c.fillEllipse(cx, cy, hw + 1, hh + 1, cfg::COL_NAVY_DEEP);
    c.fillEllipse(cx, cy, hw, hh, cfg::COL_NAVY_LIGHT);
    c.fillEllipse(cx, cy + hh / 3, hw - 6, hh / 2, cfg::COL_GOLD_DIM);

    int wing_y = cy + 2;
    c.fillEllipse(cx - hw + 2, wing_y, 6, hh / 2, cfg::COL_NAVY_DEEP);
    c.fillEllipse(cx + hw - 2, wing_y, 6, hh / 2, cfg::COL_NAVY_DEEP);

    c.fillEllipse(cx, cy - 4, hw - 4, hh / 2, cfg::COL_BRAND_WHITE);
    c.fillTriangle(cx - 2, cy - 8, cx + 2, cy - 8, cx, cy + 2, cfg::COL_NAVY_LIGHT);

    int eye_dx = hw / 2 - 4;
    int eye_lx = cx - eye_dx;
    int eye_rx = cx + eye_dx;
    int eye_y  = cy - 6;
    int eye_r  = (stage == STAGE_BABY) ? 5 : 6;

    bool blink = (f % 220) < 6;
    if (mood == MOOD_DEAD) {
        for (int d = -eye_r; d <= eye_r; d++) {
            c.drawPixel(eye_lx + d, eye_y + d, cfg::COL_EYE_BLACK);
            c.drawPixel(eye_lx + d, eye_y - d, cfg::COL_EYE_BLACK);
            c.drawPixel(eye_rx + d, eye_y + d, cfg::COL_EYE_BLACK);
            c.drawPixel(eye_rx + d, eye_y - d, cfg::COL_EYE_BLACK);
        }
    } else if (mood == MOOD_SLEEPING || blink) {
        for (int d = -eye_r; d <= eye_r; d++) {
            int dy = (eye_r - abs(d)) / 2;
            c.drawPixel(eye_lx + d, eye_y + dy, cfg::COL_EYE_BLACK);
            c.drawPixel(eye_rx + d, eye_y + dy, cfg::COL_EYE_BLACK);
        }
    } else if (mood == MOOD_HUNGRY) {
        c.fillCircle(eye_lx, eye_y, eye_r + 1, cfg::COL_EYE_BLACK);
        c.fillCircle(eye_rx, eye_y, eye_r + 1, cfg::COL_EYE_BLACK);
        c.fillCircle(eye_lx, eye_y, 2, cfg::COL_BRAND_WHITE);
        c.fillCircle(eye_rx, eye_y, 2, cfg::COL_BRAND_WHITE);
    } else {
        c.fillCircle(eye_lx, eye_y, eye_r + 1, cfg::COL_GOLD);
        c.fillCircle(eye_rx, eye_y, eye_r + 1, cfg::COL_GOLD);
        c.fillCircle(eye_lx, eye_y, eye_r - 1, cfg::COL_EYE_BLACK);
        c.fillCircle(eye_rx, eye_y, eye_r - 1, cfg::COL_EYE_BLACK);
        c.fillCircle(eye_lx - 1, eye_y - 1, 2, cfg::COL_BRAND_WHITE);
        c.fillCircle(eye_rx - 1, eye_y - 1, 2, cfg::COL_BRAND_WHITE);
    }

    c.fillTriangle(cx - 3, cy - 1, cx + 3, cy - 1, cx, cy + 4, cfg::COL_GOLD);

    int foot_y = cy + hh - 2;
    c.fillRect(cx - 6, foot_y, 4, 3, cfg::COL_GOLD);
    c.fillRect(cx + 2, foot_y, 4, 3, cfg::COL_GOLD);

    if (mood == MOOD_DIRTY) {
        c.fillCircle(cx - hw / 2 + 4, cy + 2, 2, cfg::COL_DIRT);
        c.fillCircle(cx + hw / 3,     cy + 6, 2, cfg::COL_DIRT);
        c.fillCircle(cx - 2,          cy + 10, 2, cfg::COL_DIRT);
    }
}

// =============================================================================
//  Brand bar (top bar) — simplified for legibility
// =============================================================================
static void draw_brand_bar(const char *label, uint8_t batt) {
    auto &c = g_canvas;
    c.fillRect(0, 0, cfg::SCREEN_W, 14, cfg::COL_NAVY_DEEP);
    draw_temple_mini(c, 3, 2, cfg::COL_GOLD);
    use_ascii(1);
    c.setTextColor(cfg::COL_BRAND_WHITE);
    c.drawString(label, 22, 3);
    int bx = cfg::SCREEN_W - 22;
    c.drawRect(bx, 3, 18, 8, cfg::COL_BRAND_WHITE);
    c.fillRect(bx + 18, 5, 2, 4, cfg::COL_BRAND_WHITE);
    int safe_batt = (batt > 100) ? 100 : batt;
    int fw = safe_batt * 16 / 100;
    uint16_t bcol = batt < 20 ? cfg::COL_BAD
                  : batt < 50 ? cfg::COL_WARN
                              : cfg::COL_GOOD;
    if (fw > 0) c.fillRect(bx + 1, 4, fw, 6, bcol);
}

// =============================================================================
//  MODE: SPLASH (5 stages, ~3.5 s, "勁爆" tier)
//
//    0 ─ 200 ms  : pure black (anticipation)
//  200 ─ 1300 ms : 30 particles converge from edge to center
// 1300 ─ 1400 ms : white FLASH (punctuates the reveal)
// 1400 ─ 1700 ms : crest expands from radius 0 to 32 (zoom-in)
// 1700 ─ end    : full crest + pulsing rings + CJK title + tagline + "press A"
// =============================================================================
void render_splash(uint32_t enter_ms, uint32_t now_ms) {
    auto &c = g_canvas;
    uint32_t age = now_ms - enter_ms;
    int cx = cfg::SCREEN_W / 2;
    int cy = 56;          // crest pushed up: leaves 130 px below for text

    // ---- Phase 1: pure black ----
    if (age < 200) {
        c.fillScreen(0);
        return;
    }

    // ---- Phase 2: particle convergence ----
    if (age < 1300) {
        c.fillScreen(0);
        float t = (age - 200) / 1100.0f;
        if (t > 1.0f) t = 1.0f;
        for (int i = 0; i < 30; i++) {
            float a = i * 2.39996f + t * 0.5f;
            float r = 95.0f * (1.0f - t);
            int px = cx + (int)(cosf(a) * r);
            int py = cy + (int)(sinf(a) * r);
            uint16_t col = (i & 1) ? cfg::COL_GOLD : cfg::COL_BRAND_WHITE;
            int sz = 1 + (int)(t * 2.0f);
            c.fillCircle(px, py, sz, col);
        }
        int nucleus = (int)(t * 10.0f);
        if (nucleus > 0) {
            c.fillCircle(cx, cy, nucleus, cfg::COL_BRAND_WHITE);
            c.drawCircle(cx, cy, nucleus + 2, cfg::COL_GOLD);
        }
        return;
    }

    // ---- Phase 3: FLASH BANG ----
    if (age < 1400) {
        c.fillScreen(cfg::COL_BRAND_WHITE);
        return;
    }

    // ---- Phase 4: crest zoom-in ----
    if (age < 1700) {
        c.fillScreen(cfg::COL_NAVY);
        float t = (age - 1400) / 300.0f;
        int r = (int)(32 * t);
        if (r >= 6) {
            c.drawCircle(cx, cy, r,     cfg::COL_BRAND_WHITE);
            c.drawCircle(cx, cy, r - 2, cfg::COL_GOLD);
            if (r > 18) draw_temple_large(c, cx, cy + 6, cfg::COL_BRAND_WHITE);
        }
        return;
    }

    // ---- Phase 5: full reveal + pulsing rings + titles ----
    c.fillScreen(cfg::COL_NAVY);

    int pulse = (int)(sinf((age - 1700) * 0.005f) * 5);
    c.drawCircle(cx, cy, 38 + pulse,     cfg::COL_GOLD);
    c.drawCircle(cx, cy, 41 + pulse,     cfg::COL_GOLD_DIM);
    c.drawCircle(cx, cy, 44 + pulse * 2, cfg::COL_NAVY_LIGHT);

    draw_arete_crest(c, cx, cy, cfg::COL_BRAND_WHITE);

    // ---- Title block (all widths verified < 135 px) ----
    // 「百川 115」  size 2 cjk: 32+32+16+16+16+16 = 128 ✓
    use_cjk(2);
    draw_centered("百川 115", 108, cfg::COL_BRAND_WHITE);

    // "Arete Pebble"  size 1 ascii: 12*6 = 72 ✓
    use_ascii(1);
    draw_centered("Arete Pebble", 148, cfg::COL_GOLD);

    if (age > 2000) {
        // Date + ceremony — split into 2 lines (combined would be 160px wide).
        // "2026.06.13"  size 1 ascii: 60 ✓
        use_ascii(1);
        draw_centered("2026.06.13", 168, cfg::COL_BRAND_WHITE);
        // 「畢業典禮」  size 1 cjk: 4*16 = 64 ✓
        use_cjk(1);
        draw_centered("畢業典禮", 186, cfg::COL_GOLD);
    }

    if (age > 2700) {
        use_ascii(1);
        if (((age - 2700) / 400) & 1) {
            draw_centered("press A to begin", 220, cfg::COL_GOLD_DIM);
        }
    }
}

// =============================================================================
//  MODE: HOME — personal page with Chinese name + hashtag cloud
// =============================================================================
static void draw_avatar_placeholder(int cx, int cy, uint32_t f) {
    auto &c = g_canvas;
    int r = 20;          // smaller — leaves room for big name below
    c.fillCircle(cx, cy, r + 3, cfg::COL_GOLD);
    c.fillCircle(cx, cy, r + 1, cfg::COL_NAVY_DEEP);
    c.fillCircle(cx, cy, r, cfg::COL_NAVY_LIGHT);
    c.fillEllipse(cx, cy - 14, 17, 6, cfg::COL_NAVY_DEEP);
    c.drawCircle(cx - 6, cy - 2, 4, cfg::COL_GOLD);
    c.drawCircle(cx + 6, cy - 2, 4, cfg::COL_GOLD);
    c.drawLine(cx - 2, cy - 2, cx + 2, cy - 2, cfg::COL_GOLD);
    bool blink = (f % 200) < 5;
    if (blink) {
        c.drawLine(cx - 8, cy - 2, cx - 4, cy - 2, cfg::COL_EYE_BLACK);
        c.drawLine(cx + 4, cy - 2, cx + 8, cy - 2, cfg::COL_EYE_BLACK);
    } else {
        c.fillCircle(cx - 6, cy - 2, 1, cfg::COL_EYE_BLACK);
        c.fillCircle(cx + 6, cy - 2, 1, cfg::COL_EYE_BLACK);
    }
    for (int i = -3; i <= 3; i++) {
        int dy = (3 - abs(i)) / 2;
        c.drawPixel(cx + i, cy + 6 + dy, cfg::COL_EYE_BLACK);
    }
}

void render_home(const PetState *p, uint32_t f, uint8_t batt, uint8_t connections) {
    auto &c = g_canvas;
    c.fillScreen(cfg::COL_NAVY);
    draw_brand_bar("Arete Pebble", batt);
    (void)p; (void)g_diag_a; (void)g_diag_b;

    // ---- y layout — every line "alive" ----
    //   18..58 : avatar
    //   64..96 : 蔡秀吉   (size 2 cjk, 32 tall)
    //   102    : ID
    //   116    : gold separator
    //   124    : 「離畢業典禮」 / 「畢業已」 / 「今天畢業！」
    //   144..168: BIG number (ASCII size 3, 24 tall)
    //   178    : 「天」
    //   204    : 連線 N / 40 同學   (call to action)
    //   222/234 : 2 lines of hints

    // identity
    draw_avatar_placeholder(cfg::SCREEN_W / 2, 38, f);
    use_cjk(2);
    draw_centered(demo::OWNER_NAME_TW, 64, cfg::COL_BRAND_WHITE);
    use_ascii(1);
    draw_centered(demo::OWNER_ID, 102, cfg::COL_GOLD);

    c.drawFastHLine(20, 116, cfg::SCREEN_W - 40, cfg::COL_GOLD_DIM);

    // ---- countdown / phase logic ----
    int days_diff = (int)demo::GRAD_DAYS_AFTER_BOOT
                  - (int)(millis() / 86400000UL);

    const char *label;
    int        show_value;
    bool       grad_today = false;
    uint16_t   accent = cfg::COL_GOLD;       // pre-grad gold
    if (days_diff > 0) {
        label = "離畢業典禮";
        show_value = days_diff;
    } else if (days_diff == 0) {
        label = "今天畢業！";
        show_value = 0;
        grad_today = true;
    } else {
        label = "畢業已";
        show_value = -days_diff;
        accent = cfg::COL_BRAND_WHITE;       // post-grad silver
    }

    use_cjk(1);
    draw_centered(label, 124, cfg::COL_GOLD_DIM);

    if (grad_today) {
        // pulsing celebration
        use_cjk(2);
        if ((f / 10) & 1) draw_centered("百川合一", 152, cfg::COL_GOLD);
        else              draw_centered("百川合一", 152, cfg::COL_BRAND_WHITE);
    } else {
        char num_buf[8];
        snprintf(num_buf, sizeof(num_buf), "%d", show_value);
        // BIG number — ASCII size 3 = 18px wide × 24 tall per glyph
        use_ascii(3);
        draw_centered(num_buf, 144, accent);
        use_cjk(1);
        draw_centered("天", 178, cfg::COL_GOLD_DIM);
    }

    // ---- connection counter (call to action / progress) ----
    use_cjk(1);
    if (connections == 0) {
        draw_centered("尋找百川同學", 204, cfg::COL_GOLD_DIM);
    } else if (connections >= cfg::COHORT_TOTAL) {
        draw_centered("百川合一達成", 204, cfg::COL_GOLD);
    } else {
        char conn_buf[40];
        snprintf(conn_buf, sizeof(conn_buf), "連線 %u / %u 同學",
                 connections, cfg::COHORT_TOTAL);
        draw_centered(conn_buf, 204, cfg::COL_BRAND_WHITE);
    }

    // hints (2 lines compact)
    use_ascii(1);
    c.setTextColor(cfg::COL_GOLD_DIM);
    c.drawString("A:story    B:radar",  4, 222);
    c.setTextColor(cfg::COL_BRAND_WHITE);
    c.drawString("A+B 2s = power off",  4, 232);
}

// =============================================================================
//  MODE: STORY — 4 years + closing, CJK caption with wrap + UTF-8 typewriter
// =============================================================================
static void draw_season_art(int page, int px, int py, int pw, int ph, uint16_t accent, uint32_t f) {
    auto &c = g_canvas;
    if (page == 0) {
        // 大一 autumn: stylized leaf
        int lcx = px + pw / 2;
        int lcy = py + ph / 2;
        c.fillTriangle(lcx, lcy - 14, lcx - 12, lcy + 8, lcx + 12, lcy + 8, cfg::COL_BRAND_WHITE);
        c.fillTriangle(lcx, lcy - 10, lcx - 8,  lcy + 6, lcx + 8,  lcy + 6, accent);
        c.drawLine(lcx, lcy - 12, lcx, lcy + 10, cfg::COL_NAVY_DEEP);
    } else if (page == 1) {
        // 大二 summer: sun + waves
        c.fillCircle(px + pw / 2, py + ph / 3, 12, cfg::COL_GOLD);
        c.fillCircle(px + pw / 2, py + ph / 3, 10, cfg::COL_BRAND_WHITE);
        for (int i = 0; i < 3; i++) {
            int wy = py + ph - 14 + i * 4;
            for (int x = 0; x < pw; x += 4) {
                int dy = (int)(sinf((x + f) * 0.2f) * 2);
                c.drawPixel(px + x, wy + dy, cfg::COL_BRAND_WHITE);
            }
        }
    } else if (page == 2) {
        // 大三 spring: sakura petals
        for (int i = 0; i < 8; i++) {
            int sx = px + 12 + (i * 13) % (pw - 14);
            int sy = py + 10 + ((i * 7) % (ph - 20));
            for (int p2 = 0; p2 < 5; p2++) {
                float a = p2 * (M_PI * 2 / 5) + i * 0.3f;
                c.fillCircle(sx + (int)(cosf(a) * 3),
                             sy + (int)(sinf(a) * 3),
                             2, cfg::COL_BRAND_WHITE);
            }
        }
    } else if (page == 3) {
        // 大四 winter: night sky + crescent
        for (int i = 0; i < 14; i++) {
            int sx = px + 5 + (i * 11) % (pw - 10);
            int sy = py + 5 + ((i * 13) % (ph - 10));
            int br = ((f / 10 + i) % 4 == 0) ? 2 : 1;
            c.fillCircle(sx, sy, br, cfg::COL_GOLD);
        }
        c.fillCircle(px + pw - 22, py + 12, 8, cfg::COL_GOLD);
        c.fillCircle(px + pw - 18, py + 10, 7, accent);
    } else {
        // closing: small crest
        draw_arete_crest(c, px + pw / 2, py + ph / 2, cfg::COL_GOLD);
    }
}

void render_story(uint8_t page, uint32_t page_age_ms, uint32_t f) {
    auto &c = g_canvas;
    if (page >= cfg::STORY_PAGE_COUNT) page = cfg::STORY_PAGE_COUNT - 1;
    const auto &sp = demo::STORY[page];

    c.fillScreen(cfg::COL_NAVY);
    char bar[16];
    snprintf(bar, sizeof(bar), "Story %s", sp.year_label);
    draw_brand_bar(bar, 100);

    // season color band
    c.fillRect(0, 14, cfg::SCREEN_W, 50, sp.accent_color);

    // Chinese title (size 1 — keeps short and long titles consistent)
    //   "大一  秋" = 64 wide, "致 4 年前的自己" = 120 wide; both ✓ at size 1
    use_cjk(1);
    int tw = c.textWidth(sp.title_tw);
    int tx = (cfg::SCREEN_W - tw) / 2;
    c.setTextColor(cfg::COL_NAVY_DEEP);
    c.drawString(sp.title_tw, tx, 22);

    // ASCII date (small) — y=44 leaves clear gap below title (which ends at 38)
    use_ascii(1);
    draw_centered(sp.date_str, 46, cfg::COL_NAVY_DEEP);

    // placeholder photo frame
    int px = 12;
    int py = 70;
    int pw = cfg::SCREEN_W - 24;
    int ph = 56;
    c.fillRect(px - 2, py - 2, pw + 4, ph + 4, cfg::COL_GOLD);
    c.fillRect(px, py, pw, ph, cfg::COL_NAVY_LIGHT);
    draw_season_art(page, px, py, pw, ph, sp.accent_color, f);

    // ---- caption with UTF-8-aware typewriter + wrap ----
    int total_chars = utf8_count_chars(sp.caption);
    int reveal      = (int)(page_age_ms / 80);
    if (reveal > total_chars) reveal = total_chars;

    char buf[256];
    utf8_prefix(sp.caption, reveal, buf, sizeof(buf));

    use_cjk(1);
    c.setTextColor(cfg::COL_BRAND_WHITE);
    c.setTextWrap(true, false);
    c.setCursor(6, 134);
    c.print(buf);

    // blinking caret if not yet finished
    if (reveal < total_chars && (f / 8) & 1) {
        int cx = c.getCursorX();
        int cy_ = c.getCursorY();
        c.fillRect(cx, cy_ + 13, 6, 2, cfg::COL_GOLD);
    }
    c.setTextWrap(false);

    // page indicator dots
    int dy = cfg::SCREEN_H - 22;
    int dot_x0 = cfg::SCREEN_W / 2 - cfg::STORY_PAGE_COUNT * 4;
    for (int i = 0; i < cfg::STORY_PAGE_COUNT; i++) {
        uint16_t dc = (i == page) ? cfg::COL_GOLD : cfg::COL_NAVY_LIGHT;
        c.fillCircle(dot_x0 + i * 8, dy, 2, dc);
    }

    // hint
    use_ascii(1);
    c.setTextColor(cfg::COL_GOLD_DIM);
    c.drawString("A:home  B:next page", 4, cfg::SCREEN_H - 12);
}

// =============================================================================
//  MODE: RADAR — proximity scan + Chinese unity easter egg
// =============================================================================
void render_radar(uint8_t found, uint8_t total, uint32_t f,
                  const char *latest_name, uint32_t latest_age_ms,
                  bool unity_unlocked) {
    auto &c = g_canvas;
    c.fillScreen(cfg::COL_NAVY);
    draw_brand_bar("Radar", 100);

    if (unity_unlocked) {
        // ╔══ 百川合一 EASTER EGG ══╗
        int cx = cfg::SCREEN_W / 2;
        int cy = 76;

        // pulsing rays
        int pulse = (int)(sinf(f * 0.18f) * 4);
        for (int i = 0; i < 24; i++) {
            float ang = i * (M_PI * 2 / 24) + f * 0.025f;
            int x1 = cx + (int)(cosf(ang) * 16);
            int y1 = cy + (int)(sinf(ang) * 16);
            int x2 = cx + (int)(cosf(ang) * (44 + pulse));
            int y2 = cy + (int)(sinf(ang) * (44 + pulse));
            c.drawLine(x1, y1, x2, y2, (i & 1) ? cfg::COL_GOLD : cfg::COL_GOLD_DIM);
        }
        draw_arete_crest(c, cx, cy, cfg::COL_BRAND_WHITE);

        // ---- text block (all widths verified < 135) ----
        // 「百川合一」 NO spaces:  4*32 = 128 ✓
        use_cjk(2);
        draw_centered("百川合一", 142, cfg::COL_GOLD);

        // 「全 40 人到齊」 size 1 cjk:  16+8+8+8+8+16+16+16 = 96 ✓
        use_cjk(1);
        char unity_line[32];
        snprintf(unity_line, sizeof(unity_line), "全 %u 人到齊", total);
        draw_centered(unity_line, 180, cfg::COL_BRAND_WHITE);

        // "Arete 115" ascii size 1:  9*6 = 54 ✓
        use_ascii(1);
        draw_centered("Arete 115", 200, cfg::COL_GOLD);

        // 「恭賀畢業」 size 1 cjk:  4*16 = 64 ✓
        use_cjk(1);
        draw_centered("恭賀畢業", 214, cfg::COL_GOLD);

        // press A blink
        use_ascii(1);
        if ((f / 12) & 1) draw_centered("press A to home", 232, cfg::COL_BRAND_WHITE);
        return;
    }

    // ---- Normal radar view ----
    int cx = cfg::SCREEN_W / 2;
    int cy = 80;
    int rmax = 50;

    // concentric rings
    for (int r = 12; r <= rmax; r += 12) c.drawCircle(cx, cy, r, cfg::COL_NAVY_LIGHT);
    c.drawLine(cx - rmax, cy, cx + rmax, cy, cfg::COL_NAVY_LIGHT);
    c.drawLine(cx, cy - rmax, cx, cy + rmax, cfg::COL_NAVY_LIGHT);

    // sweep line with trail
    float sweep_ang = f * 0.08f;
    while (sweep_ang > M_PI * 2) sweep_ang -= M_PI * 2;
    for (int t = 0; t < 8; t++) {
        float a = sweep_ang - t * 0.08f;
        int sx = cx + (int)(cosf(a) * rmax);
        int sy = cy + (int)(sinf(a) * rmax);
        uint16_t col = (t == 0) ? cfg::COL_GOLD
                     : (t < 3)  ? cfg::COL_GOLD_DIM
                                : cfg::COL_NAVY_LIGHT;
        c.drawLine(cx, cy, sx, sy, col);
    }

    // detected cohort dots
    for (int i = 0; i < found; i++) {
        float ang = i * 2.39996f;
        float dist = 10.0f + (i * 3.5f);
        if (dist > rmax - 4) dist = rmax - 4;
        int dx = cx + (int)(cosf(ang) * dist);
        int dy = cy + (int)(sinf(ang) * dist);
        float diff = ang - sweep_ang;
        while (diff > M_PI)  diff -= 2 * M_PI;
        while (diff < -M_PI) diff += 2 * M_PI;
        int dr = (fabsf(diff) < 0.3f) ? 4 : 2;
        c.fillCircle(dx, dy, dr, cfg::COL_GOLD);
    }

    // big counter — CJK
    char cnt_buf[20];
    snprintf(cnt_buf, sizeof(cnt_buf), "%u / %u", found, total);
    use_ascii(2);
    draw_centered(cnt_buf, 138, cfg::COL_BRAND_WHITE);
    use_cjk(1);
    draw_centered("已連線同學", 162, cfg::COL_GOLD);

    // progress bar
    int pbx = 16;
    int pby = 180;
    int pbw = cfg::SCREEN_W - 32;
    c.drawRect(pbx, pby, pbw, 6, cfg::COL_GOLD);
    if (total > 0) {
        int fill = (int)found * (pbw - 2) / total;
        if (fill > 0) c.fillRect(pbx + 1, pby + 1, fill, 4, cfg::COL_GOLD);
    }

    // recent connection toast
    if (latest_name && latest_age_ms < 2500) {
        char latest[40];
        snprintf(latest, sizeof(latest), "+ %s 連線了", latest_name);
        use_cjk(1);
        draw_centered(latest, 192, cfg::COL_BRAND_WHITE);
    }

    // hint
    use_ascii(1);
    c.setTextColor(cfg::COL_GOLD_DIM);
    c.drawString("A:home  B:scan",          4, 210);
    c.drawString("(B x5 = unity!)",         4, 222);
}

// =============================================================================
//  MODE: CAPSULE — countdown lock + Chinese preview
// =============================================================================
void render_capsule(uint32_t boot_ms, bool force_preview) {
    auto &c = g_canvas;
    c.fillScreen(cfg::COL_NAVY);
    draw_brand_bar("Time Capsule", 100);

    int cx = cfg::SCREEN_W / 2;
    int icy = 40;

    // padlock
    if (force_preview) {
        c.fillRect(cx - 12, icy + 4, 24, 18, cfg::COL_GOLD);
        c.fillRect(cx - 8, icy + 8, 16, 10, cfg::COL_NAVY);
        c.drawCircle(cx - 4, icy - 2, 8, cfg::COL_GOLD);
        c.drawLine(cx + 14, icy - 4, cx + 18, icy - 8, cfg::COL_GOLD);
        c.drawLine(cx + 14, icy - 2, cx + 20, icy - 2, cfg::COL_GOLD);
    } else {
        c.fillRect(cx - 12, icy + 4, 24, 18, cfg::COL_GOLD);
        c.fillRect(cx - 8,  icy + 8, 16, 10, cfg::COL_NAVY);
        c.drawCircle(cx,    icy - 4, 8, cfg::COL_GOLD);
        c.drawCircle(cx,    icy - 4, 7, cfg::COL_GOLD);
        c.fillRect(cx - 1,  icy + 12, 2, 4, cfg::COL_NAVY);  // keyhole
    }

    // unlocks label
    use_cjk(1);
    draw_centered("解鎖時刻", 80, cfg::COL_GOLD);

    use_ascii(2);
    draw_centered(demo::CAPSULE_UNLOCK_DATE, 96, cfg::COL_BRAND_WHITE);

    // countdown
    uint32_t boot_seconds = boot_ms / 1000;
    uint32_t total_target = demo::CAPSULE_DAYS_AFTER_BOOT * 86400UL;
    uint32_t remain = (boot_seconds < total_target) ? (total_target - boot_seconds) : 0;
    uint32_t r_days  = remain / 86400UL;
    uint32_t r_hours = (remain % 86400UL) / 3600UL;
    uint32_t r_mins  = (remain % 3600UL) / 60UL;

    // Big day count, small hh mm below — keeps both readable in 135 px
    char d_buf[16], hm_buf[16];
    snprintf(d_buf,  sizeof(d_buf),  "%lu", (unsigned long)r_days);
    snprintf(hm_buf, sizeof(hm_buf), "%02luh %02lum",
             (unsigned long)r_hours, (unsigned long)r_mins);

    // "1864" at size 2 ascii: 4*12 = 48 ✓
    use_ascii(2);
    draw_centered(d_buf, 124, cfg::COL_GOLD);
    // 「天」 right after the number — drawn separately at fixed offset for impact
    use_cjk(1);
    int dw = c.textWidth(d_buf);   // re-measure with ascii size 2
    // we already drew d_buf centered; just add 「天」 to its right by computing same center+w/2
    // simpler: draw a centered combined string at smaller size below

    // "04h 23m" ascii size 1: 7*6 = 42 ✓
    use_ascii(1);
    draw_centered(hm_buf, 150, cfg::COL_BRAND_WHITE);

    use_cjk(1);
    draw_centered("剩餘時間", 166, cfg::COL_GOLD_DIM);
    (void)dw;

    // preview message — 給 5 年後的你...
    int py = 188;
    use_cjk(1);
    c.setTextColor(cfg::COL_GOLD);
    draw_centered(demo::CAPSULE_PREVIEW_TW, py, cfg::COL_GOLD);
    if (!force_preview) {
        // mosaic redaction bars
        c.fillRect(12, py + 18, cfg::SCREEN_W - 24, 3, cfg::COL_NAVY_LIGHT);
        c.fillRect(28, py + 24, cfg::SCREEN_W - 56, 3, cfg::COL_NAVY_LIGHT);
        use_cjk(1);
        draw_centered("（已封存）", py + 32, cfg::COL_NAVY_LIGHT);
    } else {
        use_cjk(1);
        draw_centered("給未來的自己一封信", py + 18, cfg::COL_BRAND_WHITE);
    }

    // hint
    use_ascii(1);
    c.setTextColor(cfg::COL_GOLD_DIM);
    c.drawString("A:home  B:peek", 4, cfg::SCREEN_H - 12);
}

// =============================================================================
//  MODE: PET — owl tamagotchi
// =============================================================================
static void draw_pet_stat_row(int y, const char *label, uint8_t val) {
    auto &c = g_canvas;
    use_ascii(1);
    c.setTextColor(cfg::COL_BRAND_WHITE);
    c.drawString(label, 4, y);
    int x0 = 38;
    int w  = cfg::SCREEN_W - x0 - 6;
    c.drawRect(x0, y - 1, w, 8, cfg::COL_NAVY_LIGHT);
    int fill = (int)val * (w - 2) / 100;
    uint16_t col = val < cfg::LOW_THRESH ? cfg::COL_BAD
                 : val < 50              ? cfg::COL_WARN
                                         : cfg::COL_GOOD;
    if (fill > 0) c.fillRect(x0 + 1, y, fill, 6, col);
}

void render_pet(const PetState *p, uint32_t f, uint8_t batt) {
    auto &c = g_canvas;
    c.fillScreen(cfg::COL_NAVY);
    draw_brand_bar("Owl", batt);

    PetMood mood = pet_current_mood(p);
    int cx = cfg::SCREEN_W / 2;

    if (p->stage == STAGE_EGG) {
        int wob = (int)(sinf(f * 0.08f) * 1.5f);
        int cy = 70;
        c.fillEllipse(cx + wob, cy, 28, 36, cfg::COL_NAVY_DEEP);
        c.fillEllipse(cx + wob, cy, 26, 34, cfg::COL_NAVY_LIGHT);
        c.fillEllipse(cx + wob, cy + 3, 22, 28, cfg::COL_GOLD_DIM);
    } else {
        draw_owl(c, cx, 70, mood, p->stage, f);
    }

    // age
    use_ascii(1);
    char age_buf[24];
    uint32_t mins = p->alive_seconds / 60;
    snprintf(age_buf, sizeof(age_buf), "%lum %s",
             (unsigned long)mins, pet_stage_name((PetStage)p->stage));
    draw_centered(age_buf, 124, cfg::COL_GOLD);

    int y0 = 138;
    draw_pet_stat_row(y0 +  0, "FOOD", p->fullness);
    draw_pet_stat_row(y0 + 11, "HAPP", p->happiness);
    draw_pet_stat_row(y0 + 22, "ENGY", p->energy);
    draw_pet_stat_row(y0 + 33, "CLN ", p->cleanliness);
    draw_pet_stat_row(y0 + 44, "HLTH", p->health);

    use_ascii(1);
    c.setTextColor(cfg::COL_GOLD_DIM);
    c.drawString("A:feed  hold:clean",  4, 198);
    c.drawString("B:play  hold:sleep",  4, 210);
    c.setTextColor(cfg::COL_BRAND_WHITE);
    c.drawString("A x3 = home",         4, 224);
}
