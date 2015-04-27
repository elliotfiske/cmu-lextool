/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 2015 Carnegie Mellon University.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * This work was supported in part by funding from the Defense Advanced 
 * Research Projects Agency and the National Science Foundation of the 
 * United States of America, and the CMU Sphinx Speech Consortium.
 *
 * THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
 * ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
 * NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ====================================================================
 *
 */

#ifndef _LIBUTIL_BITARR_H_
#define _LIBUTIL_BITARR_H_

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
/* Win32/WinCE DLL gunk */
#include <sphinxbase/sphinxbase_export.h>

/** 
 * @file bitarr.h
 * @brief An implementation bit array - memory efficient storage for digit int and float data.
 * 
 * Implementation of basic operations of read/write digits consuming as little space as possible.
 */

#ifdef __cplusplus
extern "C" {
#endif
#if 0
/* Fool Emacs. */
}
#endif

#define SIGN_BIT (0x80000000)

/**
 * Union to map float to integer to store latter in bit array
 */
typedef union { 
    float f; 
    uint32 i; 
} float_enc;

/**
 * Structure that specifies bits required to efficiently store certain data
 */
typedef struct bitarr_mask_s {
    uint8 bits;
    uint32 mask;
}bitarr_mask_t;

/**
 * Structure that stores adress of certain value in bit array
 */
typedef struct bitarr_adress_s {
    void *base;
    uint32 offset;
}bitarr_adress_t;

/**
 * Shift bits depending on byte order in system.
 * Fun fact: __BYTE_ORDER is wrong on Solaris Sparc, but the version without __ is correct.
 * @param bit is an offset last byte
 * @param length - amount of bits for required for digit that is going to be read
 * @return shift forgiven architecture
 */
#if BYTE_ORDER == LITTLE_ENDIAN
#define get_shift(bit, length) (bit)
#elif BYTE_ORDER == BIG_ENDIAN
#define get_shift(bit, length) (64 - length - bit)
#else
#error "Bit packing code isn't written for your byte order."
#endif

/**
 * Read uint64 value from the given adress
 * @param adress to read from
 * @param pointer to value where to save read value
 * @return uint64 value that was read
 */
#if defined(__arm) || defined(__arm__)
__inline static uint64 read_off(bitarr_adress_t adress)
{
    uint64 value64;
    const uint8 *base_off = (const uint8 *)(adress.base) + (adress.offset >> 3);
    memcpy(&value64, base_off, sizeof(value64));
    return value64;
}
#else
#define read_off(adress) \
    (*(const uint64*)((const uint8 *)(adress.base) + (adress.offset >> 3)))
#endif

/**
 * Read uint64 value from bit array. 
 * Assumes mask == (1 << length) - 1 where length <= 57
 * @param adress to read from
 * @param length number of bits for value
 * @param mask of read value
 * @return uint64 value that was read
 */
#define bitarr_read_int57(adress, length, mask) \
    ((read_off(adress) >> get_shift(adress.offset & 7, length)) & mask)

/**
 * Write specified value into bit array.
 * Assumes value < (1 << length) and length <= 57.
 * Assumes the memory is zero initially.
 * @param adress to write to
 * @param length amount of active bytes in value to write
 * @param value integer to write
 */
#if defined(__arm) || defined(__arm__)
__inline static void bitarr_write_int57(bitarr_adress_t adress, uint8 length, uint64 value) 
{
    uint64 value64;
    uint8 *base_off = (uint8 *)(adress.base) + (adress.offset >> 3);
    memcpy(&value64, base_off, sizeof(value64));
    value64 |= (value << get_shift(adress.offset & 7, length));
    memcpy(base_off, &value64, sizeof(value64));
}
#else
#define bitarr_write_int57(adress, length, value) \
    (*(uint64 *)((uint8 *)(adress.base) + (adress.offset >> 3)) |= (value << get_shift(adress.offset & 7, length)))
#endif

