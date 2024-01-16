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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/dmrs.h"
#include "srsran/ran/pdcch/pdcch_constants.h"
#include "srsran/ran/physical_cell_group.h"
#include "srsran/support/units.h"

namespace srsran {

/// DCI payload data type.
using dci_payload = bounded_bitset<pdcch_constants::MAX_DCI_PAYLOAD_SIZE>;

/// \brief Resource allocation type for DCI formats 0_1 and 1_1.
/// \remark See TS38.214 Sections 5.1.2.2 and 6.1.2.2.
enum class resource_allocation { resource_allocation_type_0, resource_allocation_type_1, dynamic_switch };

/// Dynamic resource allocation indicator for DCI formats 0_1 and 1_1.
enum class dynamic_resource_allocation { type_0, type_1 };

/// \brief Subset of PMIs addressed by TPMI, where PMIs are those supported by UEs with maximum coherence capabilities.
/// \remark See TS38.214, Section 6.1.1.1.
enum class tx_scheme_codebook_subset : unsigned {
  fully_and_partial_and_non_coherent,
  partial_and_non_coherent,
  non_coherent
};

/// \brief DCI configuration parameters required to perform the DCI size alignment procedure.
///
/// The size alignment procedure, specified in TS38.212 Section 7.3.1.0, reduces the number of different DCI sizes, to
/// simplify the detection of DCI messages by the UE.
///
/// \remark CORESET 0 is assumed to be configured for the cell if \ref coreset0_bw is set to any value other than 0.
struct dci_size_config {
  /// \name DCI formats and Search Spaces the UE is expected to monitor.
  ///
  /// Configuring the UE to monitor fallback DCI formats on Common and UE-specific Search Spaces as well as
  /// DCI formats 0_1 and 1_1 on UE-specific Search Spaces might result in a size alignment that does not fulfill the
  /// conditions outlined in step 3 of the DCI size alignment procedure, namely:
  ///  - The total number of different DCI sizes configured to monitor is no more than 4 for the cell.
  ///  - The total number of different DCI sizes with C-RNTI (formats 0_0, 0_1, 1_0 and 1_1) configured to monitor is no
  ///    more than 3 for the cell.
  ///
  /// This will trigger step 4 of the size alignment procedure, i.e., the size of the fallback DCI formats monitored in
  /// a UE-specific Search Space will be set to the size of the fallback DCI formats monitored in a Common Search Space,
  /// by altering the size of the frequency domain resource assignment field.
  /// @{

  /// \brief Flag for fallback DCI formats on a UE-specific Search Space.
  ///
  /// Set to \c true if the UE is expected to monitor fallback DCI formats on a UE-specific Search Space. If set to \c
  /// false, the payload size for this DCI and SS configuration will not be computed, preventing DCI packing.
  bool dci_0_0_and_1_0_ue_ss;
  /// \brief Flag for non-fallback DCI formats on a UE-specific Search Space.
  ///
  /// Set to \c true if the UE is expected to monitor non-fallback DCI formats on a UE-specific Search Space. If set to
  /// \c false, the payload size for this DCI and SS configuration will not be computed, preventing DCI packing.
  bool dci_0_1_and_1_1_ue_ss;
  /// @}

  /// \name Bandwidth parameters.
  /// @{

  /// Bandwidth of the initial DL BWP in number of Resource Blocks.
  unsigned dl_bwp_initial_bw;
  /// Bandwidth of the active DL BWP in number of Resource Blocks.
  unsigned dl_bwp_active_bw;
  /// Bandwidth of the initial UL BWP in number of Resource Blocks.
  unsigned ul_bwp_initial_bw;
  /// Bandwidth of the active UL BWP in number of Resource Blocks.
  unsigned ul_bwp_active_bw;
  /// CORESET 0 DL bandwidth. Set to 0 if not configured.
  unsigned coreset0_bw;
  /// @}

  /// \brief Supplementary uplink flag.
  ///
  /// Set to \c true if supplementary uplink is configured, i.e., if the UE is configured with the higher layer
  /// parameter \e supplementaryUplink (see TS38.331 Section 6.3.2, Information Element \e ServingCellConfig) and more
  /// than a single carrier in the cell is configured for PUSCH transmission.
  bool sul_configured;

  /// \name Common parameters for DCI formats 0_1 and 1_1.
  /// @{

  /// \brief Cross carrier scheduling flag.
  ///
  /// Set to \c true if cross carrier scheduling is configured, i.e., if the UE is configured with the higher layer
  /// parameter \e CrossCarrierSchedulingConfig (see TS38.331 Section 6.3.2, Information Element \e ServingCellConfig),
  /// as per TS38.212 Section 7.3.1.1.2.
  bool cross_carrier_configured;
  /// \brief PDSCH HARQ-ACK codebook type.
  ///
  /// Set according to the higher layer parameter \e pdsch-HARQ-ACK-Codebook (see TS38.331 Section 6.3.2, Information
  /// Element \e PhysicalCellGroupConfig).
  pdsch_harq_ack_codebook pdsch_harq_ack_cb;
  /// @}

  /// \name Parameters for DCI format 0_1.
  /// @{

