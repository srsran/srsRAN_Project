/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/f1ap/common/f1ap_ue_id.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/rnti.h"
#include "fmt/format.h"

namespace srsran {
namespace srs_du {

/// UE F1 context that is referenced by both the F1 UE object and its bearers.
struct f1ap_ue_context {
  const du_ue_index_t       ue_index;
  const gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id;
  gnb_cu_ue_f1ap_id_t       gnb_cu_ue_f1ap_id  = gnb_cu_ue_f1ap_id_t::invalid;
  rnti_t                    rnti               = rnti_t::INVALID_RNTI;
  bool                      marked_for_release = false;

  f1ap_ue_context(du_ue_index_t ue_index_, gnb_du_ue_f1ap_id_t du_f1ap_ue_id_) :
    ue_index(ue_index_), gnb_du_ue_f1ap_id(du_f1ap_ue_id_)
  {
  }
};

} // namespace srs_du
} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::srs_du::f1ap_ue_context> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_du::f1ap_ue_context& ue, FormatContext& ctx)
  {
    if (ue.gnb_cu_ue_f1ap_id == srsran::gnb_cu_ue_f1ap_id_t::invalid) {
      return format_to(
          ctx.out(), "ue={} c-rnti={:#x} GNB-DU-UE-F1AP-ID={}", ue.ue_index, ue.rnti, ue.gnb_du_ue_f1ap_id);
    }
    return format_to(ctx.out(),
                     "ue={} c-rnti={:#x} GNB-DU-UE-F1AP-ID={} GNB-CU-UE-F1AP-ID={}",
                     ue.ue_index,
                     ue.rnti,
                     ue.gnb_du_ue_f1ap_id,
                     ue.gnb_cu_ue_f1ap_id);
  }
};

} // namespace fmt