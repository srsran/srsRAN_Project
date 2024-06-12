/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_ue_context.h"
#include "srsran/asn1/rrc_nr/ho_prep_info.h"

using namespace srsran;
using namespace srs_cu_cp;

rrc_ue_context_t::rrc_ue_context_t(const ue_index_t                       ue_index_,
                                   const rnti_t                           c_rnti_,
                                   const rrc_cell_context&                cell_,
                                   const rrc_ue_cfg_t&                    cfg_,
                                   std::optional<rrc_ue_transfer_context> rrc_context_) :
  ue_index(ue_index_),
  c_rnti(c_rnti_),
  cell(cell_),
  cfg(cfg_),
  transfer_context(rrc_context_),
  logger(srslog::fetch_basic_logger("RRC"))
{
  if (transfer_context.has_value()) {
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
