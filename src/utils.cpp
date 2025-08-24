#include "utils.h"
#include <sstream>

utils::utils(){}
// Split a comma-separated string into 6 integers.
// Store them in array 'a'. Return true if success, false otherwise.
static bool split6(const std::string &s, int a[6]) {
    std::stringstream ss(s);
    std::string item; int i=0;
    while (std::getline(ss, item, ',')) {
        if (item.size()==0) return false;
        size_t l=item.find_first_not_of(" \t\r\n");
        size_t r=item.find_last_not_of(" \t\r\n");
        if (l==std::string::npos) return false;
        std::string t=item.substr(l,r-l+1);
        try { a[i]=std::stoi(t); } catch(...) { return false; }
        if (++i==6) break;
    }
    return i==6;
}
// Parse the header line "x,y,z,px,py,pz" into 6 integers.
// Assign values to x, y, z, px, py, pz. Return true if valid.
bool utils::parseHeader(const string &s, int &x, int &y, int &z, int &px, int &py, int &pz){
    int a[6]; if(!split6(s,a)) return false;
    x=a[0]; y=a[1]; z=a[2]; px=a[3]; py=a[4]; pz=a[5];
    return true;
}
