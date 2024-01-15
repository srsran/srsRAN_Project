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

#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/rrc_nr/msg_common.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/ran/cu_types.h"
#include "srsran/security/security.h"
#include <string>
#include <vector>

namespace srsran {
namespace srs_cu_cp {

/// \brief Converts type \c pdcp_discard_timer to an RRC NR ASN.1 type.
/// \param[in] discard_timer discard timer object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
inline asn1::rrc_nr::pdcp_cfg_s::drb_s_::discard_timer_e_ discard_timer_to_asn1(pdcp_discard_timer discard_timer)
{
  asn1::rrc_nr::pdcp_cfg_s::drb_s_::discard_timer_e_ asn1_discard_timer;

  asn1::number_to_enum(asn1_discard_timer, pdcp_discard_timer_to_int(discard_timer));

  return asn1_discard_timer;
}

/// \brief Converts type \c pdcp_t_reordering to an RRC NR ASN.1 type.
/// \param[in] t_reordering t-reordering object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
inline asn1::rrc_nr::pdcp_cfg_s::t_reordering_e_ t_reordering_to_asn1(pdcp_t_reordering t_reordering)
{
  asn1::rrc_nr::pdcp_cfg_s::t_reordering_e_ asn1_t_reordering;

  asn1::number_to_enum(asn1_t_reordering, pdcp_t_reordering_to_int(t_reordering));

  return asn1_t_reordering;
}

/// \brief Converts type \c pdcp_config_t to an RRC NR ASN.1 type.
/// \param[in] pdcp_cfg pdcp config object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
///
/// Conditional presence | Explanation
/// DRB                  | This field is mandatory present when the corresponding DRB is being set up, absent for SRBs.
///                      | Otherwise this field is optionally present, need M.
/// MoreThanOneRLC       | This field is mandatory present upon RRC reconfiguration with setup of a PDCP entity for a
///                      | radio bearer with more than one associated logical channel and upon RRC reconfiguration with
///                      | the association of an additional logical channel to the PDCP entity. Upon RRC reconfiguration
///                      | when a PDCP entity is associated with multiple logical channels, this field is optionally
///                      | present need M. Otherwise, this field is absent. Need R.
/// Rlc-AM               | For RLC AM, the field is optionally present, need R. Otherwise, the field is absent.
/// Setup                | The field is mandatory present in case of radio bearer setup. Otherwise the field is
///                      | optionally present, need M.
/// SplitBearer          | The field is absent for SRBs. Otherwise, the field is optional present, need M, in case of
///                      | radio bearer with more than one associated RLC mapped to different cell groups.
/// ConnectedTo5GC       | The field is optionally present, need R, if the UE is connected to 5GC. Otherwise the field
///                      | is absent.
/// ConnectedTo5GC1      | The field is optionally present, need R, if the UE is connected to NR/5GC. Otherwise the
///                      | field is absent.
/// Setup2               | This field is mandatory present in case for radio bearer setup for RLC-AM and RLC-UM.
///                      | Otherwise, this field is absent, Need M.
inline asn1::rrc_nr::pdcp_cfg_s pdcp_config_to_rrc_nr_asn1(pdcp_config pdcp_cfg)
{
  asn1::rrc_nr::pdcp_cfg_s rrc_pdcp_cfg;

  // t reordering -- Need S
  if (pdcp_cfg.rx.t_reordering != pdcp_t_reordering::infinity) {
    rrc_pdcp_cfg.t_reordering_present = true;
    rrc_pdcp_cfg.t_reordering         = t_reordering_to_asn1(pdcp_cfg.rx.t_reordering);
  }

  // ciphering disabled present -- Cond ConnectedTo5GC
  if (!pdcp_cfg.ciphering_required) {
    rrc_pdcp_cfg.ciphering_disabled_present = true;
    // this is an extension field.
    rrc_pdcp_cfg.ext = true;
  }

  // more than one rlc
  rrc_pdcp_cfg.more_than_one_rlc_present = false; // not supported.

  // no more configurable parameters for SRBs
  if (pdcp_cfg.rb_type == pdcp_rb_type::srb) {
    return rrc_pdcp_cfg;
  }

  // drb -- Cond DRB
  rrc_pdcp_cfg.drb_present = true;

  // hdr compress
  rrc_pdcp_cfg.drb.hdr_compress.set_not_used(); // not supported.

  // discard timer -- Cond Setup
  if (pdcp_cfg.tx.discard_timer.has_value()) {
    rrc_pdcp_cfg.drb.discard_timer_present = true;
    rrc_pdcp_cfg.drb.discard_timer         = discard_timer_to_asn1(pdcp_cfg.tx.discard_timer.value());
  }

  // pdcp sn size ul -- Cond Setup2
  rrc_pdcp_cfg.drb.pdcp_sn_size_ul_present = true;
  asn1::number_to_enum(rrc_pdcp_cfg.drb.pdcp_sn_size_ul, pdcp_sn_size_to_uint(pdcp_cfg.rx.sn_size));

  // pdcp sn size dl -- Cond Setup2
  rrc_pdcp_cfg.drb.pdcp_sn_size_dl_present = true;
  asn1::number_to_enum(rrc_pdcp_cfg.drb.pdcp_sn_size_dl, pdcp_sn_size_to_uint(pdcp_cfg.tx.sn_size));

  // integrity protection present
  rrc_pdcp_cfg.drb.integrity_protection_present = pdcp_cfg.integrity_protection_required;

  // status report required present
  rrc_pdcp_cfg.drb.status_report_required_present = pdcp_cfg.tx.status_report_required;

  // out of order delivery present
  rrc_pdcp_cfg.drb.out_of_order_delivery_present = pdcp_cfg.rx.out_of_order_delivery;

  return rrc_pdcp_cfg;
}

inline asn1::rrc_nr::sdap_cfg_s::sdap_hdr_ul_opts::options sdap_hdr_ul_cfg_to_rrc_asn1(sdap_hdr_ul_cfg hdr_cfg)
{
  asn1::rrc_nr::sdap_cfg_s::sdap_hdr_ul_opts::options asn1_hdr_ul_opts;

  if (hdr_cfg == sdap_hdr_ul_cfg::absent) {
    asn1_hdr_ul_opts = asn1::rrc_nr::sdap_cfg_s::sdap_hdr_ul_opts::options::absent;
  } else {
    asn1_hdr_ul_opts = asn1::rrc_nr::sdap_cfg_s::sdap_hdr_ul_opts::options::present;
  }

  return asn1_hdr_ul_opts;
}

inline asn1::rrc_nr::sdap_cfg_s::sdap_hdr_dl_opts::options sdap_hdr_dl_cfg_to_rrc_asn1(sdap_hdr_dl_cfg hdr_cfg)
{
  asn1::rrc_nr::sdap_cfg_s::sdap_hdr_dl_opts::options asn1_hdr_dl_opts;

  if (hdr_cfg == sdap_hdr_dl_cfg::absent) {
    asn1_hdr_dl_opts = asn1::rrc_nr::sdap_cfg_s::sdap_hdr_dl_opts::options::absent;
  } else {
    asn1_hdr_dl_opts = asn1::rrc_nr::sdap_cfg_s::sdap_hdr_dl_opts::options::present;
  }

  return asn1_hdr_dl_opts;
}

/// \brief Converts type \c sdap_config to an RRC NR ASN.1 type.
/// \param sdap_cfg sdap config object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
inline asn1::rrc_nr::sdap_cfg_s sdap_config_to_rrc_asn1(sdap_config_t sdap_cfg)
{
  asn1::rrc_nr::sdap_cfg_s asn1_sdap_cfg;

  // pdu session
  asn1_sdap_cfg.pdu_session = pdu_session_id_to_uint(sdap_cfg.pdu_session);

  // sdap hdr dl
  asn1_sdap_cfg.sdap_hdr_dl = sdap_hdr_dl_cfg_to_rrc_asn1(sdap_cfg.sdap_hdr_dl);

  // sdap hdr ul
  asn1_sdap_cfg.sdap_hdr_ul = sdap_hdr_ul_cfg_to_rrc_asn1(sdap_cfg.sdap_hdr_ul);

  // default drb
  asn1_sdap_cfg.default_drb = sdap_cfg.default_drb;

  // mapped qos flow to add
  for (const auto& mapped_qps_flow_to_add : sdap_cfg.mapped_qos_flows_to_add) {
    asn1_sdap_cfg.mapped_qos_flows_to_add.push_back(qos_flow_id_to_uint(mapped_qps_flow_to_add));
  }

  // mapped qos flow to release
  for (const auto& mapped_qps_flow_to_release : sdap_cfg.mapped_qos_flows_to_release) {
    asn1_sdap_cfg.mapped_qos_flows_to_release.push_back(qos_flow_id_to_uint(mapped_qps_flow_to_release));
  }

  return asn1_sdap_cfg;
}

inline asn1::rrc_nr::ciphering_algorithm_e
ciphering_algorithm_to_rrc_asn1(const security::ciphering_algorithm& ciphering_algo)
{
  asn1::rrc_nr::ciphering_algorithm_e asn1_ciphering_algo;

  switch (ciphering_algo) {
    case srsran::security::ciphering_algorithm::nea0:
      asn1_ciphering_algo = asn1::rrc_nr::ciphering_algorithm_opts::options::nea0;
      break;
    case srsran::security::ciphering_algorithm::nea1:
      asn1_ciphering_algo = asn1::rrc_nr::ciphering_algorithm_opts::options::nea1;
      break;
    case srsran::security::ciphering_algorithm::nea2:
      asn1_ciphering_algo = asn1::rrc_nr::ciphering_algorithm_opts::options::nea2;
      break;
    case srsran::security::ciphering_algorithm::nea3:
      asn1_ciphering_algo = asn1::rrc_nr::ciphering_algorithm_opts::options::nea3;
      break;
    default:
      // error
      report_fatal_error("Cannot convert ciphering algorithm {} to ASN.1 type", ciphering_algo);
  }

  return asn1_ciphering_algo;
}

inline asn1::rrc_nr::integrity_prot_algorithm_e
integrity_prot_algorithm_to_rrc_asn1(const security::integrity_algorithm& integrity_prot_algo)
{
  asn1::rrc_nr::integrity_prot_algorithm_e asn1_integrity_prot_algo;

  switch (integrity_prot_algo) {
    case srsran::security::integrity_algorithm::nia0:
      asn1_integrity_prot_algo = asn1::rrc_nr::integrity_prot_algorithm_opts::options::nia0;
      break;
    case srsran::security::integrity_algorithm::nia1:
      asn1_integrity_prot_algo = asn1::rrc_nr::integrity_prot_algorithm_opts::options::nia1;
      break;
    case srsran::security::integrity_algorithm::nia2:
      asn1_integrity_prot_algo = asn1::rrc_nr::integrity_prot_algorithm_opts::options::nia2;
      break;
    case srsran::security::integrity_algorithm::nia3:
      asn1_integrity_prot_algo = asn1::rrc_nr::integrity_prot_algorithm_opts::options::nia3;
      break;
    default:
      // error
      report_fatal_error("Cannot convert integrity_prot algorithm {} to ASN.1 type", integrity_prot_algo);
  }

  return asn1_integrity_prot_algo;
}

} // namespace srs_cu_cp
} // namespace srsran
