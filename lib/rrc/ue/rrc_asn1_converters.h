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

#pragma once

#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "srsran/cu_cp/cu_cp_types.h"
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
asn1::rrc_nr::pdcp_cfg_s::drb_s_::discard_timer_e_ discard_timer_to_asn1(pdcp_discard_timer discard_timer);

/// \brief Converts type \c pdcp_t_reordering to an RRC NR ASN.1 type.
/// \param[in] t_reordering t-reordering object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::pdcp_cfg_s::t_reordering_e_ t_reordering_to_asn1(pdcp_t_reordering t_reordering);

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
asn1::rrc_nr::pdcp_cfg_s pdcp_config_to_rrc_nr_asn1(const pdcp_config& pdcp_cfg);

/// \brief Converts type \c sdap_hdr_ul_cfg to an RRC NR ASN.1 type.
/// \param[in] hdr_cfg sdap ul header config object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::sdap_cfg_s::sdap_hdr_ul_opts::options sdap_hdr_ul_cfg_to_rrc_asn1(sdap_hdr_ul_cfg hdr_cfg);

/// \brief Converts type \c sdap_hdr_dl_cfg to an RRC NR ASN.1 type.
/// \param[in] hdr_cfg sdap dl header config object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::sdap_cfg_s::sdap_hdr_dl_opts::options sdap_hdr_dl_cfg_to_rrc_asn1(sdap_hdr_dl_cfg hdr_cfg);

/// \brief Converts type \c sdap_config_t to an RRC NR ASN.1 type.
/// \param[in] sdap_cfg sdap config object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::sdap_cfg_s sdap_config_to_rrc_asn1(const sdap_config_t& sdap_cfg);

/// \brief Converts type \c security::ciphering_algorithm to an RRC NR ASN.1 type.
/// \param[in] ciphering_algo ciphering algorithm object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::ciphering_algorithm_e
ciphering_algorithm_to_rrc_asn1(const security::ciphering_algorithm& ciphering_algo);

/// \brief Converts type \c security::integrity_algorithm to an RRC NR ASN.1 type.
/// \param[in] integrity_prot_algo intergrity protection algorithm object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::integrity_prot_algorithm_e
integrity_prot_algorithm_to_rrc_asn1(const security::integrity_algorithm& integrity_prot_algo);

/// \brief Converts type \c asn1::fixed_bitstring<48> to an RRC NR ASN.1 type.
/// \param[in] asn1_five_g_s_tmsi five g s tmsi object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
cu_cp_five_g_s_tmsi asn1_to_five_g_s_tmsi(const asn1::fixed_bitstring<48>& asn1_five_g_s_tmsi);

/// \brief Converts type \c asn1::fixed_bitstring<39> and \c asn1::fixed_bitstring<9> to an RRC NR ASN.1 type.
/// \param[in] asn1_five_g_s_tmsi_part1 five g s tmsi part 1 object.
/// \param[in] asn1_five_g_s_tmsi_part2 five g s tmsi part 2 object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
cu_cp_five_g_s_tmsi asn1_to_five_g_s_tmsi(const asn1::fixed_bitstring<39>& asn1_five_g_s_tmsi_part1,
                                          const asn1::fixed_bitstring<9>&  asn1_five_g_s_tmsi_part2);

/// \brief Converts type \c asn1::fixed_bitstring<24> to an RRC NR ASN.1 type.
/// \param[in] asn1_amf_id amf id object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
cu_cp_amf_identifier_t asn1_to_amf_identifier(const asn1::fixed_bitstring<24>& asn1_amf_id);

} // namespace srs_cu_cp
} // namespace srsran
