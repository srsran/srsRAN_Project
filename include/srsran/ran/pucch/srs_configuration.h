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

#include "srsran/adt/variant.h"
#include "srsran/ran/alpha.h"
#include "srsran/ran/csi_rs/csi_rs_id.h"

namespace srsran {

/// Used to configure Sounding Reference Signal transmissions or to configure Sounding Reference Signal
/// measurements for CLI.
/// \remark See TS 38.331, "SRS-Config".
struct srs_config {
  /// Maximum number of SRS trigger states minus 1, i.e., the largest code point.
  static constexpr unsigned max_nof_srs_trigger_states_minus_1 = 3;
  /// Maximum number of SRS trigger states minus 2.
  static constexpr unsigned max_nof_srs_trigger_states_minus_2 = 2;

  enum srs_res_set_id : uint8_t {
    MIN_SRS_RES_SET_ID   = 0,
    MAX_SRS_RES_SET_ID   = 15,
    MAX_NOF_SRS_RES_SETS = 16,
  };

  enum srs_res_id : uint8_t {
    MIN_SRS_RES_ID          = 0,
    MAX_SRS_RES_ID          = 63,
    MAX_NOF_SRS_RES         = 64,
    MAX_NOF_SRS_RES_PER_SET = 16
  };

  struct srs_resource_set {
    struct aperiodic_resource_type {
      /// The DCI "code point" upon which the UE shall transmit SRS according to this SRS resource set configuration.
      /// See TS 38.214, clause 6.1.1.2.
      /// Values {1,..,maxNrofSRS-TriggerStates-1}, \c max_nof_srs_trigger_states_minus_1.
      uint8_t aperiodic_srs_res_trigger;
      /// ID of CSI-RS resource associated with this SRS resource set.
      /// This field is optionally present, in case of non-codebook based transmission.
      optional<nzp_csi_rs_res_id_t> csi_rs;
      /// An offset in number of slots between the triggering DCI and the actual transmission of this SRS-ResourceSet.
      /// If the field is absent the UE applies no offset (value 0). Values {1,..,32}.
      optional<unsigned> slot_offset;
      /// An additional list of DCI "code points" upon which the UE shall transmit SRS according to this SRS resource
      /// set configuration. Values {1,..,maxNrofSRS-TriggerStates-1}, \c max_nof_srs_trigger_states_minus_1.
      static_vector<uint8_t, max_nof_srs_trigger_states_minus_2> aperiodic_srs_res_trigger_list;

      bool operator==(const aperiodic_resource_type& rhs) const
      {
        return aperiodic_srs_res_trigger == rhs.aperiodic_srs_res_trigger && csi_rs == rhs.csi_rs &&
               slot_offset == rhs.slot_offset && aperiodic_srs_res_trigger_list == rhs.aperiodic_srs_res_trigger_list;
      }
      bool operator!=(const aperiodic_resource_type& rhs) const { return !(rhs == *this); }
    };

    struct semi_persistent_resource_type {
      /// This field is optionally present, in case of non-codebook based transmission.
      optional<nzp_csi_rs_res_id_t> associated_csi_rs;

      bool operator==(const semi_persistent_resource_type& rhs) const
      {
        return associated_csi_rs == rhs.associated_csi_rs;
      }
      bool operator!=(const semi_persistent_resource_type& rhs) const { return !(rhs == *this); }
    };

    struct periodic_resource_type {
      /// This field is optionally present, in case of non-codebook based transmission.
      optional<nzp_csi_rs_res_id_t> associated_csi_rs;

      bool operator==(const periodic_resource_type& rhs) const { return associated_csi_rs == rhs.associated_csi_rs; }
      bool operator!=(const periodic_resource_type& rhs) const { return !(rhs == *this); }
    };

    /// \brief SRS resource usage.
    /// \remark See TS 38.214, clause 6.2.1.
    enum class usage { beam_management, codebook, non_codebook, antenna_switching };

    enum class srs_pwr_ctrl_adjustment_states { same_as_fci2, separate_closed_loop, not_set };

