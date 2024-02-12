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

#include "five_qi.h"
#include "nr_cgi.h"
#include "qos_prio_level.h"
#include "s_nssai.h"
#include "srsran/adt/optional.h"
#include "srsran/sdap/sdap_config.h"
#include "fmt/format.h"

namespace srsran {

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

// See TS 38.463 Section 9.3.1.24: QoS Flow ID valid values: (0..63)
constexpr static uint8_t MAX_NOF_QOS_FLOWS = 64;

/// \brief QoS Flow ID.
/// \remark See TS 38.463 Section 9.3.1.21: QoS Flow ID valid values: (0..63)
enum class qos_flow_id_t : uint8_t { min = 0, max = MAX_NOF_QOS_FLOWS - 1, invalid = MAX_NOF_QOS_FLOWS };

/// Convert QoS Flow ID type to integer.
constexpr inline uint8_t qos_flow_id_to_uint(qos_flow_id_t id)
{
  return static_cast<uint8_t>(id);
}

/// Convert integer to QoS Flow ID type.
constexpr inline qos_flow_id_t uint_to_qos_flow_id(uint8_t idx)
{
  return static_cast<qos_flow_id_t>(idx);
}

/// \brief RAN_UE_ID (non ASN1 type of RAN_UE_NGAP_ID).
/// \remark See TS 38.413 Section 9.3.3.2: RAN_UE_NGAP_ID valid values: (0..2^32-1)
constexpr static uint64_t MAX_NOF_RAN_UES = ((uint64_t)1 << 32);
enum class ran_ue_id_t : uint64_t { min = 0, max = MAX_NOF_RAN_UES - 1, invalid = 0x1ffffffff };

/// Convert RAN_UE_ID type to integer.
inline uint64_t ran_ue_id_to_uint(ran_ue_id_t id)
{
  return static_cast<uint64_t>(id);
}

/// Convert integer to RAN_UE_ID type.
inline ran_ue_id_t uint_to_ran_ue_id(std::underlying_type_t<ran_ue_id_t> id)
{
  return static_cast<ran_ue_id_t>(id);
}

struct slice_support_item_t {
  s_nssai_t s_nssai;
};

struct nr_cgi_support_item_t {
  nr_cell_global_id_t nr_cgi;
};

struct packet_error_rate_t {
  uint8_t per_scalar;
  uint8_t per_exponent;
};

struct dyn_5qi_descriptor_t {
  qos_prio_level_t      qos_prio_level;
  uint16_t              packet_delay_budget;
  packet_error_rate_t   packet_error_rate;
  optional<five_qi_t>   five_qi;
  optional<std::string> delay_crit;
  optional<uint16_t>    averaging_win;
  optional<uint16_t>    max_data_burst_volume;
};

struct non_dyn_5qi_descriptor_t {
  five_qi_t                  five_qi;
  optional<qos_prio_level_t> qos_prio_level;
  optional<uint16_t>         averaging_win;
  optional<uint16_t>         max_data_burst_volume;
};

struct qos_characteristics_t {
  five_qi_t get_five_qi() const
  {
    if (non_dyn_5qi.has_value()) {
      return non_dyn_5qi.value().five_qi;
    } else if (dyn_5qi.has_value()) {
      if (dyn_5qi.value().five_qi.has_value()) {
        return dyn_5qi.value().five_qi.value();
      }
    } else {
      srsran_assertion_failure("Invalid QoS characteristics. Either dynamic or non-dynamic 5QI must be set");
    }
    return five_qi_t::invalid;
  }

