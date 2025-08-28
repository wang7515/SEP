import argparse, csv

def parse_spec(path):
    with open(path, 'r', newline='') as f:
        head = f.readline().strip()
        x,y,z,px,py,pz = map(int, head.split(','))

        tag2lab = {}
        while True:
            pos = f.tell()
            line = f.readline()
            if not line:
                break
            line = line.rstrip('\n')
            if line == '':
                break
            if ',' not in line:
                f.seek(pos); break
            t, lab = [p.strip() for p in line.split(',', 1)]
            if len(t) != 1:
                raise SystemExit(f"Bad tag: {t}")
            tag2lab[t] = lab

        vox = [[[None for _ in range(x)] for _ in range(y)] for _ in range(z)]
        for zz in range(z):
            for yy in range(y):
                row = f.readline().rstrip('\n')
                if len(row) != x:
                    raise SystemExit(f"Row len {len(row)} != x {x}")
                for xx,ch in enumerate(row):
                    vox[zz][yy][xx] = tag2lab.get(ch, ch)

            p = f.tell()
            sep = f.readline()
            if sep and sep.strip()!='':
                f.seek(p)

    return (x,y,z,px,py,pz), vox

def validate(spec, vox, out_path):
    x,y,z,px,py,pz = spec
    visited = [[[False]*x for _ in range(y)] for _ in range(z)]
    blocks = 0

    with open(out_path, 'r', newline='') as f:
        rdr = csv.reader(f)
        for row in rdr:
            blocks += 1
            X,Y,Z,DX,DY,DZ = map(int, row[:6])
            LAB = row[6]

            if (X//px)!=((X+DX-1)//px) or (Y//py)!=((Y+DY-1)//py) or (Z//pz)!=((Z+DZ-1)//pz):
                raise SystemExit(f"[INVALID] crosses parent boundary: {row}")

            for zz in range(Z, Z+DZ):
                for yy in range(Y, Y+DY):
                    for xx in range(X, X+DX):
                        if not (0<=xx<x and 0<=yy<y and 0<=zz<z):
                            raise SystemExit(f"[INVALID] out of bounds: {row}")
                        if visited[zz][yy][xx]:
                            raise SystemExit(f"[INVALID] overlap at {(xx,yy,zz)} by {row}")
                        if vox[zz][yy][xx] != LAB:
                            raise SystemExit(
                                f"[INVALID] label mismatch at {(xx,yy,zz)}: "
                                f"expected {vox[zz][yy][xx]}, got {LAB}"
                            )
                        visited[zz][yy][xx] = True

    for zz in range(z):
        for yy in range(y):
            for xx in range(x):
                if not visited[zz][yy][xx]:
                    raise SystemExit(f"[INVALID] uncovered voxel at {(xx,yy,zz)}")

    total = x*y*z
    comp = (1 - blocks/total) * 100.0
    print(f"[OK] blocks={blocks}/{total}  Compression={comp:.4f}%")

if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--input", required=True)
    ap.add_argument("--output", required=True)
    args = ap.parse_args()
    spec, vox = parse_spec(args.input)
    validate(spec, vox, args.output)
