/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/ran/alpha.h"
#include "srsran/ran/csi_rs/csi_rs_id.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/ssb_configuration.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include <variant>

namespace srsran {

/// Transmission comb size, as per \c transmissionComb, in \c SRS-Config, TS 38.331, or \f$K_{TC}\f$, as per
/// Section 6.4.1.4.1, TS 38.211. Values {2, 4}.
enum class tx_comb_size : uint8_t { n2 = 2, n4 = 4 };

/// Transmission comb pos size, as per \c transmissionComb, in \c SRS-Config, TS 38.331, or \f$K_{TC}\f$, as per
/// Section 6.4.1.4.1, TS 38.211. Values {2, 4, 8}.
enum class tx_comb_pos_size : uint8_t { n2 = 2, n4 = 4, n8 = 8 };

/// \brief \c groupOrSequenceHopping, parameter for configuring group or sequence hopping.
/// \remark See TS 38.211, clause 6.4.1.4.2, or TS 38.331, "SRS-Resource".
enum class srs_group_or_sequence_hopping { neither, group_hopping, sequence_hopping };

/// \brief \c resourceType, as per TS 38.331, "SRS-Resource".
enum class srs_resource_type { aperiodic, semi_persistent, periodic };

/// \brief SRS resource usage.
/// \remark See TS 38.214, clause 6.2.1.
enum class srs_usage : uint8_t { beam_management, codebook, non_codebook, antenna_switching };

/// Convert SRS resource type to string.
inline std::string_view to_string(srs_resource_type res_type)
{
  switch (res_type) {
    case srs_resource_type::aperiodic:
      return "aperiodic";
    case srs_resource_type::semi_persistent:
      return "semi-persistent";
    case srs_resource_type::periodic:
      return "periodic";
    default:
      break;
  }
  return "Invalid srs resource type";
}

enum srs_nof_symbols : uint8_t {
  n1 = 1,
  n2 = 2,
  n4 = 4,
};

enum class srs_periodicity : uint16_t {
  sl1    = 1,
  sl2    = 2,
  sl4    = 4,
  sl5    = 5,
  sl8    = 8,
  sl10   = 10,
  sl16   = 16,
  sl20   = 20,
  sl32   = 32,
  sl40   = 40,
  sl64   = 64,
  sl80   = 80,
  sl160  = 160,
  sl320  = 320,
  sl640  = 640,
  sl1280 = 1280,
  sl2560 = 2560
};

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

  enum srs_pos_res_id : uint8_t {
    MIN_SRS_POS_RES_ID          = 0,
    MAX_SRS_POS_RES_ID          = 63,
    MAX_NOF_SRS_POS_RES         = 64,
    MAX_NOF_SRS_POS_RES_PER_SET = 16
  };

  struct srs_res_id_t {
    /// Resource ID within the cell. The UE doesn't care about this value.
    unsigned cell_res_id;
    /// Resource ID for the UE.
    srs_res_id ue_res_id;

    bool operator==(const srs_res_id_t& rhs) const
    {
      return cell_res_id == rhs.cell_res_id && ue_res_id == rhs.ue_res_id;
    }
  };

  struct srs_resource_set {
    struct aperiodic_resource_type {
      /// The DCI "code point" upon which the UE shall transmit SRS according to this SRS resource set configuration.
      /// See TS 38.214, clause 6.1.1.2.
      /// Values {1,...,maxNrofSRS-TriggerStates-1}, \c max_nof_srs_trigger_states_minus_1.
      uint8_t aperiodic_srs_res_trigger;
      /// ID of CSI-RS resource associated with this SRS resource set.
      /// This field is optionally present, in case of non-codebook based transmission.
      std::optional<nzp_csi_rs_res_id_t> csi_rs;
      /// An offset in number of slots between the triggering DCI and the actual transmission of this SRS-ResourceSet.
      /// If the field is absent the UE applies no offset (value 0). Values {1,...,32}.
      std::optional<unsigned> slot_offset;
      /// An additional list of DCI "code points" upon which the UE shall transmit SRS according to this SRS resource
      /// set configuration. Values {1,...,maxNrofSRS-TriggerStates-1}, \c max_nof_srs_trigger_states_minus_1.
      static_vector<uint8_t, max_nof_srs_trigger_states_minus_2> aperiodic_srs_res_trigger_list;

