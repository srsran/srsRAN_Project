/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "s3g.h"

using namespace srsran;
using namespace srsran::security;

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
  return MUL_ALPHA[c];
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
  return DIV_ALPHA[c];
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
  return S1_T0[static_cast<uint8_t>(w & 0xff)] ^ S1_T1[static_cast<uint8_t>((w >> 8) & 0xff)] ^
         S1_T2[static_cast<uint8_t>((w >> 16) & 0xff)] ^ S1_T3[static_cast<uint8_t>((w >> 24) & 0xff)];
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
  return S2_T0[static_cast<uint8_t>(w & 0xff)] ^ S2_T1[static_cast<uint8_t>((w >> 8) & 0xff)] ^
         S2_T2[static_cast<uint8_t>((w >> 16) & 0xff)] ^ S2_T3[static_cast<uint8_t>((w >> 24) & 0xff)];
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
static uint64_t s3g_MUL64x(uint64_t V, uint64_t c)
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
static uint64_t s3g_MUL64xPOW(uint64_t V, uint8_t i, uint64_t c)
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
static uint64_t s3g_MUL64(uint64_t V, uint64_t P, uint64_t c)
{
  uint64_t result = 0;
  int      i      = 0;

  for (i = 0; i < 64; i++) {
    if ((P >> i) & 0x1)
      result ^= s3g_MUL64xPOW(V, i, c);
  }
  return result;
}

/// \brief Initializes the lookup tables for the MUL operation.
/// \param[out] PM lookup tables to be generated.
/// \param[in] P a 64-bit input.
/// \param[in] c a 64-bit input.
/// Implemented as specified in "Specification of the 3GPP Confidentiality and Integrity Algorithms UEA2 & UIA2 D1 v2.1"
/// Annex 2.
static void s3g_PRE_MUL_P(std::array<std::array<uint64_t, 256>, 8>& PM, uint64_t P, uint64_t c)
{
  for (uint8_t i = 0; i != 8; ++i) {
    PM[i][0] = 0;
  }
  PM[0][1] = P;

  for (uint8_t i = 1; i != 64; ++i) {
    uint64_t a                 = PM[(i - 1) >> 3][1 << ((i - 1) & 0x7)];
    PM[i >> 3][1 << (i & 0x7)] = a << 1;
    // If the leftmost bit...
    static constexpr uint64_t leftmost_bit_mask = 1UL << 63UL;
    if (a & leftmost_bit_mask) {
      PM[i >> 3][1 << (i & 0x7)] ^= c;
    }
  }

  for (uint8_t i = 0; i != 8; ++i) {
    for (uint8_t j = 1; j != 8; ++j) {
      for (uint8_t k = 1, k_end = (1 << j); k != k_end; ++k) {
        PM[i][(1 << j) + k] = PM[i][1 << j] ^ PM[i][k];
      }
    }
  }
}

/// \brief Performs the MUL operation using the precomputed lookup tables.
/// \param[in] PM lookup tables to be generated.
/// \param[in] X a 64-bit input.
/// \returns the result of the MUL operation.
/// Implemented as specified in "Specification of the 3GPP Confidentiality and Integrity Algorithms UEA2 & UIA2 D1 v2.1"
/// Annex 2.
static uint64_t s3g_MUL_P(const std::array<std::array<uint64_t, 256>, 8>& PM, uint64_t X)
{
  return PM[0][X & 0xff] ^ PM[1][(X >> 8) & 0xff] ^ PM[2][(X >> 16) & 0xff] ^ PM[3][(X >> 24) & 0xff] ^
         PM[4][(X >> 32) & 0xff] ^ PM[5][(X >> 40) & 0xff] ^ PM[6][(X >> 48) & 0xff] ^ PM[7][(X >> 56) & 0xff];
}

/* mask8bit.
 * Input n: an integer in 1-7.
 * Output : an 8 bit mask.
 * Prepares an 8 bit mask with required number of 1 bits on the MSB side.
 */
static uint8_t mask8bit(int n)
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
  // Compute the lookup tables for the MUL operation.
  std::array<std::array<uint64_t, 256>, 8> PM;
  s3g_PRE_MUL_P(PM, P, c);

  /* for 0 <= i <= D-3 */
  for (i = 0; i < D - 2; i++) {
    V    = EVAL ^ ((uint64_t)data[8 * i] << 56 | (uint64_t)data[8 * i + 1] << 48 | (uint64_t)data[8 * i + 2] << 40 |
                (uint64_t)data[8 * i + 3] << 32 | (uint64_t)data[8 * i + 4] << 24 | (uint64_t)data[8 * i + 5] << 16 |
                (uint64_t)data[8 * i + 6] << 8 | (uint64_t)data[8 * i + 7]);
    EVAL = s3g_MUL_P(PM, V);
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
  EVAL = s3g_MUL_P(PM, V);

  /* for D-1 */
  EVAL ^= length;

  /* Multiply by Q */
  // Note: s3g_MUL_P is not used here because the lookup tables were generated for P, not Q.
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