  /// \brief parameter \f$n_{BWP,RRC}\f$ for DCI format 0_1, in TS38.212 Section 7.3.1.1.2.
  ///
  /// Set to the number of UL BWPs configured by higher layers, excluding the initial UL BWP. Values: {0, 1, ..., 4}.
  unsigned nof_ul_bwp_rrc;
  /// \brief parameter \f$I\f$ in TS38.212 Section 7.3.1.1.2.
  ///
  /// As defined in TS38.214 Table 6.1.2.1.1-1, set to either:
  ///   - The number of entries in the higher layer parameter \e pusch-TimeDomainAllocationList via the RRC Information
  ///     Element \e PUSCH-Config (see TS38.331 Section 6.3.2), if configured.
  ///   - The number of entries in the higher layer parameter \e pusch-TimeDomainAllocationList via the RRC Information
  ///     Element \e PUSCH-ConfigCommon (see TS38.331 Section 6.3.2), if it is configured in \e PUSCH-ConfigCommon and
  ///     not configured in \e PUSCH-Config.
  ///   - 16 otherwise, which is the number of entries in the default table (see TS38.214 Table 6.1.2.1.1-2).
  unsigned nof_ul_time_domain_res;
  /// \brief Size of the CSI request DCI field. See TS38.214 Section 5.2.1.5.1.
  ///
  /// Set according to the higher layer parameter \e reportTriggerSize (see TS38.331 Section 6.3.2, Information Element
  /// \e CSI-Measconfig). Values: {0, 1, ..., 6}.
  unsigned report_trigger_size;
  /// \brief PUSCH frequency hopping flag.
  ///
  /// Set to \c true if PUSCH frequency hopping is configured, i.e., if the higher layer parameter \e frequencyHopping
  /// (see TS38.331 Section 6.3.2, Information Element \e PUSCH-Config) is configured.
  bool frequency_hopping_configured;
  /// \brief Non-codebook based transmission flag.
  ///
  /// Set to \c true if non-codebook based transmission is configured, i.e., if the higher layer parameter \e txConfig
  /// (see TS38.331 Section 6.3.2, Information Element \e PUSCH-Config) is set for non-codebook transmission.
  bool tx_config_non_codebook;
  /// \brief UL transform precoding flag.
  ///
  /// Set to \c true if UL transform precoding is enabled.
  bool transform_precoding_enabled;
  /// \brief PT-RS signals flag.
  ///
  /// Set to \c true if PT-RS signals are configured. See TS38.331 Section 6.3.2, Information Element \e
  /// PTRS-UplinkConfig.
  bool ptrs_uplink_configured;
  /// \brief PUSCH dynamic beta offsets flag.
  ///
  /// Set to \c true if dynamic beta offsets are configured for UCI on PUSCH, i.e., if the higher layer parameter \e
  /// betaOffsets (see TS38.331 Section 6.3.2, Information Element \e PUSCH-Config) is set to \e dynamic.
  bool dynamic_beta_offsets;
  /// \brief Dual dynamic HARQ-ACK codebook flag.
  ///
  /// Required if the PDSCH HARQ-ACK codebook type is set to \c dynamic (see \ref pdsch_harq_ack_cb). Set to \c true if
  /// the dynamic HARQ-ACK codebook has two sub-codebooks, \c false otherwise.
  optional<bool> dynamic_dual_harq_ack_cb;
  /// \brief PUSCH resource allocation type for DCI format 0_1, as per TS38.214 Section 6.1.2.2.
  ///
  /// Set according to the higher layer parameter \e resourceAllocation (see TS38.331 Section 6.3.2, Information Element
  /// \e PUSCH-Config).
  resource_allocation pusch_res_allocation_type;
  /// \brief parameter \f$N_{RBG}\f$, in TS38.214 Section 6.1.2.2.1.
  ///
  /// Used to determine the DCI format 0_1 frequency domain resource assignment field size if the PUSCH resource
  /// allocation type, given by the higher layer parameter \e resourceAllocation (see TS38.331 Section 6.3.2,
  /// Information Element \e PUSCH-Config) is either \e resourceAllocationType0  or \e dynamicSwitch. Otherwise, leave
  /// it unset. Values: {1, ..., 18}
  optional<unsigned> nof_ul_rb_groups;
  /// \brief Number of antenna ports used for PUSCH codebook-based transmission.
  ///
  /// Set according to the higher layer parameter \e nrofSRS-Ports (see TS38.331 Section 6.3.2, Information Element \e
  /// SRS-Config), as per TS38.214 Section 6.1.1.1, if codebook-based PUSCH transmission is configured, i.e., if \ref
  /// tx_config_non_codebook is set to \c false. Otherwise, leave it unset. Values: {1, 2, 4}.
  optional<unsigned> nof_srs_ports;
  /// \brief Parameter \f$N_{SRS}\f$ in TS38.212 Section 7.3.1.1.2, indicating the number of SRS resources.
  ///
  /// It is required for DCI format 0_1, i.e., if \ref dci_0_1_and_1_1_ue_ss is set to \c true. Possible values:
  ///   - {1, 2, 3, 4} for non-codebook transmission, i.e., if \ref tx_config_non_codebook is set to \c true.
  ///   - {1, 2} otherwise.
  unsigned nof_srs_resources;
  /// \brief Parameter \f$L_{max}\f$ in TS38.212 Section 7.3.1.1.2.
  ///
  /// Indicates the maximum number of PUSCH layers supported by the UE for the serving cell for non-codebook based
  /// operation. Set to the value of the higher layer parameter \e maxMIMO-Layers (see TS38.331 Section 6.3.2,
  /// Information Element \e PUSCH-ServingCellConfig) if it is configured in the serving cell and supported by the UE.
  /// Otherwise, set to the maximum number of layers supported by the UE for the serving cell and for non-codebook
  /// based operation. Values: {1, 2, 3, 4}.
  ///
  /// \remark Required if \ref tx_config_non_codebook is set to \c true, indicating non-codebook transmission.
  optional<unsigned> pusch_max_layers;
  /// \brief Maximum UE transmission rank for codebook based operation.
  ///
  /// Specifies the maximum UE transmission rank, determined by the higher layer parameter \e maxRank (see TS38.331
  /// Section 6.3.2, Information Element \e PUSCH-Config). Values: {1, ..., 4} and it must be smaller or equal than \ref
  /// nof_srs_ports.
  ///
  /// \remark Required for codebook based transmission, i.e., if \ref tx_config_non_codebook is set to \c false.
  optional<unsigned> max_rank;
  /// \brief Subset of PMIs addressed by TPMI.
  ///
  /// Restricts the available PMIs to those supported by the UE, according to its coherence capabilities. Its value is
  /// indicated by the higher layer parameter \e codebookSubset (see TS38.331 Section 6.3.2, Information Element \e
  /// PUSCH-Config). It is required for codebook based transmission with more than one antenna port, i.e., if \ref
  /// tx_config_non_codebook is set to \c false and \ref nof_srs_ports is greater than one. Otherwise, leave it unset.
  optional<tx_scheme_codebook_subset> cb_subset;
  /// \brief UL DM-RS type for DM-RS mapping type A.
  ///
  /// Set according to the higher layer parameter \e dmrs-Type (see TS38.331 Section 6.3.2, Information Element \e
  /// DMRS-UplinkConfig) via the higher layer parameter dmrs-UplinkForPUSCH-MappingTypeA (see TS38.331 Section 6.3.2,
  /// Information Element \e PUSCH-Config) if present. Otherwise, leave it unset.
  optional<dmrs_config_type> pusch_dmrs_A_type;
  /// \brief Maximum number of OFDM symbols occupied by the front-loaded UL DM-RS for DM-RS mapping type A.
  ///
  /// Set according to the higher layer parameter \e maxLength (see TS38.331 Section 6.3.2, Information Element \e
  /// DMRS-UplinkConfig) via the higher layer parameter dmrs-UplinkForPUSCH-MappingTypeA (see TS38.331 Section 6.3.2,
  /// Information Element \e PUSCH-Config) if present. Otherwise, leave it unset.
  optional<dmrs_max_length> pusch_dmrs_A_max_len;
  /// \brief UL DM-RS type for DM-RS mapping type B.
  ///
  /// Set according to the higher layer parameter \e dmrs-Type (see TS38.331 Section 6.3.2, Information Element \e
  /// DMRS-UplinkConfig) via the higher layer parameter dmrs-UplinkForPUSCH-MappingTypeB (see TS38.331 Section 6.3.2,
  /// Information Element \e PUSCH-Config) if present. Otherwise, leave it unset.
  optional<dmrs_config_type> pusch_dmrs_B_type;
  /// \brief Maximum number of OFDM symbols occupied by the front-loaded UL DM-RS for DM-RS mapping type B.
  ///
  /// Set according to the higher layer parameter \e maxLength (see TS38.331 Section 6.3.2, Information Element \e
  /// DMRS-UplinkConfig) via the higher layer parameter dmrs-UplinkForPUSCH-MappingTypeB (see TS38.331 Section 6.3.2,
  /// Information Element \e PUSCH-Config) if present. Otherwise, leave it unset.
  optional<dmrs_max_length> pusch_dmrs_B_max_len;
  /// \brief Maximum number of PUSCH Code Block Groups (CBG) per Transport Block (TB).
  ///
  /// Set according to the higher layer parameter \e maxCodeBlockGroupsPerTransportBlock if the higher layer parameter
  /// \e codeBlockGroupTransmission is present (see TS38.331 Section 6.3.2, Information Element \e
  /// PUSCH-ServingCellConfig). Otherwise, leave it unset. Values: {2, 4, 6, 8}.
  optional<unsigned> max_cbg_tb_pusch;
  /// @}

  /// \name Parameters for DCI format 1_1.
  /// @{

