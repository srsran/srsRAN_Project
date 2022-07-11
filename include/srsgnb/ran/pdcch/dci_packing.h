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

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/pdcch/pdcch_constants.h"

namespace srsgnb {

/// DCI payload data type.
using dci_payload = bounded_bitset<pdcch_constants::MAX_DCI_PAYLOAD_SIZE>;

/// \brief Describes the necessary parameters for packing a DCI format 1_0 scrambled by P-RNTI as per TS38.212
/// Section 7.3.1.2.1.
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
  /// \brief Frequency domain resource assignment - \f$\Bigl \lceil log_2(N_{RB}^{DL,BWP}(N_{RB}^{DL,BWP}+1)/2) \Bigr
  /// \rceil\f$ bits as per TS38.214 Section 5.1.2.2.2.
  /// \remark If only the short message is carried, this bit field is reserved.
  unsigned frequency_resource;
  /// \brief Time domain resource assignment - 4 bit as per TS38.214 Section 5.1.2.1.
  /// \remark If only the short message is carried, this bit field is reserved.
  unsigned time_resource;
  /// \brief VRB-to-PRB mapping - 1 bit as per to TS38.212 Table 7.3.1.2.2-5.
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

/// \brief Describes the necessary parameters for packing a DCI format 1_0 scrambled by SI-RNTI as per TS38.212
/// Section 7.3.1.2.1.
struct dci_1_0_si_rnti_configuration {
  /// Parameter \f$N_{RB}^{DL,BWP}\f$. It must be set to CORESET0 size.
  unsigned N_rb_dl_bwp;
  /// \brief Frequency domain resource assignment - \f$\Bigl \lceil log_2(N_{RB}^{DL,BWP}(N_{RB}^{DL,BWP}+1)/2) \Bigr
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

/// \brief Describes the necessary parameters for packing a DCI format 1_0 scrambled by RA-RNTI as per TS38.212
/// Section 7.3.1.2.1.
struct dci_1_0_ra_rnti_configuration {
  /// \brief Parameter \f$N_{RB}^{DL,BWP}\f$.
  ///
  /// It must be set to:
  ///   - The CORESET0 size, if the CORESET0 is configured.
  ///   - Otherwise, the initial DL BWP size.
  unsigned N_rb_dl_bwp;
  /// \brief Frequency domain resource assignment - \f$\Bigl \lceil log_2(N_{RB}^{DL,BWP}(N_{RB}^{DL,BWP}+1)/2) \Bigr
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

} // namespace srsgnb
