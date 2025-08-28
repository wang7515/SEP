#include "compressionContext.h"
#include <algorithm>

void compressionContext::configure(int X_, int Y_, int PX_, int PY_, int PZ_){
    X=X_; Y=Y_; PX=PX_; PY=PY_; PZ=PZ_;
    tilesX = (X + PX - 1) / PX;
    xyTables.assign(tilesX, {}); 
    curTileY = -1;
    activeZ.clear(); seenZ.clear();
}

void compressionContext::startSlice(int z){
    curTileY = -1;
    for (auto &m : xyTables) m.clear();
    seenZ.clear();
}

void compressionContext::endSlice(int z, const EmitFn& emit){
    flushXYBandToZ(z, emit);

    std::vector<ZKey> toFlush;
    toFlush.reserve(activeZ.size());
    for (auto &kv : activeZ){
        const ZKey& k = kv.first;
        if (!seenZ.count(k)) {           
            toFlush.push_back(k);
        }
    }
    for (auto &k : toFlush){
        const Prism &P = activeZ[k];
        emit(P.x, P.y, P.z0, P.dx, P.dy, P.dz, P.lab);
        activeZ.erase(k);
    }
    seenZ.clear();
}

void compressionContext::endZBandIfNeeded(int z, const EmitFn& emit){
    if ((z+1) % PZ == 0){                
        for (auto &kv : activeZ){
            const Prism &P = kv.second;
            emit(P.x, P.y, P.z0, P.dx, P.dy, P.dz, P.lab);
        }
        activeZ.clear();
    }
}

void compressionContext::finish(const EmitFn& emit){
    for (auto &kv : activeZ){
        const Prism &P = kv.second;
        emit(P.x, P.y, P.z0, P.dx, P.dy, P.dz, P.lab);
    }
    activeZ.clear();
}

void compressionContext::flushXYBandToZ(int z, const EmitFn& emit){
    for (auto &tbl : xyTables){
        for (auto &kv : tbl){
            const XYKey &k = kv.first;
            const ActiveRect &ar = kv.second;
            Rect R{ k.x, ar.y0, k.dx, ar.dy, k.lab };
            feedRectToZ(z, R, emit);     
        }
        tbl.clear();
    }
}

void compressionContext::feedRectToZ(int z, const Rect& R, const EmitFn& emit){
    ZKey k{ R.x, R.y, R.dx, R.dy, R.lab };
    auto it = activeZ.find(k);
    if (it != activeZ.end() && (it->second.z0 + it->second.dz == z)){
        it->second.dz += 1;
    }else{
        if (it != activeZ.end()){
            const Prism &P = it->second;
            emit(P.x, P.y, P.z0, P.dx, P.dy, P.dz, P.lab);
            activeZ.erase(it);
        }
        activeZ.emplace(k, Prism{ R.x, R.y, R.dx, R.dy, z, 1, R.lab });
    }
    seenZ.insert(k);
}

void compressionContext::processRow(
    int z, int y, const std::string& row,
    const LabelFn& labelOf, const EmitFn& emit)
{
    int tileY = y / PY;

    if (curTileY != -1 && tileY != curTileY){
        flushXYBandToZ(z, emit);
    }
    if (tileY != curTileY){
        for (auto &m : xyTables) m.clear();
        curTileY = tileY;
    }

    for (int ox = 0, tx = 0; ox < X; ox += PX, ++tx){
        int xmax = std::min(ox + PX, X);
        auto &table = xyTables[tx];
        std::unordered_set<XYKey,XYKeyHash> seen;

        int i = ox;
        while (i < xmax){
            char ch = row[i];
            int j = i + 1;
            while (j < xmax && row[j] == ch) ++j;     
            std::string lab = labelOf(ch);
            XYKey key{ i, j - i, lab };

            auto it = table.find(key);
            if (it == table.end()){
                table.emplace(key, ActiveRect{ y, 1 });   
            }else{
                it->second.dy += 1;                       
            }
            seen.insert(key);
            i = j;
        }

        for (auto it = table.begin(); it != table.end(); ){
            if (!seen.count(it->first)){
                const XYKey &k = it->first;
                const ActiveRect &ar = it->second;
                Rect R{ k.x, ar.y0, k.dx, ar.dy, k.lab };
                feedRectToZ(z, R, emit);
                it = table.erase(it);
            }else{
                ++it;
            }
        }
    }

    if (((y + 1) % PY) == 0){
        flushXYBandToZ(z, emit);
        curTileY = -1;
    }
}
