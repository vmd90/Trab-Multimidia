
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

char power(char b, char e)
{
    char p = 1;
    for (char i = 0; i < e; ++i) {
        p *= b;
    }
    return p;
}