      bool operator==(const aperiodic_resource_type& rhs) const
      {
        return aperiodic_srs_res_trigger == rhs.aperiodic_srs_res_trigger && csi_rs == rhs.csi_rs &&
               slot_offset == rhs.slot_offset && aperiodic_srs_res_trigger_list == rhs.aperiodic_srs_res_trigger_list;
      }
      bool operator!=(const aperiodic_resource_type& rhs) const { return !(rhs == *this); }
    };

    struct semi_persistent_resource_type {
      // This user provided constructor is added here to fix a Clang compilation error related to the use of nested
      // types with std::optional.
      semi_persistent_resource_type() {}

      /// This field is optionally present, in case of non-codebook based transmission.
      std::optional<nzp_csi_rs_res_id_t> associated_csi_rs;

      bool operator==(const semi_persistent_resource_type& rhs) const
      {
        return associated_csi_rs == rhs.associated_csi_rs;
      }
      bool operator!=(const semi_persistent_resource_type& rhs) const { return !(rhs == *this); }
    };

    struct periodic_resource_type {
      // This user provided constructor is added here to fix a Clang compilation error related to the use of nested
      // types with std::optional.
      periodic_resource_type() {}

      /// This field is optionally present, in case of non-codebook based transmission.
      std::optional<nzp_csi_rs_res_id_t> associated_csi_rs;

      bool operator==(const periodic_resource_type& rhs) const { return associated_csi_rs == rhs.associated_csi_rs; }
      bool operator!=(const periodic_resource_type& rhs) const { return !(rhs == *this); }
    };

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
    std::variant<aperiodic_resource_type, semi_persistent_resource_type, periodic_resource_type> res_type;
    /// Indicates if the SRS resource set is used for beam management, codebook based or non-codebook based
    /// transmission or antenna switching.
    srs_usage srs_res_set_usage;
    /// When the field is not set the UE applies the value 1.
    alpha srs_pwr_ctrl_alpha{alpha::not_set};
    /// P0 value for SRS power control. The value is in dBm. Only even values (step size 2) are allowed. Values
    /// {-202,...,24}. This field is mandatory present upon configuration of SRS-ResourceSet or SRS-Resource and
    /// optionally present otherwise.
    std::optional<int16_t> p0;
    /// Indicates whether hsrs,c(i) = fc(i,1) or hsrs,c(i) = fc(i,2) (if twoPUSCH-PC-AdjustmentStates are configured) or
    /// separate close loop is configured for SRS. This parameter is applicable only for Uls on which UE also transmits
    /// PUSCH. If not set, the UE applies the value sameAs-Fci1.
    srs_pwr_ctrl_adjustment_states pwr_ctrl_adj_states{srs_pwr_ctrl_adjustment_states::not_set};
    /// A reference signal (e.g. a CSI-RS config or a SS block) to be used for SRS path loss estimation.
    std::optional<std::variant<ssb_id_t, nzp_csi_rs_res_id_t>> pathloss_ref_rs;

    bool operator==(const srs_resource_set& rhs) const
    {
      return id == rhs.id && srs_res_id_list == rhs.srs_res_id_list && res_type == rhs.res_type &&
             srs_res_set_usage == rhs.srs_res_set_usage && srs_pwr_ctrl_alpha == rhs.srs_pwr_ctrl_alpha &&
             p0 == rhs.p0 && pwr_ctrl_adj_states == rhs.pwr_ctrl_adj_states && pathloss_ref_rs == rhs.pathloss_ref_rs;
    }
    bool operator!=(const srs_resource_set& rhs) const { return !(rhs == *this); }
  };

