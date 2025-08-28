import argparse, os, sys, time, subprocess, statistics
from typing import Tuple

# Parse first line of input: expect x,y,z,px,py,pz
def parse_dims(inp: str) -> Tuple[int,int,int,int,int,int]:
    """Read first line: x,y,z,px,py,pz"""
    with open(inp, "r", encoding="utf-8", newline="") as f:
        header = f.readline()
    parts = [p.strip() for p in header.strip().split(",")]
    if len(parts) != 6:
        raise SystemExit(f"[{inp}] bad header")
    return tuple(map(int, parts)) 

# Convert raw bytes into "B","KB","MB","GB","TB"
def pretty_bytes(n: int) -> str:
    for u in ["B","KB","MB","GB","TB"]:
        if n < 1024 or u == "TB":
            return f"{n:.1f}{u}"
        n /= 1024.0
# Run compressor once, discard stdout, return elapsed time
def run_once(bin_path: str, inp: str) -> float:
    with open(inp, "rb") as fin:
        t0 = time.perf_counter()
        cp = subprocess.run([bin_path], stdin=fin, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
        t1 = time.perf_counter()
    if cp.returncode != 0:
        sys.stderr.write(cp.stderr.decode("utf-8", errors="ignore"))
        raise SystemExit(f"compressor exited with code {cp.returncode}")
    return t1 - t0

# Benchmark one input file with multiple runs and report summary
def bench_one(bin_path: str, inp: str, repeats: int):
    x,y,z,px,py,pz = parse_dims(inp)
    voxels = x*y*z
    size_b = os.path.getsize(inp)
    print(f"\n=== {os.path.basename(inp)} ===")
    print(f"dims: x={x}, y={y}, z={z}; parent=({px},{py},{pz}) | voxels={voxels:,} | input={pretty_bytes(size_b)}")

    times = []
    for i in range(repeats):
        dt = run_once(bin_path, inp)
        times.append(dt)
        print(f"  run {i+1:02d}: {dt:.6f} s")

    best = min(times)
    med  = statistics.median(times)
    mean = statistics.mean(times)
    std  = statistics.pstdev(times) if len(times) > 1 else 0.0

    vps  = voxels / best if best > 0 else float("inf")
    mbps = (size_b / best) / (1024*1024) if best > 0 else float("inf")

    print("\n--- Summary (stdout discarded) ---")
    print(f"time  best/median/mean/std : {best:.6f} / {med:.6f} / {mean:.6f} / {std:.6f}  (s)")
    print(f"throughput (voxels/sec)    : {vps:,.0f}")
    print(f"input rate (MB/sec)        : {mbps:,.2f}")

def main():
    # Create a command-line argument parser
    ap = argparse.ArgumentParser(description="Speed benchmark (stdout -> /dev/null).")
    ap.add_argument("bin", help="path to compressor executable, e.g. ./semicolons")
    ap.add_argument("inputs", nargs="+", help="one or more TITAN input files")
    ap.add_argument("--repeats", type=int, default=5, help="runs per input (default: 5)")
    args = ap.parse_args()

    if not os.path.exists(args.bin):
        raise SystemExit(f"executable not found: {args.bin}")
    for p in args.inputs:
        if not os.path.exists(p):
            raise SystemExit(f"input not found: {p}")

    for inp in args.inputs:
        bench_one(args.bin, inp, args.repeats)

if __name__ == "__main__":
    main()
