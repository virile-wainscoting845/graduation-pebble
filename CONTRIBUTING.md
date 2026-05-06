# Contributing to graduation-pebble

Thanks for considering a contribution. This project exists to be forked and adapted by graduating cohorts worldwide, so contributions that help others adopt it are highly valued.

## Ways to contribute

| Contribution | What we want |
|---|---|
| **New theme** | Your institution's visual identity (palette + crest drawing). See `docs/creating-a-theme.md`. |
| **New cohort instance** | Add a `cohorts/<your-cohort>/` directory showcasing how your class personalized the project. Real student data must remain gitignored. |
| **Hardware ports** | M5StickC PLUS / PLUS2, AtomS3, generic ESP32-S3 boards. |
| **Bug fixes** | Especially in render layout, button routing, or NVS persistence. |
| **i18n strings** | If you add a non-Han language (Spanish, German, etc.), add a strings table. |
| **Docs / typos / translations** | README in your language is welcome. |

## Workflow

1. Fork the repo.
2. Branch from `main`: `git checkout -b feat/your-thing`.
3. Build locally: `pio run -e m5sticks3` should succeed cleanly.
4. Commit using plain messages — see "Commit style" below.
5. Open a PR against `main`.

## Commit style

Plain, terse, lowercase prefix:

```
init: graduation-pebble Phase 0 demo
feat: oxford-blue theme
fix: home countdown phase logic when days_diff = 0
docs: clarify download-mode entry on M5StickS3
refactor: extract draw_brand_bar into shared helper
chore: bump platformio platform to 6.13.0
```

**Do not include any AI-generated attribution.** No `Co-Authored-By: Claude`, no `Generated with [tool]` footers, no model signatures. Plain commits only.

## Privacy rules for cohort contributions

If you add a cohort instance:

- **Real student photos, voice clips, or full names of minors** must NOT be committed to the public repo. Use `.gitignore` patterns: `cohorts/*/personalization.h`, `cohorts/*/avatars/`, `cohorts/*/voice/`.
- **Do commit** an anonymized `personalization.h.example` so other cohorts can see the structure.
- Get explicit consent from each student before recording their voice / image into a Pebble — even though the data ends up only on the device's flash, your batch-flash tool will handle real personal data temporarily.

## Code of conduct

Be kind. This project is built by graduating students for graduating students; assume good intent.
