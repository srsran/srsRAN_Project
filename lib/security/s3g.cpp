/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/security/s3g.h"

using namespace srsran;
using namespace srsran::security;

/* S-box SQ */
static const uint8_t SQ[256] = {
    0x25, 0x24, 0x73, 0x67, 0xd7, 0xae, 0x5c, 0x30, 0xa4, 0xee, 0x6e, 0xcb, 0x7d, 0xb5, 0x82, 0xdb, 0xe4, 0x8e, 0x48,
    0x49, 0x4f, 0x5d, 0x6a, 0x78, 0x70, 0x88, 0xe8, 0x5f, 0x5e, 0x84, 0x65, 0xe2, 0xd8, 0xe9, 0xcc, 0xed, 0x40, 0x2f,
    0x11, 0x28, 0x57, 0xd2, 0xac, 0xe3, 0x4a, 0x15, 0x1b, 0xb9, 0xb2, 0x80, 0x85, 0xa6, 0x2e, 0x02, 0x47, 0x29, 0x07,
    0x4b, 0x0e, 0xc1, 0x51, 0xaa, 0x89, 0xd4, 0xca, 0x01, 0x46, 0xb3, 0xef, 0xdd, 0x44, 0x7b, 0xc2, 0x7f, 0xbe, 0xc3,
    0x9f, 0x20, 0x4c, 0x64, 0x83, 0xa2, 0x68, 0x42, 0x13, 0xb4, 0x41, 0xcd, 0xba, 0xc6, 0xbb, 0x6d, 0x4d, 0x71, 0x21,
    0xf4, 0x8d, 0xb0, 0xe5, 0x93, 0xfe, 0x8f, 0xe6, 0xcf, 0x43, 0x45, 0x31, 0x22, 0x37, 0x36, 0x96, 0xfa, 0xbc, 0x0f,
    0x08, 0x52, 0x1d, 0x55, 0x1a, 0xc5, 0x4e, 0x23, 0x69, 0x7a, 0x92, 0xff, 0x5b, 0x5a, 0xeb, 0x9a, 0x1c, 0xa9, 0xd1,
    0x7e, 0x0d, 0xfc, 0x50, 0x8a, 0xb6, 0x62, 0xf5, 0x0a, 0xf8, 0xdc, 0x03, 0x3c, 0x0c, 0x39, 0xf1, 0xb8, 0xf3, 0x3d,
    0xf2, 0xd5, 0x97, 0x66, 0x81, 0x32, 0xa0, 0x00, 0x06, 0xce, 0xf6, 0xea, 0xb7, 0x17, 0xf7, 0x8c, 0x79, 0xd6, 0xa7,
    0xbf, 0x8b, 0x3f, 0x1f, 0x53, 0x63, 0x75, 0x35, 0x2c, 0x60, 0xfd, 0x27, 0xd3, 0x94, 0xa5, 0x7c, 0xa1, 0x05, 0x58,
    0x2d, 0xbd, 0xd9, 0xc7, 0xaf, 0x6b, 0x54, 0x0b, 0xe0, 0x38, 0x04, 0xc8, 0x9d, 0xe7, 0x14, 0xb1, 0x87, 0x9c, 0xdf,
    0x6f, 0xf9, 0xda, 0x2a, 0xc4, 0x59, 0x16, 0x74, 0x91, 0xab, 0x26, 0x61, 0x76, 0x34, 0x2b, 0xad, 0x99, 0xfb, 0x72,
    0xec, 0x33, 0x12, 0xde, 0x98, 0x3b, 0xc0, 0x9b, 0x3e, 0x18, 0x10, 0x3a, 0x56, 0xe1, 0x77, 0xc9, 0x1e, 0x9e, 0x95,
    0xa3, 0x90, 0x19, 0xa8, 0x6c, 0x09, 0xd0, 0xf0, 0x86};

