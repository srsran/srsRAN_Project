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

#include "drb_manager_impl.h"
#include "srsran/asn1/rrc_nr/rrc_nr.h"
#include "srsran/rrc/drb_manager.h"
#include "srsran/rrc/rrc_cell_context.h"
#include "srsran/rrc/rrc_ue.h"
#include "srsran/rrc/rrc_ue_config.h"

namespace srsran {

namespace srs_cu_cp {

/// RRC states (3GPP 38.331 v15.5.1 Sec 4.2.1)
enum class rrc_state { idle = 0, connected, connected_inactive };

/// Holds the RRC UE context used by the UE object and all its procedures.
class rrc_ue_context_t
{
public:
  rrc_ue_context_t(const ue_index_t       ue_index_,
                   const rnti_t           c_rnti_,
                   const rrc_cell_context cell_,
                   const rrc_ue_cfg_t&    cfg_) :
    ue_index(ue_index_),
    c_rnti(c_rnti_),
    cell(cell_),
    cfg(cfg_),
    drb_mng(std::make_unique<drb_manager_impl>(cfg_.drb_cfg))
  {
  }

  drb_manager& get_drb_manager() { return *drb_mng; }

  const ue_index_t                       ue_index; // UE index assigned by the DU processor
  const rnti_t                           c_rnti;   // current C-RNTI
  const rrc_cell_context                 cell;     // current cell
  const rrc_ue_cfg_t&                    cfg;
  rrc_state                              state = rrc_state::idle;
  std::unique_ptr<drb_manager>           drb_mng;
  guami                                  current_guami; // current GUAMI
  uint64_t                               setup_ue_id = -1;
  asn1::rrc_nr::establishment_cause_opts connection_cause;
  security::sec_as_config                sec_cfg;
};

} // namespace srs_cu_cp

} // namespace srsran
