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

#include "srsgnb/f1ap/common/f1ap_types.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/rnti.h"

namespace srsgnb {
namespace srs_du {

/// UE F1 context that is referenced by both the F1 UE object and its bearers.
struct f1ap_ue_context {
  const du_ue_index_t       ue_index;
  const gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id;
  gnb_cu_ue_f1ap_id_t       gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_t::invalid;
  rnti_t                    rnti              = rnti_t::INVALID_RNTI;

  f1ap_ue_context(du_ue_index_t ue_index_, gnb_du_ue_f1ap_id_t du_f1ap_ue_id_) :
    ue_index(ue_index_), gnb_du_ue_f1ap_id(du_f1ap_ue_id_)
  {
  }
};

} // namespace srs_du

} // namespace srsgnb