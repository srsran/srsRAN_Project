/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/security/security.h"
#include "srsran/ran/pci.h"
#include "srsran/security/ssl.h"
#include "srsran/support/error_handling.h"
#include <arpa/inet.h>

using namespace srsran;
using namespace srsran::security;

bool security_context::select_algorithms(preferred_integrity_algorithms pref_inte_list,
                                         preferred_ciphering_algorithms pref_ciph_list)
{
  // Select preferred integrity algorithm.
  bool inte_algo_found = false;
  for (unsigned i = 0; i < nof_pref_algos; ++i) {
    uint16_t algo_id = to_number(pref_inte_list[i]);
    if (algo_id == 0) {
      // Do not allow NIA0
      logger.error("NIA0 selection not allowed");
      break;
    }
    if (supported_int_algos[algo_id - 1]) {
      inte_algo_found      = true;
      sel_algos.integ_algo = security::integrity_algorithm_from_number(algo_id);
      break;
    }
  }
  if (not inte_algo_found) {
    logger.error("Could not select integrity protection algorithm");
    return false;
  }

  // Select preferred ciphering algorithm.
  bool ciph_algo_found = false;
  for (unsigned i = 0; i < nof_pref_algos; ++i) {
    uint16_t algo_id = to_number(pref_ciph_list[i]);
    if (algo_id == 0) {
      ciph_algo_found       = true;
      sel_algos.cipher_algo = security::ciphering_algorithm::nea0;
      break;
    }
    if (supported_enc_algos[algo_id - 1]) {
      ciph_algo_found       = true;
      sel_algos.cipher_algo = security::ciphering_algorithm_from_number(algo_id);
      break;
    }
  }
  if (not ciph_algo_found) {
    logger.error("Could not select ciphering algorithm");
    return false;
  }
  sel_algos.algos_selected = true;
  return true;
}

void security_context::generate_as_keys()
{
  srsran_sanity_check(sel_algos.algos_selected, "Tried to generate AS keys, but no algo is selected");
  // Generate K_rrc_enc and K_rrc_int
  security::generate_k_rrc(as_keys.k_rrc_enc, as_keys.k_rrc_int, k, sel_algos.cipher_algo, sel_algos.integ_algo);

  // Generate K_up_enc and K_up_int
  security::generate_k_up(as_keys.k_up_enc, as_keys.k_up_int, k, sel_algos.cipher_algo, sel_algos.integ_algo);

  logger.debug("K_gNB {}", security::sec_as_key_to_string(k));
  logger.debug("RRC Integrity Key {}", security::sec_as_key_to_string(as_keys.k_rrc_int));
  logger.debug("RRC Encryption Key {}", security::sec_as_key_to_string(as_keys.k_rrc_enc));
  logger.debug("UP Encryption Key {}", security::sec_as_key_to_string(as_keys.k_up_enc));
  logger.debug("UP Integrity Key {}", security::sec_as_key_to_string(as_keys.k_up_int));
}

void security_context::horizontal_key_defivation(pci_t new_pci, unsigned new_dl_arfcn)
{
  logger.info("Regenerating KgNB with PCI={}, SSB-ARFCN={}", new_pci, new_dl_arfcn);
  logger.info(k.data(), sec_key_len, "Old K_gNB (k_gnb)");

  // Generate K_NG-RAN*
  sec_key k_ng_ran_star;
  generate_k_ng_ran_star(k_ng_ran_star, k, new_pci, new_dl_arfcn);

  // K_NG-RAN* becomes K_gNB
  k = k_ng_ran_star;

  generate_as_keys();
}

sec_128_as_config security_context::get_128_as_config()
{
  return truncate_config(get_as_config());
}

sec_as_config security_context::get_as_config()
{
  srsran_sanity_check(sel_algos.algos_selected, "Tried to get AS config, but no algorithms are selected");
  sec_as_config as_cfg;
  as_cfg.k_rrc_int   = as_keys.k_rrc_int;
  as_cfg.k_rrc_enc   = as_keys.k_rrc_enc;
  as_cfg.k_up_int    = as_keys.k_up_int;
  as_cfg.k_up_enc    = as_keys.k_up_enc;
  as_cfg.integ_algo  = sel_algos.integ_algo;
  as_cfg.cipher_algo = sel_algos.cipher_algo;
  return as_cfg;
}
/******************************************************************************
 * Key Generation
 *****************************************************************************/
