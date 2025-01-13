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
#include "srsran/asn1/rrc_nr/common.h"
#include "srsran/asn1/rrc_nr/dl_dcch_msg.h"
#include "srsran/asn1/rrc_nr/meas_timing_cfg.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/rrc/meas_types.h"
#include <string>
#include <vector>

namespace srsran {
namespace srs_cu_cp {

rrc_ssb_mtc asn1_to_ssb_mtc(const asn1::rrc_nr::ssb_mtc_s& asn1_ssb_mtc);

/// \brief Converts type \c subcarrier_spacing to an RRC NR ASN.1 type.
/// \param[in] sc_spacing subcarrier spacing object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
subcarrier_spacing rrc_asn1_to_subcarrier_spacing(asn1::rrc_nr::subcarrier_spacing_e asn1_sc_spacing);

/// \brief Converts type \c asn1::rrc_nr::meas_timing_s to common type.
/// \param[in] asn1_meas_timing asn1 meas timing object.
/// \return The common object where the result of the conversion is stored.
rrc_meas_timing asn1_to_meas_timing(const asn1::rrc_nr::meas_timing_s& asn1_meas_timing);

/// \brief Converts type \c subcarrier_spacing to an RRC NR ASN.1 type.
/// \param[in] sc_spacing subcarrier spacing object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::subcarrier_spacing_e subcarrier_spacing_to_rrc_asn1(subcarrier_spacing sc_spacing);

/// \brief Converts type \c rrc_ssb_mtc to an RRC NR ASN.1 type.
/// \param[in] ssb_mtc ssb mtc object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::ssb_mtc_s ssb_mtc_to_rrc_asn1(rrc_ssb_mtc ssb_mtc);

/// \brief Converty type \c rrc_ssb_cfg_mob to an RRC NR ASN.1 type.
/// \param[in] ssb_cfg_mob ssb config mobility object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::ssb_cfg_mob_s ssb_cfg_mob_to_rrc_asn1(const rrc_ssb_cfg_mob& ssb_cfg_mob);

/// \brief Convert type \c rrc_csi_rs_res_cfg_mob_setup_release to an RRC NR ASN.1 type.
/// \param[in] csi_rs_res_cfg_mob csi rs res config mobility object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::setup_release_c<asn1::rrc_nr::csi_rs_res_cfg_mob_s>
csi_res_cfg_mob_to_rrc_asn1(const rrc_csi_rs_res_cfg_mob_setup_release& csi_rs_res_cfg_mob);

/// \brief Convert type \c rrc_thres_nr to an RRC NR ASN.1 type.
/// \param[in] thres_nr threshold object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::thres_nr_s thres_nr_to_rrc_asn1(const rrc_thres_nr& thres_nr);

/// \brief Convert type \c rrc_q_offset_range_list to an RRC NR ASN.1 type.
/// \param[in] q_offset_range_list q offset range list object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::q_offset_range_list_s q_offset_range_list_to_rrc_asn1(const rrc_q_offset_range_list& q_offset_range_list);

/// \brief Convert type \c rrc_meas_obj_nr to an RRC NR ASN.1 type.
/// \param[in] meas_obj_nr meas object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::meas_obj_nr_s meas_obj_nr_to_rrc_asn1(const rrc_meas_obj_nr& meas_obj_nr);

/// \brief Convert type \c rrc_srs_periodicity_and_offset to an RRC NR ASN.1 type.
/// \param[out] asn1_srs_period_and_offset The RRC NR ASN.1 object where the result of the conversion is stored.
/// \param[in] srs_period_and_offset common type srs periodicity and offset object.
template <class asn1_srs_periodicity_and_offset>
void srs_periodicity_and_offset_to_rrc_asn1(asn1_srs_periodicity_and_offset&      asn1_srs_period_and_offset,
                                            const rrc_srs_periodicity_and_offset& srs_period_and_offset);

/// \brief Convert type \c rrc_srs_res to an RRC NR ASN.1 type.
/// \param[in] srs_res srs res object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::srs_res_s srs_res_to_rrc_asn1(const rrc_srs_res& srs_res);

/// \brief Convert type \c rrc_meas_obj_to_add_mod to an RRC NR ASN.1 type.
/// \param[in] meas_obj_to_add_mod measurement object to add mod object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::meas_obj_to_add_mod_s meas_obj_to_add_mod_to_rrc_asn1(const rrc_meas_obj_to_add_mod& meas_obj_to_add_mod);

/// \brief Convert type \c rrc_meas_report_quant to an RRC NR ASN.1 type.
/// \param[in] meas_report_quant measurement report quantity object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::meas_report_quant_s meas_report_quant_to_rrc_asn1(const rrc_meas_report_quant& meas_report_quant);

/// \brief Convert type \c rrc_nr_rs_type to an RRC NR ASN.1 type.
/// \param[in] rs_type rs type object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::nr_rs_type_e rrc_nr_rs_type_to_asn1(const rrc_nr_rs_type& rs_type);

/// \brief Convert type \c uint32_t to an RRC NR ASN.1 type.
/// \param[in] report_interval report interval.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::report_interv_e report_interval_to_asn1(uint32_t report_interval);

/// \brief Convert type \c rrc_periodical_report_cfg to an RRC NR ASN.1 type.
/// \param[in] periodical_report_cfg periodical report config object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::periodical_report_cfg_s
periodical_report_cfg_to_rrc_asn1(const rrc_periodical_report_cfg& periodical_report_cfg);

/// \brief Convert type \c rrc_meas_trigger_quant to an RRC NR ASN.1 type.
/// \param[out] asn1_meas_trigger_quant_offset The RRC NR ASN.1 object where the result of the conversion is stored.
/// \param[in] srs_period_and_omeas_trigger_quantffset common type measurement trigger quantity object.
template <class asn1_meas_trigger_quant_quant_offset>
void meas_trigger_quant_to_rrc_asn1(asn1_meas_trigger_quant_quant_offset& asn1_meas_trigger_quant_offset,
                                    const rrc_meas_trigger_quant&         meas_trigger_quant);

/// \brief Convert type \c rrc_event_trigger_cfg to an RRC NR ASN.1 type.
/// \param[in] event_trigger_cfg event trigger config object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::event_trigger_cfg_s
event_triggered_report_cfg_to_rrc_asn1(const rrc_event_trigger_cfg& event_trigger_cfg);

/// \brief Convert type \c rrc_report_cfg_nr to an RRC NR ASN.1 type.
/// \param[in] report_cfg_nr report config object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::report_cfg_nr_s report_cfg_nr_to_rrc_asn1(const rrc_report_cfg_nr& report_cfg_nr);

/// \brief Convert type \c rrc_report_cfg_to_add_mod to an RRC NR ASN.1 type.
/// \param[in] report_cfg_to_add_mod report config to add mod object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::report_cfg_to_add_mod_s
report_cfg_to_add_mod_to_rrc_asn1(const rrc_report_cfg_to_add_mod& report_cfg_to_add_mod);

/// \brief Convert type \c rrc_meas_id_to_add_mod to an RRC NR ASN.1 type.
/// \param[in] meas_id_to_add_mod measurement id to add mod object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::meas_id_to_add_mod_s meas_id_to_add_mod_to_rrc_asn1(const rrc_meas_id_to_add_mod& meas_id_to_add_mod);

/// \brief Convert type \c rrc_filt_cfg to an RRC NR ASN.1 type.
/// \param[in] filt_cfg filter config object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::filt_cfg_s filt_cfg_to_rrc_asn1(const rrc_filt_cfg& filt_cfg);

/// \brief Convert type \c rrc_quant_cfg_rs to an RRC NR ASN.1 type.
/// \param[in] quant_cfg_rs quantity config rs object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::quant_cfg_rs_s quant_cfg_rs_to_rrc_asn1(const rrc_quant_cfg_rs& quant_cfg_rs);

/// \brief Converts type \c cu_cp_meas_cfg to an RRC NR ASN.1 type.
/// \param[in] meas_cfg Meas config object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
asn1::rrc_nr::meas_cfg_s meas_config_to_rrc_asn1(const rrc_meas_cfg& meas_cfg);

/// \brief Convert ASN.1 type \c asn1::rrc_nr::meas_quant_results_s to common type.
/// \param[in] asn1_meas_quant_results measurement quant result nr ASN.1 object.
/// \return The common type object where the result of the conversion is stored.
rrc_meas_quant_results asn1_to_meas_quant_results(const asn1::rrc_nr::meas_quant_results_s& asn1_meas_quant_results);

/// \brief Convert ASN.1 type \c asn1::rrc_nr::meas_result_nr_s to common type.
/// \param[in] asn1_meas_result_nr measurement result nr ASN.1 object.
/// \return The common type object where the result of the conversion is stored.
rrc_meas_result_nr asn1_to_meas_result_nr(const asn1::rrc_nr::meas_result_nr_s& asn1_meas_result_nr);

/// \brief Convert ASN.1 type \c asn1::rrc_nr::meas_results_s to common type.
/// \param[in] asn1_meas_results measurement result ASN.1 object.
/// \return The common type object where the result of the conversion is stored.
rrc_meas_results asn1_to_measurement_results(const asn1::rrc_nr::meas_results_s& asn1_meas_results,
                                             srslog::basic_logger&               logger);

} // namespace srs_cu_cp
} // namespace srsran
