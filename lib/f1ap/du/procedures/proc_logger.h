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

#pragma once

#include "../ue_context/f1ap_ue_context.h"

namespace srsran {
namespace srs_du {

struct f1ap_log_prefix {
  f1ap_log_prefix(gnb_du_ue_f1ap_id_t du_ue_id_, const char* proc_name_) : du_ue_id(du_ue_id_), proc_name(proc_name_) {}
  f1ap_log_prefix(gnb_du_ue_f1ap_id_t du_ue_id_, gnb_cu_ue_f1ap_id_t cu_ue_id_, const char* proc_name_) :
    du_ue_id(du_ue_id_), cu_ue_id(cu_ue_id_), proc_name(proc_name_)
  {
  }
  f1ap_log_prefix(gnb_cu_ue_f1ap_id_t cu_ue_id_, const char* proc_name_) : cu_ue_id(cu_ue_id_), proc_name(proc_name_) {}
  f1ap_log_prefix(const f1ap_ue_context& context_, const char* proc_name_) :
    ue_index(context_.ue_index),
    du_ue_id(context_.gnb_du_ue_f1ap_id),
    cu_ue_id(context_.gnb_cu_ue_f1ap_id),
    proc_name(proc_name_)
  {
  }

  du_ue_index_t       ue_index  = INVALID_DU_UE_INDEX;
  gnb_du_ue_f1ap_id_t du_ue_id  = gnb_du_ue_f1ap_id_t::invalid;
  gnb_cu_ue_f1ap_id_t cu_ue_id  = gnb_cu_ue_f1ap_id_t::invalid;
  const char*         proc_name = nullptr;
};

} // namespace srs_du
} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::srs_du::f1ap_log_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_du::f1ap_log_prefix& prefix, FormatContext& ctx)
  {
    bool first_id = true;
    auto get_sep  = [&first_id]() { return std::exchange(first_id, false) ? "" : " "; };
    if (prefix.ue_index != srsran::INVALID_DU_UE_INDEX) {
      format_to(ctx.out(), "{}ue={}", get_sep(), prefix.ue_index);
    }
    if (prefix.du_ue_id != srsran::gnb_du_ue_f1ap_id_t::invalid) {
      format_to(ctx.out(), "{}GNB-DU-UE-F1AP-ID={}", get_sep(), prefix.du_ue_id);
    }
    if (prefix.cu_ue_id != srsran::gnb_cu_ue_f1ap_id_t::invalid) {
      format_to(ctx.out(), "{}GNB-CU-UE-F1AP-ID={}", get_sep(), prefix.cu_ue_id);
    }
    format_to(ctx.out(), "{}proc=\"{}\"", get_sep(), prefix.proc_name);
    return ctx.out();
  }
};

} // namespace fmt