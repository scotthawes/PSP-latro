#!/usr/bin/env python3
"""Compare two screenshots and emit a diff image.

Exits 0 when difference <= threshold, non-zero otherwise.
"""
import argparse
import sys
from PIL import Image, ImageChops, ImageStat, ImageEnhance


def parse_args():
    p = argparse.ArgumentParser(description='Compare expected vs actual screenshots')
    p.add_argument('--expected', '-e', required=True)
    p.add_argument('--actual', '-a', required=True)
    p.add_argument('--out', '-o', required=True)
    p.add_argument('--threshold', '-t', type=float, default=0.02,
                   help='normalized RMS threshold (0..1)')
    return p.parse_args()


def main():
    args = parse_args()
    exp = Image.open(args.expected).convert('RGBA')
    act = Image.open(args.actual).convert('RGBA')

    if exp.size != act.size:
        act = act.resize(exp.size, Image.BILINEAR)

    diff = ImageChops.difference(exp, act)
    stat = ImageStat.Stat(diff)

    # mean RMS across channels (PIL's ``rms`` is per-channel)
    mean_rms = sum(stat.rms) / len(stat.rms)
    norm_rms = mean_rms / 255.0

    # create a visually amplified diff for humans
    vis = diff.convert('RGB')
    vis = ImageEnhance.Brightness(vis).enhance(4.0)
    vis.save(args.out)

    print(f'Normalized RMS: {norm_rms:.6f}')
    if norm_rms <= args.threshold:
        print('PASS')
        return 0
    else:
        print('FAIL')
        return 2


if __name__ == '__main__':
    raise SystemExit(main())