    /// The ID of this resource set. It is unique in the context of the BWP in which the parent SRS-Config is defined.
    srs_res_set_id id;
    /// The IDs of the SRS-Resources used in this SRS-ResourceSet. If this SRS-ResourceSet is configured with usage set
    /// to codebook, the srs-ResourceIdList contains at most 2 entries. If this SRS-ResourceSet is configured with usage
    /// set to nonCodebook, the srs-ResourceIdList contains at most 4 entries. This field is mandatory present upon
    /// configuration of SRS-ResourceSet or SRS-Resource and optionally present otherwise.
    static_vector<srs_res_id, srs_res_id::MAX_NOF_SRS_RES_PER_SET> srs_res_id_list;
    /// Time domain behavior of SRS resource configuration. The network configures SRS resources in the same resource
    /// set with the same time domain behavior on periodic, aperiodic and semi-persistent SRS.
    variant<aperiodic_resource_type, semi_persistent_resource_type, periodic_resource_type> res_type;
    /// Indicates if the SRS resource set is used for beam management, codebook based or non-codebook based
    /// transmission or antenna switching.
    usage srs_res_set_usage;
    /// When the field is not set the UE applies the value 1.
    alpha srs_pwr_ctrl_alpha{alpha::not_set};
    /// P0 value for SRS power control. The value is in dBm. Only even values (step size 2) are allowed. Values
    /// {-202,..,24}. This field is mandatory present upon configuration of SRS-ResourceSet or SRS-Resource and
    /// optionally present otherwise.
    optional<int16_t> p0;
    /// Indicates whether hsrs,c(i) = fc(i,1) or hsrs,c(i) = fc(i,2) (if twoPUSCH-PC-AdjustmentStates are configured) or
    /// separate close loop is configured for SRS. This parameter is applicable only for Uls on which UE also transmits
    /// PUSCH. If not set, the UE applies the value sameAs-Fci1.
    srs_pwr_ctrl_adjustment_states pwr_ctrl_adj_states{srs_pwr_ctrl_adjustment_states::not_set};
    /// A reference signal (e.g. a CSI-RS config or a SS block) to be used for SRS path loss estimation.
    optional<variant<ssb_id_t, nzp_csi_rs_res_id_t>> pathloss_ref_rs;

    bool operator==(const srs_resource_set& rhs) const
    {
      return id == rhs.id && srs_res_id_list == rhs.srs_res_id_list && res_type == rhs.res_type &&
             srs_res_set_usage == rhs.srs_res_set_usage && srs_pwr_ctrl_alpha == rhs.srs_pwr_ctrl_alpha &&
             p0 == rhs.p0 && pwr_ctrl_adj_states == rhs.pwr_ctrl_adj_states && pathloss_ref_rs == rhs.pathloss_ref_rs;
    }
    bool operator!=(const srs_resource_set& rhs) const { return !(rhs == *this); }
  };

  /// \brief Periodicity and slot offset for a SRS resource.
  struct srs_periodicity_and_offset {
    enum class type_t {
      sl1,
      sl2,
      sl4,
      sl5,
      sl8,
      sl10,
      sl16,
      sl20,
      sl32,
      sl40,
      sl64,
      sl80,
      sl160,
      sl320,
      sl640,
      sl1280,
      sl2560
    };

    type_t type;
    /// If type is slX, then valid values are {0,..,X-1}.
    uint16_t value;

    bool operator==(const srs_periodicity_and_offset& rhs) const { return type == rhs.type && value == rhs.value; }
    bool operator!=(const srs_periodicity_and_offset& rhs) const { return !(rhs == *this); }
  };

  struct srs_resource {
    /// \brief Number of ports for SRS.
    enum class nof_srs_ports { port1 = 1, ports2 = 2, ports4 = 4 };

    /// \brief The PTRS port index for a SRS resource for non-codebook based UL MIMO.
    enum class ptrs_port_index { n0, n1, not_set };

    /// \brief OFDM symbol location of the SRS resource.
    struct resource_mapping {
      /// \brief Number of OFDM symbols.
      enum class nof_symbols { n1, n2, n4 };
      enum class repetition_factor { n1, n2, n4 };

      /// Values {0,...,5}. Value 0 refers to the last symbol, value 1 refers to the second last symbol.
      uint8_t           start_pos;
      nof_symbols       nof_symb;
      repetition_factor re_factor;

      bool operator==(const resource_mapping& rhs) const
      {
        return start_pos == rhs.start_pos && nof_symb == rhs.nof_symb && re_factor == rhs.re_factor;
      }
      bool operator!=(const resource_mapping& rhs) const { return !(rhs == *this); }
    };

    /// \brief Includes parameters capturing SRS frequency hopping.
    /// \remark See TS 38.214, clause 6.2.1.
    struct frequency_hopping {
      /// Values {0,...,63}.
      uint8_t c_srs;
      /// Values {0,...,3}.
      uint8_t b_srs;
      /// Values {0,...,3}.
      uint8_t b_hop;

      bool operator==(const frequency_hopping& rhs) const
      {
        return c_srs == rhs.c_srs && b_srs == rhs.b_srs && b_hop == rhs.b_hop;
      }
      bool operator!=(const frequency_hopping& rhs) const { return !(rhs == *this); }
    };

