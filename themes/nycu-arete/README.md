# Theme: nycu-arete

Reference theme for the **NYCU Arete Honors Program (百川學士學位學程)** — National Yang Ming Chiao Tung University, Taiwan.

## Visual identity

| Element | Value |
|---|---|
| Primary | navy `#102C70` (RGB565 `0x1186`) |
| Accent | gold `#FBCC05` (RGB565 `0xFE60`) |
| Surface white | `#FFFFFF` |
| Crest | Greek temple (Athenian Parthenon motif) + olive wreath + 60-tick clock-ring |

The crest is rendered procedurally in `src/render.cpp::draw_arete_crest()` (Phase 0). Phase 1 will move it here as `theme.cpp::draw_crest()`.

## Trademark

The 百川 / Arete crest visual identity is the intellectual property of National Yang Ming Chiao Tung University. This rendering is a code interpretation used for educational, non-commercial graduation-memento purposes for NYCU cohorts. Forks targeting other institutions **must replace this theme**.

## Inspirations

- Athenian Parthenon (Athena = goddess of wisdom = patron of Arete)
- Olive wreath of classical Greece (laurel of excellence)
- Clock-ring tick marks symbolizing "100 streams flow into the sea over time"
