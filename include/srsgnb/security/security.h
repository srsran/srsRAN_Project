/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

/******************************************************************************
 * Common security header - wraps ciphering/integrity check algorithms.
 *****************************************************************************/

#include "srsgnb/srslog/srslog.h"

#include <vector>

#define MAC_LEN 8
#define IK_LEN 16
#define CK_LEN 16
#define AK_LEN 6

#define KEY_LEN 32
namespace srsgnb {

// TODO remove
typedef enum {
  LIBLTE_SUCCESS = 0,
  LIBLTE_ERROR_INVALID_INPUTS,
  LIBLTE_ERROR_ENCODE_FAIL,
  LIBLTE_ERROR_DECODE_FAIL,
  LIBLTE_ERROR_INVALID_CRC,
  LIBLTE_ERROR_N_ITEMS
} LIBLTE_ERROR_ENUM;

typedef enum {
  CIPHERING_ALGORITHM_ID_EEA0 = 0,
  CIPHERING_ALGORITHM_ID_128_EEA1,
  CIPHERING_ALGORITHM_ID_128_EEA2,
  CIPHERING_ALGORITHM_ID_128_EEA3,
  CIPHERING_ALGORITHM_ID_N_ITEMS,
} CIPHERING_ALGORITHM_ID_ENUM;
static const char ciphering_algorithm_id_text[CIPHERING_ALGORITHM_ID_N_ITEMS][20] = {"EEA0",
                                                                                     "128-EEA1",
                                                                                     "128-EEA2",
                                                                                     "128-EEA3"};

typedef enum {
  INTEGRITY_ALGORITHM_ID_EIA0 = 0,
  INTEGRITY_ALGORITHM_ID_128_EIA1,
  INTEGRITY_ALGORITHM_ID_128_EIA2,
  INTEGRITY_ALGORITHM_ID_128_EIA3,
  INTEGRITY_ALGORITHM_ID_N_ITEMS,
} INTEGRITY_ALGORITHM_ID_ENUM;
static const char integrity_algorithm_id_text[INTEGRITY_ALGORITHM_ID_N_ITEMS][20] = {"EIA0",
                                                                                     "128-EIA1",
                                                                                     "128-EIA2",
                                                                                     "128-EIA3"};

typedef enum {
  CIPHERING_ALGORITHM_ID_NR_NEA0 = 0,
  CIPHERING_ALGORITHM_ID_NR_128_NEA1,
  CIPHERING_ALGORITHM_ID_NR_128_NEA2,
  CIPHERING_ALGORITHM_ID_NR_128_NEA3,
  CIPHERING_ALGORITHM_ID_NR_N_ITEMS,
} CIPHERING_ALGORITHM_ID_NR_ENUM;
static const char ciphering_algorithm_id_nr_text[CIPHERING_ALGORITHM_ID_N_ITEMS][20] = {"NEA0",
                                                                                        "128-NEA1",
                                                                                        "128-NEA2",
                                                                                        "128-NEA3"};
typedef enum {
  INTEGRITY_ALGORITHM_ID_NR_NIA0 = 0,
  INTEGRITY_ALGORITHM_ID_NR_128_NIA1,
  INTEGRITY_ALGORITHM_ID_NR_128_NIA2,
  INTEGRITY_ALGORITHM_ID_NR_128_NIA3,
  INTEGRITY_ALGORITHM_ID_NR_N_ITEMS,
} INTEGRITY_ALGORITHM_ID_NR_ENUM;
static const char integrity_algorithm_id_nr_text[INTEGRITY_ALGORITHM_ID_N_ITEMS][20] = {"NIA0",
                                                                                        "128-NIA1",
                                                                                        "128-NIA2",
                                                                                        "128-NIA3"};
typedef enum {
  SECURITY_DIRECTION_UPLINK   = 0,
  SECURITY_DIRECTION_DOWNLINK = 1,
  SECURITY_DIRECTION_N_ITEMS,
} security_direction_t;
static const char security_direction_text[INTEGRITY_ALGORITHM_ID_N_ITEMS][20] = {"Uplink", "Downlink"};

using mac_t = std::array<uint8_t, 4>;

using as_key_t = std::array<uint8_t, 32>;
struct k_enb_context_t {
  as_key_t k_enb;
  as_key_t nh;
  bool     is_first_ncc;
  uint32_t ncc;
};

/******************************************************************************
 * Integrity Protection
 *****************************************************************************/
void security_128_eia1(const uint8_t* key,
                       uint32_t       count,
                       uint32_t       bearer,
                       uint8_t        direction,
                       uint8_t*       msg,
                       uint32_t       msg_len,
                       uint8_t*       mac);

uint8_t security_128_eia2(const uint8_t* key,
                          uint32_t       count,
                          uint32_t       bearer,
                          uint8_t        direction,
                          uint8_t*       msg,
                          uint32_t       msg_len,
                          uint8_t*       mac);

uint8_t security_128_eia3(const uint8_t* key,
                          uint32_t       count,
                          uint32_t       bearer,
                          uint8_t        direction,
                          uint8_t*       msg,
                          uint32_t       msg_len,
                          uint8_t*       mac);

uint8_t security_md5(const uint8_t* input, size_t len, uint8_t* output);

/******************************************************************************
 * Encryption / Decryption
 *****************************************************************************/
void security_128_eea1(uint8_t* key,
                       uint32_t count,
                       uint8_t  bearer,
                       uint8_t  direction,
                       uint8_t* msg,
                       uint32_t msg_len,
                       uint8_t* msg_out);
/*
uint8_t security_128_eea2(uint8_t* key,
                          uint32_t count,
                          uint8_t  bearer,
                          uint8_t  direction,
                          uint8_t* msg,
                          uint32_t msg_len,
                          uint8_t* msg_out);

uint8_t security_128_eea3(uint8_t* key,
                          uint32_t count,
                          uint8_t  bearer,
                          uint8_t  direction,
                          uint8_t* msg,
                          uint32_t msg_len,
                          uint8_t* msg_out);
*/

/*********************************************************************
    Name: zero_tailing_bits

    Description: Fill tailing bits with zeros.

    Document Reference: -
*********************************************************************/
inline void zero_tailing_bits(uint8_t* data, uint32_t length_bits)
{
  uint8_t bits = (8 - (length_bits & 0x07)) & 0x07;
  data[(length_bits + 7) / 8 - 1] &= (uint8_t)(0xFF << bits);
}

} // namespace srsgnb
