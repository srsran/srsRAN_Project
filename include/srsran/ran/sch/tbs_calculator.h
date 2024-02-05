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

/// \file
/// \brief TBS-related helper functions.

#pragma once

#include "sch_mcs.h"

#include <array>

namespace srsran {

/// \brief Collection of parameters required to calculate the TBS for a shared channel transmission.
///
/// The configuration applies to the determination of the transport block size (TBS) for both PDSCH, as described in
/// TS38.214 Section 5.1.3.2, and PUSCH, as described in TS38.214 Section 6.1.4.2.
struct tbs_calculator_configuration {
  /// \brief Number of OFDM symbols allocated to the SCH within the slot.
  ///
  /// Parameter \f$N_{\textup{symb}}^{\textup{sh}}\f$ for both PDSCH and PUSCH.
  unsigned nof_symb_sh;
  /// \brief Number of REs for DM-RS per PRB in the allocated duration.
  ///
  /// Parameter \f$N_{\textup{DMRS}}^{\textup{PRB}}\f$ for both PDSCH and PUSCH. It includes the overhead of the DM-RS
  /// CDM groups without data.
  unsigned nof_dmrs_prb;
  /// \brief Number of REs configured as overhead.
  ///
  /// Parameter \f$N_{\textup{oh}}^{\textup{PRB}}\f$. Possible values are {0, 6, 12, 18}.
  ///
  /// Specifically, for PDSCH, the value is zero if:
  /// - the PDSCH is scheduled by PDCCH with a CRC scrambled by SI-RNTI, RA-RNTI or P-RNTI, or
  /// - the higher layer parameter \e xOverhead in \e PDSCH-ServingCellconfig is not configured.
  ///
  /// Otherwise, the value is provided by the parameter \e xOverhead in \e PDSCH-ServingCellconfig.
  ///
  /// Similarly, for PUSCH, the value is zero if:
  /// - the transport block corresponds to a Msg3 transmission, or
  /// - the higher layer parameter \e xOverhead in \e PUSCH-ServingCellConfig is not configured.
  ///
  /// Otherwise, the value is provided by the parameter \e xOverhead in \e PUSCH-ServingCellConfig.
  unsigned nof_oh_prb;
  /// \brief Modulation and coding scheme for the SCH transmission.
  ///
  /// The modulation scheme yields the number of bits per RE, parameter \f$Q_m\f$ for both PDSCH and PUSCH. Possible
  /// values are:
  /// - QPSK,
  /// - QAM-16,
  /// - QAM-64, and
  /// - 256-QAM.
  sch_mcs_description mcs_descr;
  /// \brief Number of layers.
  ///
  /// Parameter \f$\nu\f$. For PDSCH, the possible values are:
  /// - {1, ..., 4} for one codeword, and
  /// - {5, ..., 8} for two codewords.
  ///
  /// For PUSCH, which only admits single-codeword transmissions, the only possible values are {1, ..., 4}.
  unsigned nof_layers;
  /// \brief TB scaling field.
  ///
  /// Provides the scaling factor \f$S\f$ according to TS38.214 Table 5.1.3.2-2. Possible values are {0, 1, 2}.
  ///
  /// For the PDSCH assigned by a PDCCH with DCI format 1_0 with CRC scrambled by P-RNTI or RA-RNTI, the value is
  /// provided by the <em>TB scaling</em> field of the DCI. For other PDSCHs, it must be set to zero.
  ///
  /// For PUSCH, the parameter is meaningless and must be set to zero.
  unsigned tb_scaling_field;
  /// \brief Total number of PRBs allocated to the UE.
  ///
  /// Parameter \f$n_{PRB}\f$. Possibles values are {1, ..., 275}.
  unsigned n_prb;
};

/// \brief Converts TB scaling field into the scaling factor S (see TS38.214 Table 5.1.3.2-2).
/// \param[in] scaling TB scaling field, possible values are {0, 1, 2}.
/// \return Scaling factor S.
float tbs_calculator_pdsch_get_scaling_factor(unsigned scaling);

/// \brief Gets the smallest TBS in bits (from TS 38.214 Table 5.1.3.2-1) greater than or equal to a given value.
///
/// \param[in] nof_info_prime Quantized intermediate number of information bits, as per TS38.214 Section 5.1.3.2.
/// \remark \c nof_info_prime must not be greater than 3824 bits.
/// \return The smallest TBS from TS38.214 Table 5.1.3.2-1 that is greater than or equal to \c nof_info_prime.
unsigned tbs_calculator_table_find_smallest_not_less_than(unsigned nof_info_prime);

/// \brief Calculates the TBS for a given SCH transmission.
///
/// Implemented as per TS38.214 Section 5.1.3.2.
/// \return The TBS in bits.
unsigned tbs_calculator_calculate(const tbs_calculator_configuration& config);

} // namespace srsran