static const uint8_t S[256] = {
    99,  124, 119, 123, 242, 107, 111, 197, 48,  1,   103, 43,  254, 215, 171, 118, 202, 130, 201, 125, 250, 89,
    71,  240, 173, 212, 162, 175, 156, 164, 114, 192, 183, 253, 147, 38,  54,  63,  247, 204, 52,  165, 229, 241,
    113, 216, 49,  21,  4,   199, 35,  195, 24,  150, 5,   154, 7,   18,  128, 226, 235, 39,  178, 117, 9,   131,
    44,  26,  27,  110, 90,  160, 82,  59,  214, 179, 41,  227, 47,  132, 83,  209, 0,   237, 32,  252, 177, 91,
    106, 203, 190, 57,  74,  76,  88,  207, 208, 239, 170, 251, 67,  77,  51,  133, 69,  249, 2,   127, 80,  60,
    159, 168, 81,  163, 64,  143, 146, 157, 56,  245, 188, 182, 218, 33,  16,  255, 243, 210, 205, 12,  19,  236,
    95,  151, 68,  23,  196, 167, 126, 61,  100, 93,  25,  115, 96,  129, 79,  220, 34,  42,  144, 136, 70,  238,
    184, 20,  222, 94,  11,  219, 224, 50,  58,  10,  73,  6,   36,  92,  194, 211, 172, 98,  145, 149, 228, 121,
    231, 200, 55,  109, 141, 213, 78,  169, 108, 86,  244, 234, 101, 122, 174, 8,   186, 120, 37,  46,  28,  166,
    180, 198, 232, 221, 116, 31,  75,  189, 139, 138, 112, 62,  181, 102, 72,  3,   246, 14,  97,  53,  87,  185,
    134, 193, 29,  158, 225, 248, 152, 17,  105, 217, 142, 148, 155, 30,  135, 233, 206, 85,  40,  223, 140, 161,
    137, 13,  191, 230, 66,  104, 65,  153, 45,  15,  176, 84,  187, 22};
/*********************************************************************
    Name: s3g_mul_x

    Description: Multiplication with reduction.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.1.1
*********************************************************************/
uint8_t s3g_mul_x(uint8_t v, uint8_t c);

/*********************************************************************
    Name: s3g_mul_x_pow

    Description: Recursive multiplication with reduction.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.1.2
*********************************************************************/
uint8_t s3g_mul_x_pow(uint8_t v, uint8_t i, uint8_t c);

/*********************************************************************
    Name: s3g_mul_alpha

    Description: Multiplication with alpha.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.4.2
*********************************************************************/
uint32_t s3g_mul_alpha(uint8_t c);

/*********************************************************************
    Name: s3g_div_alpha

    Description: Division by alpha.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.4.3
*********************************************************************/
uint32_t s3g_div_alpha(uint8_t c);

/*********************************************************************
    Name: s3g_s1

    Description: S-Box S1.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.3.1
*********************************************************************/
uint32_t s3g_s1(uint32_t w);

/*********************************************************************
    Name: s3g_s2

    Description: S-Box S2.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.3.2
*********************************************************************/
uint32_t s3g_s2(uint32_t w);

/*********************************************************************
    Name: s3g_clock_lfsr

    Description: Clocking LFSR.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.4.4 and Section 3.4.5
*********************************************************************/
void s3g_clock_lfsr(S3G_STATE* state, uint32_t f);

/*********************************************************************
    Name: s3g_clock_fsm

    Description: Clocking FSM.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.4.6
*********************************************************************/
uint32_t s3g_clock_fsm(S3G_STATE* state);

/*********************************************************************
    Name: s3g_deinitialize

    Description: Deinitialization.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
*********************************************************************/
void srsran::security::s3g_deinitialize(S3G_STATE* state);

/*********************************************************************
    Name: s3g_generate_keystream

    Description: Generation of Keystream.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 4.2
*********************************************************************/
void srsran::security::s3g_generate_keystream(S3G_STATE* state, uint32_t n, uint32_t* ks);

/*********************************************************************
    Name: s3g_mul_x

    Description: Multiplication with reduction.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.1.1
*********************************************************************/
uint8_t s3g_mul_x(uint8_t v, uint8_t c)
{
  if (v & 0x80)
    return ((v << 1) ^ c);
  else
    return (v << 1);
}

/*********************************************************************
    Name: s3g_mul_x_pow

    Description: Recursive multiplication with reduction.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.1.2
*********************************************************************/
uint8_t s3g_mul_x_pow(uint8_t v, uint8_t i, uint8_t c)
{
  if (i == 0)
    return v;
  else
    return s3g_mul_x(s3g_mul_x_pow(v, i - 1, c), c);
}

/*********************************************************************
    Name: s3g_mul_alpha

    Description: Multiplication with alpha.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.4.2
*********************************************************************/
uint32_t s3g_mul_alpha(uint8_t c)
{
  return ((((uint32_t)s3g_mul_x_pow(c, 23, 0xa9)) << 24) | (((uint32_t)s3g_mul_x_pow(c, 245, 0xa9)) << 16) |
          (((uint32_t)s3g_mul_x_pow(c, 48, 0xa9)) << 8) | (((uint32_t)s3g_mul_x_pow(c, 239, 0xa9))));
}

