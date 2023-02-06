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

#include "nr_cgi.h"
#include "s_nssai.h"
#include "srsgnb/adt/optional.h"

namespace srsgnb {

// See TS 38.463 Section 9.3.1.21: PDU Session ID valid values: (0..255)
constexpr static uint16_t MAX_NOF_PDU_SESSIONS = 256;

/// \brief PDU Session ID.
/// \remark See TS 38.463 Section 9.3.1.21: PDU Session ID valid values: (0..255)
enum class pdu_session_id_t : uint16_t { min = 0, max = MAX_NOF_PDU_SESSIONS - 1, invalid = MAX_NOF_PDU_SESSIONS };

/// Convert PDU Session ID type to integer.
constexpr inline uint16_t pdu_session_id_to_uint(pdu_session_id_t id)
{
  return static_cast<uint16_t>(id);
}

/// Convert integer to PDU Session ID type.
constexpr inline pdu_session_id_t uint_to_pdu_session_id(uint16_t idx)
{
  return static_cast<pdu_session_id_t>(idx);
}

struct slice_support_item {
  s_nssai_t s_nssai;
};

struct nr_cgi_support_item {
  nr_cell_global_id_t nr_cgi;
};

struct non_dyn_5qi_descriptor_t {
  uint16_t           five_qi;
  optional<uint8_t>  qos_prio_level;
  optional<uint16_t> averaging_win;
  optional<uint16_t> max_data_burst_volume;
};

struct ng_ran_qos_support_item {
  non_dyn_5qi_descriptor_t non_dyn_5qi_descriptor;
};

struct qos_params_support_list_t {
  std::vector<ng_ran_qos_support_item> ng_ran_qos_support_list;
};

struct supported_plmns_item_t {
  std::string                            plmn_id;
  std::vector<slice_support_item>        slice_support_list;
  std::vector<nr_cgi_support_item>       nr_cgi_support_list;
  std::vector<qos_params_support_list_t> qos_params_support_list;
};

} // namespace srsgnb