  /// \brief Periodicity and slot offset for a SRS resource, as per \c SRS-PeriodicityAndOffset, TS 38.331,
  /// \c SRS-resource.
  struct srs_periodicity_and_offset {
    /// SRS period, in slots.
    srs_periodicity period;
    /// Offset, Values {0,...,period_slots-1}.
    uint16_t offset;

    bool operator==(const srs_periodicity_and_offset& rhs) const
    {
      return period == rhs.period && offset == rhs.offset;
    }
    bool operator!=(const srs_periodicity_and_offset& rhs) const { return !(rhs == *this); }
  };

  /// \brief OFDM symbol location of the SRS resource.
  struct resource_mapping {
    /// Values {0,...,5}. Value 0 refers to the last symbol, value 1 refers to the second last symbol.
    uint8_t         start_pos;
    srs_nof_symbols nof_symb;
    /// \c repetitionFactor, as per 3GPP TS 38.331, "SRS-Resource".
    /// \remark \ref The condition rept_factor <= \ref nof_symb must be verified, as per TS 38.211, Section 6.4.1.4.3.
    srs_nof_symbols rept_factor;

    bool operator==(const resource_mapping& rhs) const
    {
      return start_pos == rhs.start_pos && nof_symb == rhs.nof_symb && rept_factor == rhs.rept_factor;
    }
    bool operator!=(const resource_mapping& rhs) const { return !(rhs == *this); }
  };

  struct srs_resource {
    /// \brief Number of ports for SRS.
    enum class nof_srs_ports { port1 = 1, ports2 = 2, ports4 = 4 };

    /// \brief The PTRS port index for a SRS resource for non-codebook based UL MIMO.
    enum class ptrs_port_index { n0, n1, not_set };

    /// \brief Includes parameters capturing SRS frequency hopping.
    /// \remark See 3GPP TS 38.331, "SRS-Resource".
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

    /// \brief Configuration of the spatial relation between a reference RS and the target SRS.
    struct srs_spatial_relation_info {
      std::optional<du_cell_index_t> serv_cell_id;

      struct srs_ref_signal {
        srs_res_id res_id;
        bwp_id_t   ul_bwp;

        bool operator==(const srs_ref_signal& rhs) const { return res_id == rhs.res_id && ul_bwp == rhs.ul_bwp; }
        bool operator!=(const srs_ref_signal& rhs) const { return !(rhs == *this); }
      };

      std::variant<ssb_id_t, nzp_csi_rs_res_id_t, srs_ref_signal> reference_signal;

      bool operator==(const srs_spatial_relation_info& rhs) const
      {
        return serv_cell_id == rhs.serv_cell_id && reference_signal == rhs.reference_signal;
      }
      bool operator!=(const srs_spatial_relation_info& rhs) const { return !(rhs == *this); }
    };

    srs_res_id_t    id;
    nof_srs_ports   nof_ports;
    ptrs_port_index ptrs_port{ptrs_port_index::not_set};

    struct tx_comb_params {
      tx_comb_size size;
      /// Values {0,...,tx_comb_size-1}. See 3GPP TS 38.331, "SRS-Resource".
      uint8_t tx_comb_offset;
      /// For Transmission Comb size:
      ///  2 => Cyclic shift configuration. Values {0,...,7}.
      ///  4 => Cyclic shift configuration. Values {0,...,11}.
      /// See 3GPP TS 38.331, "SRS-Resource".
      uint8_t tx_comb_cyclic_shift;

      bool operator==(const tx_comb_params& rhs) const
      {
        return size == rhs.size and tx_comb_offset == rhs.tx_comb_offset and
               tx_comb_cyclic_shift == rhs.tx_comb_cyclic_shift;
      }
      bool operator!=(const tx_comb_params& rhs) const { return !(rhs == *this); }
    };