/*********************************************************************
    Name: s3g_div_alpha

    Description: Division by alpha.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.4.3
*********************************************************************/
uint32_t s3g_div_alpha(uint8_t c)
{
  return ((((uint32_t)s3g_mul_x_pow(c, 16, 0xa9)) << 24) | (((uint32_t)s3g_mul_x_pow(c, 39, 0xa9)) << 16) |
          (((uint32_t)s3g_mul_x_pow(c, 6, 0xa9)) << 8) | (((uint32_t)s3g_mul_x_pow(c, 64, 0xa9))));
}

/*********************************************************************
    Name: s3g_s1

    Description: S-Box S1.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.3.1
*********************************************************************/
uint32_t s3g_s1(uint32_t w)
{
  uint8_t r0 = 0, r1 = 0, r2 = 0, r3 = 0;
  uint8_t srw0 = S[(uint8_t)((w >> 24) & 0xff)];
  uint8_t srw1 = S[(uint8_t)((w >> 16) & 0xff)];
  uint8_t srw2 = S[(uint8_t)((w >> 8) & 0xff)];
  uint8_t srw3 = S[(uint8_t)((w)&0xff)];

  r0 = ((s3g_mul_x(srw0, 0x1b)) ^ (srw1) ^ (srw2) ^ ((s3g_mul_x(srw3, 0x1b)) ^ srw3));

  r1 = (((s3g_mul_x(srw0, 0x1b)) ^ srw0) ^ (s3g_mul_x(srw1, 0x1b)) ^ (srw2) ^ (srw3));

  r2 = ((srw0) ^ ((s3g_mul_x(srw1, 0x1b)) ^ srw1) ^ (s3g_mul_x(srw2, 0x1b)) ^ (srw3));

  r3 = ((srw0) ^ (srw1) ^ ((s3g_mul_x(srw2, 0x1b)) ^ srw2) ^ (s3g_mul_x(srw3, 0x1b)));

  return ((((uint32_t)r0) << 24) | (((uint32_t)r1) << 16) | (((uint32_t)r2) << 8) | (((uint32_t)r3)));
}

/*********************************************************************
    Name: s3g_s2

    Description: S-Box S2.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.3.2
*********************************************************************/
uint32_t s3g_s2(uint32_t w)
{
  uint8_t r0 = 0, r1 = 0, r2 = 0, r3 = 0;
  uint8_t sqw0 = SQ[(uint8_t)((w >> 24) & 0xff)];
  uint8_t sqw1 = SQ[(uint8_t)((w >> 16) & 0xff)];
  uint8_t sqw2 = SQ[(uint8_t)((w >> 8) & 0xff)];
  uint8_t sqw3 = SQ[(uint8_t)((w)&0xff)];

  r0 = ((s3g_mul_x(sqw0, 0x69)) ^ (sqw1) ^ (sqw2) ^ ((s3g_mul_x(sqw3, 0x69)) ^ sqw3));

  r1 = (((s3g_mul_x(sqw0, 0x69)) ^ sqw0) ^ (s3g_mul_x(sqw1, 0x69)) ^ (sqw2) ^ (sqw3));

  r2 = ((sqw0) ^ ((s3g_mul_x(sqw1, 0x69)) ^ sqw1) ^ (s3g_mul_x(sqw2, 0x69)) ^ (sqw3));

  r3 = ((sqw0) ^ (sqw1) ^ ((s3g_mul_x(sqw2, 0x69)) ^ sqw2) ^ (s3g_mul_x(sqw3, 0x69)));

  return ((((uint32_t)r0) << 24) | (((uint32_t)r1) << 16) | (((uint32_t)r2) << 8) | (((uint32_t)r3)));
}

/*********************************************************************
    Name: s3g_clock_lfsr

    Description: Clocking LFSR.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.4.4 and Section 3.4.5
*********************************************************************/
void s3g_clock_lfsr(S3G_STATE* state, uint32_t f)
{
  uint32_t v = (((state->lfsr[0] << 8) & 0xffffff00) ^ (s3g_mul_alpha((uint8_t)((state->lfsr[0] >> 24) & 0xff))) ^
                (state->lfsr[2]) ^ ((state->lfsr[11] >> 8) & 0x00ffffff) ^
                (s3g_div_alpha((uint8_t)((state->lfsr[11]) & 0xff))) ^ (f));
  uint8_t  i;

  for (i = 0; i < 15; i++) {
    state->lfsr[i] = state->lfsr[i + 1];
  }
  state->lfsr[15] = v;
}

