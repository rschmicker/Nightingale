#include "sbk.h"
#include "pacc.h"

int main()
{
    // SBK s;
    // s.filename = "private.pem";
    // s.outputname = "key.sbk";
    // generate_key(&s);
    // generate_hash(&s);
    // generate_seeds(&s);
    // generate_rands(&s);
    // transpose(&s);
    // shuffle(&s);
    // write_key(&s);
    // read_key(&s);

    PACC p;
    fill_ascii_to_pacc_map(&p);
    ascii_hex_to_pacc(&p);
    return 0;
}
