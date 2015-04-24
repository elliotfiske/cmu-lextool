#ifndef __LM_TRIE_BITS_H__
#define __LM_TRIE_BITS_H__

/* Bit-level packing routines 
 *
 * WARNING WARNING WARNING:
 * The write functions assume that memory is zero initially.  This makes them
 * faster and is the appropriate case for mmapped language model construction.
 * These routines assume that unaligned access to uint64_t is fast.  This is
 * the case on x86_64.  I'm not sure how fast unaligned 64-bit access is on
 * x86 but my target audience is large language models for which 64-bit is
 * necessary.  
 *
 * Call the BitPackingSanity function to sanity check.  Calling once suffices,
 * but it may be called multiple times when that's inconvenient.  
 *
 * ARM and MinGW ports contributed by Hideo Okuma and Tomoyuki Yoshimura at
 * NICT.
 */

#include <assert.h>
#include <string.h>
#include <stdint.h>

#ifdef __APPLE__
#include <architecture/byte_order.h>
#elif __linux__
#include <endian.h>
#elif !defined(_WIN32) && !defined(_WIN64)
#include <arpa/nameser_compat.h>
#endif 

#include <sphinxbase/prim_type.h>

#include "lm_trie_misc.h"

typedef union { float f; uint32 i; } float_enc;

// Fun fact: __BYTE_ORDER is wrong on Solaris Sparc, but the version without __ is correct.
__inline static uint8 bit_pack_shift(uint8 bit, uint8 length) 
{
#if BYTE_ORDER == LITTLE_ENDIAN
  return bit;
#elif BYTE_ORDER == BIG_ENDIAN
  return 64 - length - bit;
#else
#error "Bit packing code isn't written for your byte order."
#endif
}

__inline static uint64 read_off(const void *base, uint64 bit_off) 
{
#if defined(__arm) || defined(__arm__)
  const uint8 *base_off = (const uint8 *)(base) + (bit_off >> 3);
  uint64 value64;
  memcpy(&value64, base_off, sizeof(value64));
  return value64;
#else
  return *(const uint64*)((const uint8 *)(base) + (bit_off >> 3));
#endif
}

/* Pack integers up to 57 bits using their least significant digits. 
 * The length is specified using mask:
 * Assumes mask == (1 << length) - 1 where length <= 57.   
 */
__inline static uint64 read_int57(const void *base, uint64 bit_off, uint8 length, uint64 mask) 
{
  return (read_off(base, bit_off) >> bit_pack_shift(bit_off & 7, length)) & mask;
}

/* Assumes value < (1 << length) and length <= 57.
 * Assumes the memory is zero initially. 
 */
__inline static void write_int57(void *base, uint64 bit_off, uint8 length, uint64 value) 
{
#if defined(__arm) || defined(__arm__)
  uint8 *base_off = (uint8 *)(base) + (bit_off >> 3);
  uint64 value64;
  memcpy(&value64, base_off, sizeof(value64));
  value64 |= (value << bit_pack_shift(bit_off & 7, length));
  memcpy(base_off, &value64, sizeof(value64));
#else
  *(uint64 *)((uint8 *)(base) + (bit_off >> 3)) |= 
    (value << bit_pack_shift(bit_off & 7, length));
#endif
}

__inline static float read_nonposfloat31(const void *base, uint64 bit_off) {
    float_enc encoded;
    encoded.i = (uint32)(read_off(base, bit_off) >> bit_pack_shift(bit_off & 7, 31));
    // Sign bit set means negative.  
    encoded.i |= K_SIGN_BIT;
    return encoded.f;
}

__inline static void write_nonposfloat31(void *base, uint64 bit_off, float value) {
    float_enc encoded;
    encoded.f = value;
    encoded.i &= ~K_SIGN_BIT;
    write_int57(base, bit_off, 31, encoded.i);
}

__inline static float read_float32(const void *base, uint64 bit_off) {
    float_enc encoded;
    encoded.i = (uint32)(read_off(base, bit_off) >> bit_pack_shift(bit_off & 7, 32));
    return encoded.f;
}

__inline static void write_float32(void *base, uint64 bit_off, float value) {
    float_enc encoded;
    encoded.f = value;
    write_int57(base, bit_off, 32, encoded.i);
}

/* Same caveats as above, but for a 25 bit limit. */
__inline static uint32 read_int25(const void *base, uint64 bit_off, uint8 length, uint32 mask) {
#if defined(__arm) || defined(__arm__)
  const uint8 *base_off = (const uint8_t*)(base) + (bit_off >> 3);
  uint32 value32;
  memcpy(&value32, base_off, sizeof(value32));
  return (value32 >> bit_pack_shift(bit_off & 7, length)) & mask;
#else
    return (*(const uint32_t*)((const uint8_t*)(base) + (bit_off >> 3)) >> bit_pack_shift(bit_off & 7, length)) & mask;
#endif
}

__inline static void write_int25(void *base, uint64 bit_off, uint8 length, uint32 value) {
#if defined(__arm) || defined(__arm__)
    uint8 *base_off = (uint8 *)(base) + (bit_off >> 3);
    uint32 value32;
    memcpy(&value32, base_off, sizeof(value32));
    value32 |= (value << bit_pack_shift(bit_off & 7, length));
    memcpy(base_off, &value32, sizeof(value32));
#else
    *(uint32_t *)((uint8 *)(base) + (bit_off >> 3)) |= (value << bit_pack_shift(bit_off & 7, length));
#endif
}


typedef struct bit_mask_s {
    uint8 bits;
    uint64 mask;
}bit_mask_t;

typedef struct bit_adress_s {
    void *base;
    uint64 offset;
}bit_adress_t;

void bit_mask_from_max(bit_mask_t *bit_mask, uint32 max_value);

// Return bits required to store integers upto max_value.  Not the most
// efficient implementation, but this is only called a few times to size tries. 
uint8 required_bits(uint32 max_value);

void bit_packing_sanity();

#endif /* __LM_TRIE_BITS_H__ */