    /// \c transmissionComb, as per TS 38.331, "SRS-Resource".
    tx_comb_params tx_comb;
    /// \c resourceMapping, as per TS 38.331, "SRS-Resource".
    resource_mapping res_mapping;
    /// \c freqDomainPosition, as per TS 38.331, "SRS-Resource", or \f$n_{RRC}\f$, as per TS 38.211, Section 6.4.1.4.3.
    /// Values {0,...,67}.
    uint8_t freq_domain_pos;
    /// \c freqDomainShift, as per TS 38.331, "SRS-Resource", or \f$n_{shift}\f$, as per TS 38.211, Section 6.4.1.4.3.
    /// Values {0,...,268}.
    uint16_t                      freq_domain_shift;
    frequency_hopping             freq_hop;
    srs_group_or_sequence_hopping grp_or_seq_hop;
    srs_resource_type             res_type;
    /// Set/Valid only if resource type is "semi-persistent" or "periodic".
    std::optional<srs_periodicity_and_offset> periodicity_and_offset;
    /// Sequence ID used to initialize pseudo random group and sequence hopping. Values {0,...,1023}.
    uint16_t                                 sequence_id;
    std::optional<srs_spatial_relation_info> spatial_relation_info;

    bool operator==(const srs_resource& rhs) const
    {
      return id == rhs.id && nof_ports == rhs.nof_ports && ptrs_port == rhs.ptrs_port && tx_comb == rhs.tx_comb &&
             res_mapping == rhs.res_mapping && freq_domain_pos == rhs.freq_domain_pos &&
             freq_domain_shift == rhs.freq_domain_shift && freq_hop == rhs.freq_hop &&
             grp_or_seq_hop == rhs.grp_or_seq_hop && res_type == rhs.res_type &&
             periodicity_and_offset == rhs.periodicity_and_offset && sequence_id == rhs.sequence_id &&
             spatial_relation_info == rhs.spatial_relation_info;
    }
    bool operator!=(const srs_resource& rhs) const { return !(rhs == *this); }
  };

  struct srs_pos_resource {
    struct tx_comb_pos_params {
      tx_comb_pos_size size;
      /// Values {0,...,tx_comb_size-1}. See 3GPP TS 38.331, "SRS-Resource".
      uint8_t tx_comb_pos_offset;
      /// For Transmission Comb size:
      ///  2 => Cyclic shift configuration. Values {0,...,7}.
      ///  4 => Cyclic shift configuration. Values {0,...,11}.
      ///  8 => Cyclic shift configuration. Values {0,...,5}.
      /// See 3GPP TS 38.331, "SRS-Resource".
      uint8_t tx_comb_pos_cyclic_shift;

      bool operator==(const tx_comb_pos_params& rhs) const
      {
        return size == rhs.size and tx_comb_pos_offset == rhs.tx_comb_pos_offset and
               tx_comb_pos_cyclic_shift == rhs.tx_comb_pos_cyclic_shift;
      }
      bool operator!=(const tx_comb_pos_params& rhs) const { return !(rhs == *this); }
    };

    /// \brief Configuration of the spatial relation between a reference RS and the target SRS.
    struct srs_spatial_relation_pos {
      struct ssb {
        pci_t                   pci_nr;
        std::optional<ssb_id_t> ssb_idx;

        bool operator==(const ssb& rhs) const { return pci_nr == rhs.pci_nr && ssb_idx == rhs.ssb_idx; }
        bool operator!=(const ssb& rhs) const { return !(rhs == *this); }
      };

      struct prs {
        uint16_t               id;
        uint8_t                prs_res_set_id;
        std::optional<uint8_t> prs_res_id;

        bool operator==(const prs& rhs) const
        {
          return id == rhs.id && prs_res_set_id == rhs.prs_res_set_id && prs_res_id == rhs.prs_res_id;
        }
        bool operator!=(const prs& rhs) const { return !(rhs == *this); }
      };

