# Phase 1 tooling (work in progress)

Python utilities for batch-flashing many devices, each with its own student personalization data.

## Planned tools

| Script | Purpose | Status |
|---|---|---|
| `pack_personalization.py` | Read a CSV roster + per-student photo / audio assets, generate one `cohorts/<cohort>/personalization.h` per student. | stub |
| `batch_flash.py` | Walk a directory of generated personalization headers, sequentially flash N devices, log device-serial ↔ student-id pairings. | stub |
| `ascii_avatar.py` | Convert a JPG/PNG photo into a 1-bit dithered 48×48 ASCII-style bitmap embeddable in firmware. | stub |
| `verify_assets.py` | Validate the data collected from `graduation-pebble-form` (size limits, format, charset, length caps). | stub |

## Privacy

Generated artifacts (`build/`, `output/`, real `personalization.h`, real photos and audio) are gitignored. Never commit student PII.

## Quickstart (once tools land)

```bash
# 1. Drop your private roster CSV (from graduation-pebble-form's exported sheet)
mv ~/Downloads/cohort_roster.csv tools/input/

# 2. Run the packer
python tools/pack_personalization.py \
    --roster tools/input/cohort_roster.csv \
    --avatars tools/input/avatars/ \
    --cohort cohorts/your-cohort/ \
    --out tools/output/

# 3. For each device: enter download mode, run batch_flash.py
python tools/batch_flash.py --in tools/output/ --port COM9
```
