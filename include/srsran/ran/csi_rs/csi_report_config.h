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

#include "codebook_config.h"
#include "csi_resource_config.h"
#include "csi_rs_constants.h"
#include "srsran/adt/variant.h"
#include "srsran/ran/pusch/pusch_configuration.h"
#include "srsran/ran/pusch/pusch_constants.h"

namespace srsran {

enum class csi_resource_periodicity;

/// \brief CSI-ReportConfigId is used to identify one CSI-ReportConfig.
/// \remark See TS 38.331, \c CSI-ReportConfigId.
enum csi_report_config_id_t : uint8_t {
  MIN_CSI_REPORT_CONFIG_ID   = 0,
  MAX_CSI_REPORT_CONFIG_ID   = 47,
  MAX_NOF_CSI_REPORT_CONFIGS = 48,
};

/// Maximum CSI report period in slots.
constexpr unsigned MAX_CSI_REPORT_PERIOD = 320;

/// \brief Periodicity and slot offset.
/// \remark See TS 38.331, \c CSI-ReportPeriodicityAndOffset.
enum class csi_report_periodicity {
  slots4   = 4,
  slots5   = 5,
  slots8   = 8,
  slots10  = 10,
  slots16  = 16,
  slots20  = 20,
  slots40  = 40,
  slots80  = 80,
  slots160 = 160,
  slots320 = 320
};

inline unsigned csi_report_periodicity_to_uint(csi_report_periodicity period)
{
  return static_cast<unsigned>(period);
}

/// \brief CQI table to use for CQI calculation.
/// \remark See TS 38.331, \c cqi-Table in \c CSI-ReportConfig and TS 38.214, clause 5.2.2.1.
/// \remark Value \c table4 is for Rel.16 or above only.
enum class cqi_table_t { table1, table2, table3, table4 };

/// CSI-ReportConfig is used to configure a periodic or semi-persistent report sent on PUCCH on the cell in which the
/// CSI-ReportConfig is included, or to configure a semi-persistent or aperiodic report sent on PUSCH triggered by DCI
/// received on the cell in which the CSI-ReportConfig is included.
/// \remark See TS 38.331, \c CSI-ReportConfig.
struct csi_report_config {
  /// See TS 38.331, \c csi-ReportingBand.
  static constexpr unsigned MAX_NOF_CSI_REPORTING_SUBBANDS = 19;

  /// \brief PUCCH resource for the associated uplink BWP. Only PUCCH-Resource of format 2, 3 and 4 is supported.
  /// \remark See TS 38.331, \c pucch-Resource under \c CSI-ReportConfig.
  struct pucch_csi_resource {
    bwp_id_t ul_bwp;
    /// Defines the PUCCH resource ID used for this config report. Ref to \ref srsran::pucch_res_id_t for the
    /// documentation.
    pucch_res_id_t pucch_res_id = {0, 0};

    bool operator==(const pucch_csi_resource& rhs) const
    {
      return ul_bwp == rhs.ul_bwp && pucch_res_id == rhs.pucch_res_id;
    }
    bool operator!=(const pucch_csi_resource& rhs) const { return !(rhs == *this); }
  };

  /// See TS 38.331, \c reportConfigType under \c CSI-ReportConfig.
  struct periodic_or_semi_persistent_report_on_pucch {
    enum class report_type_t { periodic, semi_persistent };

    report_type_t          report_type;
    csi_report_periodicity report_slot_period;
    /// Values {0,...,(period_in_nof_slot - 1)}.
    unsigned                                        report_slot_offset;
    static_vector<pucch_csi_resource, MAX_NOF_BWPS> pucch_csi_res_list;

    bool operator==(const periodic_or_semi_persistent_report_on_pucch& rhs) const
    {
      return report_type == rhs.report_type && report_slot_period == rhs.report_slot_period &&
             report_slot_offset == rhs.report_slot_offset && pucch_csi_res_list == rhs.pucch_csi_res_list;
    }
    bool operator!=(const periodic_or_semi_persistent_report_on_pucch& rhs) const { return !(rhs == *this); }
  };

  struct semi_persistent_report_on_pusch {
    /// Values {slots5, slots10, slots20, slots40, slots80, slots160, slots320}.
    csi_report_periodicity slot_cfg;
    /// Timing offset Y for semi persistent reporting using PUSCH. This field lists the allowed offset values. This list
    /// must have the same number of entries as the pusch-TimeDomainAllocationList in PUSCH-Config. A particular value
    /// is indicated in DCI.Values {0,...,32}.
    static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS> report_slot_offset_list;
    /// Index of the p0-alpha set determining the power control for this CSI report transmission.
    p0_pusch_alphaset_id p0_alpha;