/*********************************************************************
    Name: s3g_clock_fsm

    Description: Clocking FSM.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.4.6
*********************************************************************/
uint32_t s3g_clock_fsm(S3G_STATE* state)
{
  uint32_t f = ((state->lfsr[15] + state->fsm[0]) & 0xffffffff) ^ state->fsm[1];
  uint32_t r = (state->fsm[1] + (state->fsm[2] ^ state->lfsr[5])) & 0xffffffff;

  state->fsm[2] = s3g_s2(state->fsm[1]);
  state->fsm[1] = s3g_s1(state->fsm[0]);
  state->fsm[0] = r;

  return f;
}

/*********************************************************************
    Name: s3g_initialize

    Description: Initialization.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 4.1
*********************************************************************/
void srsran::security::s3g_initialize(S3G_STATE* state, uint32_t k[4], uint32_t iv[4])
{
  uint8_t  i = 0;
  uint32_t f = 0x0;

  state->lfsr = (uint32_t*)calloc(16, sizeof(uint32_t));
  state->fsm  = (uint32_t*)calloc(3, sizeof(uint32_t));

  state->lfsr[15] = k[3] ^ iv[0];
  state->lfsr[14] = k[2];
  state->lfsr[13] = k[1];
  state->lfsr[12] = k[0] ^ iv[1];

  state->lfsr[11] = k[3] ^ 0xffffffff;
  state->lfsr[10] = k[2] ^ 0xffffffff ^ iv[2];
  state->lfsr[9]  = k[1] ^ 0xffffffff ^ iv[3];
  state->lfsr[8]  = k[0] ^ 0xffffffff;
  state->lfsr[7]  = k[3];
  state->lfsr[6]  = k[2];
  state->lfsr[5]  = k[1];
  state->lfsr[4]  = k[0];
  state->lfsr[3]  = k[3] ^ 0xffffffff;
  state->lfsr[2]  = k[2] ^ 0xffffffff;
  state->lfsr[1]  = k[1] ^ 0xffffffff;
  state->lfsr[0]  = k[0] ^ 0xffffffff;

  state->fsm[0] = 0x0;
  state->fsm[1] = 0x0;
  state->fsm[2] = 0x0;
  for (i = 0; i < 32; i++) {
    f = s3g_clock_fsm(state);
    s3g_clock_lfsr(state, f);
  }
}

/*********************************************************************
    Name: s3g_deinitialize

    Description: Deinitialization.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
*********************************************************************/
void srsran::security::s3g_deinitialize(S3G_STATE* state)
{
  free(state->lfsr);
  free(state->fsm);
}

/*********************************************************************
    Name: s3g_generate_keystream

    Description: Generation of Keystream.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 4.2
*********************************************************************/
void srsran::security::s3g_generate_keystream(S3G_STATE* state, uint32_t n, uint32_t* ks)
{
  uint32_t t = 0;
  uint32_t f = 0x0;

  // Clock FSM once. Discard the output.
  s3g_clock_fsm(state);
  //  Clock LFSR in keystream mode once.
  s3g_clock_lfsr(state, 0x0);

  for (t = 0; t < n; t++) {
    f = s3g_clock_fsm(state);
    // Note that ks[t] corresponds to z_{t+1} in section 4.2
    ks[t] = f ^ state->lfsr[0];
    s3g_clock_lfsr(state, 0x0);
  }
}

/* MUL64x.
 * Input V: a 64-bit input.
 * Input c: a 64-bit input.
 * Output : a 64-bit output.
 * A 64-bit memory is allocated which is to be freed by the calling
 * function.
 * See section 4.3.2 for details.
 */
uint64_t s3g_MUL64x(uint64_t V, uint64_t c)
{
  if (V & 0x8000000000000000)
    return (V << 1) ^ c;
  else
    return V << 1;
}

/* MUL64xPOW.
 * Input V: a 64-bit input.
 * Input i: a positive integer.
 * Input c: a 64-bit input.
 * Output : a 64-bit output.
 * A 64-bit memory is allocated which is to be freed by the calling function.
 * See section 4.3.3 for details.
 */
uint64_t s3g_MUL64xPOW(uint64_t V, uint8_t i, uint64_t c)
{
  if (i == 0)
    return V;
  else
    return s3g_MUL64x(s3g_MUL64xPOW(V, i - 1, c), c);
}

/* MUL64.
 * Input V: a 64-bit input.
 * Input P: a 64-bit input.
 * Input c: a 64-bit input.
 * Output : a 64-bit output.
 * A 64-bit memory is allocated which is to be freed by the calling
 * function.
 * See section 4.3.4 for details.
 */
uint64_t s3g_MUL64(uint64_t V, uint64_t P, uint64_t c)
{
  uint64_t result = 0;
  int      i      = 0;

  for (i = 0; i < 64; i++) {
    if ((P >> i) & 0x1)
      result ^= s3g_MUL64xPOW(V, i, c);
  }
  return result;
}

