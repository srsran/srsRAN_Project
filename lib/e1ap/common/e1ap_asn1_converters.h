/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"
#include "srsran/asn1/e1ap/e1ap.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "srsran/ran/bcd_helpers.h"
#include <string>
#include <vector>

namespace srsran {

/// \brief Converts type \c ciphering_algorithm to an E1AP ASN.1 type.
/// \param[in] ciph_algo Cyphering Algorithm object.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
inline asn1::e1ap::ciphering_algorithm_e
ciphering_algorithm_to_e1ap_asn1(const srsran::security::ciphering_algorithm& ciph_algo)
{
  asn1::e1ap::ciphering_algorithm_e asn1_ciph_algo;

  switch (ciph_algo) {
    case srsran::security::ciphering_algorithm::nea0:
      asn1_ciph_algo = asn1::e1ap::ciphering_algorithm_opts::nea0;
      break;
    case srsran::security::ciphering_algorithm::nea1:
      asn1_ciph_algo = asn1::e1ap::ciphering_algorithm_opts::c_neg128_nea1;
      break;
    case srsran::security::ciphering_algorithm::nea2:
      asn1_ciph_algo = asn1::e1ap::ciphering_algorithm_opts::c_neg128_nea2;
      break;
    case srsran::security::ciphering_algorithm::nea3:
      asn1_ciph_algo = asn1::e1ap::ciphering_algorithm_opts::c_neg128_nea3;
      break;
    default:
      srsran_assert(false, "Invalid ciphering algorithm ({})", ciph_algo);
      break;
  }

  return asn1_ciph_algo;
}

/// \brief Converts E1AP ASN.1 type to type and \c ciphering_algorithm type.
/// \param[in] asn1_ciph_algo ASN.1 Cyphering Algorithm object.
/// \return The \c ciphering_algorithm object where the result of the conversion is stored.
inline srsran::security::ciphering_algorithm
e1ap_asn1_to_ciphering_algorithm(const asn1::e1ap::ciphering_algorithm_e& asn1_ciph_algo)
{
  srsran::security::ciphering_algorithm ciph_algo = {};

  switch (asn1_ciph_algo) {
    case asn1::e1ap::ciphering_algorithm_opts::nea0:
      ciph_algo = srsran::security::ciphering_algorithm::nea0;
      break;
    case asn1::e1ap::ciphering_algorithm_opts::c_neg128_nea1:
      ciph_algo = srsran::security::ciphering_algorithm::nea1;
      break;
    case asn1::e1ap::ciphering_algorithm_opts::c_neg128_nea2:
      ciph_algo = srsran::security::ciphering_algorithm::nea2;
      break;
    case asn1::e1ap::ciphering_algorithm_opts::c_neg128_nea3:
      ciph_algo = srsran::security::ciphering_algorithm::nea3;
      break;
    default:
      srsran_assert(false, "Invalid ciphering algorithm ({})", asn1_ciph_algo);
  }

  return ciph_algo;
}

/// \brief Converts type \c integrity_algorithm to an E1AP ASN.1 type.
/// \param int_algo Integrity Protection Algorithm object.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
inline asn1::e1ap::integrity_protection_algorithm_e
integrity_algorithm_to_e1ap_asn1(const srsran::security::integrity_algorithm& int_algo)
{
  asn1::e1ap::integrity_protection_algorithm_e asn1_int_algo = {};

  switch (int_algo) {
    case srsran::security::integrity_algorithm::nia0:
      asn1_int_algo = asn1::e1ap::integrity_protection_algorithm_opts::nia0;
      break;
    case srsran::security::integrity_algorithm::nia1:
      asn1_int_algo = asn1::e1ap::integrity_protection_algorithm_opts::i_neg128_nia1;
      break;
    case srsran::security::integrity_algorithm::nia2:
      asn1_int_algo = asn1::e1ap::integrity_protection_algorithm_opts::i_neg128_nia2;
      break;
    case srsran::security::integrity_algorithm::nia3:
      asn1_int_algo = asn1::e1ap::integrity_protection_algorithm_opts::i_neg128_nia3;
      break;
    default:
      srsran_assert(false, "Invalid integrity protection algorithm ({})", int_algo);
  }

  return asn1_int_algo;
}

/// \brief Converts E1AP ASN.1 type to type and \c integrity_algorithm type.
/// \param[in] asn1_int_algo ASN.1 Integrity Protection Algorithm object.
/// \return The \c integrity_algorithm object where the result of the conversion is stored.
inline srsran::security::integrity_algorithm
e1ap_asn1_to_integrity_algorithm(const asn1::e1ap::integrity_protection_algorithm_e& asn1_int_algo)
{
  srsran::security::integrity_algorithm int_algo = {};

  switch (asn1_int_algo) {
    case asn1::e1ap::integrity_protection_algorithm_opts::nia0:
      int_algo = srsran::security::integrity_algorithm::nia0;
      break;
    case asn1::e1ap::integrity_protection_algorithm_opts::i_neg128_nia1:
      int_algo = srsran::security::integrity_algorithm::nia1;
      break;
    case asn1::e1ap::integrity_protection_algorithm_opts::i_neg128_nia2:
      int_algo = srsran::security::integrity_algorithm::nia2;
      break;
    case asn1::e1ap::integrity_protection_algorithm_opts::i_neg128_nia3:
      int_algo = srsran::security::integrity_algorithm::nia3;
      break;
    default:
      srsran_assert(false, "Invalid integrity protection algorithm ({})", asn1_int_algo);
  }

  return int_algo;
}

/// \brief Convert \c s_nssai_t type to E1AP ASN1 s-NSSAI.
/// \param snssai The s-NSSAI type.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
inline asn1::e1ap::snssai_s snssai_to_e1ap_asn1(srsran::s_nssai_t snssai)
{
  asn1::e1ap::snssai_s asn1_snssai;
  asn1_snssai.sst.from_number(snssai.sst);
  if (snssai.sd.has_value()) {
    asn1_snssai.sd_present = true;
    asn1_snssai.sd.from_number(snssai.sd.value());
  }

  return asn1_snssai;
}

/// \brief Convert E1AP ASN1 s-NSSAI to \c s_nssai_t type.
/// \param asn1_snssai The E1AP ASN1 s-NSSAI.
/// \return The \c s_nssai_t type object where the result of the conversion is stored.
inline srsran::s_nssai_t e1ap_asn1_to_snssai(asn1::e1ap::snssai_s asn1_snssai)
{
  srsran::s_nssai_t snssai;
  snssai.sst = asn1_snssai.sst.to_number();

  if (asn1_snssai.sd_present) {
    snssai.sd = asn1_snssai.sd.to_number();
  }

  return snssai;
}

/// \brief Converts type \c sdap_config to an E1AP ASN.1 type.
/// \param sdap_cfg sdap config object.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
inline asn1::e1ap::sdap_cfg_s sdap_config_to_e1ap_asn1(sdap_config_t sdap_cfg)
{
  asn1::e1ap::sdap_cfg_s asn1_sdap_cfg;

  if (sdap_cfg.default_drb) {
    asn1_sdap_cfg.default_drb = asn1::e1ap::default_drb_opts::options::true_value;
  } else {
    asn1_sdap_cfg.default_drb = asn1::e1ap::default_drb_opts::options::false_value;
  }

  asn1::string_to_enum(asn1_sdap_cfg.sdap_hdr_dl, sdap_cfg.sdap_hdr_dl);
  asn1::string_to_enum(asn1_sdap_cfg.sdap_hdr_ul, sdap_cfg.sdap_hdr_ul);

  return asn1_sdap_cfg;
}

/// \brief Converts E1AP ASN.1 type to \c sdap_config to type.
/// \param asn1_sdap_cfg E1AP ASN.1 config object.
/// \return The \c sdap_config object where the result of the conversion is stored.
inline sdap_config_t e1ap_asn1_to_sdap_config(asn1::e1ap::sdap_cfg_s asn1_sdap_cfg)
{
  sdap_config_t sdap_cfg;

  if (asn1_sdap_cfg.default_drb == asn1::e1ap::default_drb_opts::options::true_value) {
    sdap_cfg.default_drb = true;
  } else {
    sdap_cfg.default_drb = false;
  }

  sdap_cfg.sdap_hdr_dl = asn1_sdap_cfg.sdap_hdr_dl.to_string();
  sdap_cfg.sdap_hdr_ul = asn1_sdap_cfg.sdap_hdr_ul.to_string();

  return sdap_cfg;
}

/// \brief Converts \c rlc_mode type to an E1AP ASN.1 type.
/// \param rlc_mod rlc_mode type.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
inline asn1::e1ap::rlc_mode_e rlc_mode_to_asn1(srsran::rlc_mode rlc_mod)
{
  asn1::e1ap::rlc_mode_e asn1_rlc_mode = {};

  switch (rlc_mod) {
    case srsran::rlc_mode::tm:
      asn1_rlc_mode = asn1::e1ap::rlc_mode_opts::rlc_tm;
      break;
    case srsran::rlc_mode::um_bidir:
      asn1_rlc_mode = asn1::e1ap::rlc_mode_opts::rlc_um_bidirectional;
      break;
    case srsran::rlc_mode::um_unidir_ul:
      asn1_rlc_mode = asn1::e1ap::rlc_mode_opts::rlc_um_unidirectional_ul;
      break;
    case srsran::rlc_mode::um_unidir_dl:
      asn1_rlc_mode = asn1::e1ap::rlc_mode_opts::rlc_um_unidirectional_dl;
      break;
    case srsran::rlc_mode::am:
      asn1_rlc_mode = asn1::e1ap::rlc_mode_opts::rlc_am;
      break;
    default:
      srsran_assert(false, "Invalid RLC mode ({})", rlc_mod);
  }

  return asn1_rlc_mode;
}

/// \brief Converts E1AP ASN.1 type to an \c rlc_mode type.
/// \param asn1_rlc_mod E1AP ASN.1 type.
/// \return The rlc_mode object where the result of the conversion is stored.
inline srsran::rlc_mode asn1_to_rlc_mode(asn1::e1ap::rlc_mode_e asn1_rlc_mod)
{
  srsran::rlc_mode rlc_mode = {};

  switch (asn1_rlc_mod) {
    case asn1::e1ap::rlc_mode_opts::rlc_tm:
      rlc_mode = srsran::rlc_mode::tm;
      break;
    case asn1::e1ap::rlc_mode_opts::rlc_um_bidirectional:
      rlc_mode = srsran::rlc_mode::um_bidir;
      break;
    case asn1::e1ap::rlc_mode_opts::rlc_um_unidirectional_ul:
      rlc_mode = srsran::rlc_mode::um_unidir_ul;
      break;
    case asn1::e1ap::rlc_mode_opts::rlc_um_unidirectional_dl:
      rlc_mode = srsran::rlc_mode::um_unidir_dl;
      break;
    case asn1::e1ap::rlc_mode_opts::rlc_am:
      rlc_mode = srsran::rlc_mode::am;
      break;
    default:
      srsran_assert(false, "Invalid RLC mode: {}", asn1_rlc_mod.to_string());
      break;
  }

  return rlc_mode;
}

/// \brief Convert PDCP SN size from enum to E1AP ASN.1.
inline asn1::e1ap::pdcp_sn_size_e pdcp_sn_size_to_asn1(pdcp_sn_size sn_size)
{
  asn1::e1ap::pdcp_sn_size_e asn1_sn_size = {};

  switch (sn_size) {
    case pdcp_sn_size::size12bits:
      asn1_sn_size = asn1::e1ap::pdcp_sn_size_e::s_neg12;
      break;
    case pdcp_sn_size::size18bits:
      asn1_sn_size = asn1::e1ap::pdcp_sn_size_e::s_neg18;
      break;
    default:
      srsran_assert(false, "Unsupported PDCP SN size. PDCP SN size={}", sn_size);
  }

  return asn1_sn_size;
}

/// \brief Convert E1AP ASN.1 to PDCP SN size.
inline pdcp_sn_size asn1_to_pdcp_sn_size(asn1::e1ap::pdcp_sn_size_e asn1_sn_size)
{
  pdcp_sn_size sn_size = {};

  switch (asn1_sn_size) {
    case asn1::e1ap::pdcp_sn_size_e::s_neg12:
      sn_size = pdcp_sn_size::size12bits;
      break;
    case asn1::e1ap::pdcp_sn_size_e::s_neg18:
      sn_size = pdcp_sn_size::size18bits;
      break;
    default:
      srsran_assert(false, "Unsupported PDCP SN size. PDCP SN size={}", asn1_sn_size);
  }

  return sn_size;
}

/// \brief Converts E1AP ASN.1 discard timer type to \c pdcp_discard_timer type.
inline pdcp_discard_timer asn1_to_pdcp_discard_timer(asn1::e1ap::discard_timer_e asn1_discard_timer)
{
  pdcp_discard_timer discard_timer = {};

  switch (asn1_discard_timer) {
    case asn1::e1ap::discard_timer_opts::ms10:
      discard_timer = pdcp_discard_timer::ms10;
      break;
    case asn1::e1ap::discard_timer_opts::ms20:
      discard_timer = pdcp_discard_timer::ms20;
      break;
    case asn1::e1ap::discard_timer_opts::ms30:
      discard_timer = pdcp_discard_timer::ms30;
      break;
    case asn1::e1ap::discard_timer_opts::ms40:
      discard_timer = pdcp_discard_timer::ms40;
      break;
    case asn1::e1ap::discard_timer_opts::ms50:
      discard_timer = pdcp_discard_timer::ms50;
      break;
    case asn1::e1ap::discard_timer_opts::ms60:
      discard_timer = pdcp_discard_timer::ms60;
      break;
    case asn1::e1ap::discard_timer_opts::ms75:
      discard_timer = pdcp_discard_timer::ms75;
      break;
    case asn1::e1ap::discard_timer_opts::ms100:
      discard_timer = pdcp_discard_timer::ms100;
      break;
    case asn1::e1ap::discard_timer_opts::ms150:
      discard_timer = pdcp_discard_timer::ms150;
      break;
    case asn1::e1ap::discard_timer_opts::ms200:
      discard_timer = pdcp_discard_timer::ms200;
      break;
    case asn1::e1ap::discard_timer_opts::ms250:
      discard_timer = pdcp_discard_timer::ms250;
      break;
    case asn1::e1ap::discard_timer_opts::ms300:
      discard_timer = pdcp_discard_timer::ms300;
      break;
    case asn1::e1ap::discard_timer_opts::ms500:
      discard_timer = pdcp_discard_timer::ms500;
      break;
    case asn1::e1ap::discard_timer_opts::ms750:
      discard_timer = pdcp_discard_timer::ms750;
      break;
    case asn1::e1ap::discard_timer_opts::ms1500:
      discard_timer = pdcp_discard_timer::ms1500;
      break;
    case asn1::e1ap::discard_timer_opts::infinity:
      discard_timer = pdcp_discard_timer::infinity;
      break;
    default:
      srsran_assert(false, "Unsupported PDCP discard timer. PDCP discard timer={}", asn1_discard_timer);
  }
  return discard_timer;
}

/// \brief Converts \c pdcp_discard_timer type to E1AP ASN.1 discard timer type.
inline asn1::e1ap::discard_timer_e pdcp_discard_timer_to_asn1(pdcp_discard_timer discard_timer)
{
  asn1::e1ap::discard_timer_e asn1_discard_timer = {};

  switch (discard_timer) {
    case pdcp_discard_timer::ms10:
      asn1_discard_timer = asn1::e1ap::discard_timer_opts::ms10;
      break;
    case pdcp_discard_timer::ms20:
      asn1_discard_timer = asn1::e1ap::discard_timer_opts::ms20;
      break;
    case pdcp_discard_timer::ms30:
      asn1_discard_timer = asn1::e1ap::discard_timer_opts::ms30;
      break;
    case pdcp_discard_timer::ms40:
      asn1_discard_timer = asn1::e1ap::discard_timer_opts::ms40;
      break;
    case pdcp_discard_timer::ms50:
      asn1_discard_timer = asn1::e1ap::discard_timer_opts::ms50;
      break;
    case pdcp_discard_timer::ms60:
      asn1_discard_timer = asn1::e1ap::discard_timer_opts::ms60;
      break;
    case pdcp_discard_timer::ms75:
      asn1_discard_timer = asn1::e1ap::discard_timer_opts::ms75;
      break;
    case pdcp_discard_timer::ms100:
      asn1_discard_timer = asn1::e1ap::discard_timer_opts::ms100;
      break;
    case pdcp_discard_timer::ms150:
      asn1_discard_timer = asn1::e1ap::discard_timer_opts::ms150;
      break;
    case pdcp_discard_timer::ms200:
      asn1_discard_timer = asn1::e1ap::discard_timer_opts::ms200;
      break;
    case pdcp_discard_timer::ms250:
      asn1_discard_timer = asn1::e1ap::discard_timer_opts::ms250;
      break;
    case pdcp_discard_timer::ms300:
      asn1_discard_timer = asn1::e1ap::discard_timer_opts::ms300;
      break;
    case pdcp_discard_timer::ms500:
      asn1_discard_timer = asn1::e1ap::discard_timer_opts::ms500;
      break;
    case pdcp_discard_timer::ms750:
      asn1_discard_timer = asn1::e1ap::discard_timer_opts::ms750;
      break;
    case pdcp_discard_timer::ms1500:
      asn1_discard_timer = asn1::e1ap::discard_timer_opts::ms1500;
      break;
    case pdcp_discard_timer::infinity:
      asn1_discard_timer = asn1::e1ap::discard_timer_opts::infinity;
      break;
    default:
      srsran_assert(false, "Unsupported PDCP discard timer. PDCP discard timer={}", discard_timer);
  }

  return asn1_discard_timer;
}

/// \brief Converts E1AP ASN.1 t-Reordering type to \c pdcp_t_reordering type.
inline pdcp_t_reordering asn1_to_pdcp_t_reordering(asn1::e1ap::t_reordering_e asn1_t_reordering)
{
  pdcp_t_reordering t_reordering = {};

  // t-Reordering
  switch (asn1_t_reordering) {
    case asn1::e1ap::t_reordering_e::ms0:
      t_reordering = pdcp_t_reordering::ms0;
      break;
    case asn1::e1ap::t_reordering_e::ms1:
      t_reordering = pdcp_t_reordering::ms1;
      break;
    case asn1::e1ap::t_reordering_e::ms2:
      t_reordering = pdcp_t_reordering::ms2;
      break;
    case asn1::e1ap::t_reordering_e::ms4:
      t_reordering = pdcp_t_reordering::ms4;
      break;
    case asn1::e1ap::t_reordering_e::ms5:
      t_reordering = pdcp_t_reordering::ms5;
      break;
    case asn1::e1ap::t_reordering_e::ms8:
      t_reordering = pdcp_t_reordering::ms8;
      break;
    case asn1::e1ap::t_reordering_e::ms10:
      t_reordering = pdcp_t_reordering::ms10;
      break;
    case asn1::e1ap::t_reordering_e::ms15:
      t_reordering = pdcp_t_reordering::ms15;
      break;
    case asn1::e1ap::t_reordering_e::ms20:
      t_reordering = pdcp_t_reordering::ms20;
      break;
    case asn1::e1ap::t_reordering_e::ms30:
      t_reordering = pdcp_t_reordering::ms30;
      break;
    case asn1::e1ap::t_reordering_e::ms40:
      t_reordering = pdcp_t_reordering::ms40;
      break;
    case asn1::e1ap::t_reordering_e::ms50:
      t_reordering = pdcp_t_reordering::ms50;
      break;
    case asn1::e1ap::t_reordering_e::ms60:
      t_reordering = pdcp_t_reordering::ms60;
      break;
    case asn1::e1ap::t_reordering_e::ms80:
      t_reordering = pdcp_t_reordering::ms80;
      break;
    case asn1::e1ap::t_reordering_e::ms100:
      t_reordering = pdcp_t_reordering::ms100;
      break;
    case asn1::e1ap::t_reordering_e::ms120:
      t_reordering = pdcp_t_reordering::ms120;
      break;
    case asn1::e1ap::t_reordering_e::ms140:
      t_reordering = pdcp_t_reordering::ms140;
      break;
    case asn1::e1ap::t_reordering_e::ms160:
      t_reordering = pdcp_t_reordering::ms160;
      break;
    case asn1::e1ap::t_reordering_e::ms180:
      t_reordering = pdcp_t_reordering::ms180;
      break;
    case asn1::e1ap::t_reordering_e::ms200:
      t_reordering = pdcp_t_reordering::ms200;
      break;
    case asn1::e1ap::t_reordering_e::ms220:
      t_reordering = pdcp_t_reordering::ms220;
      break;
    case asn1::e1ap::t_reordering_e::ms240:
      t_reordering = pdcp_t_reordering::ms240;
      break;
    case asn1::e1ap::t_reordering_e::ms260:
      t_reordering = pdcp_t_reordering::ms260;
      break;
    case asn1::e1ap::t_reordering_e::ms280:
      t_reordering = pdcp_t_reordering::ms280;
      break;
    case asn1::e1ap::t_reordering_e::ms300:
      t_reordering = pdcp_t_reordering::ms300;
      break;
    case asn1::e1ap::t_reordering_e::ms500:
      t_reordering = pdcp_t_reordering::ms500;
      break;
    case asn1::e1ap::t_reordering_e::ms750:
      t_reordering = pdcp_t_reordering::ms750;
      break;
    case asn1::e1ap::t_reordering_e::ms1000:
      t_reordering = pdcp_t_reordering::ms1000;
      break;
    case asn1::e1ap::t_reordering_e::ms1250:
      t_reordering = pdcp_t_reordering::ms1250;
      break;
    case asn1::e1ap::t_reordering_e::ms1500:
      t_reordering = pdcp_t_reordering::ms1500;
      break;
    case asn1::e1ap::t_reordering_e::ms1750:
      t_reordering = pdcp_t_reordering::ms1750;
      break;
    case asn1::e1ap::t_reordering_e::ms2000:
      t_reordering = pdcp_t_reordering::ms2000;
      break;
    case asn1::e1ap::t_reordering_e::ms2250:
      t_reordering = pdcp_t_reordering::ms2250;
      break;
    case asn1::e1ap::t_reordering_e::ms2500:
      t_reordering = pdcp_t_reordering::ms2500;
      break;
    case asn1::e1ap::t_reordering_e::ms2750:
      t_reordering = pdcp_t_reordering::ms2750;
      break;
    case asn1::e1ap::t_reordering_e::ms3000:
      t_reordering = pdcp_t_reordering::ms3000;
      break;
    default:
      srsran_assert(false, "Unsupported PDCP t-reordering timer. PDCP t-reordering timer={}", asn1_t_reordering);
  }

  return t_reordering;
}

inline asn1::e1ap::t_reordering_e pdcp_t_reordering_to_asn1(pdcp_t_reordering t_reordering)
{
  asn1::e1ap::t_reordering_e asn1_t_reordering = {};

  switch (t_reordering) {
    case pdcp_t_reordering::ms0:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms0;
      break;
    case pdcp_t_reordering::ms1:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms1;
      break;
    case pdcp_t_reordering::ms2:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms2;
      break;
    case pdcp_t_reordering::ms4:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms4;
      break;
    case pdcp_t_reordering::ms5:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms5;
      break;
    case pdcp_t_reordering::ms8:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms8;
      break;
    case pdcp_t_reordering::ms10:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms10;
      break;
    case pdcp_t_reordering::ms15:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms15;
      break;
    case pdcp_t_reordering::ms20:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms20;
      break;
    case pdcp_t_reordering::ms30:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms30;
      break;
    case pdcp_t_reordering::ms40:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms40;
      break;
    case pdcp_t_reordering::ms50:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms50;
      break;
    case pdcp_t_reordering::ms60:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms60;
      break;
    case pdcp_t_reordering::ms80:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms80;
      break;
    case pdcp_t_reordering::ms100:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms100;
      break;
    case pdcp_t_reordering::ms120:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms120;
      break;
    case pdcp_t_reordering::ms140:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms140;
      break;
    case pdcp_t_reordering::ms160:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms160;
      break;
    case pdcp_t_reordering::ms180:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms180;
      break;
    case pdcp_t_reordering::ms200:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms200;
      break;
    case pdcp_t_reordering::ms220:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms220;
      break;
    case pdcp_t_reordering::ms240:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms240;
      break;
    case pdcp_t_reordering::ms260:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms260;
      break;
    case pdcp_t_reordering::ms280:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms280;
      break;
    case pdcp_t_reordering::ms300:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms300;
      break;
    case pdcp_t_reordering::ms500:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms500;
      break;
    case pdcp_t_reordering::ms750:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms750;
      break;
    case pdcp_t_reordering::ms1000:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms1000;
      break;
    case pdcp_t_reordering::ms1250:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms1250;
      break;
    case pdcp_t_reordering::ms1500:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms1500;
      break;
    case pdcp_t_reordering::ms1750:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms1750;
      break;
    case pdcp_t_reordering::ms2000:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms2000;
      break;
    case pdcp_t_reordering::ms2250:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms2250;
      break;
    case pdcp_t_reordering::ms2500:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms2500;
      break;
    case pdcp_t_reordering::ms2750:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms2750;
      break;
    case pdcp_t_reordering::ms3000:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::ms3000;
      break;
    case pdcp_t_reordering::infinity:
      asn1_t_reordering = asn1::e1ap::t_reordering_e::nulltype;
      break;
    default:
      srsran_assert(false, "Unsupported PDCP t-reordering timer. PDCP t-reordering timer={}", t_reordering);
  }

  return asn1_t_reordering;
}

/// \brief Converts type \c e1ap_pdcp_config to an E1AP ASN.1 type.
/// \param pdcp_cfg pdcp config object.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
inline asn1::e1ap::pdcp_cfg_s pdcp_config_to_e1ap_asn1(e1ap_pdcp_config pdcp_cfg)
{
  asn1::e1ap::pdcp_cfg_s asn1_pdcp_cfg;

  // pdcp sn size dl
  asn1_pdcp_cfg.pdcp_sn_size_dl = pdcp_sn_size_to_asn1(pdcp_cfg.pdcp_sn_size_dl);

  // pdcp sn size ul
  asn1_pdcp_cfg.pdcp_sn_size_ul = pdcp_sn_size_to_asn1(pdcp_cfg.pdcp_sn_size_ul);

  // rlc mode
  asn1_pdcp_cfg.rlc_mode = rlc_mode_to_asn1(pdcp_cfg.rlc_mod);

  // rohc params
  if (pdcp_cfg.rohc_params.has_value()) {
    asn1_pdcp_cfg.rohc_params_present = true;
    if (pdcp_cfg.rohc_params.value().rohc.has_value()) {
      asn1_pdcp_cfg.rohc_params.set_rohc();
      auto& e1ap_rohc         = asn1_pdcp_cfg.rohc_params.rohc();
      e1ap_rohc.max_c_id      = pdcp_cfg.rohc_params.value().rohc.value().max_cid;
      e1ap_rohc.rohc_profiles = pdcp_cfg.rohc_params.value().rohc.value().rohc_profiles;
      if (pdcp_cfg.rohc_params.value().rohc.value().continue_rohc.has_value()) {
        e1ap_rohc.continue_rohc_present = true;
        if (pdcp_cfg.rohc_params.value().rohc.value().continue_rohc.value()) {
          e1ap_rohc.continue_rohc = asn1::e1ap::rohc_s::continue_rohc_opts::options::true_value;
        } else {
          e1ap_rohc.continue_rohc = asn1::e1ap::rohc_s::continue_rohc_opts::options::nulltype;
        }
      }
    } else if (pdcp_cfg.rohc_params.value().ul_only_rohc.has_value()) {
      asn1_pdcp_cfg.rohc_params.set_ul_only_rohc();
      auto& e1ap_rohc         = asn1_pdcp_cfg.rohc_params.ul_only_rohc();
      e1ap_rohc.max_c_id      = pdcp_cfg.rohc_params.value().ul_only_rohc.value().max_cid;
      e1ap_rohc.rohc_profiles = pdcp_cfg.rohc_params.value().ul_only_rohc.value().rohc_profiles;
      if (pdcp_cfg.rohc_params.value().ul_only_rohc.value().continue_rohc.has_value()) {
        e1ap_rohc.continue_rohc_present = true;
        if (pdcp_cfg.rohc_params.value().ul_only_rohc.value().continue_rohc.value()) {
          e1ap_rohc.continue_rohc = asn1::e1ap::ul_only_rohc_s::continue_rohc_opts::options::true_value;
        } else {
          e1ap_rohc.continue_rohc = asn1::e1ap::ul_only_rohc_s::continue_rohc_opts::options::nulltype;
        }
      }
    } else {
      asn1_pdcp_cfg.rohc_params_present = false;
    }
  }

  // t reordering timer
  if (pdcp_cfg.t_reordering_timer.has_value()) {
    asn1_pdcp_cfg.t_reordering_timer_present = true;

    asn1_pdcp_cfg.t_reordering_timer.t_reordering = pdcp_t_reordering_to_asn1(pdcp_cfg.t_reordering_timer.value());
  }

  // discard timer
  if (pdcp_cfg.discard_timer.has_value() && pdcp_cfg.discard_timer != pdcp_discard_timer::not_configured) {
    asn1_pdcp_cfg.discard_timer_present = true;
    asn1_pdcp_cfg.discard_timer         = pdcp_discard_timer_to_asn1(pdcp_cfg.discard_timer.value());
  }

  // ul data split thres
  if (pdcp_cfg.ul_data_split_thres.has_value()) {
    asn1_pdcp_cfg.ul_data_split_thres_present = true;
    asn1::number_to_enum(asn1_pdcp_cfg.ul_data_split_thres, pdcp_cfg.ul_data_split_thres.value());
  }

  // pdcp dupl
  if (pdcp_cfg.pdcp_dupl.has_value()) {
    asn1_pdcp_cfg.pdcp_dupl_present = true;
    if (pdcp_cfg.pdcp_dupl.value()) {
      asn1_pdcp_cfg.pdcp_dupl = asn1::e1ap::pdcp_dupl_opts::options::true_value;
    } else {
      asn1_pdcp_cfg.pdcp_dupl = asn1::e1ap::pdcp_dupl_opts::options::nulltype;
    }
  }

  // pdcp reest
  if (pdcp_cfg.pdcp_reest.has_value()) {
    asn1_pdcp_cfg.pdcp_reest_present = true;
    if (pdcp_cfg.pdcp_reest.value()) {
      asn1_pdcp_cfg.pdcp_reest = asn1::e1ap::pdcp_reest_opts::options::true_value;
    } else {
      asn1_pdcp_cfg.pdcp_reest = asn1::e1ap::pdcp_reest_opts::options::nulltype;
    }
  }

  // pdcp data recovery
  if (pdcp_cfg.pdcp_data_recovery.has_value()) {
    asn1_pdcp_cfg.pdcp_data_recovery_present = true;
    if (pdcp_cfg.pdcp_data_recovery.value()) {
      asn1_pdcp_cfg.pdcp_data_recovery = asn1::e1ap::pdcp_data_recovery_opts::options::true_value;
    } else {
      asn1_pdcp_cfg.pdcp_data_recovery = asn1::e1ap::pdcp_data_recovery_opts::options::nulltype;
    }
  }

  // dupl activation
  if (pdcp_cfg.dupl_activation.has_value()) {
    asn1_pdcp_cfg.dupl_activation_present = true;
    asn1::string_to_enum(asn1_pdcp_cfg.dupl_activation, pdcp_cfg.dupl_activation.value());
  }

  // out of orfder delivery
  if (pdcp_cfg.out_of_order_delivery.has_value()) {
    asn1_pdcp_cfg.out_of_order_delivery_present = true;
    if (pdcp_cfg.out_of_order_delivery.value()) {
      asn1_pdcp_cfg.out_of_order_delivery = asn1::e1ap::out_of_order_delivery_opts::options::true_value;
    } else {
      asn1_pdcp_cfg.out_of_order_delivery = asn1::e1ap::out_of_order_delivery_opts::options::nulltype;
    }
  }

  return asn1_pdcp_cfg;
}

/// \brief Converts E1AP ASN.1 type to an \c e1ap_pdcp_config type.
/// \param asn1_pdcp_cfg E1AP ASN.1 object.
/// \return The pdcp config object where the result of the conversion is stored.
inline e1ap_pdcp_config e1ap_asn1_to_pdcp_config(asn1::e1ap::pdcp_cfg_s asn1_pdcp_cfg)
{
  e1ap_pdcp_config pdcp_cfg;

  // pdcp sn size dl
  pdcp_cfg.pdcp_sn_size_dl = asn1_to_pdcp_sn_size(asn1_pdcp_cfg.pdcp_sn_size_dl);

  // pdcp sn size ul
  pdcp_cfg.pdcp_sn_size_ul = asn1_to_pdcp_sn_size(asn1_pdcp_cfg.pdcp_sn_size_ul);

  // rlc mode
  pdcp_cfg.rlc_mod = asn1_to_rlc_mode(asn1_pdcp_cfg.rlc_mode);

  // rohc params
  if (asn1_pdcp_cfg.rohc_params_present) {
    if (asn1_pdcp_cfg.rohc_params.type().value == asn1::e1ap::rohc_params_c::types_opts::rohc) {
      e1ap_rohc_params rohc_params = {};
      e1ap_rohc        rohc        = {};

      rohc.max_cid       = asn1_pdcp_cfg.rohc_params.rohc().max_c_id;
      rohc.rohc_profiles = asn1_pdcp_cfg.rohc_params.rohc().rohc_profiles;
      if (asn1_pdcp_cfg.rohc_params.rohc().continue_rohc_present) {
        if (asn1_pdcp_cfg.rohc_params.rohc().continue_rohc ==
            asn1::e1ap::rohc_s::continue_rohc_opts::options::true_value) {
          rohc.continue_rohc = true;
        } else {
          rohc.continue_rohc = false;
        }
      }

      rohc_params.rohc     = rohc;
      pdcp_cfg.rohc_params = rohc_params;
    } else if (asn1_pdcp_cfg.rohc_params.type().value == asn1::e1ap::rohc_params_c::types_opts::ul_only_rohc) {
      e1ap_rohc_params rohc_params = {};
      e1ap_rohc        ul_only_rohc;

      ul_only_rohc.max_cid       = asn1_pdcp_cfg.rohc_params.ul_only_rohc().max_c_id;
      ul_only_rohc.rohc_profiles = asn1_pdcp_cfg.rohc_params.ul_only_rohc().rohc_profiles;

      if (asn1_pdcp_cfg.rohc_params.ul_only_rohc().continue_rohc_present) {
        if (asn1_pdcp_cfg.rohc_params.ul_only_rohc().continue_rohc ==
            asn1::e1ap::ul_only_rohc_s::continue_rohc_opts::options::true_value) {
          ul_only_rohc.continue_rohc = true;
        } else {
          ul_only_rohc.continue_rohc = false;
        }
      }

      rohc_params.ul_only_rohc = ul_only_rohc;
      pdcp_cfg.rohc_params     = rohc_params;
    }
  }

  // t reordering timer
  if (asn1_pdcp_cfg.t_reordering_timer_present) {
    pdcp_cfg.t_reordering_timer = asn1_to_pdcp_t_reordering(asn1_pdcp_cfg.t_reordering_timer.t_reordering);
  }

  // discard timer
  if (asn1_pdcp_cfg.discard_timer_present) {
    pdcp_cfg.discard_timer = asn1_to_pdcp_discard_timer(asn1_pdcp_cfg.discard_timer);
  }

  // ul data split thres
  if (asn1_pdcp_cfg.ul_data_split_thres_present) {
    pdcp_cfg.ul_data_split_thres = asn1_pdcp_cfg.ul_data_split_thres.to_number();
  }

  // pdcp dupl
  if (asn1_pdcp_cfg.pdcp_dupl_present) {
    if (asn1_pdcp_cfg.pdcp_dupl == asn1::e1ap::pdcp_dupl_opts::options::true_value) {
      pdcp_cfg.pdcp_dupl = true;
    } else {
      pdcp_cfg.pdcp_dupl = false;
    }
  }

  // pdcp reest
  if (asn1_pdcp_cfg.pdcp_reest_present) {
    if (asn1_pdcp_cfg.pdcp_reest == asn1::e1ap::pdcp_reest_opts::options::true_value) {
      pdcp_cfg.pdcp_reest = true;
    } else {
      pdcp_cfg.pdcp_reest = false;
    }
  }

  // pdcp data recovery
  if (asn1_pdcp_cfg.pdcp_data_recovery_present) {
    if (asn1_pdcp_cfg.pdcp_data_recovery == asn1::e1ap::pdcp_data_recovery_opts::options::true_value) {
      pdcp_cfg.pdcp_data_recovery = true;
    } else {
      pdcp_cfg.pdcp_data_recovery = false;
    }
  }

  // dupl activation
  if (asn1_pdcp_cfg.dupl_activation_present) {
    pdcp_cfg.dupl_activation = asn1_pdcp_cfg.dupl_activation.to_string();
  }

  // out of orfder delivery
  if (asn1_pdcp_cfg.out_of_order_delivery_present) {
    if (asn1_pdcp_cfg.out_of_order_delivery == asn1::e1ap::out_of_order_delivery_opts::options::true_value) {
      pdcp_cfg.out_of_order_delivery = true;
    } else {
      pdcp_cfg.out_of_order_delivery = false;
    }
  }

  return pdcp_cfg;
}

/// \brief Convert E1AP Cause to \c cause_t type.
/// \param e1ap_cause The E1AP Cause.
/// \return The cause type.
inline cause_t e1ap_cause_to_cause(asn1::e1ap::cause_c e1ap_cause)
{
  cause_t cause;

  switch (e1ap_cause.type()) {
    case asn1::e1ap::cause_c::types_opts::radio_network:
      cause = cause_t::radio_network;
      break;
    case asn1::e1ap::cause_c::types_opts::transport:
      cause = cause_t::transport;
      break;
    case asn1::e1ap::cause_c::types_opts::protocol:
      cause = cause_t::protocol;
      break;
    case asn1::e1ap::cause_c::types_opts::misc:
      cause = cause_t::misc;
      break;
    default:
      cause = cause_t::nulltype;
  }

  return cause;
}

/// \brief Convert \c cause_t type to E1AP cause.
/// \param cause The cause_t type.
/// \return The E1AP cause.
inline asn1::e1ap::cause_c cause_to_e1ap_cause(cause_t cause)
{
  asn1::e1ap::cause_c e1ap_cause;

  switch (cause) {
    case cause_t::radio_network:
      e1ap_cause.set(asn1::e1ap::cause_c::types_opts::radio_network);
      break;
    case cause_t::transport:
      e1ap_cause.set(asn1::e1ap::cause_c::types_opts::transport);
      break;
    case cause_t::protocol:
      e1ap_cause.set(asn1::e1ap::cause_c::types_opts::protocol);
      break;
    case cause_t::misc:
      e1ap_cause.set(asn1::e1ap::cause_c::types_opts::misc);
      break;
    default:
      e1ap_cause.set(asn1::e1ap::cause_c::types_opts::nulltype);
      break;
  }

  return e1ap_cause;
}

/// \brief Convert E1AP NG DL UP Unchanged to its boolean representation
/// \param ng_dl_up_unchanged The E1AP NG DL UP Unchanged.
/// \return The boolean representation of E1AP NG DL UP Unchanged.
inline bool
e1ap_asn1_ng_dl_up_unchanged_to_bool(asn1::e1ap::pdu_session_res_setup_item_s::ng_dl_up_unchanged_e_ ng_dl_up_unchanged)
{
  return ng_dl_up_unchanged.value ==
         asn1::e1ap::pdu_session_res_setup_item_s::ng_dl_up_unchanged_opts::options::true_value;
}

inline e1ap_pdcp_count e1ap_asn1_pdcp_count_to_pdcp_count(asn1::e1ap::pdcp_count_s asn1_pdcp_count)
{
  e1ap_pdcp_count pdcp_count;

  pdcp_count.hfn     = asn1_pdcp_count.hfn;
  pdcp_count.pdcp_sn = asn1_pdcp_count.pdcp_sn;

  return pdcp_count;
}

inline nr_cell_global_id_t cgi_from_e1ap_asn1(const asn1::e1ap::nr_cgi_s& asn1_cgi)
{
  nr_cell_global_id_t cgi          = {};
  uint32_t            encoded_plmn = asn1_cgi.plmn_id.to_number();
  ngap_plmn_to_mccmnc(encoded_plmn, &cgi.mcc, &cgi.mnc);

  std::string mcc_string, mnc_string;
  mcc_to_string(cgi.mcc, &mcc_string);
  mnc_to_string(cgi.mnc, &mnc_string);
  cgi.plmn = mcc_string + mnc_string;

  // Set PLMN hex string
  cgi.plmn_hex = asn1_cgi.plmn_id.to_string();

  cgi.nci.packed = asn1_cgi.nr_cell_id.to_number();

  cgi.nci.cell_idenity = (asn1_cgi.nr_cell_id.to_number() & 0xfffc0000); // bits 4 to 14
  cgi.nci.gnb_identity = (asn1_cgi.nr_cell_id.to_number() & 0X3ff0);     // bits 22 to 32

  return cgi;
}

} // namespace srsran
