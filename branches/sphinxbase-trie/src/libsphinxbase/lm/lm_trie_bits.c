
#include <sphinxbase/err.h>

#include "lm_trie_bits.h"

uint8 required_bits(uint32 max_value)
{
    uint8 res;

    if (!max_value) return 0;
    res = 1;
    while (max_value >>= 1) res++;
    return res;
}

void bit_mask_from_max(bit_mask_t *bit_mask, uint32 max_value)
{
    bit_mask->bits = required_bits(max_value);
    bit_mask->mask = (1U << bit_mask->bits) - 1;
}

void bit_packing_sanity() {
  const float_enc neg1 = { -1.0 }, pos1 = { 1.0 };
  const uint32 test31 = 0x1234567U;
  char mem[32+8];
  uint32 b;
  if ((neg1.i ^ pos1.i) != 0x80000000) {
      E_ERROR("Sign bit is not 0x80000000\n");
  }
  memset(mem, 0, sizeof(mem));
  for (b = 0; b < 32 * 8; b += 32) {
    write_int31(mem, b, 32, test31);
  }
  for (b = 0; b < 32 * 8; b += 32) {
    if (test31 != read_int31(mem, b, 32, ((1ULL << 32) - 1)))
      E_ERROR("The bit packing routines are failing for your architecture.  Please send a bug report with your architecture, operating system, and compiler\n");
  }
  // TODO: more checks.  
}