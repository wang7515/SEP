#include "compressorController.h"

compressorController::compressorController(){}

bool compressorController::readline(string &out){
    if (hasPush){ out=pushLine; hasPush=false; return true; }
    if (!std::getline(std::cin, out)) return false;
    if (!out.empty() && out.back()=='\r') out.pop_back();
    return true;
}
void compressorController::unread(const string &s){ pushLine=s; hasPush=true; }
void compressorController::takeInput(){
    utils u; string line;

    if(!readline(line)){ std::cerr<<"empty input\n"; std::exit(1); }
    if(!u.parseHeader(line, x_count,y_count,z_count, px,py,pz)){
        std::cerr<<"bad header: need 6 comma-separated integers\n"; std::exit(1);
    }
    ctx.configure(x_count, y_count, px, py, pz); 

    tag_to_label.clear();
    auto trim=[](string &t){
        size_t l=t.find_first_not_of(" \t\r\n");
        size_t r=t.find_last_not_of(" \t\r\n");
        if (l==string::npos){ t.clear(); return; }
        t=t.substr(l, r-l+1);
    };
    while (true){
        if(!readline(line)) break;
        string s=line; trim(s);
        if (s.empty()) break;
        auto pos=s.find(',');
        if (pos==string::npos){ unread(line); break; }
        string tag=s.substr(0,pos), lab=s.substr(pos+1);
        trim(tag); trim(lab);
        if (tag.size()!=1){
            std::cerr<<"tag must be a single character: "<<tag<<"\n"; std::exit(1);
        }
        tag_to_label[tag[0]]=lab;
    }
}

void compressorController::runCompressor(){
    string line;
    auto emit = [&](int x,int y,int z,int dx,int dy,int dz,const std::string& lab){
        std::cout<<x<<","<<y<<","<<z<<","<<dx<<","<<dy<<","<<dz<<","<<lab<<"\n";
    };
    auto labelOfFn = [&](char ch)->std::string { return labelOf(ch); };

    for (int z=0; z<z_count; ++z){
        ctx.startSlice(z);
        for (int y=0; y<y_count; ++y){
            do{
                if(!readline(line)){
                    std::cerr<<"unexpected EOF in slice data at z="<<z<<", y="<<y<<"\n";
                    std::exit(1);
                }
            } while(line.empty());
            if ((int)line.size() != x_count){
                std::cerr<<"row length "<<line.size()<<" != x_count "<<x_count<<"\n";
                std::exit(1);
            }

            ctx.processRow(z, y, line, labelOfFn, emit);
        }

        ctx.endSlice(z, emit);
        ctx.endZBandIfNeeded(z, emit);
        if (readline(line)){ if (!line.empty()) unread(line); }
    }
}

void compressorController::writeOutput(){
    auto emit = [&](int x,int y,int z,int dx,int dy,int dz,const std::string& lab){
        std::cout<<x<<","<<y<<","<<z<<","<<dx<<","<<dy<<","<<dz<<","<<lab<<"\n";
    };
    ctx.finish(emit);
}