    /// \brief Parameter(s) for configuring group or sequence hopping.
    /// \remark See TS 38.211, clause 6.4.1.4.2
    enum class group_or_sequence_hopping { neither, groupHopping, sequenceHopping };

    enum resource_type { aperiodic, semi_persistent, periodic };

    /// \brief Configuration of the spatial relation between a reference RS and the target SRS.
    struct srs_spatial_relation_info {
      optional<du_cell_index_t> serv_cell_id;

      struct srs_ref_signal {
        srs_res_id res_id;
        bwp_id_t   ul_bwp;

        bool operator==(const srs_ref_signal& rhs) const { return res_id == rhs.res_id && ul_bwp == rhs.ul_bwp; }
        bool operator!=(const srs_ref_signal& rhs) const { return !(rhs == *this); }
      };

      variant<ssb_id_t, nzp_csi_rs_res_id_t, srs_ref_signal> reference_signal;

      bool operator==(const srs_spatial_relation_info& rhs) const
      {
        return serv_cell_id == rhs.serv_cell_id && reference_signal == rhs.reference_signal;
      }
      bool operator!=(const srs_spatial_relation_info& rhs) const { return !(rhs == *this); }
    };

    srs_res_id      id;
    nof_srs_ports   nof_ports;
    ptrs_port_index ptrs_port{ptrs_port_index::not_set};

    /// Values {2,4,8}. See 3GPP TS 38.331, "SRS-Resource".
    uint8_t trans_comb_value;
    /// Values {0,...,trans_comb_value-1}. See 3GPP TS 38.331, "SRS-Resource".
    uint8_t trans_comb_offset;
    /// For Transmission Comb value:
    ///  2 => Cyclic shift configuration. Values {0,...,7}.
    ///  4 => Cyclic shift configuration. Values {0,...,11}.
    /// See TS 38.214, clause 6.2.1.
    uint8_t          trans_comb_cyclic_shift;
    resource_mapping res_mapping;
    /// Values {0,...,67}.
    uint8_t freq_domain_pos;
    /// Values {0,...,268}.
    uint16_t                  freq_domain_shift;
    frequency_hopping         freq_hop;
    group_or_sequence_hopping grp_or_seq_hop;
    resource_type             res_type;
    /// Set/Valid only if resource type is semi-persistent.
    srs_periodicity_and_offset semi_pers_res_type_periodicity_and_offset;
    /// Set/Valid only if resource type is periodic.
    srs_periodicity_and_offset per_res_type_periodicity_and_offset;
    /// Sequence ID used to initialize pseudo random group and sequence hopping. Values {0,...,1023}.
    uint16_t                            sequence_id;
    optional<srs_spatial_relation_info> spatial_relation_info;

    bool operator==(const srs_resource& rhs) const
    {
      return id == rhs.id && nof_ports == rhs.nof_ports && ptrs_port == rhs.ptrs_port &&
             trans_comb_value == rhs.trans_comb_value && trans_comb_offset == rhs.trans_comb_offset &&
             trans_comb_cyclic_shift == rhs.trans_comb_cyclic_shift && res_mapping == rhs.res_mapping &&
             freq_domain_pos == rhs.freq_domain_pos && freq_domain_shift == rhs.freq_domain_shift &&
             freq_hop == rhs.freq_hop && grp_or_seq_hop == rhs.grp_or_seq_hop && res_type == rhs.res_type &&
             semi_pers_res_type_periodicity_and_offset == rhs.semi_pers_res_type_periodicity_and_offset &&
             per_res_type_periodicity_and_offset == rhs.per_res_type_periodicity_and_offset &&
             sequence_id == rhs.sequence_id && spatial_relation_info == rhs.spatial_relation_info;
    }
    bool operator!=(const srs_resource& rhs) const { return !(rhs == *this); }
  };

  static_vector<srs_resource_set, srs_res_set_id::MAX_NOF_SRS_RES_SETS> srs_res_set;
  static_vector<srs_resource, srs_res_id::MAX_NOF_SRS_RES>              srs_res;
  /// Mapping to NR RRC: If true, the field is present in NR RRC indicating value disabled and UE applies the TPC
  /// command without accumulation (this applies to SRS when a separate closed loop is configured for SRS). If false,
  /// the field is absent in NR RRC and UE applies TPC commands via accumulation.
  bool is_tpc_accum_disabled{false};

  bool operator==(const srs_config& rhs) const
  {
    return srs_res_set == rhs.srs_res_set && srs_res == rhs.srs_res &&
           is_tpc_accum_disabled == rhs.is_tpc_accum_disabled;
  }
  bool operator!=(const srs_config& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
