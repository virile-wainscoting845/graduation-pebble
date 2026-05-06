# Theme template

Copy this directory to `themes/<your-school>/` and fill in:

1. **Palette**: pick 3 colors (deep / mid / light) plus a contrasting accent.
2. **Crest**: a procedural drawing of your school's emblem.
3. **Trademark notes**: who owns the original, what your fork does, what others should do.

In Phase 0 (current), the actual rendering is hardcoded in `src/render.cpp` — this directory is documentation-only. Phase 1 will introduce a swappable `theme.cpp`.

## Recommended palette format (will become `theme.h`)

```cpp
constexpr uint16_t COL_PRIMARY    = 0xXXXX;
constexpr uint16_t COL_ACCENT     = 0xXXXX;
constexpr uint16_t COL_SURFACE    = 0xFFFF;
constexpr uint16_t COL_DEEP       = 0xXXXX;
```

## Recommended crest function shape (will become `theme.cpp`)

```cpp
void draw_crest(M5Canvas &c, int cx, int cy, uint16_t col) {
    // outer ring (universal academic motif — keep or replace)
    c.drawCircle(cx, cy, 32, col);
    // your central emblem
    your_emblem(c, cx, cy, col);
    // wreath / decoration
    your_decoration(c, cx, cy, col);
}
```