      std::variant<ssb, prs> reference_signal;

      bool operator==(const srs_spatial_relation_pos& rhs) const { return reference_signal == rhs.reference_signal; }
      bool operator!=(const srs_spatial_relation_pos& rhs) const { return !(rhs == *this); }
    };

    srs_res_id_t id;
    /// \c transmissionComb, as per TS 38.331, "SRS-PosResource".
    tx_comb_pos_params tx_comb;
    /// \c resourceMapping, as per TS 38.331, "SRS-PosResource".
    resource_mapping res_mapping;
    /// \c freqDomainPosition, as per TS 38.331, "SRS-PosResource", or \f$n_{RRC}\f$, as per TS 38.211,
    /// Section 6.4.1.4.3. Values {0,...,67}.
    uint8_t freq_domain_pos;
    /// \c freqDomainShift, as per TS 38.331, "SRS-PosResource", or \f$n_{shift}\f$, as per TS 38.211,
    /// Section 6.4.1.4.3. Values {0,...,268}.
    uint16_t freq_domain_shift;
    /// Values {0,...,63}.
    uint8_t                       c_srs;
    srs_group_or_sequence_hopping grp_or_seq_hop;
    srs_resource_type             res_type;
    /// Set/Valid only if resource type is "semi-persistent" or "periodic".
    std::optional<srs_periodicity_and_offset> periodicity_and_offset;
    /// Set/Valid only if resource type is "aperiodic". Values {0,...,32}.
    std::optional<uint8_t> slot_offset;
    /// Sequence ID used to initialize pseudo random group and sequence hopping. Values {0,...,1023}.
    uint16_t                                sequence_id;
    std::optional<srs_spatial_relation_pos> spatial_relation_info;

    bool operator==(const srs_pos_resource& rhs) const
    {
      return id == rhs.id && tx_comb == rhs.tx_comb && res_mapping == rhs.res_mapping &&
             freq_domain_pos == rhs.freq_domain_pos && freq_domain_shift == rhs.freq_domain_shift &&
             c_srs == rhs.c_srs && grp_or_seq_hop == rhs.grp_or_seq_hop && res_type == rhs.res_type &&
             periodicity_and_offset == rhs.periodicity_and_offset && slot_offset == rhs.slot_offset &&
             sequence_id == rhs.sequence_id && spatial_relation_info == rhs.spatial_relation_info;
    }
    bool operator!=(const srs_pos_resource& rhs) const { return !(rhs == *this); }
  };

  static_vector<srs_resource_set, srs_res_set_id::MAX_NOF_SRS_RES_SETS> srs_res_set_list;
  static_vector<srs_resource, srs_res_id::MAX_NOF_SRS_RES>              srs_res_list;
  static_vector<srs_resource_set, srs_res_set_id::MAX_NOF_SRS_RES_SETS> pos_srs_res_set_list;
  static_vector<srs_pos_resource, srs_pos_res_id::MAX_NOF_SRS_POS_RES>  pos_srs_res_list;
  /// Mapping to NR RRC: If true, the field is present in NR RRC indicating value disabled and UE applies the TPC
  /// command without accumulation (this applies to SRS when a separate closed loop is configured for SRS). If false,
  /// the field is absent in NR RRC and UE applies TPC commands via accumulation.
  bool is_tpc_accum_disabled{false};

  bool operator==(const srs_config& rhs) const
  {
    return srs_res_set_list == rhs.srs_res_set_list && srs_res_list == rhs.srs_res_list &&
           pos_srs_res_set_list == rhs.pos_srs_res_set_list && pos_srs_res_list == rhs.pos_srs_res_list &&
           is_tpc_accum_disabled == rhs.is_tpc_accum_disabled;
  }
  bool operator!=(const srs_config& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
