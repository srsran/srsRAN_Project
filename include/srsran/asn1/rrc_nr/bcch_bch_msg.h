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

#include "srsran/asn1/asn1_utils.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// PDCCH-ConfigSIB1 ::= SEQUENCE
struct pdcch_cfg_sib1_s {
  uint8_t coreset_zero      = 0;
  uint8_t search_space_zero = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIB ::= SEQUENCE
struct mib_s {
  struct sub_carrier_spacing_common_opts {
    enum options { scs15or60, scs30or120, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using sub_carrier_spacing_common_e_ = enumerated<sub_carrier_spacing_common_opts>;
  struct dmrs_type_a_position_opts {
    enum options { pos2, pos3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using dmrs_type_a_position_e_ = enumerated<dmrs_type_a_position_opts>;
  struct cell_barred_opts {
    enum options { barred, not_barred, nulltype } value;

    const char* to_string() const;
  };
  using cell_barred_e_ = enumerated<cell_barred_opts>;
  struct intra_freq_resel_opts {
    enum options { allowed, not_allowed, nulltype } value;

    const char* to_string() const;
  };
  using intra_freq_resel_e_ = enumerated<intra_freq_resel_opts>;

  // member variables
  fixed_bitstring<6>            sys_frame_num;
  sub_carrier_spacing_common_e_ sub_carrier_spacing_common;
  uint8_t                       ssb_subcarrier_offset = 0;
  dmrs_type_a_position_e_       dmrs_type_a_position;
  pdcch_cfg_sib1_s              pdcch_cfg_sib1;
  cell_barred_e_                cell_barred;
  intra_freq_resel_e_           intra_freq_resel;
  fixed_bitstring<1>            spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCCH-BCH-MessageType ::= CHOICE
struct bcch_bch_msg_type_c {
  struct types_opts {
    enum options { mib, msg_class_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  bcch_bch_msg_type_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  mib_s& mib()
  {
    assert_choice_type(types::mib, type_, "BCCH-BCH-MessageType");
    return c;
  }
  const mib_s& mib() const
  {
    assert_choice_type(types::mib, type_, "BCCH-BCH-MessageType");
    return c;
  }
  mib_s& set_mib();
  void   set_msg_class_ext();

private:
  types type_;
  mib_s c;
};

// BCCH-BCH-Message ::= SEQUENCE
struct bcch_bch_msg_s {
  bcch_bch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