/* mask8bit.
 * Input n: an integer in 1-7.
 * Output : an 8 bit mask.
 * Prepares an 8 bit mask with required number of 1 bits on the MSB side.
 */
uint8_t mask8bit(int n)
{
  return 0xff ^ ((1 << (8 - n)) - 1);
}

/* f9.
 * Input key: 128 bit Integrity Key.
 * Input count:32-bit Count, Frame dependent input.
 * Input fresh: 32-bit Random number.
 * Input dir:1 bit, direction of transmission (in the LSB).
 * Input data: length number of bits, input bit stream.
 * Input length: 64 bit Length, i.e., the number of bits to be MAC'd.
 * Output  : 32 bit block used as MAC
 * Generates 32-bit MAC using UIA2 algorithm as defined in Section 4.
 */
void srsran::security::s3g_f9(srsran::security::sec_mac& mac,
                              const uint8_t*             key,
                              uint32_t                   count,
                              uint32_t                   fresh,
                              uint32_t                   dir,
                              uint8_t*                   data,
                              uint64_t                   length)
{
  uint32_t  K[4], IV[4], z[5];
  uint32_t  i = 0, D;
  uint64_t  EVAL;
  uint64_t  V;
  uint64_t  P;
  uint64_t  Q;
  uint64_t  c;
  S3G_STATE state, *state_ptr;

  uint64_t M_D_2;
  int      rem_bits = 0;
  state_ptr         = &state;
  /* Load the Integrity Key for SNOW3G initialization as in section 4.4. */
  for (i = 0; i < 4; i++)
    K[3 - i] = (key[4 * i] << 24) ^ (key[4 * i + 1] << 16) ^ (key[4 * i + 2] << 8) ^ (key[4 * i + 3]);

  /* Prepare the Initialization Vector (IV) for SNOW3G initialization as
     in section 4.4. */
  IV[3] = count;
  IV[2] = fresh;
  IV[1] = count ^ (dir << 31);
  IV[0] = fresh ^ (dir << 15);

  z[0] = z[1] = z[2] = z[3] = z[4] = 0;

  /* Run SNOW 3G to produce 5 keystream words z_1, z_2, z_3, z_4 and z_5. */
  s3g_initialize(state_ptr, K, IV);
  s3g_generate_keystream(state_ptr, 5, z);
  s3g_deinitialize(state_ptr);
  P = (uint64_t)z[0] << 32 | (uint64_t)z[1];
  Q = (uint64_t)z[2] << 32 | (uint64_t)z[3];

  /* Calculation */
  if ((length % 64) == 0)
    D = (length >> 6) + 1;
  else
    D = (length >> 6) + 2;
  EVAL = 0;
  c    = 0x1b;

  /* for 0 <= i <= D-3 */
  for (i = 0; i < D - 2; i++) {
    V    = EVAL ^ ((uint64_t)data[8 * i] << 56 | (uint64_t)data[8 * i + 1] << 48 | (uint64_t)data[8 * i + 2] << 40 |
                (uint64_t)data[8 * i + 3] << 32 | (uint64_t)data[8 * i + 4] << 24 | (uint64_t)data[8 * i + 5] << 16 |
                (uint64_t)data[8 * i + 6] << 8 | (uint64_t)data[8 * i + 7]);
    EVAL = s3g_MUL64(V, P, c);
  }

  /* for D-2 */
  rem_bits = length % 64;
  if (rem_bits == 0)
    rem_bits = 64;

  M_D_2 = 0;
  i     = 0;
  while (rem_bits > 7) {
    M_D_2 |= (uint64_t)data[8 * (D - 2) + i] << (8 * (7 - i));
    rem_bits -= 8;
    i++;
  }
  if (rem_bits > 0)
    M_D_2 |= (uint64_t)(data[8 * (D - 2) + i] & mask8bit(rem_bits)) << (8 * (7 - i));

  V    = EVAL ^ M_D_2;
  EVAL = s3g_MUL64(V, P, c);

  /* for D-1 */
  EVAL ^= length;

  /* Multiply by Q */
  EVAL = s3g_MUL64(EVAL, Q, c);

  /* XOR with z_5: this is a modification to the reference C code,
     which forgot to XOR z[5] */
  for (i = 0; i < 4; i++) {
    /*
    MAC_I[i] = (mac32 >> (8*(3-i))) & 0xff;
    */
    mac[i] = ((EVAL >> (56 - (i * 8))) ^ (z[4] >> (24 - (i * 8)))) & 0xff;
  }
}
