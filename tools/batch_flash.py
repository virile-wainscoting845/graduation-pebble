#!/usr/bin/env python3
"""Sequentially flash N M5StickS3 devices, each with its own personalization.h.

PHASE 1 — STUB. Implementation pending.

Workflow:
    1. Operator enters M5StickS3 download mode (long-press PWR ~2 sec).
    2. Tool detects newly-attached USB device with VID 0x303A PID 0x1001.
    3. Picks the next unprocessed personalization.h from --in directory.
    4. Generates a per-build override of include/demo_data.h, runs `pio run -t upload`.
    5. Logs device serial -> student_id pairing into roster_built.csv.
    6. Operator unplugs device; loop to next.

Usage:
    python batch_flash.py --in tools/output/ --pio-env m5sticks3
"""
import argparse
import sys


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--in", dest="indir", required=True, help="Directory of personalization.h files")
    parser.add_argument("--pio-env", default="m5sticks3", help="PlatformIO env name")
    args = parser.parse_args()

    print("[stub] batch_flash.py is not yet implemented.", file=sys.stderr)
    print(f"  in     : {args.indir}", file=sys.stderr)
    print(f"  pio-env: {args.pio_env}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    sys.exit(main())
