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

#include "srsran/fapi_adaptor/phy/messages/pdcch.h"
#include "srsran/fapi_adaptor/precoding_matrix_repository.h"
#include "srsran/srsvec/bit.h"

using namespace srsran;
using namespace fapi_adaptor;

/// Fills the DL DCI parameters of the PDCCH processor PDU.
static void fill_dci(pdcch_processor::pdu_t&            proc_pdu,
                     const fapi::dl_pdcch_pdu&          fapi_pdu,
                     uint16_t                           i_dci,
                     const precoding_matrix_repository& pm_repo)
{
  const auto& fapi_dci    = fapi_pdu.dl_dci[i_dci];
  const auto& fapi_dci_v3 = fapi_pdu.maintenance_v3.info[i_dci];
  const auto& fapi_dci_v4 = fapi_pdu.parameters_v4.params[i_dci];

  pdcch_processor::dci_description& dci = proc_pdu.dci;

  dci.rnti              = to_value(fapi_dci.rnti);
  dci.n_id_pdcch_data   = fapi_dci.nid_pdcch_data;
  dci.n_id_pdcch_dmrs   = fapi_dci_v4.nid_pdcch_dmrs;
  dci.n_rnti            = fapi_dci.nrnti_pdcch_data;
  dci.cce_index         = fapi_dci.cce_index;
  dci.aggregation_level = fapi_dci.aggregation_level;

  dci.dmrs_power_offset_dB = (fapi_dci.power_control_offset_ss_profile_nr == -127)
                                 ? static_cast<float>(fapi_dci_v3.pdcch_dmrs_power_offset_profile_sss) * 0.001F
                                 : static_cast<float>(fapi_dci.power_control_offset_ss_profile_nr);

  dci.data_power_offset_dB = (fapi_dci_v3.pdcch_data_power_offset_profile_sss ==
                              std::numeric_limits<decltype(fapi_dci_v3.pdcch_data_power_offset_profile_sss)>::min())
                                 ? dci.dmrs_power_offset_dB
                                 : float(fapi_dci_v3.pdcch_data_power_offset_profile_sss) * 0.001F;

  // Unpack the payload.
  dci.payload.resize(fapi_dci.payload.size());
  for (unsigned j = 0, je = fapi_dci.payload.size(); j != je; ++j) {
    dci.payload[j] = fapi_dci.payload.test(j);
  }

  srsran_assert(fapi_dci.precoding_and_beamforming.prgs.size() == 1U,
                "Unsupported number of PRGs={}",
                fapi_dci.precoding_and_beamforming.prgs.size());
  dci.precoding = precoding_configuration::make_wideband(
      pm_repo.get_precoding_matrix(fapi_dci.precoding_and_beamforming.prgs.front().pm_index));

  // Fill PDCCH context for logging.
  proc_pdu.context = fapi_pdu.dl_dci[i_dci].context;
}

/// Fills the CORESET parameters of the PDCCH processor PDU.
static void fill_coreset(pdcch_processor::coreset_description& coreset, const fapi::dl_pdcch_pdu& fapi_pdu)
{
  coreset.bwp_size_rb         = fapi_pdu.coreset_bwp_size;
  coreset.bwp_start_rb        = fapi_pdu.coreset_bwp_start;
  coreset.start_symbol_index  = fapi_pdu.start_symbol_index;
  coreset.duration            = fapi_pdu.duration_symbols;
  coreset.frequency_resources = fapi_pdu.freq_domain_resource;

  // Configure CCE-to-REG mapping depending on PDCCH CORESET.
  if (fapi_pdu.coreset_type == fapi::pdcch_coreset_type::pbch_or_coreset0) {
    // The PDCCH is located in CORESET0.
    coreset.cce_to_reg_mapping = pdcch_processor::cce_to_reg_mapping_type::CORESET0;

    // The REG bundle size and interleaver size are ignored.
    coreset.reg_bundle_size  = 0;
    coreset.interleaver_size = 0;
    coreset.shift_index      = fapi_pdu.shift_index;
    return;
  }

  // The PDCCH is NOT located in CORESET0 and non interleaved.
  if (fapi_pdu.cce_reg_mapping_type == fapi::cce_to_reg_mapping_type::non_interleaved) {
    // Non-interleaved case.
    coreset.cce_to_reg_mapping = pdcch_processor::cce_to_reg_mapping_type::NON_INTERLEAVED;

    // The REG bundle size and interleaver size are ignored.
    coreset.reg_bundle_size  = 0;
    coreset.interleaver_size = 0;
    coreset.shift_index      = 0;
    return;
  }

  // The PDCCH is NOT located in CORESET0 and is interleaved.
  coreset.cce_to_reg_mapping = pdcch_processor::cce_to_reg_mapping_type::INTERLEAVED;

  // The REG bundle size and interleaver size are ignored.
  coreset.reg_bundle_size  = fapi_pdu.reg_bundle_size;
  coreset.interleaver_size = fapi_pdu.interleaver_size;
  coreset.shift_index      = fapi_pdu.shift_index;
}

void srsran::fapi_adaptor::convert_pdcch_fapi_to_phy(pdcch_processor::pdu_t&            proc_pdu,
                                                     const fapi::dl_pdcch_pdu&          fapi_pdu,
                                                     uint16_t                           sfn,
                                                     uint16_t                           slot,
                                                     uint16_t                           i_dci,
                                                     const precoding_matrix_repository& pm_repo)
{
  proc_pdu.slot = slot_point(fapi_pdu.scs, sfn, slot);
  proc_pdu.cp   = fapi_pdu.cp;

  fill_coreset(proc_pdu.coreset, fapi_pdu);

  fill_dci(proc_pdu, fapi_pdu, i_dci, pm_repo);
}
