#include "sbk.h"

int main()
{
    SBK s;
    s.filename = "private.pem";
    s.outputname = "key.sbk";
    generate_key(&s);
    generate_hash(&s);
    generate_seeds(&s);
    generate_rands(&s);
    key_output(&s);
    read_key(&s);
    return 0;
}
