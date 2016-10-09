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
    encode(&p);
    print_encoded_map(&p);
    print_decoded_map(&p);

    return 0;
}