/**
 * Read uint32 value from bit array. 
 * Assumes mask == (1 << length) - 1 where length <= 25
 * @param adress to read from
 * @param length number of bits for value
 * @param mask of read value
 * @return uint32 value that was read
 */
#if defined(__arm) || defined(__arm__)
__inline static uint32 bitarr_read_int25(bitarr_adress_t adress, uint8 length, uint32 mask) 
{
    uint32 value32;
    const uint8 *base_off = (const uint8_t*)(adress.base) + (adress.offset >> 3);
    memcpy(&value32, adress.offset, sizeof(value32));
    return (value32 >> get_shift(adress.offset & 7, length)) & mask;
}
#else
#define bitarr_read_int25(adress, length, mask)  \
    ((*(const uint32_t*)((const uint8_t*)(adress.base) + (adress.offset >> 3)) >> get_shift(adress.offset & 7, length)) & mask)
#endif

/**
 * Write specified value into bit array.
 * Assumes value < (1 << length) and length <= 25.
 * Assumes the memory is zero initially.
 * @param adress in bit array ti write to
 * @param length amount of active bytes in value to write
 * @param value integer to write
 */
#if defined(__arm) || defined(__arm__)
__inline static void bitarr_write_int25(bitarr_adress_t adress, uint8 length, uint32 value)
{
    uint32 value32;
    uint8 *base_off = (uint8 *)(adress.base) + (adress.offset >> 3);
    memcpy(&value32, base_off, sizeof(value32));
    value32 |= (value << get_shift(adress.offset & 7, length));
    memcpy(base_off, &value32, sizeof(value32));
}
#else
#define bitarr_write_int25(adress, length, value)  \
    (*(uint32_t *)((uint8 *)(adress.base) + (adress.offset >> 3)) |= (value << get_shift(adress.offset & 7, length)))
#endif

/**
 * Read non positive float32 from bit array.
 * Probability [0, 1) in log domain can be stored like this.
 * @param adress of value in bit array
 * @return float value taht was read
 */
__inline static float bitarr_read_negfloat(bitarr_adress_t adress) {
    float_enc encoded;
    encoded.i = (uint32)(read_off(adress) >> get_shift(adress.offset & 7, 31));
    // Sign bit set means negative.  
    encoded.i |= SIGN_BIT;
    return encoded.f;
}

/**
 * Writes non positive float32 to bit array.
 * Probability [0, 1) in log domain can be stored like this
 * @param adress where to write
 * @param value what to write
 */
__inline static void bitarr_write_negfloat(bitarr_adress_t adress, float value) {
    float_enc encoded;
    encoded.f = value;
    encoded.i &= ~SIGN_BIT;
    bitarr_write_int57(adress, 31, encoded.i);
}

/**
 * Reads float32 from bit array
 * @param adress in bit array from where to read
 * @return value float32 that was read
 */
__inline static float bitarr_read_float(bitarr_adress_t adress) {
    float_enc encoded;
    encoded.i = (uint32)(read_off(adress) >> get_shift(adress.offset & 7, 32));
    return encoded.f;
}

/**
 * Writes float32 to bit array
 * @param adress in bit array where to write
 * @param value float32 to write
 */
__inline static void bitarr_write_float(bitarr_adress_t adress, float value) {
    float_enc encoded;
    encoded.f = value;
    bitarr_write_int57(adress, 32, encoded.i);
}

/**
 * Fills mask for certain int range according to provided max value
 * @param bit_mask mask that is filled
 * @param max_value bigest integer that is going to be stored using this mask
 */
SPHINXBASE_EXPORT
void bitarr_mask_from_max(bitarr_mask_t *bit_mask, uint32 max_value);

/**
 * Computes amount of bits required ti store integers upto value provided.
 * @param max_value bigest integer that going to be stored using this amount of bits
 * @return amount of bits required to store integers from range with maximum provided
 */
SPHINXBASE_EXPORT
uint8 bitarr_required_bits(uint32 max_value);

#ifdef __cplusplus
}
#endif

#endif /* _LIBUTIL_BITARR_H_ */