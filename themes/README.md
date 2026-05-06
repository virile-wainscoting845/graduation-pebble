# Themes

A "theme" defines the **visual identity** of a graduation-pebble instance: its color palette, crest geometry, and any institution-specific iconography.

## Phase 0 status

In Phase 0 the theme is hardcoded in `src/render.cpp` (`draw_arete_crest`, palette in `include/config.h`). The `nycu-arete/` directory below is a **placeholder for documentation**; the theme abstraction (a swappable `theme.cpp` per directory) ships in Phase 1.

## Adding a theme

See [`docs/creating-a-theme.md`](../docs/creating-a-theme.md) (Phase 1) and the existing reference [`nycu-arete/`](nycu-arete/).

## Existing themes

| Theme | Institution | Status |
|---|---|---|
| `nycu-arete/` | NYCU Arete Honors Program (百川學士學位學程), Taiwan | reference (hardcoded into `src/render.cpp`) |

Want yours listed? PR welcome.
