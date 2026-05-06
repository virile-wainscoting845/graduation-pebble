# Inspirations

`graduation-pebble` is a remix of multiple lineages.

## Anthropic Claude Code Buddy (April 2026)

In April 2026, Anthropic shipped an Easter-egg feature in Claude Code (npm `@anthropic-ai/claude-code`) called **Buddy** — a virtual pet that lives in your terminal. The full source for Buddy was inadvertently exposed when version 2.1.88 shipped a `.map` file containing the entire `src/buddy/` directory. The pet system has 18 species, 5 rarities, and 5 stats; the renderer uses 5×12 ASCII art with 3 idle frames per species.

The owl species (internal ID `IG8`) and the axolotl species (`UG8`) directly inspired this project's mascot. The original ASCII frames are preserved as comments in the codebase for reference, but the on-device renderer is independently implemented using procedural primitives — ASCII art reads poorly on a 1.14" 135×240 LCD at arm's length.

## Tamagotchi (1996, Bandai)

The "always-on, occasionally needy" interaction loop is pure Tamagotchi. Even though we mostly hide that fact behind a polished graduation-memorial UI, the underlying state machine — fullness, happiness, energy, cleanliness, health, sleep cycles — is a direct descendant of the original.

## Athena's Owl

The Greek goddess Athena (Ἀθηνᾶ) is the patron of wisdom, weaving, and warfare strategy. Her sacred bird is the little owl (Athene noctua). Greek coinage from Athens features the owl on the reverse side; the obverse is Athena's helmeted head. The graduation Pebble's Greek-temple-and-olive-wreath crest, paired with an owl mascot, is a direct visual line back to that classical iconography.

The word **Arete (ἀρετή)** in classical Greek means "excellence", "virtue", or "fulfilling one's potential". The NYCU 百川 program took its English name from this concept — but Arete itself is not NYCU's; it's Hellenic and shared with anyone who graduates.

## 百川 / Hundred Rivers

A Chinese idiom: 百川入海 (*bǎi chuān rù hǎi*) — "a hundred rivers flow into the sea". Each river takes a unique route, but together they become one. NYCU's Arete Honors Program adopted this metaphor for its interdisciplinary cohort: every student picks their own combination of subjects, and graduation is when "the river meets the sea". The radar mode's cohort-discovery animation, and the Unity easter egg ("百川合一" — *the hundred rivers become one*), are direct expressions of this idiom.

The pebble itself — a small stone in a riverbed — completes the metaphor. Each pebble is unique in shape, smoothed by its own particular journey downstream.

## M5Stack

This project would not exist without the M5Stack ecosystem: the M5StickS3 K150 hardware kit, the M5Unified library, M5GFX with its bundled CJK fonts, and the LovyanGFX renderer underneath. M5Stack's commitment to making ESP32-family devices accessible to hobbyists made this 38-day build possible.

## 1.14-inch LCD design language

The very specific constraints of a 1.14" 135×240 LCD pushed many design decisions:
- Procedural drawing over bitmap sprites (so we can scale/style without re-encoding)
- Full-screen sprite in PSRAM for tear-free rendering
- ~30 fps target (`delay(33)`) — Tamagotchi-era refresh, modern smoothness
- Big CJK glyphs (size 2 = 32 px tall) for readability at arm's length
- Procedural countdown digits (size 3 ASCII = 24 px tall)

## Acknowledgements

- The graduating cohort of NYCU 百川 115 級, who will field-test the first 40 devices.
- The system 系上 of NYCU Arete Honors Program for sponsoring the hardware.
- Everyone who tolerated the 7-iteration UI redesign in the first session.
