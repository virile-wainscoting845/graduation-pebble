# Cohort: nycu-arete-115

Reference cohort: **NYCU Arete Honors Program (百川學士學位學程) 115th cohort**, graduating 2026-06-13 in Hsinchu, Taiwan.

| Field | Value |
|---|---|
| Institution | National Yang Ming Chiao Tung University (NYCU) |
| Program | 百川學士學位學程 (Arete Honors Program) |
| Cohort number | 115 (Republic of China year 115 = 2026) |
| Ceremony | 2026-06-13 |
| Theme | [`themes/nycu-arete/`](../../themes/nycu-arete/) |
| Capsule unlock | 2031-06-13 (5 years post-grad) |
| Approx size | 40 students |

## Phase 0 status

The Phase 0 demo bakes a single student (蔡秀吉 / 110950008) into `include/demo_data.h` for visual verification. This cohort directory currently holds **only documentation**; per-student personalization data is generated locally during Phase 1 batch flashing.

## Phase 1 personalization workflow (planned)

```
[Google Form] -> [Google Sheet] -> [pack_personalization.py]
   ^                                       |
   |                                       v
[student fills]                  [40x personalization.h files]
                                            |
                                            v
                                  [batch_flash.py over USB]
                                            |
                                            v
                                  [40x M5StickS3 ready for ceremony]
```

The form lives at the companion repo: [`graduation-pebble-form`](https://github.com/thc1006/graduation-pebble-form).

## Roster

The 40-student roster is private and gitignored. The cohort identifies itself publicly as "Arete 115" — individual student data on each device is owned by that student.
