
#include <fstream>
#include "utils.hpp"

void copy_file(std::fstream& outf, std::fstream& inf)
{
    while (true) {
        char c;
        inf.read(&c, sizeof (c));
        if(inf.eof()) {
            break;
        }
        outf.write(&c, sizeof (c));
    }
}

unsigned char pow(unsigned char b, unsigned char e)
{
    unsigned char p = 1;
    for (int i = 0; i < e; ++i) {
        p *= b;
    }
    return p;
}
