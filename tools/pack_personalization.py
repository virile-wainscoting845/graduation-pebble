#!/usr/bin/env python3
"""Generate per-student personalization.h files from a CSV roster.

PHASE 1 — STUB. Implementation pending.

Reads a CSV with columns:
    student_id, name_tw, name_en, enroll_yyyymm, kw1..kw5,
    y1_caption, y2_caption, y3_caption, y4_caption, future_letter

For each row, emits cohorts/<cohort>/build/<student_id>/personalization.h
ready to be #included into the firmware build for that specific device.

Usage:
    python pack_personalization.py \\
        --roster path/to/roster.csv \\
        --avatars path/to/avatars/ \\
        --cohort cohorts/<your-cohort>/ \\
        --out tools/output/
"""
import argparse
import sys


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--roster", required=True, help="CSV roster file")
    parser.add_argument("--avatars", required=True, help="Directory of avatar images")
    parser.add_argument("--cohort", required=True, help="Path to cohorts/<cohort>/")
    parser.add_argument("--out", required=True, help="Output directory")
    args = parser.parse_args()

    print("[stub] pack_personalization.py is not yet implemented.", file=sys.stderr)
    print(f"  roster : {args.roster}", file=sys.stderr)
    print(f"  avatars: {args.avatars}", file=sys.stderr)
    print(f"  cohort : {args.cohort}", file=sys.stderr)
    print(f"  out    : {args.out}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    sys.exit(main())
