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

#include "rrc_ue_context.h"

using namespace srsran;
using namespace srs_cu_cp;

rrc_ue_context_t::rrc_ue_context_t(const ue_index_t                  ue_index_,
                                   const rnti_t                      c_rnti_,
                                   const rrc_cell_context&           cell_,
                                   const rrc_ue_cfg_t&               cfg_,
                                   optional<rrc_ue_transfer_context> rrc_context_) :
  ue_index(ue_index_),
  c_rnti(c_rnti_),
  cell(cell_),
  cfg(cfg_),
  transfer_context(rrc_context_),
  logger(srslog::fetch_basic_logger("RRC"))
{
  if (transfer_context.has_value()) {
    // Update security config.
    sec_context = transfer_context.value().sec_context;
    sec_context.horizontal_key_derivation(cell.pci, cell.ssb_arfcn);
    logger.debug("ue={} refreshed keys horizontally. pci={} ssb-arfcn={}", ue_index, cell.pci, cell.ssb_arfcn);
    security_enabled = true;

    // Handle handover preparation info.
    if (!transfer_context.value().handover_preparation_info.empty()) {
      asn1::rrc_nr::ho_prep_info_s ho_prep;
      asn1::cbit_ref               bref({transfer_context.value().handover_preparation_info.begin(),
                                         transfer_context.value().handover_preparation_info.end()});

      if (ho_prep.unpack(bref) != asn1::SRSASN_SUCCESS) {
        logger.error("Couldn't unpack Handover Preparation Info.");
        return;
      }

      capabilities_list = ho_prep.crit_exts.c1().ho_prep_info().ue_cap_rat_list;
    }

    // Measurement config (will be updated during Reconfiguration)
    meas_cfg = transfer_context.value().meas_cfg;
  }
}
