/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/f1ap_ue_id_types.h"

namespace srsran {
namespace srs_cu_cp {

/// Identifiers that associate a UE in the F1AP-CU.
struct f1ap_ue_ids {
  const ue_index_t          ue_index      = ue_index_t::invalid;
  const gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_t::invalid;
  gnb_du_ue_f1ap_id_t       du_ue_f1ap_id = gnb_du_ue_f1ap_id_t::invalid;
};

} // namespace srs_cu_cp
} // namespace srsran
