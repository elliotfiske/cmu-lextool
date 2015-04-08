
#include <sphinxbase/err.h>

#include "lm_trie_bits.h"

uint8 required_bits(uint64 max_value)
{
    uint8 res;

    if (!max_value) return 0;
    res = 1;
    while (max_value >>= 1) res++;
    return res;
}

void bit_mask_from_max(bit_mask_t *bit_mask, uint64 max_value)
{
    bit_mask->bits = required_bits(max_value);
    bit_mask->mask = (1ULL << bit_mask->bits) - 1;
}

void bit_packing_sanity() {
  const float_enc neg1 = { -1.0 }, pos1 = { 1.0 };
  const uint64 test57 = 0x123456789abcdefULL;
  char mem[57+8];
  uint64 b;
  if ((neg1.i ^ pos1.i) != 0x80000000) {
      E_ERROR("Sign bit is not 0x80000000\n");
  }
  memset(mem, 0, sizeof(mem));
  for (b = 0; b < 57 * 8; b += 57) {
    write_int57(mem, b, 57, test57);
  }
  for (b = 0; b < 57 * 8; b += 57) {
    if (test57 != read_int57(mem, b, 57, (1ULL << 57) - 1))
      E_ERROR("The bit packing routines are failing for your architecture.  Please send a bug report with your architecture, operating system, and compiler\n");
  }
  // TODO: more checks.  
}