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

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e1ap/e1ap_ies.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "srsran/ran/bcd_helpers.h"
#include "srsran/ran/cause/e1ap_cause.h"
#include "srsran/ran/qos_prio_level.h"
#include "srsran/support/error_handling.h"
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

/// \brief Convert \c nr_cell_global_id_t to E1AP ASN.1.
/// \param[in] nr_cgi The common type nr cgi.
/// \return The ASN.1 nr cgi.
inline asn1::e1ap::nr_cgi_s nr_cgi_to_e1ap_asn1(const nr_cell_global_id_t& nr_cgi)
{
  asn1::e1ap::nr_cgi_s asn1_nr_cgi;

  // nr cell id
  asn1_nr_cgi.nr_cell_id.from_number(nr_cgi.nci);

  // plmn id
  asn1_nr_cgi.plmn_id.from_string(nr_cgi.plmn_hex);

  return asn1_nr_cgi;
}

inline asn1::e1ap::sdap_hdr_ul_opts::options sdap_hdr_ul_cfg_to_e1ap_asn1(sdap_hdr_ul_cfg hdr_cfg)
{
  asn1::e1ap::sdap_hdr_ul_opts::options asn1_hdr_ul_opts;

  if (hdr_cfg == sdap_hdr_ul_cfg::absent) {
    asn1_hdr_ul_opts = asn1::e1ap::sdap_hdr_ul_opts::absent;
  } else {
    asn1_hdr_ul_opts = asn1::e1ap::sdap_hdr_ul_opts::present;
  }

  return asn1_hdr_ul_opts;
}

inline asn1::e1ap::sdap_hdr_dl_opts::options sdap_hdr_dl_cfg_to_e1ap_asn1(sdap_hdr_dl_cfg hdr_cfg)
{
  asn1::e1ap::sdap_hdr_dl_opts::options asn1_hdr_dl_opts;

  if (hdr_cfg == sdap_hdr_dl_cfg::absent) {
    asn1_hdr_dl_opts = asn1::e1ap::sdap_hdr_dl_opts::absent;
  } else {
    asn1_hdr_dl_opts = asn1::e1ap::sdap_hdr_dl_opts::present;
  }

  return asn1_hdr_dl_opts;
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

  asn1_sdap_cfg.sdap_hdr_ul.value = sdap_hdr_ul_cfg_to_e1ap_asn1(sdap_cfg.sdap_hdr_ul);
  asn1_sdap_cfg.sdap_hdr_dl.value = sdap_hdr_dl_cfg_to_e1ap_asn1(sdap_cfg.sdap_hdr_dl);

  return asn1_sdap_cfg;
}

inline sdap_hdr_ul_cfg e1ap_asn1_to_sdap_hdr_ul_cfg(asn1::e1ap::sdap_hdr_ul_opts::options asn1_hdr_ul_opts)
{
  sdap_hdr_ul_cfg hdr_cfg;

  switch (asn1_hdr_ul_opts) {
    case asn1::e1ap::sdap_hdr_ul_opts::absent:
      hdr_cfg = sdap_hdr_ul_cfg::absent;
      break;
    case asn1::e1ap::sdap_hdr_ul_opts::present:
      hdr_cfg = sdap_hdr_ul_cfg::present;
      break;
    default:
      srsran_assertion_failure("Invalid SDAP-Header-UL option ({})", asn1_hdr_ul_opts);
      hdr_cfg = {};
  }

  return hdr_cfg;
}

