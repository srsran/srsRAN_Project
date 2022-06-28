/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SCHEDULER_SUPPORT_TBS_CALCULATOR_H
#define SRSGNB_SCHEDULER_SUPPORT_TBS_CALCULATOR_H

#include <array>

namespace srsgnb {

/// TS38.214 Table 5.1.3.2-1.
const std::array<unsigned, 93> table_valid_tbs = {
    24,   32,   40,   48,   56,   64,   72,   80,   88,   96,   104,  112,  120,  128,  136,  144,  152,  160,  168,
    176,  184,  192,  208,  224,  240,  256,  272,  288,  304,  320,  336,  352,  368,  384,  408,  432,  456,  480,
    504,  528,  552,  576,  608,  640,  672,  704,  736,  768,  808,  848,  888,  928,  984,  1032, 1064, 1128, 1160,
    1192, 1224, 1256, 1288, 1320, 1352, 1416, 1480, 1544, 1608, 1672, 1736, 1800, 1864, 1928, 2024, 2088, 2152, 2216,
    2280, 2408, 2472, 2536, 2600, 2664, 2728, 2792, 2856, 2976, 3104, 3240, 3368, 3496, 3624, 3752, 3824};

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

/// \brief Calculates the TBS for a given PDSCH transmission.
///
/// Implemented as per TS38.214 Section 5.1.3.2.
unsigned tbs_calculator_pdsch_calculate(const tbs_calculator_pdsch_configuration& config);

} // namespace srsgnb

#endif // SRSGNB_SCHEDULER_SUPPORT_TBS_CALCULATOR_H
