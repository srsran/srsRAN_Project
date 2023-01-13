/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/e1/cu_cp/e1_cu_cp.h"
#include "srsgnb/e1/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include <string>
#include <vector>

namespace srsgnb {
namespace srs_cu_cp {

/// \brief Converts type \c ciphering_algorithm to an E1AP ASN.1 type.
/// \param ciph_algo Cyphering Algorithm object.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
inline asn1::e1ap::ciphering_algorithm_e
ciphering_algorithm_to_e1ap_asn1(const srsgnb::security::ciphering_algorithm& ciph_algo)
{
  switch (ciph_algo) {
    case srsgnb::security::ciphering_algorithm::nea0:
      return asn1::e1ap::ciphering_algorithm_opts::nea0;
      break;
    case srsgnb::security::ciphering_algorithm::nea1:
      return asn1::e1ap::ciphering_algorithm_opts::c_neg128_nea1;
      break;
    case srsgnb::security::ciphering_algorithm::nea2:
      return asn1::e1ap::ciphering_algorithm_opts::c_neg128_nea2;
      break;
    case srsgnb::security::ciphering_algorithm::nea3:
      return asn1::e1ap::ciphering_algorithm_opts::c_neg128_nea3;
      break;
    default:
      return asn1::e1ap::ciphering_algorithm_opts::nulltype;
  }
}

/// \brief Converts type \c integrity_algorithm to an E1AP ASN.1 type.
/// \param int_algo Integrity Protection Algorithm object.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
inline asn1::e1ap::integrity_protection_algorithm_e
integrity_algorithm_to_e1ap_asn1(const srsgnb::security::integrity_algorithm& int_algo)
{
  switch (int_algo) {
    case srsgnb::security::integrity_algorithm::nia0:
      return asn1::e1ap::integrity_protection_algorithm_opts::nia0;
      break;
    case srsgnb::security::integrity_algorithm::nia1:
      return asn1::e1ap::integrity_protection_algorithm_opts::i_neg128_nia1;
      break;
    case srsgnb::security::integrity_algorithm::nia2:
      return asn1::e1ap::integrity_protection_algorithm_opts::i_neg128_nia2;
      break;
    case srsgnb::security::integrity_algorithm::nia3:
      return asn1::e1ap::integrity_protection_algorithm_opts::i_neg128_nia3;
      break;
    default:
      return asn1::e1ap::integrity_protection_algorithm_opts::nulltype;
  }
}

/// \brief Convert CU-CP s-NSSAI to E1AP ASN1 s-NSSAI.
/// \param cu_cp_snssai The CU-CP s-NSSAI.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
inline asn1::e1ap::snssai_s snssai_to_e1ap_asn1(srsgnb::srs_cu_cp::cu_cp_s_nssai cu_cp_snssai)
{
  asn1::e1ap::snssai_s snssai;
  snssai.sst.from_number(cu_cp_snssai.sst);
  if (cu_cp_snssai.sd.has_value()) {
    snssai.sd_present = true;
    snssai.sd.from_number(cu_cp_snssai.sd.value());
  }

  return snssai;
}

/// \brief Converts type \c sdap_config to an E1AP ASN.1 type.
/// \param sdap_cfg sdap config object.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
inline asn1::e1ap::sdap_cfg_s sdap_config_to_e1ap_asn1(cu_cp_sdap_config sdap_cfg)
{
  asn1::e1ap::sdap_cfg_s e1ap_sdap_cfg;

  if (sdap_cfg.default_drb) {
    e1ap_sdap_cfg.default_drb = asn1::e1ap::default_drb_opts::options::true_value;
  } else {
    e1ap_sdap_cfg.default_drb = asn1::e1ap::default_drb_opts::options::false_value;
  }

  asn1::string_to_enum(e1ap_sdap_cfg.sdap_hdr_dl, sdap_cfg.sdap_hdr_dl);
  asn1::string_to_enum(e1ap_sdap_cfg.sdap_hdr_ul, sdap_cfg.sdap_hdr_ul);

  return e1ap_sdap_cfg;
}

/// \brief Converts \c rlc_mode type to an E1AP ASN.1 type.
/// \param rlc_mod rlc_mode type.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
inline asn1::e1ap::rlc_mode_e rlc_mode_to_asn1(srsgnb::rlc_mode rlc_mod)
{
  asn1::e1ap::rlc_mode_e e1ap_rlc_mode;

  switch (rlc_mod) {
    case srsgnb::rlc_mode::tm:
      e1ap_rlc_mode = asn1::e1ap::rlc_mode_opts::rlc_tm;
      break;
    case srsgnb::rlc_mode::um_bidir:
      e1ap_rlc_mode = asn1::e1ap::rlc_mode_opts::rlc_um_bidirectional;
      break;
    case srsgnb::rlc_mode::um_unidir_ul:
      e1ap_rlc_mode = asn1::e1ap::rlc_mode_opts::rlc_um_unidirectional_ul;
      break;
    case srsgnb::rlc_mode::um_unidir_dl:
      e1ap_rlc_mode = asn1::e1ap::rlc_mode_opts::rlc_um_unidirectional_dl;
      break;
    case srsgnb::rlc_mode::am:
      e1ap_rlc_mode = asn1::e1ap::rlc_mode_opts::rlc_am;
      break;
    default:
      e1ap_rlc_mode = asn1::e1ap::rlc_mode_opts::nulltype;
  }

  return e1ap_rlc_mode;
}

/// \brief Converts type \c e1ap_pdcp_config to an E1AP ASN.1 type.
/// \param pdcp_cfg pdcp config object.
/// \return The E1AP ASN.1 object where the result of the conversion is stored.
inline asn1::e1ap::pdcp_cfg_s pdcp_config_to_e1ap_asn1(e1ap_pdcp_config pdcp_cfg)
{
  asn1::e1ap::pdcp_cfg_s e1ap_pdcp_cfg;

  // pdcp sn size dl
  asn1::number_to_enum(e1ap_pdcp_cfg.pdcp_sn_size_dl, pdcp_cfg.pdcp_sn_size_dl);

  // pdcp sn size ul
  asn1::number_to_enum(e1ap_pdcp_cfg.pdcp_sn_size_ul, pdcp_cfg.pdcp_sn_size_ul);

  // rlc mode
  e1ap_pdcp_cfg.rlc_mode = rlc_mode_to_asn1(pdcp_cfg.rlc_mod);

  // rohc params
  if (pdcp_cfg.rohc_params.has_value()) {
    e1ap_pdcp_cfg.rohc_params_present = true;
    if (pdcp_cfg.rohc_params.value().rohc.has_value()) {
      e1ap_pdcp_cfg.rohc_params.set_rohc();
      auto& e1ap_rohc         = e1ap_pdcp_cfg.rohc_params.rohc();
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
      e1ap_pdcp_cfg.rohc_params.set_ul_only_rohc();
      auto& e1ap_rohc         = e1ap_pdcp_cfg.rohc_params.ul_only_rohc();
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
      e1ap_pdcp_cfg.rohc_params_present = false;
    }
  }

  // t reordering timer
  if (pdcp_cfg.t_reordering_timer.has_value()) {
    e1ap_pdcp_cfg.t_reordering_timer_present = true;
    asn1::number_to_enum(e1ap_pdcp_cfg.t_reordering_timer.t_reordering, pdcp_cfg.t_reordering_timer.value());
  }

  // discard timer
  if (pdcp_cfg.discard_timer.has_value()) {
    e1ap_pdcp_cfg.discard_timer_present = true;
    asn1::number_to_enum(e1ap_pdcp_cfg.discard_timer, pdcp_cfg.discard_timer.value());
  }

  // ul data split thres
  if (pdcp_cfg.ul_data_split_thres.has_value()) {
    e1ap_pdcp_cfg.ul_data_split_thres_present = true;
    asn1::number_to_enum(e1ap_pdcp_cfg.ul_data_split_thres, pdcp_cfg.ul_data_split_thres.value());
  }

  // pdcp dupl
  if (pdcp_cfg.pdcp_dupl.has_value()) {
    e1ap_pdcp_cfg.pdcp_dupl_present = true;
    if (pdcp_cfg.pdcp_dupl.value()) {
      e1ap_pdcp_cfg.pdcp_dupl = asn1::e1ap::pdcp_dupl_opts::options::true_value;
    } else {
      e1ap_pdcp_cfg.pdcp_dupl = asn1::e1ap::pdcp_dupl_opts::options::nulltype;
    }
  }

  // pdcp reest
  if (pdcp_cfg.pdcp_reest.has_value()) {
    e1ap_pdcp_cfg.pdcp_reest_present = true;
    if (pdcp_cfg.pdcp_reest.value()) {
      e1ap_pdcp_cfg.pdcp_reest = asn1::e1ap::pdcp_reest_opts::options::true_value;
    } else {
      e1ap_pdcp_cfg.pdcp_reest = asn1::e1ap::pdcp_reest_opts::options::nulltype;
    }
  }

  // pdcp data recovery
  if (pdcp_cfg.pdcp_data_recovery.has_value()) {
    e1ap_pdcp_cfg.pdcp_data_recovery_present = true;
    if (pdcp_cfg.pdcp_data_recovery.value()) {
      e1ap_pdcp_cfg.pdcp_data_recovery = asn1::e1ap::pdcp_data_recovery_opts::options::true_value;
    } else {
      e1ap_pdcp_cfg.pdcp_data_recovery = asn1::e1ap::pdcp_data_recovery_opts::options::nulltype;
    }
  }

  // dupl activation
  if (pdcp_cfg.dupl_activation.has_value()) {
    e1ap_pdcp_cfg.dupl_activation_present = true;
    if (pdcp_cfg.dupl_activation.value() == "active") {
      e1ap_pdcp_cfg.dupl_activation = asn1::e1ap::dupl_activation_opts::options::active;
    } else if (pdcp_cfg.dupl_activation.value() == "inactive") {
      e1ap_pdcp_cfg.dupl_activation = asn1::e1ap::dupl_activation_opts::options::inactive;
    } else {
      e1ap_pdcp_cfg.dupl_activation = asn1::e1ap::dupl_activation_opts::options::nulltype;
    }
  }

  // out of orfder delivery
  if (pdcp_cfg.out_of_order_delivery.has_value()) {
    e1ap_pdcp_cfg.out_of_order_delivery_present = true;
    if (pdcp_cfg.out_of_order_delivery.value()) {
      e1ap_pdcp_cfg.out_of_order_delivery = asn1::e1ap::out_of_order_delivery_opts::options::true_value;
    } else {
      e1ap_pdcp_cfg.out_of_order_delivery = asn1::e1ap::out_of_order_delivery_opts::options::nulltype;
    }
  }

  return e1ap_pdcp_cfg;
}

/// \brief Convert E1AP Cause to CU-CP Cause.
/// \param e1ap_cause The E1AP Cause.
/// \return The CU-CP Cause.
inline cu_cp_cause_t e1ap_cause_to_cu_cp_cause(asn1::e1ap::cause_c e1ap_cause)
{
  cu_cp_cause_t cu_cp_cause;

  switch (e1ap_cause.type()) {
    case asn1::e1ap::cause_c::types_opts::radio_network:
      cu_cp_cause = cu_cp_cause_t::radio_network;
      return cu_cp_cause;
      break;
    case asn1::e1ap::cause_c::types_opts::transport:
      cu_cp_cause = cu_cp_cause_t::transport;
      return cu_cp_cause;
      break;
    case asn1::e1ap::cause_c::types_opts::protocol:
      cu_cp_cause = cu_cp_cause_t::protocol;
      return cu_cp_cause;
      break;
    case asn1::e1ap::cause_c::types_opts::misc:
      cu_cp_cause = cu_cp_cause_t::misc;
      return cu_cp_cause;
      break;
    default:
      cu_cp_cause = cu_cp_cause_t::nulltype;
      return cu_cp_cause;
  }
}

/// \brief Convert CU-CP cause to E1AP cause.
/// \param cu_cp_cause The CU-CP cause.
/// \return The E1AP cause.
inline asn1::e1ap::cause_c cu_cp_cause_to_e1ap_cause(cu_cp_cause_t cu_cp_cause)
{
  asn1::e1ap::cause_c e1ap_cause;

  switch (cu_cp_cause) {
    case cu_cp_cause_t::radio_network:
      e1ap_cause.set(asn1::e1ap::cause_c::types_opts::radio_network);
      return e1ap_cause;
      break;
    case cu_cp_cause_t::transport:
      e1ap_cause.set(asn1::e1ap::cause_c::types_opts::transport);
      return e1ap_cause;
      break;
    case cu_cp_cause_t::protocol:
      e1ap_cause.set(asn1::e1ap::cause_c::types_opts::protocol);
      return e1ap_cause;
      break;
    case cu_cp_cause_t::misc:
      e1ap_cause.set(asn1::e1ap::cause_c::types_opts::misc);
      return e1ap_cause;
      break;
    default:
      e1ap_cause.set(asn1::e1ap::cause_c::types_opts::nulltype);
      return e1ap_cause;
  }
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

} // namespace srs_cu_cp
} // namespace srsgnb