  /// \brief parameter \f$n_{BWP,RRC}\f$ for DCI format 1_1, in TS38.212 Section 7.3.1.2.2.
  ///
  /// Set to the number of DL BWPs configured by higher layers, excluding the initial DL BWP. Values: {0, 1, ..., 4}.
  unsigned nof_dl_bwp_rrc;
  /// \brief parameter \f$I\f$ in TS38.212 Section 7.3.1.2.2.
  ///
  /// As defined in TS38.214 Table 5.1.2.1.1-1, set to either:
  ///   - The number of entries in the higher layer parameter \e pdsch-TimeDomainAllocationList via the RRC Information
  ///     Element \e PDSCH-Config (see TS38.331 Section 6.3.2), if configured.
  ///   - The number of entries in the higher layer parameter \e pdsch-TimeDomainAllocationList via the RRC Information
  ///     Element \e PDSCH-ConfigCommon (see TS38.331 Section 6.3.2), if it is configured in \e PDSCH-ConfigCommon and
  ///     not configured in \e PDSCH-Config.
  ///   - 16 otherwise, which is the number of entries in the default table (see TS38.214 Table 5.1.2.1.1-2).
  unsigned nof_dl_time_domain_res;
  /// \brief Number of configured aperiodic ZP CSI-RS resource sets.
  ///
  /// Parameter \f$n_{ZP}\f$ in TS38.212 Section 7.3.1.2.2, used to determine the size of the ZP CSI-RS trigger field,
  /// as per TS38.214 Section 5.1.4.2. Set according to the number of entries of the higher layer parameter \e
  /// aperiodic-ZP-CSI-RS-ResourceSetsToAddModList, in TS38.331 Section 6.3.2, Information Element \e PDSCH-Config.
  /// Values: {0, 1, 2, 3}.
  unsigned nof_aperiodic_zp_csi;
  /// \brief Number of PDSCH to the DL ACK timings.
  ///
  /// Parameter \f$I\f$ in TS38.212 Section 7.3.1.2.2, representing the number of entries of the the higher layer
  /// parameter \e dl-DataToUL-ACK (see TS38.331 Section 6.3.2, Information Element \e PUCCH-Config). Values: {1, ...,
  /// 8}.
  unsigned nof_pdsch_ack_timings;
  /// \brief Dynamic PRB bundling flag.
  ///
  /// Set to \c true if dynamic PRB bundling has been enabled, i.e., if the higher layer parameter \e prb-BundlingType
  /// (see TS38.331 Section 6.3.2, Information Element \e PDSCH-Config) is set to \e dynamicBundling.
  bool dynamic_prb_bundling;
  /// \brief Rate matching pattern group 1 flag.
  ///
  /// Set to \c true if the higher layer parameter \e RateMatchPatternGroup1 (see TS38.331 Section 6.3.2, Information
  /// Element \e PDSCH-Config) is configured.
  bool rm_pattern_group1;
  /// \brief Rate matching pattern group 2 flag.
  ///
  /// Set to \c true if the higher layer parameter \e RateMatchPatternGroup2 (see TS38.331 Section 6.3.2, Information
  /// Element \e PDSCH-Config) is configured.
  bool rm_pattern_group2;
  /// \brief Two codewords in PDSCH flag.
  ///
  /// Set to \c true if PDSCH has been configured to transmit two codewords. A PDSCH transmission accommodates two
  /// codewords when the number of layers is greater than 4. DCI must schedule two codewords when indicated by the
  /// higher layer parameter \e maxNrofCodeWordsScheduledByDCI (see TS38.331 Section 6.3.2, Information Element \e
  /// PDSCH-Config).
  bool pdsch_two_codewords;
  /// \brief Multiple serving cells flag.
  ///
  /// Set to \c true if the UE has been configured with multiple serving cells.
  bool multiple_scells;
  /// \brief Transmission Configuration Indication (TCI) flag.
  ///
  /// Set to \c true if Transmission Configuration Indication (TCI) is present in DCI format 1_1, i.e., if the higher
  /// layer parameter \e tci-PresentInDCI (see TS38.331 Section 6.3.2, Information Element \e ControlResourceSet) is
  /// enabled.
  bool pdsch_tci;
  /// \brief CBG flushing out information flag.
  ///
  /// Set to \c true if DCI format 1_1 will include the CBG flushing out information field, i.e., if the higher layer
  /// parameter \e codeBlockGroupFlushIndicator (see TS38.331 Section 6.3.2, Information Element \e
  /// PDSCH-ServingCellConfig) is enabled.
  bool cbg_flush_indicator;
  /// PDSCH resource allocation type for DCI format 1_1, as per TS38.214 Section 5.1.2.2.
  resource_allocation pdsch_res_allocation_type;
  /// \brief parameter \f$N_{RBG}\f$, in TS38.214 Section 5.1.2.2.1.
  ///
  /// Used to determine the DCI format 1_1 frequency domain resource assignment field size if the PDSCH resource
  /// allocation type, given by the higher layer parameter \e resourceAllocation (see TS38.331 Section 6.3.2,
  /// Information Element \e PDSCH-Config) is either \e resourceAllocationType0  or \e dynamicSwitch. Values: {1, ...,
  /// 18}.
  ///
  /// \remark Required if \c pdsch_res_allocation_type is either \e resourceAllocationType0 or \e dynamicSwitch.
  optional<unsigned> nof_dl_rb_groups;
  /// \brief Interleaved VRB-to-PRB mapping flag.
  ///
  /// Set to \c true if interleaved VRB-to-PRB mapping is configured, \c false otherwise.
  ///
  /// \remark Required if \c pdsch_res_allocation_type is either \e resourceAllocationType1 or \e dynamicSwitch.
  optional<bool> interleaved_vrb_prb_mapping;
  /// \brief DM-RS type for PDSCH DM-RS mapping type A.
  ///
  /// Set according to the higher layer parameter \e dmrs-Type (see TS38.331 Section 6.3.2, Information Element \e
  /// DMRS-DownlinkConfig) via the higher layer parameter dmrs-DownlinkForPDSCH-MappingTypeA (see TS38.331 Section
  /// 6.3.2, Information Element \e PDSCH-Config) if present. Otherwise, leave it unset.
  optional<dmrs_config_type> pdsch_dmrs_A_type;
  /// \brief Maximum number of OFDM symbols occupied by the front-loaded DL DM-RS for DM-RS mapping type A.
  ///
  /// Set according to the higher layer parameter \e maxLength (see TS38.331 Section 6.3.2, Information Element \e
  /// DMRS-DownlinkConfig) via the higher layer parameter dmrs-DownlinkForPDSCH-MappingTypeA (see TS38.331 Section
  /// 6.3.2, Information Element \e PDSCH-Config) if present. Otherwise, leave it unset.
  optional<dmrs_max_length> pdsch_dmrs_A_max_len;
  /// \brief DM-RS type for PDSCH DM-RS mapping type B.
  ///
  /// Set according to the higher layer parameter \e dmrs-Type (see TS38.331 Section 6.3.2, Information Element \e
  /// DMRS-DownlinkConfig) via the higher layer parameter dmrs-DownlinkForPDSCH-MappingTypeB (see TS38.331 Section
  /// 6.3.2, Information Element \e PDSCH-Config) if present. Otherwise, leave it unset.
  optional<dmrs_config_type> pdsch_dmrs_B_type;
  /// \brief Maximum number of OFDM symbols occupied by the front-loaded DL DM-RS for DM-RS mapping type B.
  ///
  /// Set according to the higher layer parameter \e maxLength (see TS38.331 Section 6.3.2, Information Element \e
  /// DMRS-DownlinkConfig) via the higher layer parameter dmrs-DownlinkForPDSCH-MappingTypeB (see TS38.331 Section
  /// 6.3.2, Information Element \e PDSCH-Config) if present. Otherwise, leave it unset.
  optional<dmrs_max_length> pdsch_dmrs_B_max_len;
  /// \brief Maximum number of PDSCH Code Block Groups (CBG) per Transport Block (TB).
  ///
  /// Set according to the higher layer parameter \e maxCodeBlockGroupsPerTransportBlock (see TS38.331 Section 6.3.2,
  /// Information Element \e PDSCH-ServingCellConfig) if the higher layer parameter \e codeBlockGroupTransmission (see
  /// TS38.331 Section 6.3.2, Information Element \e PDSCH-ServingCellConfig) is present. Otherwise, leave it unset.
  /// Values: {2, 4, 6, 8}.
  optional<unsigned> max_cbg_tb_pdsch;
  ///@}
};

/// \brief DCI format 0_0 payload size parameters.
///
/// Contains the DCI format 0_0 field sizes required to generate a DCI format 0_0 payload, as per TS38.212
/// Section 7.3.1.1.1. See \ref get_dci_sizes for more information.
struct dci_0_0_size {
  /// Total DCI payload size in number of bits.
  units::bits total;
  /// Size of the frequency domain resource assignment field - number of bits as per TS38.212 Section 7.3.1.1.1
  units::bits frequency_resource;
  /// Number of padding bits, including the optional UL/SUL indicator field.
  units::bits padding_incl_ul_sul;
};

/// \brief DCI format 1_0 payload size parameters.
///
/// Contains the DCI format 1_0 field sizes required to generate a DCI format 1_0 payload, as per TS38.212
/// Section 7.3.1.2.1. See \ref get_dci_sizes for more information.
struct dci_1_0_size {
  /// Total DCI payload size in number of bits.
  units::bits total;
  /// Size of the frequency domain resource assignment field - number of bits as per TS38.212 Section 7.3.1.2.1.
  units::bits frequency_resource;
  /// Number of padding bits.
  units::bits padding;
};

/// \brief DCI format 0_1 payload size parameters.
///
/// Contains the DCI format 0_1 field sizes required to generate a DCI format 0_1 payload, as per TS38.212 Section
/// 7.3.1.1.2. See \ref get_dci_sizes for more information.
struct dci_0_1_size {
  /// Total DCI payload size in number of bits.
  units::bits total;
  /// Carrier indicator field size - 0 or 3 bits.
  units::bits carrier_indicator;
  /// UL/SUL indicator field size - 0 or 1 bit.
  units::bits ul_sul_indicator;
  /// BWP indicator field size - 0, 1 or 2 bits.
  units::bits bwp_indicator;
  /// Frequency domain resource assignment field size.
  units::bits frequency_resource;
  /// Time domain resource assignment field size - 0, 1, 2, 3 or 4 bits.
  units::bits time_resource;
  /// Frequency hopping flag field size - 0 or 1 bit.
  units::bits freq_hopping_flag;
  /// 1st downlink assignment index field size - 1 or 2 bits.
  units::bits first_dl_assignment_idx;
  /// 2nd downlink assignment index field size - 2 bits if present.
  units::bits second_dl_assignment_idx;
  /// SRS resource indicator field size.
  units::bits srs_resource_indicator;
  /// Precoding information and number of layers field size.
  units::bits precoding_info_nof_layers;
  /// Antenna ports field size - 2, 3, 4 or 5 bits.
  units::bits antenna_ports;
  /// SRS request field size - 2 or 3 bits.
  units::bits srs_request;
  /// CSI request field size - 0 to 6 bits.
  units::bits csi_request;
  /// CBG Transmission Information (CBGTI) field size - 0, 2, 4, 6 or 8 bits.
  units::bits cbg_transmission_info;
  /// PT-RS/DM-RS association field size - 0 or 2 bits.
  units::bits ptrs_dmrs_association;
  /// Beta offset indicator field size - 0 or 2 bits.
  units::bits beta_offset_indicator;
  /// DM-RS sequence initialization field size - 0 or 1 bit.
  units::bits dmrs_seq_initialization;
  /// Number of padding bits.
  units::bits padding;
};

/// \brief DCI format 1_1 payload size parameters.
///
/// Contains the DCI format 1_1 field sizes required to generate a DCI format 1_1 payload, as per TS38.212 Section
/// 7.3.1.2.2. See \ref get_dci_sizes for more information.
struct dci_1_1_size {
  /// Total DCI payload size in number of bits.
  units::bits total;
  /// Carrier indicator field size - 0 or 3 bits.
  units::bits carrier_indicator;
  /// BWP indicator field size - 0, 1 or 2 bits.
  units::bits bwp_indicator;
  /// Frequency domain resource assignment field size.
  units::bits frequency_resource;
  /// Time domain resource assignment field size - 0, 1, 2, 3 or 4 bits.
  units::bits time_resource;
  /// VRP-to-PRB mapping field size - 0 or 1 bit.
  units::bits vrb_prb_mapping;
  /// PRB bundling size indicator field size - 0 or 1 bit.
  units::bits prb_bundling_size_indicator;
  /// Rate matching indicator field size - 0, 1 or 2 bits.
  units::bits rate_matching_indicator;
  /// ZP CSI-RS trigger field size - 0, 1 or 2 bits.
  units::bits zp_csi_rs_trigger;
  /// Modulation and coding scheme for TB 2 field size - 0 or 5 bits.
  units::bits tb2_modulation_coding_scheme;
  /// New data indicator for TB 2 field size - 0 or 1 bit.
  units::bits tb2_new_data_indicator;
  /// Redundancy version for TB 2 field size - 0 or 2 bits.
  units::bits tb2_redundancy_version;
  /// Downlink Assignment Index (DAI) field size - 0, 2 or 4 bits.
  units::bits downlink_assignment_index;
  /// PDSCH to HARQ feedback timing indicator field size - 0, 1, 2 or 3 bits.
  units::bits pdsch_harq_fb_timing_indicator;
  /// Antenna ports field size - 4, 5 or 6 bits.
  units::bits antenna_ports;
  /// Transmission configuration indication field size - 0 or 3 bits.
  units::bits tx_config_indication;
  /// SRS request field size - 2 or 3 bits.
  units::bits srs_request;
  /// CBG Transmission Information (CBGTI) field size - 0, 2, 4, 6 or 8 bits.
  units::bits cbg_transmission_info;
  /// CBG Flushing Out Information (CBGFI) field size - 0 or 1 bit.
  units::bits cbg_flushing_info;
  /// Number of padding bits.
  units::bits padding;
};

/// \brief DCI payload sizes.
///
/// Holds the DCI payload sizes, in number of bits, after performing the DCI size alignment procedure specified in
/// TS38.212 Section 7.3.1.0.
struct dci_sizes {
  /// Payload size parameters for a DCI format 0_0 message monitored in a common search space.
  dci_0_0_size format0_0_common_size;
  /// Payload size parameters for a DCI format 1_0 message monitored in a common search space.
  dci_1_0_size format1_0_common_size;
  /// \brief Payload size parameters for a DCI format 0_0 message monitored in a UE-specific search space.
  ///
  /// Computed during the size alignment procedure if \ref dci_size_config::dci_0_0_and_1_0_ue_ss is set to \c true.
  optional<dci_0_0_size> format0_0_ue_size;
  /// \brief Payload size parameters for a DCI format 1_0 message monitored in a UE-specific search space.
  ///
  /// Computed during the size alignment procedure if \ref dci_size_config::dci_0_0_and_1_0_ue_ss is set to \c true.
  optional<dci_1_0_size> format1_0_ue_size;
  /// \brief Payload size parameters for a DCI format 0_1 message monitored in a UE-specific search space.
  ///
  /// Computed during the size alignment procedure if \ref dci_size_config::dci_0_1_and_1_1_ue_ss is set to \c true.
  optional<dci_0_1_size> format0_1_ue_size;
  /// \brief Payload size parameters for a DCI format 1_1 message monitored in a UE-specific search space.
  ///
  /// Computed during the size alignment procedure if \ref dci_size_config::dci_0_1_and_1_1_ue_ss is set to \c true.
  optional<dci_1_1_size> format1_1_ue_size;
};

/// \brief DCI payload size alignment procedure.
///
/// Computes the DCI payload sizes, in number of bits, with the size alignment procedure specified in TS38.212
/// Section 7.3.1.0.
///
/// \param[in] config Required DCI size configuration parameters.
/// \return The DCI payload sizes in number of bits.
/// \remark The DCI size configuration parameters for DCI format 0_1 should be filled with the values corresponding to
/// the DCI payload scrambled by C-RNTI. DCI payloads used for PUSCH Configured Grant Scheduling, which are scrambled
/// by CS-RNTI, take the size of the DCI payload scrambled by C-RNTI, as per TS38.212 Section 7.3.1.1.2.
/// \remark Configuring a UE to monitor DCI format 1_1 messages with different sizes in multiple search spaces
/// associated with multiple CORESETs in a BWP is not currently supported. See TS38.212 Section 7.3.1.2.2.
dci_sizes get_dci_sizes(const dci_size_config& config);

/// \brief Required parameters for packing a DCI format 0_0 scrambled by C-RNTI, CS-RNTI or MCS-C-RNTI.
/// \remark Defined in TS38.212 Section 7.3.1.1.1.
struct dci_0_0_c_rnti_configuration {
  /// \brief DCI format 0_0 payload size parameters.
  ///
  /// The DCI payload size is determined by the DCI size alignment procedure specified in TS38.212 Section 7.3.1.0. See
  /// \ref get_dci_sizes for more information.
  dci_0_0_size payload_size;
  /// \brief Parameter \f$N_{\textup{UL\_hop}}\f$, as per TS38.212 Section 7.3.1.1.1.
  ///
  /// \f$N_{\textup{UL\_hop}}\f$ is the number of bits used in the DCI payload to pack the frequency hopping offset,
  /// represented by \ref hopping_offset.
  ///
  /// If frequency hopping is enabled via the \ref frequency_hopping_flag, it must be set to:
  ///   - 1, if the RRC parameter \e frequencyHoppingOffsetLists has 2 possible offsets.
  ///   - 2, if the RRC parameter \e frequencyHoppingOffsetLists has 4 possible offsets.
  unsigned N_ul_hop;
  /// \brief Frequency hopping offset, selected from the RRC parameter \e frequencyHoppingOffsetLists.
  ///
  /// \e frequencyHoppingOffsetLists, given by TS38.331 Section 6.3.2, \e PUSCH-Config Information Element, provides 2
  /// or 4 selectable frequency offsets, as per TS38.214 Section 6.3. The \c hopping_offset field acts as an index for
  /// \e frequencyHoppingOffsetLists, determining which of the possible offsets is used.
  ///
  /// If frequency hopping is enabled via the \ref frequency_hopping_flag, it must be set to one of the following
  /// values:
  ///   - (0, 1), if \e frequencyHoppingOffsetLists has 2 possible offsets.
  ///   - (0, 1, 2, 3), if \e frequencyHoppingOffsetLists has 4 possible offsets.
  unsigned hopping_offset;
  /// \brief Frequency domain resource assignment - \f$\Bigl \lceil \log_2(N_{RB}^{UL,BWP}(N_{RB}^{UL,BWP}+1)/2) \Bigr
  /// \rceil\f$ bits as per TS38.214 Section 6.1.2.2.2.
  unsigned frequency_resource;
  /// Time domain resource assignment - 4 bit as per TS38.214 Section 6.1.2.1.
  unsigned time_resource;
  /// Frequency hopping flag - 1 bit as per TS38.214 Section 6.3 and Table 7.3.1.1.1-3.
  unsigned frequency_hopping_flag;
  /// Modulation and coding scheme - 5 bits as per TS38.214 Section 6.1.4.1.
  unsigned modulation_coding_scheme;
  /// New data indicator - 1 bit.
  unsigned new_data_indicator;
  /// Redundancy version - 2 bits as per TS38.212 Table 7.3.1.1.1-2.
  unsigned redundancy_version;
  /// HARQ process number - 4 bits.
  unsigned harq_process_number;
  /// TPC command for scheduled PUSCH - 2 bits as per TS38.213 Section 7.1.1.
  unsigned tpc_command;
  /// UL/SUL indicator - 1 bit if present, as per TS38.212 Section 7.3.1.1.1 and Table 7.3.1.1.1-1.
  optional<bool> ul_sul_indicator;
};

/// Packs a DCI format 0_0 scrambled by C-RNTI, CS-RNTI or MCS-C-RNTI.
dci_payload dci_0_0_c_rnti_pack(const dci_0_0_c_rnti_configuration& config);

/// \brief Required parameters for packing a DCI format 0_0 scrambled by TC-RNTI.
/// \remark Defined in TS38.212 Section 7.3.1.1.1.
struct dci_0_0_tc_rnti_configuration {
  /// \brief DCI format 0_0 payload size parameters.
  ///
  /// The DCI payload size is determined by the DCI size alignment procedure specified in TS38.212 Section 7.3.1.0. See
  /// \ref get_dci_sizes for more information.
  dci_0_0_size payload_size;
  /// \brief Parameter \f$N_{\textup{UL\_hop}}\f$, as per TS38.212 Section 7.3.1.1.1.
  ///
  /// \f$N_{\textup{UL\_hop}}\f$ is the number of bits used in the DCI payload to pack the frequency hopping offset,
  /// represented by \ref hopping_offset.
  ///
  /// If frequency hopping is enabled via the \ref frequency_hopping_flag, it must be set to:
  ///   - 1, if \f$N_{RB}^{UL,BWP}\f$ is smaller than 50.
  ///   - 2 otherwise.
  unsigned N_ul_hop;
  /// \brief Frequency hopping offset, as per TS 38.213 Section 8.3 and Table 8.3-1.
  ///
  /// If frequency hopping is enabled via the \ref frequency_hopping_flag, it must be set to one of the following
  /// values:
  ///   - (0, 1), if \ref N_ul_hop is set to 1.
  ///   - (0, 1, 2, 3), if \ref N_ul_hop is set to 2.
  unsigned hopping_offset;
  /// \brief Frequency domain resource assignment - \f$\Bigl \lceil \log_2(N_{RB}^{UL,BWP}(N_{RB}^{UL,BWP}+1)/2) \Bigr
  /// \rceil\f$ bits as per TS38.214 Section 6.1.2.2.2.
  unsigned frequency_resource;
  /// Time domain resource assignment - 4 bit as per TS38.214 Section 6.1.2.1.
  unsigned time_resource;
  /// Frequency hopping flag - 1 bit as per TS38.214 Section 6.3 and Table 7.3.1.1.1-3.
  unsigned frequency_hopping_flag;
  /// Modulation and coding scheme - 5 bits as per TS38.214 Section 6.1.4.1.
  unsigned modulation_coding_scheme;
  /// Redundancy version - 2 bits as per TS38.212 Table 7.3.1.1.1-2.
  unsigned redundancy_version;
  /// TPC command for scheduled PUSCH - 2 bits as per TS38.213 Section 7.1.1.
  unsigned tpc_command;
};

/// Packs a DCI format 0_0 scrambled by TC-RNTI.
dci_payload dci_0_0_tc_rnti_pack(const dci_0_0_tc_rnti_configuration& config);

/// \brief Required parameters for packing a DCI format 1_0 scrambled by C-RNTI, CS-RNTI or MCS-C-RNTI.
/// \remark Defined in TS38.212 Section 7.3.1.2.1.
/// \remark The case where the random access procedure is initiated by a PDCCH order is handled separately.
struct dci_1_0_c_rnti_configuration {
  /// \brief DCI format 1_0 payload size parameters.
  ///
  /// The DCI payload size is determined by the DCI size alignment procedure specified in TS38.212 Section 7.3.1.0. See
  /// \ref get_dci_sizes for more information.
  dci_1_0_size payload_size;
  /// \brief Frequency domain resource assignment - \f$\Bigl \lceil \log_2(N_{RB}^{DL,BWP}(N_{RB}^{DL,BWP}+1)/2) \Bigr
  /// \rceil\f$ bits as per TS38.214 Section 5.1.2.2.2.
  unsigned frequency_resource;
  /// Time domain resource assignment - 4 bit as per TS38.214 Section 5.1.2.1.
  unsigned time_resource;
  /// VRB-to-PRB mapping - 1 bit as per TS38.212 Table 7.3.1.2.2-5.
  unsigned vrb_to_prb_mapping;
  /// Modulation and coding scheme - 5 bits as per TS38.214 Section 5.1.3.
  unsigned modulation_coding_scheme;
  /// New data indicator - 1 bit.
  unsigned new_data_indicator;
  /// Redundancy version - 2 bits as per TS38.212 Table 7.3.1.1.1-2.
  unsigned redundancy_version;
  /// HARQ process number - 4 bits.
  unsigned harq_process_number;
  /// Downlink assignment index - 2 bits as per TS38.213 Section 9.1.3.
  unsigned dl_assignment_index;
  /// TPC command for scheduled PUCCH - 2 bits as per TS38.213 Section 7.2.1.
  unsigned tpc_command;
  /// PUCCH resource indicator - 3 bits as per TS38.213 Section 9.2.3.
  unsigned pucch_resource_indicator;
  /// PDSCH to HARQ feedback timing indicator - 3 bits as per TS38.213 Section 9.2.3.
  unsigned pdsch_harq_fb_timing_indicator;
};

/// Packs a DCI format 1_0 scrambled by C-RNTI, CS-RNTI or MCS-C-RNTI.
dci_payload dci_1_0_c_rnti_pack(const dci_1_0_c_rnti_configuration& config);

/// \brief Required parameters for packing a DCI format 1_0 scrambled by P-RNTI.
/// \remark Defined in TS38.212 Section 7.3.1.2.1.
struct dci_1_0_p_rnti_configuration {
  /// Indicates the content of the DCI format 1_0 payload when scrambled by P-RNTI.
  enum class payload_info {
    /// Indicates that only scheduling information for paging is present in the DCI.
    scheduling_information = 1,
    /// Indicates that only short message is present in the DCI.
    short_messages = 2,
    /// Indicates that both scheduling information for paging and short message are present in the DCI.
    both = 3
  };
  /// Parameter \f$N_{RB}^{DL,BWP}\f$. It must be set to CORESET0 size.
  unsigned N_rb_dl_bwp;
  /// Short Messages Indicator - 2 bits as per TS38.212 Section 7.3.1.2 and Table 7.3.1.2.1-1.
  payload_info short_messages_indicator;
  /// \brief Short Messages - 8 bits as per TS38.331 Section 6.5 and Table 6.5-1.
  /// \remark If only the scheduling information for paging is carried, this bit field is reserved.
  unsigned short_messages;
  /// \brief Frequency domain resource assignment - \f$\Bigl \lceil \log_2(N_{RB}^{DL,BWP}(N_{RB}^{DL,BWP}+1)/2) \Bigr
  /// \rceil\f$ bits as per TS38.214 Section 5.1.2.2.2.
  /// \remark If only the short message is carried, this bit field is reserved.
  unsigned frequency_resource;
  /// \brief Time domain resource assignment - 4 bit as per TS38.214 Section 5.1.2.1.
  /// \remark If only the short message is carried, this bit field is reserved.
  unsigned time_resource;
  /// \brief VRB-to-PRB mapping - 1 bit as per TS38.212 Table 7.3.1.2.2-5.
  /// \remark If only the short message is carried, this bit field is reserved.
  unsigned vrb_to_prb_mapping;
  /// \brief Modulation and coding scheme - 5 bits as per TS38.214 Section 5.1.3 and Table 5.1.3.1-1.
  /// \remark If only the short message is carried, this bit field is reserved.
  unsigned modulation_coding_scheme;
  /// \brief Transport Block scaling - 2 bits as per TS38.214 Section 5.1.3 and Table 5.1.3.2-2.
  /// \remark The value \c 0b11 is reserved.
  /// \remark If only the short message is carried, this bit field is reserved.
  unsigned tb_scaling;
  // Reserved bits - 6 bits.
};

/// Packs a DCI format 1_0 scrambled by P-RNTI.
dci_payload dci_1_0_p_rnti_pack(const dci_1_0_p_rnti_configuration& config);

/// \brief Required parameters for packing a DCI format 1_0 scrambled by SI-RNTI.
/// \remark Defined in TS38.212 Section 7.3.1.2.1.
struct dci_1_0_si_rnti_configuration {
  /// Parameter \f$N_{RB}^{DL,BWP}\f$. It must be set to CORESET0 size.
  unsigned N_rb_dl_bwp;
  /// \brief Frequency domain resource assignment - \f$\Bigl \lceil \log_2(N_{RB}^{DL,BWP}(N_{RB}^{DL,BWP}+1)/2) \Bigr
  /// \rceil\f$ bits as per TS38.214 Section 5.1.2.2.2.
  unsigned frequency_resource;
  /// Time domain resource assignment - 4 bit as per TS38.214 Section 5.1.2.1.
  unsigned time_resource;
  /// VRB-to-PRB mapping - 1 bit as per TS38.212 Table 7.3.1.2.2-5.
  unsigned vrb_to_prb_mapping;
  /// Modulation coding scheme - 5 bits as per TS38.214 Section 5.1.3 and Table 5.1.3.1-1.
  unsigned modulation_coding_scheme;
  /// Redundancy version - 2 bits as per TS38.212 Table 7.3.1.1.1-2.
  unsigned redundancy_version;
  /// System information indicator - 1 bit as per TS38.212 Table 7.3.1.2.1-2.
  unsigned system_information_indicator;
  // Reserved bits - 15 bits.
};

/// Packs a DCI format 1_0 scrambled by SI-RNTI.
dci_payload dci_1_0_si_rnti_pack(const dci_1_0_si_rnti_configuration& config);

/// \brief Required parameters for packing a DCI format 1_0 scrambled by RA-RNTI.
/// \remark Defined in TS38.212 Section 7.3.1.2.1.
struct dci_1_0_ra_rnti_configuration {
  /// \brief Parameter \f$N_{RB}^{DL,BWP}\f$.
  ///
  /// It must be set to:
  ///   - The CORESET0 size, if the CORESET0 is configured.
  ///   - Otherwise, the initial DL BWP size.
  unsigned N_rb_dl_bwp;
  /// \brief Frequency domain resource assignment - \f$\Bigl \lceil \log_2(N_{RB}^{DL,BWP}(N_{RB}^{DL,BWP}+1)/2) \Bigr
  /// \rceil\f$ bits as per TS38.214 Section 5.1.2.2.2.
  unsigned frequency_resource;
  /// Time domain resource assignment - 4 bits as per TS38.214 Section 5.1.2.1.
  unsigned time_resource;
  /// VRB-to-PRB mapping - 1 bit as per to TS38.212 Table 7.3.1.2.2-5.
  unsigned vrb_to_prb_mapping;
  /// Modulation and coding scheme - 5 bits as per TS38.214 Section 5.1.3 and Table 5.1.3.1-1.
  unsigned modulation_coding_scheme;
  /// \brief Transport Block scaling - 2 bits as per TS38.214 Section 5.1.3 and Table 5.1.3.2-2.
  /// \remark The value \c 0b11 is reserved.
  unsigned tb_scaling;
  // Reserved bits - 16 bits.
};

/// Packs a DCI format 1_0 scrambled by RA-RNTI.
dci_payload dci_1_0_ra_rnti_pack(const dci_1_0_ra_rnti_configuration& config);

/// \brief Required parameters for packing a DCI format 1_0 scrambled by TC-RNTI.
/// \remark Defined in TS38.212 Section 7.3.1.2.1.
struct dci_1_0_tc_rnti_configuration {
  /// Parameter \f$N_{RB}^{DL,BWP}\f$. It must be set to the CORESET0 size.
  unsigned N_rb_dl_bwp;
  /// \brief Frequency domain resource assignment - \f$\Bigl \lceil \log_2(N_{RB}^{DL,BWP}(N_{RB}^{DL,BWP}+1)/2) \Bigr
  /// \rceil\f$ bits as per TS38.214 Section 5.1.2.2.2.
  unsigned frequency_resource;
  /// Time domain resource assignment - 4 bit as per TS38.214 Section 5.1.2.1.
  unsigned time_resource;
  /// VRB-to-PRB mapping - 1 bit as per TS38.212 Table 7.3.1.2.2-5.
  unsigned vrb_to_prb_mapping;
  /// Modulation and coding scheme - 5 bits as per TS38.214 Table 5.1.3.1-1.
  unsigned modulation_coding_scheme;
  /// New data indicator - 1 bit.
  unsigned new_data_indicator;
  /// Redundancy version - 2 bits as per TS38.212 Table 7.3.1.1.1-2.
  unsigned redundancy_version;
  /// HARQ process number - 4 bits.
  unsigned harq_process_number;
  /// TPC command for scheduled PUCCH - 2 bits as per TS38.213 Section 7.2.1.
  unsigned tpc_command;
  /// PUCCH resource indicator - 3 bits as per TS38.213 Section 9.2.3.
  unsigned pucch_resource_indicator;
  /// PDSCH to HARQ feedback timing indicator - 3 bits as per TS38.213 Section 9.2.3.
  unsigned pdsch_harq_fb_timing_indicator;
};

/// Packs a DCI format 1_0 scrambled by TC-RNTI.
dci_payload dci_1_0_tc_rnti_pack(const dci_1_0_tc_rnti_configuration& config);

/// \brief Required parameters for packing a DCI format 0_1 scrambled by C-RNTI, CS-RNTI, SP-CSI-RNTI or MCS-C-RNTI.
/// \remark Defined in TS38.212 Section 7.3.1.1.2.
struct dci_0_1_configuration {
  /// \brief DCI format 0_1 payload size parameters.
  ///
  /// The DCI payload size is determined by the DCI size alignment procedure specified in TS38.212 Section 7.3.1.0. See
  /// \ref get_dci_sizes for more information.
  dci_0_1_size payload_size;
  /// \brief Carrier indicator - 3 bits if present, as per TS38.213 Section 10.1.
  ///
  /// Set to the value indicated by the higher layer parameter \e CrossCarrierSchedulingConfig (see TS38.331 Section
  /// 6.3.2, Information Element \e ServingCellConfig) if configured, i.e., if \ref
  /// dci_size_config::cross_carrier_configured is set to \c true when computing the DCI sizes. Otherwise, leave it
  /// unset.
  optional<unsigned> carrier_indicator;
  /// \brief UL/SUL indicator - 1 bit if present, as per TS38.212 Table 7.3.1.1.1-1.
  ///
  /// Set as per TS38.212 Table 7.3.1.1.1-1 to signal a resource allocation on the Supplementary Uplink (SUL) if the UE
  /// is configured with the higher layer parameter \e supplementaryUplink (see TS38.331 Section 6.3.2, Information
  /// Element \e ServingCellConfig) and more than a single carrier in the cell is configured for PUSCH transmission,
  /// i.e., if \ref dci_size_config::sul_configured is set to \c true when computing the DCI sizes. Otherwise, leave it
  /// unset.
  optional<bool> ul_sul_indicator;
  /// Bandwidth part indicator - 0, 1 or 2 bits.
  ///
  /// Identifies the BWP for the frequency domain allocation. The number of bits is determined by the number of UL BWP
  /// configured by higher layers, \f$n_{\textup{BWP,RRC}}\f$ (see dci_size_config::nof_ul_bwp_rrc). It can be either:
  ///   - \f$\bigl \lceil \log_2(n_{\textup{BWP,RRC}} + 1)\bigr \rceil\f$ bits if \f$n_{\textup{BWP,RRC}} \leq 3\f$, set
  ///     according to the higher layer parameter \e BWP-Id (see TS38.331 Section 6.3.2, Information Element \e BWP-Id).
  ///   - \f$\bigl \lceil \log_2(n_{\textup{BWP,RRC}})\bigr \rceil\f$ bits otherwise, as per TS38.212 Table 7.3.1.1.2-1.
  ///
  /// It is required if any BWP besides the initial UL BWP is configured by higher layers, i.e., if \ref
  /// dci_size_config::nof_ul_bwp_rrc is not 0.
  optional<unsigned> bwp_indicator;
  /// \brief PUSCH resource allocation type selector for dynamic PUSCH resource allocations.
  ///
  /// Set this parameter for UEs configured with dynamic PUSCH resource allocation, as indicated by the higher layer
  /// parameter \e resourceAllocation (see \ref dci_size_config::pusch_res_allocation_type). Otherwise, leave it unset.
  optional<dynamic_resource_allocation> dynamic_pusch_res_allocation_type;
  /// \brief Frequency hopping offset, selected from the RRC parameter \e frequencyHoppingOffsetLists.
  ///
  /// \e frequencyHoppingOffsetLists, given by TS38.331 Section 6.3.2, \e PUSCH-Config Information Element, provides 2
  /// or 4 selectable frequency offsets, as per TS38.214 Section 6.3. The \c hopping_offset field acts as an index for
  /// \e frequencyHoppingOffsetLists, determining which of the possible offsets is used.
  ///
  /// it must be set to one of the following values:
  ///   - (0, 1), if \e frequencyHoppingOffsetLists has 2 possible offsets.
  ///   - (0, 1, 2, 3), if \e frequencyHoppingOffsetLists has 4 possible offsets.
  ///
  /// It is required if frequency hopping is enabled via the \ref frequency_hopping_flag. Otherwise, leave it unset.
  optional<unsigned> hopping_offset;
  /// \brief Parameter \f$N_{\textup{UL\_hop}}\f$, as per TS38.212 Section 7.3.1.1.2.
  ///
  /// Number of bits used in the DCI payload to pack the frequency hopping offset, represented by \ref hopping_offset.
  ///
  /// it must be set to:
  ///   - 1, if the RRC parameter \e frequencyHoppingOffsetLists has 2 possible offsets.
  ///   - 2, if the RRC parameter \e frequencyHoppingOffsetLists has 4 possible offsets.
  ///
  /// The frequency hopping offset is packed within the most significant bits of the frequency domain resource
  /// assignment field, therefore, setting \c N_ul_hop does not affect the total payload size. It is required if
  /// frequency hopping is enabled via the \ref frequency_hopping_flag. Otherwise, leave it unset.
  optional<unsigned> N_ul_hop;
  /// \brief Frequency domain resource assignment - number of bits as per TS38.212 Section 7.3.1.1.2.
  ///
  /// For resource allocation type 0, this field occupies \f$N_{RBG}\f$ bits, providing the resource allocation as per
  /// TS38.214 Section 6.1.2.2.1 (see \ref dci_size_config::nof_ul_rb_groups). For resource allocation type 1, this
  /// field takes \f$\Bigl \lceil \log_2(N_{RB}^{UL,BWP}(N_{RB}^{UL,BWP}+1)/2) \Bigr \rceil\f$ bits as per TS38.214
  /// Section 6.1.2.2.2. If the resource allocation type is dynamically selected, this field occupies 1 bit more than
  /// the resource allocation type with the largest field size. In this case, the MSB bit is used to pack the resource
  /// allocation type (see \ref dynamic_pusch_res_allocation_type).
  unsigned frequency_resource;
  /// \brief Time domain resource assignment - 0, 1, 2, 3 or 4 bits, as per TS38.214 Section 6.1.2.1
  ///
  /// The number of bits is determined as \f$\Bigl \lceil \log_2(I) \Bigr \rceil\f$, where \f$I\f$ is either:
  ///   - The number of entries in the higher layer parameter \e pusch-TimeDomainAllocationList (see TS38.331 Section
  ///     6.3.2, Information Element \e PUSCH-Config), if configured.
  ///   - 16 otherwise, which is the number of entries in the default table (see TS38.214 Table 6.1.2.1.1-2).
  unsigned time_resource;
  /// \brief Frequency hopping flag - 1 bit if present, as per TS38.212 Section 7.3.1.1.2 and TS38.214 Section 6.3.
  ///
  /// Set as per TS38.212 Table 7.3.1.1.1-3 and TS38.214 Section 6.3 if the higher layer parameter \e frequencyHopping
  /// (see TS38.331 Section 6.3.2, Information Element \e PUSCH-Config) and resource allocation type 1 are configured,
  /// i.e., if \ref dci_size_config::frequency_hopping_configured is set to \c true and \ref
  /// dci_size_config::pusch_res_allocation_type is not set to \c resource_allocation_type_0 when computing the DCI
  /// sizes. Otherwise, leave it unset.
  optional<unsigned> frequency_hopping_flag;
  /// Modulation and coding scheme - 5 bits as per TS38.214 Section 6.1.4.1.
  unsigned modulation_coding_scheme;
  /// New data indicator - 1 bit.
  unsigned new_data_indicator;
  /// Redundancy version - 2 bits as per TS38.212 Table 7.3.1.1.1-2.
  unsigned redundancy_version;
  /// HARQ process number - 4 bits.
  unsigned harq_process_number;
  /// \brief 1st downlink assignment index - 1 or 2 bits.
  ///
  /// It occupies 1 bit for semi-static HARQ-ACK codebook and 2 bits for dynamic HARQ-ACK codebook (see
  /// dci_size_config::pdsch_harq_ack_cb).
  unsigned first_dl_assignment_index;
  /// \brief 2nd downlink assignment index - 2 bits if present.
  ///
  /// Required for dynamic HARQ-ACK codebooks with two HARQ-ACK sub-codebooks. (see \ref
  /// dci_size_config::pdsch_harq_ack_cb and dci_size_config::dynamic_dual_harq_ack_cb).
  optional<unsigned> second_dl_assignment_index;
  /// TPC command for scheduled PUSCH - 2 bits as per TS38.213 Section 7.1.1.
  unsigned tpc_command;
  /// \brief SRS Resource Indicator (SRI).
  ///
  /// For codebook based transmission, the SRI selects between one of two SRS resources belonging to different antenna
  /// pannels, and occupies 1 bit (see TS38.212 Table 7.3.1.1.2-32). For non-codebook based transmission, the SRI is set
  /// as per TS38.212 Tables 7.3.1.1.2-28/29/30/31, and occupies \f$\Biggl \lceil \log_2 \Biggl
  /// (\sum_{k=1}^{\min\{L_{max}, N_{SRS}\}}\binom{N}{k} \Biggr) \Biggr \rceil \f$ bits (see
  /// dci_size_config::pusch_max_layers and dci_size_config::nof_srs_resources).
  unsigned srs_resource_indicator;
  /// \brief Precoding information and number of layers.
  ///
  /// The content and actual number of bits is defined by TS38.212 Tables 7.3.1.1.2-2/3/4/5, according to whether
  /// transform precoding is enabled or disabled, and the values of the higher layer parameters \e maxRank (see
  /// dci_size_config::max_rank) and \e codebookSubset (see dci_size_config::cb_subset). It is required for codebook
  /// based transmission with more than one antenna port, i.e., if \ref dci_size_config::cb_subset is set when computing
  /// the DCI sizes. Otherwise, leave it unset.
  optional<unsigned> precoding_info_nof_layers;
  /// \brief Antenna ports for PUSCH transmission - 2, 3, 4 or 5 bits.
  ///
  /// Indicates the antenna ports the UE must use to transmit the PUSCH and the corresponding DM-RS, as per TS38.212
  /// Section 7.3.1.1.2 and Tables 7.3.1.1.2-6 to 7.3.1.1.2-23, according to wether transform precoding is enabled, the
  /// DM-RS type (see dci_size_config::pusch_dmrs_A_type and dci_size_config::pusch_dmrs_B_type) and the maximum DM-RS
  /// length (see dci_size_config::pusch_dmrs_A_max_len and dci_size_config::pusch_dmrs_B_max_len).
  unsigned antenna_ports;
  /// \brief SRS request - 2 or 3 bits.
  ///
  /// This parameter occupies:
  ///   - 2 bits for a UE in a cell not configured with the higher layer parameter \e supplementaryUplink (see TS38.331
  ///     Section 6.3.2, Information Element \e ServingCellConfig), filled as per TS38.212 Table 7.3.1.1.2-24.
  ///   - 3 bits for a UE in a cell configured with \e supplementaryUplink. The first bit is the UL/SUL indicator in
  ///     TS38.212 Table 7.3.1.1.1-1 and the remaining bits are filled as per TS38.212 Table 7.3.1.1.2-24.
  ///
  /// This field may also indicate the associated CSI-RS according to TS38.214 Section 6.1.1.2.
  unsigned srs_request;
  /// \brief CSI request - 0 to 6 bits.
  ///
  /// Required if the higher layer parameter \e reportTriggerSize (see dci_size_config::report_trigger_size) is not 0.
  optional<unsigned> csi_request;
  /// \brief CBG Transmission Information (CBGTI) - 0, 2, 4, 6 or 8 bits.
  ///
  /// Number of bits determined by the higher layer parameter \e maxCodeBlockGroupsPerTransportBlock (see \ref
  /// dci_size_config::max_cbg_tb_pusch). Set as per TS38.214 Section 6.1.5.2 if the UE is configured to use CBGs, i.e.,
  /// if \ref dci_size_config::max_cbg_tb_pusch is set when computing the DCI sizes.
  optional<unsigned> cbg_transmission_info;
  /// \brief PT-RS/DM-RS association - 2 bit if present.
  ///
  /// This parameter is used to link a PT-RS to a DM-RS. It must be set if PT-RS signals are configured (see
  /// dci_size_config::ptrs_uplink_configured) and there is more than one UL DM-RS, that is, when transform precoding is
  /// disabled and the maximum configured UL rank is grater than 1 (see dci_size_config::max_rank).
  optional<unsigned> ptrs_dmrs_association;
  /// \brief Beta offset indicator - 2 bits if present.
  ///
  /// Indicates beta offset values as per TS38.213 Table 9.3-3. Required if dynamic beta offsets are configured for UCI
  /// on PUSCH, i.e., if \ref dci_size_config::dynamic_beta_offsets is set to \c true when computing the DCI sizes.
  optional<unsigned> beta_offset_indicator;
  /// \brief DM-RS sequence initialization - 1 bit if present.
  ///
  /// Required if transform precoding is disabled, i.e., if \ref dci_size_config::transform_precoding_enabled is set to
  /// \c false when computing the DCI sizes.
  optional<unsigned> dmrs_seq_initialization;
  /// \brief UL-SCH indicator - 1 bit.
  ///
  /// Indicates if UL-SCH should be included in a PUSCH transmission, according to its possible values:
  ///   - 0: Exclude UL-SCH from the PUSCH transmission. Allows UCI on PUSCH without application data.
  ///   - 1: Include UL-SCH in the PUSCH transmission.
  unsigned ul_sch_indicator;
};

/// Packs a DCI format 0_1 scrambled by C-RNTI, CS-RNTI, SP-CSI-RNTI or MCS-C-RNTI.
dci_payload dci_0_1_pack(const dci_0_1_configuration& config);

/// \brief Required parameters for packing a DCI format 1_1 scrambled by C-RNTI, CS-RNTI, SP-CSI-RNTI or MCS-C-RNTI.
/// \remark Defined in TS38.212 Section 7.3.1.2.2.
struct dci_1_1_configuration {
  /// \brief DCI format 1_1 payload size parameters.
  ///
  /// The DCI payload size is determined by the DCI size alignment procedure specified in TS38.212 Section 7.3.1.0. See
  /// \ref get_dci_sizes for more information.
  dci_1_1_size payload_size;
  /// \brief Carrier indicator - 3 bits if present, as per TS38.213 Section 10.1.
  ///
  /// Set to the value indicated by the higher layer parameter \e CrossCarrierSchedulingConfig (see TS38.331 Section
  /// 6.3.2, Information Element \e ServingCellConfig) if configured, i.e., if \ref
  /// dci_size_config::cross_carrier_configured is set to \c true when computing the DCI sizes. Otherwise, leave it
  /// unset.
  optional<unsigned> carrier_indicator;
  /// Bandwidth part indicator - 0, 1 or 2 bits.
  ///
  /// Identifies the BWP for the frequency domain allocation. The number of bits is determined by the number of DL BWP
  /// configured by higher layers, \f$n_{\textup{BWP,RRC}}\f$ (see dci_size_config::nof_dl_bwp_rrc). It can be either:
  ///   - \f$\bigl \lceil \log_2(n_{\textup{BWP,RRC}} + 1)\bigr \rceil\f$ bits if \f$n_{\textup{BWP,RRC}} \leq 3\f$, set
  ///     according to the higher layer parameter \e BWP-Id (see TS38.331 Section 6.3.2, Information Element \e BWP-Id).
  ///   - \f$\bigl \lceil \log_2(n_{\textup{BWP,RRC}})\bigr \rceil\f$ bits otherwise, as per TS38.212 Table 7.3.1.1.2-1.
  ///
  /// It is required if any BWP besides the initial DL BWP is configured by higher layers, i.e., if \ref
  /// dci_size_config::nof_dl_bwp_rrc is not 0.
  optional<unsigned> bwp_indicator;
  /// \brief PDSCH resource allocation type selector for dynamic PDSCH resource allocations.
  ///
  /// Set this parameter for UEs configured with dynamic PDSCH resource allocation, as indicated by the higher layer
  /// parameter \e resourceAllocation (see \ref dci_size_config::pdsch_res_allocation_type). Otherwise, leave it unset.
  optional<dynamic_resource_allocation> dynamic_pdsch_res_allocation_type;
  /// \brief Frequency domain resource assignment - number of bits as per TS38.212 Section 7.3.1.2.2.
  ///
  /// For resource allocation type 0, this field occupies \f$N_{RBG}\f$ bits, providing the resource allocation as per
  /// TS38.214 Section 5.1.2.2.1 (see \ref dci_size_config::nof_dl_rb_groups). For resource allocation type 1, this
  /// field takes \f$\Bigl \lceil \log_2(N_{RB}^{DL,BWP}(N_{RB}^{DL,BWP}+1)/2) \Bigr \rceil\f$ bits as per TS38.214
  /// Section 5.1.2.2.2. If the resource allocation type is dynamically selected, this field occupies 1 bit more than
  /// the resource allocation type with the largest field size. In this case, the MSB bit is used to indicate the
  /// resource allocation type (see \ref dynamic_pdsch_res_allocation_type).
  unsigned frequency_resource;
  /// \brief Time domain resource assignment - 0, 1, 2, 3 or 4 bit, as per TS38.214 Section 5.1.2.1
  ///
  /// The number of bits is determined as \f$\Bigl \lceil \log_2(I) \Bigr \rceil\f$, where \f$I\f$ is either:
  ///   - The number of entries in the higher layer parameter \e pdsch-TimeDomainAllocationList (see TS38.331 Section
  ///     6.3.2, Information Elements \e PDSCH-Config), if configured.
  ///   - Otherwise, the number of entries in the default table (see TS38.214 Table 5.1.2.1.1-2).
  unsigned time_resource;
  /// \brief VRB-to-PRB mapping - 1 bit if present.
  ///
  /// Indicates if non-interleaved or interleaved VRB to PRB mapping is used. Set as per TS38.212 Table 7.3.1.2.2-5 if
  /// resource allocation type 1 and interleaved VRB-to-PRB mapping are configured (see \ref
  /// dci_size_config::pdsch_res_allocation_type and \ref dci_size_config::interleaved_vrb_prb_mapping). Otherwise,
  /// leave it unset.
  optional<unsigned> vrb_prb_mapping;
  /// \brief PRB bundling size indicator - 1 bit if present.
  ///
  /// Dynamically selects between the configured PRB bundling size sets 1 and 2. Set as per TS38.214 Section 5.1.2.3 if
  /// dynamic PRB bundling is used, i.e., if \ref dci_size_config::dynamic_prb_bundling is set to \c true for the DCI
  /// size alignment procedure. Otherwise, leave it unset.
  optional<unsigned> prb_bundling_size_indicator;
  /// \brief Rate matching indicator - 0, 1 or 2 bits.
  ///
  /// Indicates which pattern of reserved resources with RB / OFDM symbol granularity, if any, is applicable to the
  /// PDSCH transmission. Set as per TS38.212 Section 7.3.1.2.2, if any \e RateMatchPattern is configured (see
  /// dci_size_config::rm_pattern_group1 and dci_size_config::rm_pattern_group2). Otherwise, leave it unset.
  optional<unsigned> rate_matching_indicator;
  /// \brief ZP CSI-RS trigger - 0, 1 or 2 bits.
  ///
  /// Indicates which set of aperiodic ZP CSI-RS reserved resources, if any, is applicable to the PDSCH transmission.
  /// Set as per TS38.214 Section 5.1.4.2 if any aperiodic ZP CSI-RS resource sets are configured, i.e., if \ref
  /// dci_size_config::nof_aperiodic_zp_csi is not set to 0 when computing the DCI sizes. Otherwise, leave it unset.
  optional<unsigned> zp_csi_rs_trigger;
  /// Modulation and coding scheme for TB 1 - 5 bits as per TS38.214 Section 5.1.3.1.
  unsigned tb1_modulation_coding_scheme;
  /// New data indicator for TB 1 - 1 bit.
  unsigned tb1_new_data_indicator;
  /// Redundancy version for TB 1 - 2 bits as per TS38.212 Table 7.3.1.1.1-2.
  unsigned tb1_redundancy_version;
  /// \brief Modulation and coding scheme for TB 2 - 5 bits if present.
  ///
  /// Set as per TS38.214 Section 5.1.3.1 if DCI is configured to schedule two codewords, i.e., if \ref
  /// dci_size_config::pdsch_two_codewords is set to \c true when computing the DCI sizes.
  optional<unsigned> tb2_modulation_coding_scheme;
  /// \brief New data indicator for TB 2 - 1 bit if present.
  ///
  /// Set if DCI is configured to schedule two codewords, i.e., if \ref dci_size_config::pdsch_two_codewords is set to
  /// \c true when computing the DCI sizes.
  optional<unsigned> tb2_new_data_indicator;
  /// \brief Redundancy version for TB 2 - 2 bits if present.
  ///
  /// Set as per TS38.212 Table 7.3.1.1.1-2 if DCI is configured to schedule two codewords, i.e., if \ref
  /// dci_size_config::pdsch_two_codewords is set to \c true when computing the DCI sizes.
  optional<unsigned> tb2_redundancy_version;
  /// HARQ process number - 4 bits.
  unsigned harq_process_number;
  /// \brief Downlink Assignment Index (DAI) - 0, 2 or 4 bits.
  ///
  /// This parameter occupies:
  ///   - 4 bits if more than one serving cell is configured in the DL and PDSCH uses dynamic HARQ-ACK codebook, where
  ///     the 2 MSB bits are the counter DAI and the 2 LSB bits are the total DAI.
  ///   - 2 bits if one serving cell is configured in the DL and PDSCH uses dynamic HARQ-ACK codebook, where the 2 bits
  ///     are the counter DAI.
  ///
  /// It is required if dynamic HARQ-ACK codebook is used in the PDSCH (see \ref dci_size_config::pdsch_harq_ack_cb).
  optional<unsigned> downlink_assignment_index;
  /// TPC command for scheduled PUCCH - 2 bits as per TS38.213 Section 7.2.1.
  unsigned tpc_command;
  /// PUCCH resource indicator - 3 bits as per TS38.213 Section 9.2.3.
  unsigned pucch_resource_indicator;
  /// \brief PDSCH to HARQ feedback timing indicator - 0, 1, 2 or 3 bits.
  ///
  ///
  /// Number of slots between reception of PDSCH and transmission of the HARQ-ACK. It occupies \f$\log_2(I)\f$ bits,
  /// where \f$I\f$ is the the number of entries of the higher layer parameter \e dl-DataToUL-ACK (see TS38.331 Section
  /// 6.3.2, Information Element \e PUCCH-Config). Set as per TS38.213 Section 9.2.3 if The higher layer parameter \e
  /// dl-DataToUL-ACK is configured with more than a single value, i.e., if \ref dci_size_config::nof_pdsch_ack_timings
  /// is greater than 1 when computing the DCI sizes.
  optional<unsigned> pdsch_harq_fb_timing_indicator;
  /// \brief Antenna ports for PDSCH transmission - 4, 5 or 6 bits.
  ///
  /// Indicates the antenna ports used to transmit the PDSCH and the corresponding DM-RS, as per TS38.212 Section
  /// 7.3.1.2.2 and Tables 7.3.1.2.2-1/2/3/4, according to the DM-RS type (see
  /// dci_size_config::pdsch_dmrs_A_type and dci_size_config::pdsch_dmrs_B_type) and the maximum DM-RS length (see
  /// dci_size_config::pdsch_dmrs_A_max_len and dci_size_config::pdsch_dmrs_B_max_len). If both DM-RS mapping type A and
  /// B are configured, this field occupies the number of bits required to pack either DM-RS port allocation.
  unsigned antenna_ports;
  /// \brief Transmission configuration indication - 3 bits if present.
  ///
  /// Set as per TS38.214 Section 5.1.5 if the higher layer parameter \e tci-PresentInDCI (see TS38.331 Section 6.3.2,
  /// Information Element \e ControlResourceSet) is enabled, i.e., if \ref dci_size_config::pdsch_tci is set to \c true
  /// when computing the DCI sizes.
  optional<unsigned> tx_config_indication;
  /// \brief SRS request - 2 or 3 bits.
  ///
  /// This parameter occupies:
  ///   - 2 bits for a UE in a cell not configured with the higher layer parameter \e supplementaryUplink (see TS38.331
  ///     Section 6.3.2, Information Element \e ServingCellConfig), filled as per TS38.212 Table 7.3.1.1.2-24.
  ///   - 3 bits for a UE in a cell configured with \e supplementaryUplink. The first bit is the UL/SUL indicator in
  ///     TS38.212 Table 7.3.1.1.1-1 and the remaining bits are filled as per TS38.212 Table 7.3.1.1.2-24.
  ///
  /// This field may also indicate the associated CSI-RS according to TS38.214 Section 6.1.1.2.
  unsigned srs_request;
  /// \brief CBG Transmission Information (CBGTI) - 0, 2, 4, 6 or 8 bits.
  ///
  /// Number of bits determined by the higher layer parameters \e maxCodeBlockGroupsPerTransportBlock (see \ref
  /// dci_size_config::max_cbg_tb_pdsch) and \e maxNrofCodeWordsScheduledByDCI (see \ref
  /// dci_size_config::pdsch_two_codewords). Set as per TS38.214 Section 5.1.7 if the UE is configured to use CBGs,
  /// i.e., if \ref dci_size_config::max_cbg_tb_pdsch is set when computing the DCI sizes.
  optional<unsigned> cbg_transmission_info;
  /// \brief CBG Flushing Information (CBGFI) - 1 bit if present.
  ///
  /// Indicates whether the set of CBGs being transmitted can be combined with previous transmissions. Required if the
  /// higher layer parameter \e codeBlockGroupFlushIndicator is set to \c true, i.e., if \ref
  /// dci_size_config::cbg_flush_indicator is set to \c true when computing the DCI sizes.
  optional<unsigned> cbg_flushing_info;
  /// DM-RS sequence initialization - 1 bit.
  unsigned dmrs_seq_initialization;
};

/// Packs a DCI format 1_1 scrambled by C-RNTI, CS-RNTI, SP-CSI-RNTI or MCS-C-RNTI.
dci_payload dci_1_1_pack(const dci_1_1_configuration& config);

/// Collects the Random Access Response grant content fields described by TS38.213 Table 8.2-1.
struct dci_rar_configuration {
  /// Frequency hopping flag - 1 bit.
  unsigned frequency_hopping_flag;
  /// PUSCH frequency resource allocation - 14 bits.
  unsigned frequency_resource;
  /// PUSCH time resource allocation - 4 bits.
  unsigned time_resource;
  /// Modulation and coding scheme - 4 bits.
  unsigned modulation_coding_scheme;
  /// Transmission power control for PUSCH - 3 bits.
  unsigned tpc;
  /// CSI request - 1 bit.
  unsigned csi_request;
};

/// Packs a DCI transmitted in a Random Access Response.
dci_payload dci_rar_pack(const dci_rar_configuration& config);

/// Validates a DCI configuration for the DCI size alignment procedure.
bool validate_dci_size_config(const dci_size_config& config);

} // namespace srsran
