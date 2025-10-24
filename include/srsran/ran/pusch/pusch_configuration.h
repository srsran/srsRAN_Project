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
#include "srsran/ran/pusch/pusch_mcs.h"
#include "srsran/ran/pusch/tx_scheme_configuration.h"
#include "srsran/ran/ssb/ssb_configuration.h"
#include "srsran/ran/uci/uci_configuration.h"
#include "srsran/scheduler/config/dmrs.h"
#include <optional>

namespace srsran {

enum p0_pusch_alphaset_id : uint8_t {
  MIN_P0_PUSCH_ALPHASET_ID   = 0,
  MAX_P0_PUSCH_ALPHASET_ID   = 29,
  MAX_NOF_P0_PUSCH_ALPHASETS = 30
};

/// \brief Used to configure the UE specific PUSCH parameters applicable to a particular BWP.
/// \remark See TS 38.331, PUSCH-Config.
struct pusch_config {
  struct pusch_power_control {
    struct p0_pusch_alphaset {
      p0_pusch_alphaset_id id;
      /// P0 value for PUSCH with grant (except msg3) in steps of 1dB. When the field is absent the UE applies the value
      /// 0. See TS 38.213, clause 7.1. Values {-16,..,15}.
      std::optional<int8_t> p0;
      /// alpha value for PUSCH with grant (except msg3). When the field is not set the UE applies the value 1. See
      /// TS 38.213, clause 7.1.
      alpha p0_pusch_alpha{alpha::not_set};

      bool operator==(const p0_pusch_alphaset& rhs) const
      {
        return id == rhs.id && p0 == rhs.p0 && p0_pusch_alpha == rhs.p0_pusch_alpha;
      }
      bool operator!=(const p0_pusch_alphaset& rhs) const { return !(rhs == *this); }
    };

    enum pusch_pathloss_ref_rs_id : uint8_t {
      MIN_PUSCH_PATHLOSS_REF_RS_ID  = 0,
      MAX_PUSCH_PATHLOSS_REF_RS_ID  = 3,
      MAX_NOF_PUSCH_PATHLOSS_REF_RS = 4
    };

    /// \brief Reference Signals (e.g. a CSI-RS config or a SS block) to be used for PUSCH path loss estimation.
    /// \remark See TS 38.213, clause 7.1 and TS 38.331, "PUSCH-PathlossReferenceRS".
    struct pusch_pathloss_ref_rs {
      pusch_pathloss_ref_rs_id                    id;
      std::variant<nzp_csi_rs_res_id_t, ssb_id_t> rs;

      bool operator==(const pusch_pathloss_ref_rs& rhs) const { return id == rhs.id && rs == rhs.rs; }
      bool operator!=(const pusch_pathloss_ref_rs& rhs) const { return !(rhs == *this); }
    };

    enum sri_pusch_pwr_ctrl_id : uint8_t {
      MIN_SRI_PUSCH_PWR_CTRL_ID  = 0,
      MAX_SRI_PUSCH_PWR_CTRL_ID  = 15,
      MAX_NOF_SRI_PUSCH_MAPPINGS = 16
    };

    /// \brief SRI-PUSCH-PowerControl elements.
    /// \remark See TS 38.331, "SRI-PUSCH-PowerControl".
    struct sri_pusch_pwr_ctrl {
      /// \brief The index of the closed power control loop associated with this SRI-PUSCH-PowerControl.
      enum class sri_pusch_closed_loop_index : unsigned { i0, i1 };

      /// The ID of this SRI-PUSCH-PowerControl configuration. It is used as the codepoint (payload) in the SRI DCI
      /// field.
      sri_pusch_pwr_ctrl_id id;
      /// The ID of PUSCH-PathlossReferenceRS as configured in the pathlossReferenceRSToAddModList in
      /// PUSCH-PowerControl.
      pusch_pathloss_ref_rs_id sri_pusch_pathloss_ref_rs_id;
      /// The ID of a P0-PUSCH-AlphaSet as configured in p0-AlphaSets in PUSCH-PowerControl.
      p0_pusch_alphaset_id        sri_p0_pusch_alphaset_id;
      sri_pusch_closed_loop_index closed_loop_idx;