    bool operator==(const semi_persistent_report_on_pusch& rhs) const
    {
      return slot_cfg == rhs.slot_cfg && report_slot_offset_list == rhs.report_slot_offset_list &&
             p0_alpha == rhs.p0_alpha;
    }
    bool operator!=(const semi_persistent_report_on_pusch& rhs) const { return !(rhs == *this); }
  };
  struct aperiodic_report {
    /// Timing offset Y for aperiodic reporting using PUSCH. This field lists the allowed offset values. This list
    /// must have the same number of entries as the pusch-TimeDomainAllocationList in PUSCH-Config. A particular value
    /// is indicated in DCI.Values {0,...,32}.
    static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS> report_slot_offset_list;

    bool operator==(const aperiodic_report& rhs) const
    {
      return report_slot_offset_list == rhs.report_slot_offset_list;
    }
    bool operator!=(const aperiodic_report& rhs) const { return !(rhs == *this); }
  };

  /// \brief The CSI related quantities to report.
  /// \remark See TS 38.214, clause 5.2.1 and TS 38.331, \c reportQuantity.
  enum class report_quantity_type_t {
    none,
    cri_ri_pmi_cqi,
    cri_ri_i1,
    cri_ri_i1_cqi,
    cri_ri_cqi,
    cri_rsrp,
    ssb_index_rsrp,
    cri_ri_li_pmi_cqi
  };

  /// PRB bundling size to assume for CQI calculation when reportQuantity is CRI/RI/i1/CQI. Value \c none means UE
  /// assumes that no PRB bundling is applied.
  /// \remark See TS 38.331, \c pdsch-BundleSizeForCSI.
  enum class csi_ri_i1_cqi_pdsch_bundle_size_for_csi { n2, n4, none };

  /// \brief Indicates whether the UE shall report a single (wideband) or multiple (subband) CQI.
  /// \remark See TS 38.331, \c cqi-FormatIndicator.
  enum class cqi_format_indicator { wideband_cqi, subband_cqi, none };
  /// \brief Indicates whether the UE shall report a single (wideband) or multiple (subband) PMI.
  /// \remark See TS 38.331, \c pmi-FormatIndicator.
  enum class pmi_format_indicator { wideband_pmi, subband_pmi, none };

  /// \brief Reporting configuration in the frequency domain.
  /// \remark See TS 38.331, \c reportFreqConfiguration.
  struct report_frequency_config {
    cqi_format_indicator cqi_format_ind{cqi_format_indicator::none};
    pmi_format_indicator pmi_format_ind{pmi_format_indicator::none};
    /// Indicates a contiguous or non-contiguous subset of subbands in the bandwidth part which CSI shall be reported
    /// for. This field is absent if there are less than 24 PRBs. Values {3,...,19}. See TS 38.331, \c
    /// csi-ReportingBand.
    optional<unsigned> nof_csi_reporting_subbands;
    /// Used in conjunction with \c nof_csi_reporting_subbands, where each bit in the bit-string represents one subband.
    bounded_bitset<MAX_NOF_CSI_REPORTING_SUBBANDS> csi_reporting_subbands_bitmap;

    bool operator==(const report_frequency_config& rhs) const
    {
      return cqi_format_ind == rhs.cqi_format_ind && pmi_format_ind == rhs.pmi_format_ind &&
             nof_csi_reporting_subbands == rhs.nof_csi_reporting_subbands &&
             csi_reporting_subbands_bitmap == rhs.csi_reporting_subbands_bitmap;
    }
    bool operator!=(const report_frequency_config& rhs) const { return !(rhs == *this); }
  };

  /// Indicates one out of two possible BWP-dependent values for the subband size as indicated in TS 38.214,
  /// table 5.2.1.4-2.
  /// \remark See TS 38.331, \c subbandSize in \c CSI-ReportConfig.
  enum class subband_size_t { value1, value2 };

  /// \brief See TS 38.331, \c PortIndexFor8Ranks in \c CSI-ReportConfig.
  struct port_index_for_8_ranks {
    /// Port-Index configuration for up to rank 8. If present, the network configures port indexes for at least one of
    /// the ranks.
    /// \remark See TS 38.331, \c portIndex8.
    enum class port_index_type_t { port_index_1 = 0, port_index_2 = 2, port_index_4 = 4, port_index_8 = 8 };

    port_index_type_t port_index_type;
    /// Used only for \c port_index_type_t > \c port_index_1.
    optional<unsigned> rank1_x;
    /// Max. size of vector is 2.
    std::vector<unsigned> rank2_x;
    /// \c {rank3_x, rank4_x} are used only for \c port_index_type_t > \c port_index_2.
    /// Max. size of vector is 3.
    std::vector<unsigned> rank3_x;
    /// Max. size of vector is 4.
    std::vector<unsigned> rank4_x;
    /// \c {rank5_x, rank6_x, rank7_x, rank8_x} are used only for \c port_index_type_t > \c port_index_4.
    /// Max. size of vector is 5.
    std::vector<unsigned> rank5_x;
    /// Max. size of vector is 6.
    std::vector<unsigned> rank6_x;
    /// Max. size of vector is 7.
    std::vector<unsigned> rank7_x;
    /// Max. size of vector is 8.
    std::vector<unsigned> rank8_x;