  optional<dyn_5qi_descriptor_t>     dyn_5qi;
  optional<non_dyn_5qi_descriptor_t> non_dyn_5qi;
};

struct ng_ran_qos_support_item_t {
  non_dyn_5qi_descriptor_t non_dyn_5qi_descriptor;
};

struct supported_plmns_item_t {
  std::string                            plmn_id;
  std::vector<slice_support_item_t>      slice_support_list;
  std::vector<nr_cgi_support_item_t>     nr_cgi_support_list;
  std::vector<ng_ran_qos_support_item_t> ng_ran_qos_support_list;
};

struct sdap_config_t {
  pdu_session_id_t           pdu_session = pdu_session_id_t::invalid;
  sdap_hdr_dl_cfg            sdap_hdr_dl = sdap_hdr_dl_cfg::absent;
  sdap_hdr_ul_cfg            sdap_hdr_ul = sdap_hdr_ul_cfg::absent;
  bool                       default_drb = false;
  std::vector<qos_flow_id_t> mapped_qos_flows_to_add;
  std::vector<qos_flow_id_t> mapped_qos_flows_to_release;
};

enum class integrity_protection_result_t { performed, not_performed };
enum class confidentiality_protection_result_t { performed, not_performed };

struct security_result_t {
  integrity_protection_result_t       integrity_protection_result;
  confidentiality_protection_result_t confidentiality_protection_result;
};

enum class integrity_protection_indication_t { required, preferred, not_needed };
inline bool from_string(integrity_protection_indication_t& integrity_protection_ind, const std::string& str)
{
  if (str == "required") {
    integrity_protection_ind = integrity_protection_indication_t::required;
    return true;
  }
  if (str == "preferred") {
    integrity_protection_ind = integrity_protection_indication_t::preferred;
    return true;
  }
  if (str == "not_needed") {
    integrity_protection_ind = integrity_protection_indication_t::not_needed;
    return true;
  }
  return false;
}

enum class confidentiality_protection_indication_t { required, preferred, not_needed };
inline bool from_string(confidentiality_protection_indication_t& confidentiality_protection_ind, const std::string& str)
{
  if (str == "required") {
    confidentiality_protection_ind = confidentiality_protection_indication_t::required;
    return true;
  }
  if (str == "preferred") {
    confidentiality_protection_ind = confidentiality_protection_indication_t::preferred;
    return true;
  }
  if (str == "not_needed") {
    confidentiality_protection_ind = confidentiality_protection_indication_t::not_needed;
    return true;
  }
  return false;
}

struct security_indication_t {
  integrity_protection_indication_t       integrity_protection_ind;
  confidentiality_protection_indication_t confidentiality_protection_ind;
};

/// \brief Checks whether a \c security_result shall be sent.
///
/// Helper function to determine whether the \c security_indication shall be replied with a \c security_result by the
/// peer entity, i.e. if either integrity or confidentiality are set to 'preferred' so the peer entity can decide
/// according to its capabilities.
/// Ref: TS 38.413 Sec. 8.2.1.2, TS 38.463 Sec. 8.3.1.2
///
/// \param security_indication The security_indication to be checked.
/// \return True if either integrity or confidentiality are set to 'preferred'; False otherwise.
inline bool security_result_required(const security_indication_t& security_indication)
{
  return security_indication.integrity_protection_ind == integrity_protection_indication_t::preferred ||
         security_indication.confidentiality_protection_ind == confidentiality_protection_indication_t::preferred;
}

enum class activity_notification_level_t : uint8_t { ue = 0, pdu_session = 1, drb = 2, invalid = 3 };

} // namespace srsran

// Formatters
namespace fmt {
template <>
struct formatter<srsran::pdu_session_id_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::pdu_session_id_t& sid, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "psi={:#}", pdu_session_id_to_uint(sid));
  }
};

template <>
struct formatter<srsran::qos_flow_id_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::qos_flow_id_t& qfi, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    switch (qfi) {
      case srsran::qos_flow_id_t::invalid:
        return format_to(ctx.out(), "invalid QFI");
      default:
        return format_to(ctx.out(), "QFI={:#}", qos_flow_id_to_uint(qfi));
    }
  }
};

template <>
struct formatter<srsran::integrity_protection_indication_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::integrity_protection_indication_t& ind, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    switch (ind) {
      case srsran::integrity_protection_indication_t::not_needed:
        return format_to(ctx.out(), "not_needed");
      case srsran::integrity_protection_indication_t::preferred:
        return format_to(ctx.out(), "preferred");
      case srsran::integrity_protection_indication_t::required:
        return format_to(ctx.out(), "required");
    }
    return format_to(ctx.out(), "invalid");
  }
};

template <>
struct formatter<srsran::confidentiality_protection_indication_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::confidentiality_protection_indication_t& ind, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    switch (ind) {
      case srsran::confidentiality_protection_indication_t::not_needed:
        return format_to(ctx.out(), "not_needed");
      case srsran::confidentiality_protection_indication_t::preferred:
        return format_to(ctx.out(), "preferred");
      case srsran::confidentiality_protection_indication_t::required:
        return format_to(ctx.out(), "required");
    }
    return format_to(ctx.out(), "invalid");
  }
};

template <>
struct formatter<srsran::security_indication_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::security_indication_t& security_ind, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "integrity_ind={} confidentiality_ind={}",
                     security_ind.integrity_protection_ind,
                     security_ind.confidentiality_protection_ind);
  }
};

} // namespace fmt