      bool operator==(const sri_pusch_pwr_ctrl& rhs) const
      {
        return id == rhs.id && sri_pusch_pathloss_ref_rs_id == rhs.sri_pusch_pathloss_ref_rs_id &&
               sri_p0_pusch_alphaset_id == rhs.sri_p0_pusch_alphaset_id && closed_loop_idx == rhs.closed_loop_idx;
      }
      bool operator!=(const sri_pusch_pwr_ctrl& rhs) const { return !(rhs == *this); }
    };

    /// If enabled, UE applies TPC commands via accumulation. If not enabled, UE applies the TPC command without
    /// accumulation.
    /// Mapping to NR RRC: If true, the field is present in NR RRC indicating value disabled to UE. If false, the field
    /// is absent in NR RRC and TPC accumulation is enabled.
    bool is_tpc_accumulation_disabled{false};
    /// Dedicated alpha value for msg3 PUSCH. When the field is not set the UE applies the value 1.
    alpha msg3_alpha{alpha::not_set};
    /// P0 value for UL grant-free/SPS based PUSCH. Value in dBm. Only even values (step size 2) allowed. See TS 38.213,
    /// clause 7.1. Values {-202,..,24}.
    std::optional<int16_t> p0_nominal_without_grant;
    /// Configuration {p0-pusch, alpha} sets for PUSCH (except msg3), i.e. { {p0,alpha,index1}, {p0,alpha,index2},...}.
    /// When no set is configured, the UE uses the P0-nominal for msg3 PUSCH, P0-UE is set to 0 and alpha is set
    /// according to msg3-Alpha configured for msg3 PUSCH.
    static_vector<p0_pusch_alphaset, p0_pusch_alphaset_id::MAX_NOF_P0_PUSCH_ALPHASETS> p0_alphasets;
    /// Set of Reference Signals (e.g. a CSI-RS config or a SS block) to be used for PUSCH path loss estimation.
    static_vector<pusch_pathloss_ref_rs, pusch_pathloss_ref_rs_id::MAX_NOF_PUSCH_PATHLOSS_REF_RS> pathloss_ref_rs;
    /// A list of SRI-PUSCH-PowerControl elements among which one is selected by the SRI field in DCI (see TS 38.213,
    /// clause 7.1).
    static_vector<sri_pusch_pwr_ctrl, sri_pusch_pwr_ctrl_id::MAX_NOF_SRI_PUSCH_MAPPINGS> sri_pusch_mapping;

    // TODO: Remaining.

    bool operator==(const pusch_power_control& rhs) const
    {
      return is_tpc_accumulation_disabled == rhs.is_tpc_accumulation_disabled && msg3_alpha == rhs.msg3_alpha &&
             p0_nominal_without_grant == rhs.p0_nominal_without_grant && p0_alphasets == rhs.p0_alphasets &&
             pathloss_ref_rs == rhs.pathloss_ref_rs && sri_pusch_mapping == rhs.sri_pusch_mapping;
    }
    bool operator!=(const pusch_power_control& rhs) const { return !(rhs == *this); }
  };

  /// \brief Resource allocation type of to DCI format 0_1.
  /// \remark See TS 38.214, clause 6.1.2.
  enum class resource_allocation : unsigned { resource_allocation_type_0, resource_allocation_type_1, dynamic_switch };

  /// \brief Transformer precoder for PUSCH.
  /// \remark See TS 38.214, clause 6.1.3.
  enum class transform_precoder : unsigned { enabled, disabled, not_set };

  /// \brief HARQ process number for DCI Format 0_1.
  /// \remark See TS38.212, Section 7.3.1.1.2 Format 0_1.
  enum class harq_process_num_dci_0_1_size { n4 = 4, n5 = 5 };

