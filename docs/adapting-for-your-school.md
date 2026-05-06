# Adapting graduation-pebble for your school

This guide assumes you've successfully built and flashed the reference firmware (the NYCU Arete 115 demo) following the [main README](../README.md). Now you want to make it *yours*.

---

## What's hardcoded today (Phase 0)

To stay shippable in a 38-day timeline for the original NYCU 115 cohort, **the Phase 0 firmware bakes the theme and cohort data straight into `src/render.cpp` and `include/demo_data.h`**. Phase 1 (in progress) introduces a clean theme/cohort abstraction.

Until Phase 1 lands, adapting requires editing four places:

| File | What to change | What it controls |
|---|---|---|
| `include/demo_data.h` | Owner name, ID, story captions, keywords, capsule message | Personal content for one student |
| `include/config.h` | Palette (`COL_NAVY`, `COL_GOLD`, `COL_AXO_*`) | Brand colors |
| `src/render.cpp` | `draw_arete_crest()`, `draw_temple_*`, `draw_olive_wreath()` | Crest geometry |
| `platformio.ini` | (no changes needed unless you fork) | Build config |

---

## Step 1 — Replace the cohort data

Open `include/demo_data.h`. Update:

```cpp
constexpr const char* OWNER_NAME_TW  = "你的名字";
constexpr const char* OWNER_ID       = "your-id";
constexpr const char* COHORT_LABEL   = "Your School 20XX";
constexpr const char* PROGRAM_TW     = "你的學程";

constexpr uint32_t ENROLL_DAYS_BEFORE_BOOT = NNN;   // days from enrollment to today
constexpr uint32_t GRAD_DAYS_AFTER_BOOT    = NN;    // days from today to ceremony
constexpr uint32_t CAPSULE_DAYS_AFTER_BOOT = NN + 365 * 5;   // 5 years post-grad

constexpr const char* KEYWORDS[5] = {
    "#your", "#interests", "#here", "#five", "#tags",
};

constexpr StoryPage STORY[5] = {
    { "Year 1 title", "Year 1", "20XX.MM", "Your year-1 caption.", 0xFC68 },
    // ... fill in 4 years + closing
};
```

If your name or labels are not in Traditional Chinese, you can keep using the `efontTW_16` font (it covers ASCII + Latin extended) or swap to a different M5GFX font (e.g. `efontJA_16` for Japanese, `efontKR_16` for Korean — check available fonts in `M5GFX/src/lgfx/v1/lgfx_fonts.cpp`).

---

## Step 2 — Replace the visual theme

The two boldest visual signatures of the NYCU theme are:

1. **Palette**: navy (`COL_NAVY = 0x1186`) + gold (`COL_GOLD = 0xFE60`) + white. Edit `include/config.h`.
2. **Crest**: Greek temple + olive wreath + tick-mark ring, drawn procedurally in `src/render.cpp::draw_arete_crest()`.

### Drop-in palette swap

Pick your school's two-color identity (deep + accent) and convert RGB888 → RGB565:

```python
# Helper: python3
def rgb565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

print(hex(rgb565(0x10, 0x2C, 0x70)))  # 0x1186 (NYCU navy)
print(hex(rgb565(0xFB, 0xCC, 0x05)))  # 0xFE60 (gold)
```

### Replace the crest

The crest is composed of three procedural shapes. Pick what makes sense for your institution:

- **Coat of arms?** Replace `draw_temple_large()` with your shield shape.
- **Logo letterform?** Replace it with a stylized first letter of your school.
- **Mascot silhouette?** Draw your animal/object instead of the temple.
- **Keep wreath, change center?** Olive wreaths read as universal "academic excellence" — a safe baseline.

The crest functions are organized so you can swap one at a time:

```cpp
static void draw_<your-school>_crest(M5Canvas &c, int cx, int cy, uint16_t col) {
    // outer ring (keep or replace)
    c.drawCircle(cx, cy, 32, col);
    // your central emblem
    your_emblem(c, cx, cy, col);
    // wreath (keep or replace)
    draw_olive_wreath(c, cx, cy, 38, col);
}
```

Then update `render_splash()` and the radar unity easter egg to call your function instead.

---

## Step 3 — Translate UI strings

The current build mixes English (mode names like "Story", "Radar") with Traditional Chinese (titles like 「百川 115」, 「離畢業典禮」). To translate:

1. Search the codebase for hardcoded Chinese strings: `grep -rn "百川\|離畢業\|連線\|恭賀畢業" src/ include/`
2. Replace with your language's equivalents.
3. Verify width fits the 135 px LCD using `c.textWidth()` semantics. CJK glyphs at `efontTW_16` are 16 px wide each; ASCII at the same font is 8 px wide.
4. For non-CJK languages, you can use Latin-only fonts like `fonts::FreeSans12pt7b` from M5GFX — adjust width calculations.

A i18n-aware string table is on the Phase 4 roadmap.

---

## Step 4 — Build & flash

```bash
pio run -e m5sticks3
# enter download mode: long-press side button ~2 s, release on green LED blink
pio run -t upload -e m5sticks3
```

If the device fails to enter download mode, see [`hardware-guide.md`](hardware-guide.md).

---

## Step 5 — Share back (optional, encouraged)

Open a PR adding your cohort's directory under `cohorts/<your-cohort>/` and (if appropriate) your theme under `themes/<your-theme>/`. Even an empty directory with a README pointing to your fork helps signal the community is using this.

---

## Where Phase 1 will help

Once the theme/cohort abstraction lands, the steps above collapse to:

```bash
cp themes/_template themes/my-school
cp cohorts/_template cohorts/my-class
$EDITOR themes/my-school/theme.cpp     # palette + crest only
$EDITOR cohorts/my-class/personalization.h  # student data
pio run -e my-school__my-class -t upload
```

No more touching `src/render.cpp`. Watch the [Roadmap section in the main README](../README.md#roadmap).
