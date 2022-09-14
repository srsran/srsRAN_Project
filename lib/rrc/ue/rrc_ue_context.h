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

#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/cu_cp/ue_context.h"

namespace srsgnb {

namespace srs_cu_cp {

/// Holds the RRC UE context used by the UE object and all its procedures.
class rrc_ue_context_t
{
public:
  rrc_ue_context_t(const ue_index_t              ue_index_,
                   const rnti_t                  c_rnti_,
                   const nr_cell_global_identity cgi_,
                   const rrc_ue_cfg_t&           cfg_) :
    ue_index(ue_index_), c_rnti(c_rnti_), cgi(cgi_), cfg(cfg_)
  {
  }
  const ue_index_t                       ue_index; // UE index assigned by the DU processor
  const rnti_t                           c_rnti;   // current C-RNTI
  nr_cell_global_identity                cgi;      /// global cell ID
  const rrc_ue_cfg_t&                    cfg;
  uint64_t                               setup_ue_id = -1;
  asn1::rrc_nr::establishment_cause_opts connection_cause;
};

} // namespace srs_cu_cp

} // namespace srsgnb
