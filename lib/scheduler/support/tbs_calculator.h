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

#include <array>

namespace srsgnb {

/// Collects the parameters required to calculate the TBS for a PDSCH transmission.
struct tbs_calculator_pdsch_configuration {
  /// \brief Number of OFDM symbols of the PDSCH allocation within the slot, parameter \f$N_{symb}^{sh}\f$.
  unsigned nof_symb_sh;
  /// \brief Number of RE for DMRS per PRB in the PDSCH transmission, parameter \f$N_{DMRS}^{PRB}\f$.
  ///
  /// It includes the overhead of the DM-RS CDM groups without data.
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
  /// Target Code Rate, parameter \f$R\f$.
  float tcr;
  /// \brief Number of bits per RE, parameter \f$Q_m\f$.
  ///
  /// Possible values are:
  /// - 2 for QPSK,
  /// - 4 for 16-QAM,
  /// - 6 for 64-QAM, and
  /// - 8 for 256-QAM.
  unsigned modulation_order;
  /// \brief Number of layers, parameter \f$\nu\f$.
  ///
  /// Possible values are:
  /// - {1...4} for one codeword, and
  /// - {5...8} for two codewords.
  unsigned nof_layers;
  /// \brief TB scaling field as per TS38.214 Table 5.1.3.2-2, it is used to derive parameter \f$S\f$.
  ///
  /// Possible values are {0,1,2}.
  ///
  /// For the PDSCH assigned by a PDCCH with DCI format 1_0 with CRC scrambled by P-RNTI, or RA-RNTI the value is
  /// determined from field <tt>TB scaling</tt>.
  ///
  /// Otherwise, it must be set to zero.
  unsigned tb_scaling_field;
  /// \brief Total number of allocated PRBs for the UE, parameter \f$n_{PRB}\f$.
  ///
  /// Possibles values are {1...275}.
  unsigned n_prb;
};

/// \brief Converts TB scaling field into Scaling Factor S (see Table 5.1.3.2-2, TS 38.214).
/// \param[in] scaling is the TB scaling field, possible values are {0,1,2}.
/// \return Scaling factor S corresponding to scaling.
float tbs_calculator_pdsch_get_scaling_factor(unsigned scaling);

/// \brief Get smallest TBS in bits (from Table 5.1.3.2-1, TS 38.214) greater than or equal to a given value.
///
/// \param[in] nof_info_prime is the quantized intermediate num. of information bits, as per Section 5.1.3.2, TS 38.214.
/// \remark nof_info_prime must not be greater than 3824 bits.
/// \return The smallest TBS from Table 5.1.3.2-1, TS 38.214 that is greater than or equal to nof_info_prime.
unsigned tbs_calculator_table_find_smallest_not_less_than(unsigned nof_info_prime);

/// \brief Calculates the TBS for a given PDSCH transmission.
///
/// Implemented as per TS38.214 Section 5.1.3.2.
unsigned tbs_calculator_pdsch_calculate(const tbs_calculator_pdsch_configuration& config);

} // namespace srsgnb
