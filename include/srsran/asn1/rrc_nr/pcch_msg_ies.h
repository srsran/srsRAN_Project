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

/*******************************************************************************
 *
 *                    3GPP TS ASN1 RRC NR v17.4.0 (2023-03)
 *
 ******************************************************************************/

#pragma once

#include "radio_bearer_cfg.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// PagingRecord-v1700 ::= SEQUENCE
struct paging_record_v1700_s {
  bool paging_cause_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingUE-Identity ::= CHOICE
struct paging_ue_id_c {
  struct types_opts {
    enum options { ng_5_g_s_tmsi, full_i_rnti, /*...*/ nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  paging_ue_id_c() = default;
  paging_ue_id_c(const paging_ue_id_c& other);
  paging_ue_id_c& operator=(const paging_ue_id_c& other);
  ~paging_ue_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<48>& ng_5_g_s_tmsi()
  {
    assert_choice_type(types::ng_5_g_s_tmsi, type_, "PagingUE-Identity");
    return c.get<fixed_bitstring<48>>();
  }
  fixed_bitstring<40>& full_i_rnti()
  {
    assert_choice_type(types::full_i_rnti, type_, "PagingUE-Identity");
    return c.get<fixed_bitstring<40>>();
  }
  const fixed_bitstring<48>& ng_5_g_s_tmsi() const
  {
    assert_choice_type(types::ng_5_g_s_tmsi, type_, "PagingUE-Identity");
    return c.get<fixed_bitstring<48>>();
  }
  const fixed_bitstring<40>& full_i_rnti() const
  {
    assert_choice_type(types::full_i_rnti, type_, "PagingUE-Identity");
    return c.get<fixed_bitstring<40>>();
  }
  fixed_bitstring<48>& set_ng_5_g_s_tmsi();
  fixed_bitstring<40>& set_full_i_rnti();

private:
  types                                type_;
  choice_buffer_t<fixed_bitstring<48>> c;

  void destroy_();
};

// PagingGroupList-r17 ::= SEQUENCE (SIZE (1..32)) OF TMGI-r17
using paging_group_list_r17_l = dyn_array<tmgi_r17_s>;

// PagingRecord ::= SEQUENCE
struct paging_record_s {
  bool           ext                 = false;
  bool           access_type_present = false;
  paging_ue_id_c ue_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingRecordList-v1700 ::= SEQUENCE (SIZE (1..32)) OF PagingRecord-v1700
using paging_record_list_v1700_l = dyn_array<paging_record_v1700_s>;

// Paging-v1700-IEs ::= SEQUENCE
struct paging_v1700_ies_s {
  bool                       non_crit_ext_present = false;
  paging_record_list_v1700_l paging_record_list_v1700;
  paging_group_list_r17_l    paging_group_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingRecordList ::= SEQUENCE (SIZE (1..32)) OF PagingRecord
using paging_record_list_l = dyn_array<paging_record_s>;

// Paging ::= SEQUENCE
struct paging_s {
  bool                 non_crit_ext_present = false;
  paging_record_list_l paging_record_list;
  dyn_octstring        late_non_crit_ext;
  paging_v1700_ies_s   non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
