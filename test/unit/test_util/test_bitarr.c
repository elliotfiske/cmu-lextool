/**
 * @file test_bitarr.c Test bit array io
 */

#include "bitarr.h"
#include "test_macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char *argv[])
{
    float_enc neg1 = { -1.0 }, pos1 = { 1.0 };
    uint64 test57 = 0x123456789abcdefULL;
    char mem[57+8];
    bitarr_adress_t adress;
    if ((neg1.i ^ pos1.i) != 0x80000000) {
        E_ERROR("Sign bit is not 0x80000000\n");
    }
    memset(mem, 0, sizeof(mem));
    adress.base = mem;
    for (adress.offset = 0; adress.offset < 57 * 8; adress.offset += 57) {
      bitarr_write_int57(adress, 57, test57);
    }
    for (adress.offset = 0; adress.offset < 57 * 8; adress.offset += 57) {
      TEST_EQUAL(test57, bitarr_read_int57(adress, 57, ((1ULL << 57) - 1)));
    }
    // TODO: more checks.
    return 0;
}