inline sdap_hdr_dl_cfg e1ap_asn1_to_sdap_hdr_dl_cfg(asn1::e1ap::sdap_hdr_dl_opts::options asn1_hdr_dl_opts)
{
  sdap_hdr_dl_cfg hdr_cfg;

  switch (asn1_hdr_dl_opts) {
    case asn1::e1ap::sdap_hdr_dl_opts::absent:
      hdr_cfg = sdap_hdr_dl_cfg::absent;
      break;
    case asn1::e1ap::sdap_hdr_dl_opts::present:
      hdr_cfg = sdap_hdr_dl_cfg::present;
      break;
    default:
      srsran_assertion_failure("Invalid SDAP-Header-DL option ({})", asn1_hdr_dl_opts);
      hdr_cfg = {};
  }

  return hdr_cfg;
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

  sdap_cfg.sdap_hdr_dl = e1ap_asn1_to_sdap_hdr_dl_cfg(asn1_sdap_cfg.sdap_hdr_dl);
  sdap_cfg.sdap_hdr_ul = e1ap_asn1_to_sdap_hdr_ul_cfg(asn1_sdap_cfg.sdap_hdr_ul);

  return sdap_cfg;
}

/// \brief Converts \c rlc_mode type to an E1AP ASN.1 type.
/// \param rlc_mod rlc_mode type.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
inline asn1::e1ap::rlc_mode_e rlc_mode_to_asn1(srsran::pdcp_rlc_mode rlc_mod)
{
  asn1::e1ap::rlc_mode_e asn1_rlc_mode = {};

  switch (rlc_mod) {
    case srsran::pdcp_rlc_mode::um:
      asn1_rlc_mode = asn1::e1ap::rlc_mode_opts::rlc_um_bidirectional;
      break;
    case srsran::pdcp_rlc_mode::am:
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
inline srsran::pdcp_rlc_mode asn1_to_rlc_mode(asn1::e1ap::rlc_mode_e asn1_rlc_mod)
{
  srsran::pdcp_rlc_mode rlc_mode = {};

  switch (asn1_rlc_mod) {
    case asn1::e1ap::rlc_mode_opts::rlc_um_bidirectional:
    case asn1::e1ap::rlc_mode_opts::rlc_um_unidirectional_ul:
    case asn1::e1ap::rlc_mode_opts::rlc_um_unidirectional_dl:
      rlc_mode = srsran::pdcp_rlc_mode::um;
      break;
    case asn1::e1ap::rlc_mode_opts::rlc_am:
      rlc_mode = srsran::pdcp_rlc_mode::am;
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
    asn1_pdcp_cfg.t_reordering_timer_present      = true;
    asn1_pdcp_cfg.t_reordering_timer.t_reordering = pdcp_t_reordering_to_asn1(pdcp_cfg.t_reordering_timer.value());
  }

  // discard timer
  if (pdcp_cfg.discard_timer.has_value()) {
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

/// \brief Convert E1AP Cause to \c e1ap_cause_t type.
/// \param e1ap_cause The E1AP Cause.
/// \return The cause type.
inline e1ap_cause_t asn1_to_cause(asn1::e1ap::cause_c e1ap_cause)
{
  e1ap_cause_t cause;

  switch (e1ap_cause.type()) {
    case asn1::e1ap::cause_c::types_opts::radio_network:
      cause = static_cast<e1ap_cause_radio_network_t>(e1ap_cause.radio_network().value);
      break;
    case asn1::e1ap::cause_c::types_opts::transport:
      cause = static_cast<e1ap_cause_transport_t>(e1ap_cause.transport().value);
      break;
    case asn1::e1ap::cause_c::types_opts::protocol:
      cause = static_cast<cause_protocol_t>(e1ap_cause.protocol().value);
      break;
    case asn1::e1ap::cause_c::types_opts::misc:
      cause = static_cast<cause_misc_t>(e1ap_cause.misc().value);
      break;
    default:
      report_fatal_error("Cannot convert E1AP ASN.1 cause {} to common type", e1ap_cause.type());
  }

  return cause;
}

/// \brief Convert \c e1ap_cause_t type to E1AP ASN.1 cause.
/// \param cause The e1ap_cause_t type.
/// \return The E1AP ASN.1 cause.
inline asn1::e1ap::cause_c cause_to_asn1(e1ap_cause_t cause)
{
  asn1::e1ap::cause_c asn1_cause;

  if (variant_holds_alternative<e1ap_cause_radio_network_t>(cause)) {
    asn1_cause.set_radio_network() =
        static_cast<asn1::e1ap::cause_radio_network_opts::options>(variant_get<e1ap_cause_radio_network_t>(cause));
  } else if (variant_holds_alternative<e1ap_cause_transport_t>(cause)) {
    asn1_cause.set_transport() =
        static_cast<asn1::e1ap::cause_transport_opts::options>(variant_get<e1ap_cause_transport_t>(cause));
  } else if (variant_holds_alternative<cause_protocol_t>(cause)) {
    asn1_cause.set_protocol() =
        static_cast<asn1::e1ap::cause_protocol_opts::options>(variant_get<cause_protocol_t>(cause));
  } else if (variant_holds_alternative<cause_misc_t>(cause)) {
    asn1_cause.set_misc() = static_cast<asn1::e1ap::cause_misc_opts::options>(variant_get<cause_misc_t>(cause));
  } else {
    report_fatal_error("Cannot convert cause to E1AP type: {}", cause);
  }

  return asn1_cause;
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

/// \brief Convert E1AP ASN.1 PDCP Count to \c e1ap_pdcp_count type.
inline e1ap_pdcp_count e1ap_asn1_pdcp_count_to_pdcp_count(asn1::e1ap::pdcp_count_s asn1_pdcp_count)
{
  e1ap_pdcp_count pdcp_count;

  pdcp_count.hfn     = asn1_pdcp_count.hfn;
  pdcp_count.pdcp_sn = asn1_pdcp_count.pdcp_sn;

  return pdcp_count;
}

/// \brief Convert E1AP ASN.1 CGI to \c nr_cell_global_id_t type.
inline nr_cell_global_id_t e1ap_asn1_to_cgi(const asn1::e1ap::nr_cgi_s& asn1_cgi)
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

  cgi.nci = asn1_cgi.nr_cell_id.to_number();

  return cgi;
}

/// \brief Convert E1AP ASN.1 QoS Flow Map Item to \c e1ap_qos_flow_map_item type.
inline e1ap_qos_flow_map_item
asn1_e1ap_to_qos_flow_map_item(const asn1::e1ap::qos_flow_map_item_s& asn1_qos_flow_map_item)
{
  e1ap_qos_flow_map_item qos_flow_map_item;
  qos_flow_map_item.qos_flow_id = uint_to_qos_flow_id(asn1_qos_flow_map_item.qos_flow_id);

  if (asn1_qos_flow_map_item.qos_flow_map_ind_present) {
    qos_flow_map_item.qos_flow_map_ind = asn1_qos_flow_map_item.qos_flow_map_ind.to_string();
  }

  return qos_flow_map_item;
}

/// \brief Convert E1AP ASN.1 Cell Group Info to \c std::vector<e1ap_cell_group_info_item> type.
inline void e1ap_asn1_to_cell_group_info(std::vector<e1ap_cell_group_info_item>& cell_group_info,
                                         const asn1::e1ap::cell_group_info_l&    asn1_cell_group_info)
{
  for (const auto& asn1_cell_group_info_item : asn1_cell_group_info) {
    e1ap_cell_group_info_item cell_group_info_item;
    cell_group_info_item.cell_group_id = asn1_cell_group_info_item.cell_group_id;

    if (asn1_cell_group_info_item.ul_cfg_present) {
      cell_group_info_item.ul_cfg = asn1_cell_group_info_item.ul_cfg.to_string();
    }

    if (asn1_cell_group_info_item.dl_tx_stop_present) {
      cell_group_info_item.dl_tx_stop = asn1_cell_group_info_item.dl_tx_stop.to_string();
    }

    if (asn1_cell_group_info_item.rat_type_present) {
      cell_group_info_item.rat_type = asn1_cell_group_info_item.rat_type.to_string();
    }

    cell_group_info.push_back(cell_group_info_item);
  }
}

/// \brief Convert E1AP ASN.1 QoS Flow Map Info to \c slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_qos_param_item>
/// type.
inline void e1ap_asn1_to_flow_map_info(slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_qos_param_item>& flow_map_info,
                                       const asn1::e1ap::qos_flow_qos_param_list_l& ans1_flow_map_list)
{
  for (const auto& asn1_flow_map_item : ans1_flow_map_list) {
    e1ap_qos_flow_qos_param_item flow_map_item;
    flow_map_item.qos_flow_id = uint_to_qos_flow_id(asn1_flow_map_item.qos_flow_id);
    // Add qos flow level params

    // Add qos characteristics
    if (asn1_flow_map_item.qos_flow_level_qos_params.qos_characteristics.type() ==
        asn1::e1ap::qos_characteristics_c::types_opts::dyn_5qi) {
      auto& asn1_dyn_5qi = asn1_flow_map_item.qos_flow_level_qos_params.qos_characteristics.dyn_5qi();

      dyn_5qi_descriptor_t dyn_5qi;
      dyn_5qi.qos_prio_level                 = uint_to_qos_prio_level(asn1_dyn_5qi.qos_prio_level);
      dyn_5qi.packet_delay_budget            = asn1_dyn_5qi.packet_delay_budget;
      dyn_5qi.packet_error_rate.per_exponent = asn1_dyn_5qi.packet_error_rate.per_exponent;
      dyn_5qi.packet_error_rate.per_scalar   = asn1_dyn_5qi.packet_error_rate.per_scalar;

      if (asn1_dyn_5qi.five_qi_present) {
        dyn_5qi.five_qi = uint_to_five_qi(asn1_dyn_5qi.five_qi);
      }
      if (asn1_dyn_5qi.delay_crit_present) {
        dyn_5qi.delay_crit = asn1_dyn_5qi.delay_crit.to_string();
      }
      if (asn1_dyn_5qi.averaging_win_present) {
        dyn_5qi.averaging_win = asn1_dyn_5qi.averaging_win;
      }
      if (asn1_dyn_5qi.max_data_burst_volume_present) {
        dyn_5qi.max_data_burst_volume = asn1_dyn_5qi.max_data_burst_volume;
      }

      flow_map_item.qos_flow_level_qos_params.qos_characteristics.dyn_5qi = dyn_5qi;
    } else {
      auto& asn1_non_dyn_5qi = asn1_flow_map_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi();

      non_dyn_5qi_descriptor_t non_dyn_5qi;
      non_dyn_5qi.five_qi = uint_to_five_qi(asn1_non_dyn_5qi.five_qi);
      if (asn1_non_dyn_5qi.qos_prio_level_present) {
        non_dyn_5qi.qos_prio_level = uint_to_qos_prio_level(asn1_non_dyn_5qi.qos_prio_level);
      }
      if (asn1_non_dyn_5qi.averaging_win_present) {
        non_dyn_5qi.averaging_win = asn1_non_dyn_5qi.averaging_win;
      }
      if (asn1_non_dyn_5qi.max_data_burst_volume_present) {
        non_dyn_5qi.max_data_burst_volume = asn1_non_dyn_5qi.max_data_burst_volume;
      }

      flow_map_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi = non_dyn_5qi;
    }
    // add ng ran alloc retention prio
    flow_map_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.prio_level =
        asn1_flow_map_item.qos_flow_level_qos_params.ngra_nalloc_retention_prio.prio_level;
    flow_map_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_cap =
        asn1_flow_map_item.qos_flow_level_qos_params.ngra_nalloc_retention_prio.pre_emption_cap.to_string();
    flow_map_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_vulnerability =
        asn1_flow_map_item.qos_flow_level_qos_params.ngra_nalloc_retention_prio.pre_emption_vulnerability.to_string();

    // Add gbr qos flow info
    if (asn1_flow_map_item.qos_flow_level_qos_params.gbr_qos_flow_info_present) {
      e1ap_gbr_qos_flow_info gbr_qos_flow_info;
      gbr_qos_flow_info.max_flow_bit_rate_dl =
          asn1_flow_map_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_flow_bit_rate_dl;
      gbr_qos_flow_info.max_flow_bit_rate_ul =
          asn1_flow_map_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_flow_bit_rate_ul;
      gbr_qos_flow_info.guaranteed_flow_bit_rate_dl =
          asn1_flow_map_item.qos_flow_level_qos_params.gbr_qos_flow_info.guaranteed_flow_bit_rate_dl;
      gbr_qos_flow_info.guaranteed_flow_bit_rate_ul =
          asn1_flow_map_item.qos_flow_level_qos_params.gbr_qos_flow_info.guaranteed_flow_bit_rate_ul;
      if (asn1_flow_map_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_packet_loss_rate_dl_present) {
        gbr_qos_flow_info.max_packet_loss_rate_dl =
            asn1_flow_map_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_packet_loss_rate_dl;
      }
      if (asn1_flow_map_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_packet_loss_rate_ul_present) {
        gbr_qos_flow_info.max_packet_loss_rate_ul =
            asn1_flow_map_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_packet_loss_rate_ul;
      }

      flow_map_item.qos_flow_level_qos_params.gbr_qos_flow_info = gbr_qos_flow_info;
    }

    // Add reflective qos attribute
    if (asn1_flow_map_item.qos_flow_level_qos_params.reflective_qos_attribute_present) {
      flow_map_item.qos_flow_level_qos_params.reflective_qos_attribute =
          asn1::enum_to_bool(asn1_flow_map_item.qos_flow_level_qos_params.reflective_qos_attribute);
    }

    // Add add qos info
    if (asn1_flow_map_item.qos_flow_level_qos_params.add_qos_info_present) {
      flow_map_item.qos_flow_level_qos_params.add_qos_info =
          asn1::enum_to_bool(asn1_flow_map_item.qos_flow_level_qos_params.add_qos_info);
    }

    // Add paging policy ind
    if (asn1_flow_map_item.qos_flow_level_qos_params.paging_policy_ind_present) {
      flow_map_item.qos_flow_level_qos_params.paging_policy_ind =
          asn1_flow_map_item.qos_flow_level_qos_params.paging_policy_ind;
    }

    // Add reflective qos ind
    if (asn1_flow_map_item.qos_flow_level_qos_params.reflective_qos_ind_present) {
      flow_map_item.qos_flow_level_qos_params.reflective_qos_ind =
          asn1::enum_to_bool(asn1_flow_map_item.qos_flow_level_qos_params.reflective_qos_ind);
    }

    // Add qos flow map ind
    if (asn1_flow_map_item.qos_flow_map_ind_present) {
      flow_map_item.qos_flow_map_ind = asn1_flow_map_item.qos_flow_map_ind.to_string();
    }
    flow_map_info.emplace(uint_to_qos_flow_id(asn1_flow_map_item.qos_flow_id), flow_map_item);
  }
}

/// \brief Convert E1AP ASN.1 Data Forwarding Info Request to \c e1ap_data_forwarding_info_request type
inline e1ap_data_forwarding_info_request e1ap_asn1_to_data_forwarding_info_request(
    const asn1::e1ap::data_forwarding_info_request_s& asn1_data_forwarding_info_request)
{
  e1ap_data_forwarding_info_request data_forwarding_info_request;

  // drb data forwarding info request
  data_forwarding_info_request.data_forwarding_request =
      asn1_data_forwarding_info_request.data_forwarding_request.to_string();

  for (const auto& asn1_qos_flows_forwarded_item :
       asn1_data_forwarding_info_request.qos_flows_forwarded_on_fwd_tunnels) {
    e1ap_qos_flow_map_item qos_flows_forwarded_item = asn1_e1ap_to_qos_flow_map_item(asn1_qos_flows_forwarded_item);

    data_forwarding_info_request.qos_flows_forwarded_on_fwd_tunnels.emplace(
        uint_to_qos_flow_id(asn1_qos_flows_forwarded_item.qos_flow_id), qos_flows_forwarded_item);
  }

  return data_forwarding_info_request;
}

/// \brief Convert E1AP ASN.1 PDCP SN Status Info to \c e1ap_pdcp_sn_status_info type
inline e1ap_pdcp_sn_status_info
e1ap_asn1_to_pdcp_sn_status_info(const asn1::e1ap::pdcp_sn_status_info_s& asn1_pdcp_sn_status_info)
{
  e1ap_pdcp_sn_status_info pdcp_sn_status_info = {};

  pdcp_sn_status_info.pdcp_status_transfer_ul.count_value.pdcp_sn =
      asn1_pdcp_sn_status_info.pdcp_status_transfer_ul.count_value.pdcp_sn;
  pdcp_sn_status_info.pdcp_status_transfer_ul.count_value.hfn =
      asn1_pdcp_sn_status_info.pdcp_status_transfer_ul.count_value.hfn;

  if (asn1_pdcp_sn_status_info.pdcp_status_transfer_ul.receive_statusof_pdcp_sdu_present) {
    pdcp_sn_status_info.pdcp_status_transfer_ul.receive_status_of_pdcp_sdu =
        asn1_pdcp_sn_status_info.pdcp_status_transfer_ul.receive_statusof_pdcp_sdu.to_number();
  }

  pdcp_sn_status_info.pdcp_status_transfer_dl.pdcp_sn = asn1_pdcp_sn_status_info.pdcp_status_transfer_dl.pdcp_sn;
  pdcp_sn_status_info.pdcp_status_transfer_dl.hfn     = asn1_pdcp_sn_status_info.pdcp_status_transfer_dl.hfn;

  return pdcp_sn_status_info;
}

template <typename template_asn1_item>
inline void
e1ap_drb_item_list_to_asn1(asn1::dyn_array<template_asn1_item>&                           asn1_drb_item_list,
                           const slotted_id_vector<drb_id_t, e1ap_drb_setup_item_ng_ran>& drb_setup_list_ng_ran)
{
  for (const auto& drb_setup_item_ng_ran : drb_setup_list_ng_ran) {
    template_asn1_item asn1_drb_setup_item;

    // Add DRB ID
    asn1_drb_setup_item.drb_id = drb_id_to_uint(drb_setup_item_ng_ran.drb_id);

    // Add UL UP Transport Params
    for (const auto& ul_up_transport_param : drb_setup_item_ng_ran.ul_up_transport_params) {
      asn1::e1ap::up_params_item_s asn1_up_transport_param;
      up_transport_layer_info_to_asn1(asn1_up_transport_param.up_tnl_info, ul_up_transport_param.up_tnl_info);
      asn1_drb_setup_item.ul_up_transport_params.push_back(asn1_up_transport_param);
    }

    // Add Flow setup List
    for (const auto& qos_flow_item : drb_setup_item_ng_ran.flow_setup_list) {
      asn1::e1ap::qos_flow_item_s asn1_flow_item;
      asn1_flow_item.qos_flow_id = qos_flow_id_to_uint(qos_flow_item.qos_flow_id);
      asn1_drb_setup_item.flow_setup_list.push_back(asn1_flow_item);
    }

    // Add Flow Failed List
    for (const auto& flow_failed_item : drb_setup_item_ng_ran.flow_failed_list) {
      asn1::e1ap::qos_flow_failed_item_s asn1_flow_failed_item;
      asn1_flow_failed_item.qos_flow_id = qos_flow_id_to_uint(flow_failed_item.qos_flow_id);
      asn1_drb_setup_item.flow_failed_list.push_back(asn1_flow_failed_item);
    }

    // Add DRB Data Forwarding Info Response
    if (drb_setup_item_ng_ran.drb_data_forwarding_info_resp.has_value()) {
      asn1_drb_setup_item.drb_data_forwarding_info_resp_present = true;
      if (drb_setup_item_ng_ran.drb_data_forwarding_info_resp.value().ul_data_forwarding.has_value()) {
        asn1_drb_setup_item.drb_data_forwarding_info_resp.ul_data_forwarding_present = true;
        up_transport_layer_info_to_asn1(
            asn1_drb_setup_item.drb_data_forwarding_info_resp.ul_data_forwarding,
            drb_setup_item_ng_ran.drb_data_forwarding_info_resp.value().ul_data_forwarding.value());
      }
      if (drb_setup_item_ng_ran.drb_data_forwarding_info_resp.value().dl_data_forwarding.has_value()) {
        asn1_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding_present = true;
        up_transport_layer_info_to_asn1(
            asn1_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding,
            drb_setup_item_ng_ran.drb_data_forwarding_info_resp.value().dl_data_forwarding.value());
      }
    }

    asn1_drb_item_list.push_back(asn1_drb_setup_item);
  }
}

template <typename template_asn1_item>
inline void e1ap_drb_failed_item_list_to_asn1(
    asn1::dyn_array<template_asn1_item>&                            asn1_drb_item_list,
    const slotted_id_vector<drb_id_t, e1ap_drb_failed_item_ng_ran>& drb_failed_list_ng_ran)
{
  for (const auto& drb_failed_item : drb_failed_list_ng_ran) {
    template_asn1_item asn1_drb_failed_item;
    // Add DRB ID
    asn1_drb_failed_item.drb_id = drb_id_to_uint(drb_failed_item.drb_id);
    // Add Cause
    asn1_drb_failed_item.cause = cause_to_asn1(drb_failed_item.cause);
    asn1_drb_item_list.push_back(asn1_drb_failed_item);
  }
}

inline activity_notification_level_t
asn1_to_activity_notification_level(const asn1::e1ap::activity_notif_level_e& asn1_activity_notification)
{
  activity_notification_level_t activity_notification_level;

  switch (asn1_activity_notification.value) {
    case asn1::e1ap::activity_notif_level_opts::options::drb:
      activity_notification_level = activity_notification_level_t::drb;
      break;
    case asn1::e1ap::activity_notif_level_opts::options::pdu_session:
      activity_notification_level = activity_notification_level_t::pdu_session;
      break;
    case asn1::e1ap::activity_notif_level_opts::options::ue:
      activity_notification_level = activity_notification_level_t::ue;
      break;
    default:
      activity_notification_level = activity_notification_level_t::invalid;
      break;
  }

  return activity_notification_level;
}

/// \brief Converts type \c security_result_t to an ASN.1 type.
/// \param[out] asn1obj ASN.1 object where the result of the conversion is stored.
/// \param[in] security_result Security Result IE contents.
inline void security_result_to_asn1(asn1::e1ap::security_result_s& asn1obj, const security_result_t& security_result)
{
  switch (security_result.integrity_protection_result) {
    case integrity_protection_result_t::performed:
    case integrity_protection_result_t::not_performed:
      asn1obj.integrity_protection_result.value = static_cast<asn1::e1ap::integrity_protection_result_opts::options>(
          security_result.integrity_protection_result);
      break;
    default:
      report_fatal_error("Cannot convert security result to E1AP type");
  }

  switch (security_result.confidentiality_protection_result) {
    case confidentiality_protection_result_t::performed:
    case confidentiality_protection_result_t::not_performed:
      asn1obj.confidentiality_protection_result.value =
          static_cast<asn1::e1ap::confidentiality_protection_result_opts::options>(
              security_result.confidentiality_protection_result);
      break;
    default:
      report_fatal_error("Cannot convert security result to E1AP type");
  }
}

/// \brief Converts ASN.1 type to \c security_result_t.
/// \param[out] security_result Security Result IE contents.
/// \param[in] asn1obj ASN.1 object from which the result is taken.
inline void asn1_to_security_result(security_result_t& security_result, const asn1::e1ap::security_result_s& asn1obj)
{
  switch (asn1obj.integrity_protection_result) {
    case asn1::e1ap::integrity_protection_result_opts::performed:
    case asn1::e1ap::integrity_protection_result_opts::not_performed:
      security_result.integrity_protection_result =
          static_cast<integrity_protection_result_t>(asn1obj.integrity_protection_result.value);
      break;
    default:
      srslog::fetch_basic_logger("E1AP").error("Cannot convert security result to E1AP type");
  }

  switch (asn1obj.confidentiality_protection_result) {
    case asn1::e1ap::confidentiality_protection_result_opts::performed:
    case asn1::e1ap::confidentiality_protection_result_opts::not_performed:
      security_result.confidentiality_protection_result =
          static_cast<confidentiality_protection_result_t>(asn1obj.confidentiality_protection_result.value);
      break;
    default:
      srslog::fetch_basic_logger("E1AP").error("Cannot convert security result to E1AP type");
  }
}

/// \brief Converts type \c security_indication to an ASN.1 type.
/// \param asn1obj ASN.1 object where the result of the conversion is stored.
/// \param security_indication Security Indication IE contents.
inline void security_indication_to_asn1(asn1::e1ap::security_ind_s& asn1obj, const security_indication_t& security_ind)
{
  switch (security_ind.integrity_protection_ind) {
    case integrity_protection_indication_t::not_needed:
    case integrity_protection_indication_t::preferred:
    case integrity_protection_indication_t::required:
      asn1obj.integrity_protection_ind.value =
          static_cast<asn1::e1ap::integrity_protection_ind_opts::options>(security_ind.integrity_protection_ind);
      break;
    default:
      report_fatal_error("Cannot convert security indication to E1AP type");
  }

  switch (security_ind.confidentiality_protection_ind) {
    case confidentiality_protection_indication_t::not_needed:
    case confidentiality_protection_indication_t::preferred:
    case confidentiality_protection_indication_t::required:
      asn1obj.confidentiality_protection_ind.value =
          static_cast<asn1::e1ap::confidentiality_protection_ind_opts::options>(
              security_ind.confidentiality_protection_ind);
      break;
    default:
      report_fatal_error("Cannot convert security indication to E1AP type");
  }
}

/// \brief Converts type \c security_indication to an ASN.1 type.
/// \param asn1obj ASN.1 object where the result of the conversion is stored.
/// \param security_indication Security Indication IE contents.
inline void asn1_to_security_indication(security_indication_t& security_ind, const asn1::e1ap::security_ind_s& asn1obj)
{
  switch (asn1obj.integrity_protection_ind) {
    case asn1::e1ap::integrity_protection_ind_opts::not_needed:
    case asn1::e1ap::integrity_protection_ind_opts::preferred:
    case asn1::e1ap::integrity_protection_ind_opts::required:
      security_ind.integrity_protection_ind =
          static_cast<integrity_protection_indication_t>(asn1obj.integrity_protection_ind.value);
      break;
    default:
      srslog::fetch_basic_logger("E1AP").error("Cannot convert security indication to E1AP type");
  }

  switch (asn1obj.confidentiality_protection_ind) {
    case asn1::e1ap::confidentiality_protection_ind_opts::not_needed:
    case asn1::e1ap::confidentiality_protection_ind_opts::preferred:
    case asn1::e1ap::confidentiality_protection_ind_opts::required:
      security_ind.confidentiality_protection_ind =
          static_cast<confidentiality_protection_indication_t>(asn1obj.confidentiality_protection_ind.value);
      break;
    default:
      srslog::fetch_basic_logger("E1AP").error("Cannot convert security indication to E1AP type");
  }
}

} // namespace srsran
