# Cohorts

A "cohort" is a graduating class — typically a single year, single program. Each cohort has its own student data (names, IDs, photos, stories, future-self letters).

## Privacy first

**Real student data must NEVER be committed to a public repository.** Patterns enforced in `.gitignore`:

```gitignore
cohorts/*/personalization.h
cohorts/*/roster.csv
cohorts/*/avatars/
cohorts/*/voice/
```

Each cohort directory should contain only:

- `README.md` — describing the cohort (anonymously OK)
- `personalization.h.example` — anonymized template showing the data structure
- `theme.config` (Phase 1) — pointer to which `themes/<x>/` to use

The actual `personalization.h` is generated locally by Phase 1 tools from a private CSV/Sheet.

## Existing cohorts

| Cohort | Theme | Ceremony | Status |
|---|---|---|---|
| `nycu-arete-115/` | `nycu-arete` | 2026-06-13 | Phase 0 demo done; ~40 devices targeted for ceremony day |

PRs welcome to register your cohort here.

## Adding a cohort

See [`docs/creating-a-cohort.md`](../docs/creating-a-cohort.md) (forthcoming) and copy [`_template/`](_template/) to `cohorts/<your-cohort>/`.
