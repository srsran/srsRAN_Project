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

#include "meas_cfg.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// MeasurementTimingConfiguration-v1610-IEs ::= SEQUENCE
struct meas_timing_cfg_v1610_ies_s {
  struct csi_rs_cfg_r16_s_ {
    subcarrier_spacing_e csi_rs_subcarrier_spacing_r16;
    csi_rs_cell_mob_s    csi_rs_cell_mob_r16;
    uint32_t             ref_ssb_freq_r16 = 0;
  };

  // member variables
  bool              non_crit_ext_present = false;
  csi_rs_cfg_r16_s_ csi_rs_cfg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasTiming ::= SEQUENCE
struct meas_timing_s {
  struct freq_and_timing_s_ {
    bool                 ss_rssi_meas_present = false;
    uint32_t             carrier_freq         = 0;
    subcarrier_spacing_e ssb_subcarrier_spacing;
    ssb_mtc_s            ssb_meas_timing_cfg;
    ss_rssi_meas_s       ss_rssi_meas;
  };

  // member variables
  bool               ext                     = false;
  bool               freq_and_timing_present = false;
  freq_and_timing_s_ freq_and_timing;
  // ...
  // group 0
  bool                       pci_present = false;
  copy_ptr<ssb_to_measure_c> ssb_to_measure;
  uint16_t                   pci = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementTimingConfiguration-v1550-IEs ::= SEQUENCE
struct meas_timing_cfg_v1550_ies_s {
  bool                        non_crit_ext_present      = false;
  bool                        camp_on_first_ssb         = false;
  bool                        ps_cell_only_on_first_ssb = false;
  meas_timing_cfg_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasTimingList ::= SEQUENCE (SIZE (1..32)) OF MeasTiming
using meas_timing_list_l = dyn_array<meas_timing_s>;

// MeasurementTimingConfiguration-IEs ::= SEQUENCE
struct meas_timing_cfg_ies_s {
  bool                        non_crit_ext_present = false;
  meas_timing_list_l          meas_timing;
  meas_timing_cfg_v1550_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementTimingConfiguration ::= SEQUENCE
struct meas_timing_cfg_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { meas_timing_conf, spare3, spare2, spare1, nulltype } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      c1_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      meas_timing_cfg_ies_s& meas_timing_conf()
      {
        assert_choice_type(types::meas_timing_conf, type_, "c1");
        return c;
      }
      const meas_timing_cfg_ies_s& meas_timing_conf() const
      {
        assert_choice_type(types::meas_timing_conf, type_, "c1");
        return c;
      }
      meas_timing_cfg_ies_s& set_meas_timing_conf();
      void                   set_spare3();
      void                   set_spare2();
      void                   set_spare1();

    private:
      types                 type_;
      meas_timing_cfg_ies_s c;
    };
    struct types_opts {
      enum options { c1, crit_exts_future, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c1_c_& c1()
    {
      assert_choice_type(types::c1, type_, "criticalExtensions");
      return c;
    }
    const c1_c_& c1() const
    {
      assert_choice_type(types::c1, type_, "criticalExtensions");
      return c;
    }
    c1_c_& set_c1();
    void   set_crit_exts_future();

  private:
    types type_;
    c1_c_ c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
