/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_GET_NOF_PRBS_H
#define SRSGNB_GET_NOF_PRBS_H

#include "srsgnb/ran/resource_block.h"
#include "srsgnb/support/math_utils.h"

namespace srsgnb {

/// \brief Collects the parameters to calculate the num. of PRBS given the payload size, for a PDSCH transmission.
///
/// \remark For the definition of the parameters within this struct, refer to TS 38.214, Section 5.1.3.2.
struct prbs_calculator_pdsch_config {
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
  /// \brief  Number of bits per RE, parameter \f$Q_m\f$, as per Tables 5.1.3.1-1, 5.1.3.1-2, 5.1.3.1-3, in
  /// TS38.214.
  ///
  /// Possible values are {2, 4, 6, 8}.
  unsigned mod_order;
  /// Target Code Rate, parameter \f$R\f$, as per Tables 5.1.3.1-1, 5.1.3.1-2, 5.1.3.1-3, in
  /// TS38.214.
  float target_code_rate;
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

/// \brief Computes the required number of PRBs necessary to transmit a given payload size.
///
/// This function implements the inverse procedure of the TBS calculation procedure (as per TS38.214 Section 5.1.3.2) to
/// derive the number of PRBs from a given payload in bytes.
///
/// \remark This function only works for payload size <= 478 bytes (3824 bits).
/// \param[in] pdsch_config is a struct with the PDSCH configuration to compute the num. of PRBs.
/// \return If the payload_size is not greater than 478 bytes (3824 bits), returns the number of PRBs. Else, it returns
/// -1.
unsigned get_nof_prbs(const prbs_calculator_pdsch_config& pdsch_config);

} // namespace srsgnb

#endif // SRSGNB_GET_NOF_PRBS_H