    bool operator==(const port_index_for_8_ranks& rhs) const
    {
      return port_index_type == rhs.port_index_type && rank1_x == rhs.rank1_x && rank2_x == rhs.rank2_x &&
             rank3_x == rhs.rank3_x && rank4_x == rhs.rank4_x && rank5_x == rhs.rank5_x && rank6_x == rhs.rank6_x &&
             rank7_x == rhs.rank7_x && rank8_x == rhs.rank8_x;
    }
    bool operator!=(const port_index_for_8_ranks& rhs) const { return !(rhs == *this); }
  };

  csi_report_config_id_t report_cfg_id;
  /// Indicates in which serving cell the CSI-ResourceConfig indicated below are to be found. If the field is absent,
  /// the resources are on the same serving cell as this report configuration.
  optional<du_cell_index_t> carrier;
  /// Resources for channel measurement.
  csi_res_config_id_t res_for_channel_meas;
  /// CSI IM resources for interference measurement.
  optional<csi_res_config_id_t> csi_im_res_for_interference;
  /// NZP CSI RS resources for interference measurement.
  optional<csi_res_config_id_t> nzp_csi_rs_res_for_interference;
  /// Time domain behavior of reporting configuration.
  variant<periodic_or_semi_persistent_report_on_pucch, semi_persistent_report_on_pusch, aperiodic_report>
                         report_cfg_type;
  report_quantity_type_t report_qty_type;
  /// Relevant only when \c report_quantity_type_t is of type \c cri_ri_i1_cqi.
  csi_ri_i1_cqi_pdsch_bundle_size_for_csi pdsch_bundle_size_for_csi{csi_ri_i1_cqi_pdsch_bundle_size_for_csi::none};
  optional<report_frequency_config>       report_freq_cfg;
  /// Time domain measurement restriction for the channel (signal) measurements. See TS 38.214, clause 5.2.1.1.
  bool is_time_restrictions_for_channel_meas_configured{false};
  /// Time domain measurement restriction for interference measurements. See TS 38.214, clause 5.2.1.1.
  bool is_time_restrictions_for_interference_meas_configured{false};
  /// Codebook configuration for Type-1 or Type-2 including codebook subset restriction.
  optional<codebook_config> codebook_cfg;
  /// Turning on/off group beam based reporting.
  bool is_group_based_beam_reporting_enabled;
  /// Value is relevant only when \c is_group_based_beam_reporting_enabled is false. Values {1, 2, 3, 4}.
  optional<unsigned>    nof_reported_rs;
  optional<cqi_table_t> cqi_table;
  /// If csi-ReportingBand is absent, the UE shall ignore this field.
  subband_size_t subband_size;
  /// Port indication for RI/CQI calculation. For each CSI-RS resource in the linked ResourceConfig for channel
  /// measurement, a port indication for each rank R, indicating which R ports to use. Applicable only for non-PMI
  /// feedback. See TS 38.214, clause 5.2.1.4.2.
  static_vector<port_index_for_8_ranks, MAX_NOF_NZP_CSI_RS_RESOURCES_PER_CONFIG> non_pmi_port_indication;

  bool operator==(const csi_report_config& rhs) const
  {
    return report_cfg_id == rhs.report_cfg_id && carrier == rhs.carrier &&
           res_for_channel_meas == rhs.res_for_channel_meas &&
           csi_im_res_for_interference == rhs.csi_im_res_for_interference &&
           nzp_csi_rs_res_for_interference == rhs.nzp_csi_rs_res_for_interference &&
           report_cfg_type == rhs.report_cfg_type && report_qty_type == rhs.report_qty_type &&
           pdsch_bundle_size_for_csi == rhs.pdsch_bundle_size_for_csi && report_freq_cfg == rhs.report_freq_cfg &&
           is_time_restrictions_for_channel_meas_configured == rhs.is_time_restrictions_for_channel_meas_configured &&
           is_time_restrictions_for_interference_meas_configured ==
               rhs.is_time_restrictions_for_interference_meas_configured &&
           codebook_cfg == rhs.codebook_cfg &&
           is_group_based_beam_reporting_enabled == rhs.is_group_based_beam_reporting_enabled &&
           nof_reported_rs == rhs.nof_reported_rs && cqi_table == rhs.cqi_table && subband_size == rhs.subband_size &&
           non_pmi_port_indication == rhs.non_pmi_port_indication;
  }
  bool operator!=(const csi_report_config& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