  /// \brief HARQ process number for DCI Format 0_2.
  /// \remark See TS38.212, Section 7.3.1.1.3 Format 0_2.
  enum class harq_process_num_dci_0_2_size { n0 = 0, n1 = 1, n2 = 2, n3 = 3, n4 = 4, n5 = 5 };

  /// Identifier used to initalite data scrambling (c_init) for PUSCH. If the field is absent, the UE applies the
  /// physical cell ID. See TS 38.211, clause 6.3.1.1.
  std::optional<uint16_t> data_scrambling_id_pusch;
  /// \brief PUSCH transmission schemes according to TS 38.214 Section 6.1.1.
  ///
  /// The transmit scheme is given by the field \e txConfig in TS 38.331 Section 6.3.2, Information Element
  /// \e PUSCH-Config.
  ///
  /// The UE is not supposed to receive DCI Format 0_1 if this field is not present.
  std::optional<pusch_tx_scheme_configuration> tx_cfg;
  /// DMRS configuration for PUSCH transmissions using PUSCH (chosen dynamically via
  /// PUSCH-TimeDomainResourceAllocation). Only the fields dmrs-Type, dmrs-AdditionalPosition and maxLength may be set
  /// differently for mapping type A and B. The field dmrs-UplinkForPUSCH-MappingTypeA applies to DCI format 0_1.
  std::optional<dmrs_uplink_config>  pusch_mapping_type_a_dmrs;
  std::optional<dmrs_uplink_config>  pusch_mapping_type_b_dmrs;
  std::optional<pusch_power_control> pusch_pwr_ctrl;
  resource_allocation                res_alloc;
  /// PUSCH time domain resource allocations. Size: (0..maxNrofUL-Allocations=16).
  std::vector<pusch_time_domain_resource_allocation> pusch_td_alloc_list;
  /// Indicates which MCS table the UE shall use for PUSCH.
  pusch_mcs_table mcs_table{pusch_mcs_table::qam64};
  /// The UE specific selection of transformer precoder for PUSCH. When the field is not set the UE applies the value of
  /// the field msg3-transformPrecoder.
  transform_precoder trans_precoder{transform_precoder::not_set};

  /// \c uci-OnPUSCH.
  std::optional<uci_on_pusch> uci_cfg;

  /// The UE specific HARQ Process number field size in DCI Format 0_1.
  /// 5 bits if higher layer parameter harq-ProcessNumberSizeDCI-0-1 is configured; otherwise 4 bit.
  std::optional<harq_process_num_dci_0_1_size> harq_process_num_size_dci_0_1{harq_process_num_dci_0_1_size::n4};

  /// The UE specific HARQ Process number field size in DCI Format 0_2.
  /// 5 bits determined by higher layer parameter harq-ProcessNumberSizeDCI-0-2-v1700 if configured.
  /// otherwise 0, 1, 2, 3 or 4 bits determined by higher layer parameter harq-ProcessNumberSizeDCI-0-2.
  std::optional<harq_process_num_dci_0_2_size> harq_process_num_size_dci_0_2{harq_process_num_dci_0_2_size::n4};

  bool operator==(const pusch_config& rhs) const
  {
    return data_scrambling_id_pusch == rhs.data_scrambling_id_pusch && tx_cfg == rhs.tx_cfg &&
           pusch_mapping_type_a_dmrs == rhs.pusch_mapping_type_a_dmrs &&
           pusch_mapping_type_b_dmrs == rhs.pusch_mapping_type_b_dmrs && pusch_pwr_ctrl == rhs.pusch_pwr_ctrl &&
           res_alloc == rhs.res_alloc && mcs_table == rhs.mcs_table && trans_precoder == rhs.trans_precoder &&
           uci_cfg == rhs.uci_cfg && pusch_td_alloc_list == rhs.pusch_td_alloc_list &&
           harq_process_num_size_dci_0_1 == rhs.harq_process_num_size_dci_0_1 &&
           harq_process_num_size_dci_0_2 == rhs.harq_process_num_size_dci_0_2;
  }
  bool operator!=(const pusch_config& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
