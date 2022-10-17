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

#include "srsgnb/security/security.h"
#include "srsgnb/security/ssl.h"
#include <arpa/inet.h>

#ifdef HAVE_MBEDTLS
#include "mbedtls/md5.h"
#endif
#ifdef HAVE_POLARSSL
#include "polarssl/md5.h"
#endif

using namespace srsgnb;

/******************************************************************************
 * Key Generation
 *****************************************************************************/
void srsgnb::security_generate_k_rrc(sec_as_key&               k_rrc_enc,
                                     sec_as_key&               k_rrc_int,
                                     const sec_as_key&         k_gnb,
                                     const ciphering_algorithm enc_alg_id,
                                     const integrity_algorithm int_alg_id)
{
  // Derive RRC ENC
  // algorithm type distinguisher
  std::vector<uint8_t> algo_distinguisher;
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = to_number(security_algo_distinguisher::rrc_enc_alg);

  // algorithm type distinguisher
  std::vector<uint8_t> algorithm_identity;
  algorithm_identity.resize(1);
  algorithm_identity[0] = static_cast<uint8_t>(enc_alg_id);

  generic_kdf(k_rrc_enc, k_gnb, fc_value::algorithm_key_derivation, algo_distinguisher, algorithm_identity);

  // Derive RRC INT
  // algorithm type distinguisher
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = to_number(security_algo_distinguisher::rrc_int_alg);

  // algorithm type distinguisher
  algorithm_identity.resize(1);
  algorithm_identity[0] = static_cast<uint8_t>(int_alg_id);

  // Derive RRC int
  generic_kdf(k_rrc_int, k_gnb, fc_value::algorithm_key_derivation, algo_distinguisher, algorithm_identity);
}

void srsgnb::security_generate_k_up(sec_as_key&               k_up_enc,
                                    sec_as_key&               k_up_int,
                                    const sec_as_key&         k_gnb,
                                    const ciphering_algorithm enc_alg_id,
                                    const integrity_algorithm int_alg_id)
{
  // Derive UP ENC
  // algorithm type distinguisher
  std::vector<uint8_t> algo_distinguisher;
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = to_number(security_algo_distinguisher::up_enc_alg);

  // algorithm type distinguisher
  std::vector<uint8_t> algorithm_identity;
  algorithm_identity.resize(1);
  algorithm_identity[0] = static_cast<uint8_t>(enc_alg_id);

  generic_kdf(k_up_enc, k_gnb, fc_value::algorithm_key_derivation, algo_distinguisher, algorithm_identity);

  // Derive UP INT
  // algorithm type distinguisher
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = to_number(security_algo_distinguisher::up_int_alg);

  // algorithm type distinguisher
  algorithm_identity.resize(1);
  algorithm_identity[0] = static_cast<uint8_t>(int_alg_id);

  // Derive UP int
  generic_kdf(k_up_int, k_gnb, fc_value::algorithm_key_derivation, algo_distinguisher, algorithm_identity);
}

void srsgnb::generic_kdf(sec_as_key&                 key_out,
                         const sec_as_key&           key_in,
                         const fc_value              fc,
                         const std::vector<uint8_t>& p0,
                         const std::vector<uint8_t>& p1)
{
  union p_len {
    uint16_t                                  length_value;
    std::array<uint8_t, sizeof(length_value)> bytes;
  };

  std::vector<uint8_t> s;
  s.resize(1 + p0.size() + sizeof(p_len) + p1.size() + sizeof(p_len));

  std::vector<uint8_t>::iterator s_it = s.begin();

  // FC
  *s_it = to_number(fc);
  s_it++;

  // P0
  std::copy(p0.begin(), p0.end(), s_it);
  s_it += p0.size();

  p_len p0_len = {htons(p0.size())};
  std::copy(p0_len.bytes.begin(), p0_len.bytes.end(), s_it);
  s_it += sizeof(p0_len);

  // P1
  std::copy(p1.begin(), p1.end(), s_it);
  s_it += p1.size();

  p_len p1_len = {htons(p1.size())};
  std::copy(p1_len.bytes.begin(), p1_len.bytes.end(), s_it);
  s_it += sizeof(p1_len);

  sha256(key_in.data(), key_in.size(), s.data(), s.size(), key_out.data(), 0);
}

sec_128_as_key srsgnb::truncate_key(const sec_as_key& key_in)
{
  sec_128_as_key key_out = {};
  static_assert(sec_key_len > 0, "sec_key_len too small");
  static_assert(sec_128_key_len > 0, "sec_128_key_len too small");
  static_assert(sec_128_key_len <= sec_key_len, "sec_128_key_len is larger than sec_key_len");
  std::copy(key_in.begin() + (sec_key_len - sec_128_key_len), key_in.end(), key_out.begin());
  return key_out;
}
