#ifndef COMPRESSIONCONTEXT_H
#define COMPRESSIONCONTEXT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>

struct Rect { int x,y,dx,dy; std::string lab; };
struct Prism{ int x,y,dx,dy,z0,dz; std::string lab; };

struct XYKey{
    int x, dx;
    std::string lab;
    bool operator==(const XYKey& o) const { return x==o.x && dx==o.dx && lab==o.lab; }
};
struct XYKeyHash{
    size_t operator()(const XYKey& k) const {
        return ((std::hash<int>()(k.x)*131u + std::hash<int>()(k.dx)) * 131u)
               ^ std::hash<std::string>()(k.lab);
    }
};
struct ActiveRect{ int y0, dy; };

struct ZKey{
    int x,y,dx,dy;
    std::string lab;
    bool operator==(const ZKey& o) const {
        return x==o.x && y==o.y && dx==o.dx && dy==o.dy && lab==o.lab;
    }
};
struct ZKeyHash{
    size_t operator()(const ZKey& k) const {
        size_t h = std::hash<int>()(k.x);
        h = h*131u + std::hash<int>()(k.y);
        h = h*131u + std::hash<int>()(k.dx);
        h = h*131u + std::hash<int>()(k.dy);
        return (h*131u) ^ std::hash<std::string>()(k.lab);
    }
};

class compressionContext{
public:
    compressionContext() = default;

    using EmitFn = std::function<void(int,int,int,int,int,int,const std::string&)>;
    using LabelFn = std::function<std::string(char)>;

    void configure(int X, int Y, int PX, int PY, int PZ);

    void startSlice(int z);
    void endSlice(int z, const EmitFn& emit);

    void processRow(int z, int y, const std::string& row,
                    const LabelFn& labelOf, const EmitFn& emit);

    void endZBandIfNeeded(int z, const EmitFn& emit);
    void finish(const EmitFn& emit);

private:
    int X=0, Y=0, PX=1, PY=1, PZ=1;
    int tilesX=0;
    int curTileY=-1;
    std::vector<std::unordered_map<XYKey,ActiveRect,XYKeyHash>> xyTables;
    std::unordered_map<ZKey,Prism,ZKeyHash> activeZ;
    std::unordered_set<ZKey,ZKeyHash> seenZ;


    void flushXYBandToZ(int z, const EmitFn& emit);
    void feedRectToZ(int z, const Rect& R, const EmitFn& emit);
};

#endif
