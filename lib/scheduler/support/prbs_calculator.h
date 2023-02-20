/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/ran/modulation_scheme.h"
#include "srsgnb/ran/resource_block.h"
#include "srsgnb/ran/sch_mcs.h"
#include "srsgnb/support/math_utils.h"

namespace srsgnb {

/// \brief Collects the parameters to calculate the num. of PRBS given the payload size, for a PDSCH transmission.
///
/// \remark For the definition of the parameters within this struct, refer to TS 38.214, Section 5.1.3.2.
struct prbs_calculator_sch_config {
  /// \brief Size (in bytes) of payload that has to fit within the TBS. Maximum value is 478 bytes (3824 bits).
  unsigned payload_size_bytes;
  /// \brief Number of OFDM symbols of the PDSCH allocation within the slot, parameter \f$N_{symb}^{sh}\f$.
  unsigned nof_symb_sh;
  /// \brief Number of RE for DMRS per PRB in the PDSCH transmission, parameter \f$N_{DMRS}^{PRB}\f$.
  unsigned nof_dmrs_prb;
  /// \brief Number of RE configured as overhead, parameter \f$N_{oh}^{PRB}\f$.
  ///
  /// Possible values are {0, 6, 12, 18}.
  ///
  /// It must be set to zero if:
  /// - the PDSCH is scheduled by PDCCH with a CRC scrambled by SI-RNTI, RA-RNTI or P-RNTI, or
  /// - the higher layer parameter \c xOverhead in \c PDSCH-ServingCellconfig is not present.
  ///
  /// Otherwise, it must be set to the higher layer parameter \c xOverhead in \c PDSCH-ServingCellconfig.
  unsigned nof_oh_prb;
  /// \brief Modulation and coding scheme.
  ///
  /// The modulation scheme translates to the number of bits per RE, parameter \f$Q_m\f$, as per TS38.214,
  /// Tables 5.1.3.1-1, 5.1.3.1-2, 5.1.3.1-3. Possible values are {QPSK, QAM-16, QAM-64, QAM-256}.
  sch_mcs_description mcs_descr;
  /// \brief Number of layers, parameter \f$\nu\f$.
  ///
  /// Possible values are:
  /// - {1...4} for one codeword, and
  /// - {5...8} for two codewords.
  unsigned nof_layers;
  /// \brief TB scaling field as per TS38.214 Table 5.1.3.2-2, it is used to derive parameter \f$S\f$.
  ///
  /// Possible values are {0,1,2}.
  unsigned tb_scaling_field{0};
};

/// \brief Collects the outputs of the helper that returns the num. of PRBs and TBS, for a PDSCH transmission.
struct sch_prbs_tbs {
  /// Number of PRBs required for a given PDSCH transmission.
  unsigned nof_prbs;
  /// TBS in bytes corresponding to the PRBs required for a given PDSCH transmission.
  unsigned tbs_bytes;
};

/// \brief Computes the required number of PRBs necessary to transmit a given payload size.
///
/// This function implements the inverse procedure of the TBS calculation procedure (as per TS38.214 Section 5.1.3.2) to
/// derive the number of PRBs from a given payload in bytes.
///
/// \param[in] sch_config is a struct with the PDSCH configuration to compute the num. of PRBs.
/// \return Returns a struct with the number of PRBs and the corresponding TBS. If the payload_size is greater than 478
/// bytes (3824 bits), it returns the number of PRBs and TBS corresponding to maximum allowed payload size of 478 bytes.
sch_prbs_tbs get_nof_prbs(const prbs_calculator_sch_config& sch_config);

/// \brief Computes a coarse estimate of the number of required PRBs to transmit a given payload size. This estimate
/// might lead to a TBS that is smaller or larger than the given payload size.
///
/// \param[in] sch_config is a struct with the PDSCH configuration to compute the num. of PRBs.
/// \return estimate of the number of PRBs.
unsigned estimate_required_nof_prbs(const prbs_calculator_sch_config& sch_config);

} // namespace srsgnb