void srsran::security::generate_k_rrc(sec_key&                  k_rrc_enc,
                                      sec_key&                  k_rrc_int,
                                      const sec_key&            k_gnb,
                                      const ciphering_algorithm enc_alg_id,
                                      const integrity_algorithm int_alg_id)
{
  // Derive RRC ENC
  // algorithm type distinguisher
  std::array<uint8_t, 1> algo_distinguisher{to_number(security_algo_distinguisher::rrc_enc_alg)};

  // algorithm type distinguisher
  std::array<uint8_t, 1> algorithm_identity{static_cast<uint8_t>(enc_alg_id)};

  generic_kdf(k_rrc_enc, k_gnb, fc_value::algorithm_key_derivation, algo_distinguisher, algorithm_identity);

  // Derive RRC INT
  // algorithm type distinguisher
  algo_distinguisher = {to_number(security_algo_distinguisher::rrc_int_alg)};

  // algorithm type distinguisher
  algorithm_identity = {static_cast<uint8_t>(int_alg_id)};

  // Derive RRC int
  generic_kdf(k_rrc_int, k_gnb, fc_value::algorithm_key_derivation, algo_distinguisher, algorithm_identity);
}

void srsran::security::generate_k_up(sec_key&                  k_up_enc,
                                     sec_key&                  k_up_int,
                                     const sec_key&            k_gnb,
                                     const ciphering_algorithm enc_alg_id,
                                     const integrity_algorithm int_alg_id)
{
  // Derive UP ENC
  // algorithm type distinguisher
  std::array<uint8_t, 1> algo_distinguisher{to_number(security_algo_distinguisher::up_enc_alg)};

  // algorithm type distinguisher
  std::array<uint8_t, 1> algorithm_identity{static_cast<uint8_t>(enc_alg_id)};

  generic_kdf(k_up_enc, k_gnb, fc_value::algorithm_key_derivation, algo_distinguisher, algorithm_identity);

  // Derive UP INT
  // algorithm type distinguisher
  algo_distinguisher = {to_number(security_algo_distinguisher::up_int_alg)};

  // algorithm type distinguisher
  algorithm_identity = {static_cast<uint8_t>(int_alg_id)};

  // Derive UP int
  generic_kdf(k_up_int, k_gnb, fc_value::algorithm_key_derivation, algo_distinguisher, algorithm_identity);
}

void srsran::security::generate_k_ng_ran_star(sec_key&       k_star,
                                              const sec_key& k,
                                              const pci_t&   pci_,
                                              const uint32_t earfcn_)
{
  // PCI
  std::vector<uint8_t> pci;
  pci.resize(2);
  pci[0] = (pci_ >> 8) & 0xff;
  pci[1] = pci_ & 0xff;

  // ARFCN, can be two or three bytes
  std::vector<uint8_t> earfcn;
  if (earfcn_ < pow(2, 16)) {
    earfcn.resize(2);
    earfcn[0] = (earfcn_ >> 8) & 0xff;
    earfcn[1] = earfcn_ & 0xff;
  } else if (earfcn_ < pow(2, 24)) {
    earfcn.resize(3);
    earfcn[0] = (earfcn_ >> 16) & 0xff;
    earfcn[1] = (earfcn_ >> 8) & 0xff;
    earfcn[2] = earfcn_ & 0xff;
  } else {
    report_error("Invalid ARFCN in K_NG-RAN* derivation");
  }

  generic_kdf(k_star, k, fc_value::k_ng_ran_star_derivation, pci, earfcn);
}

void srsran::security::generic_kdf(sec_key&            key_out,
                                   const sec_key&      key_in,
                                   const fc_value      fc,
                                   span<const uint8_t> p0,
                                   span<const uint8_t> p1)
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
  ++s_it;

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

sec_128_key srsran::security::truncate_key(const sec_key& key_in)
{
  sec_128_key key_out = {};
  static_assert(sec_key_len > 0, "sec_key_len too small");
  static_assert(sec_128_key_len > 0, "sec_128_key_len too small");
  static_assert(sec_128_key_len <= sec_key_len, "sec_128_key_len is larger than sec_key_len");
  std::copy(key_in.begin() + (sec_key_len - sec_128_key_len), key_in.end(), key_out.begin());
  return key_out;
}

sec_128_as_config srsran::security::truncate_config(const sec_as_config& cfg_in)
{
  sec_128_as_config cfg_out = {};
  cfg_out.k_128_rrc_int     = truncate_key(cfg_in.k_rrc_int);
  cfg_out.k_128_rrc_enc     = truncate_key(cfg_in.k_rrc_enc);
  cfg_out.k_128_up_int      = truncate_key(cfg_in.k_up_int);
  cfg_out.k_128_up_enc      = truncate_key(cfg_in.k_up_enc);
  cfg_out.integ_algo        = cfg_in.integ_algo;
  cfg_out.cipher_algo       = cfg_in.cipher_algo;
  return cfg_out;
}
