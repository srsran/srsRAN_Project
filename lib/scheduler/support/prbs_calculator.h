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
#include "tbs_calculator.h"

namespace srsgnb {

/// \brief Computes the required number of PRBs necessary to transmit a given payload.
///
/// This function implements the inverse procedure of the TBS calculation procedure (as per TS38.214 Section 5.1.3.2) to
/// derive the number of PRBs from the given payload in bits.
///
/// \remark This function only works for payload size <= 3824 bits (478 bytes).
///
/// \param[in] payload_size_bytes size of payload in bytes that has to fit within the TBS.
/// \param[in] nof_symb_sh Number of OFDM symbols of the PDSCH allocation within the slot, parameter
/// \f$N_{symb}^{sh}\f$.
/// \param[in] nof_dmrs_prb Number of RE for DMRS per PRB in the PDSCH transmission, parameter \f$N_{DMRS}^{PRB}\f$.
/// \param[in] nof_oh_prb Number of RE configured as overhead, parameter \f$N_{oh}^{PRB}\f$.
/// \param[in] mod_order Number of bits per RE, parameter \f$Q_m\f$, as per Tables 5.1.3.1-1, 5.1.3.1-2, 5.1.3.1-3, in
/// TS38.214.
/// \param[in] target_code_rate Target Code Rate, parameter \f$R\f$, as per Tables 5.1.3.1-1, 5.1.3.1-2, 5.1.3.1-3, in
/// TS38.214.
/// \param[in] nof_layers Number of layers, parameter \f$\nu\f$.
/// \param[in] tb_scaling_field TB scaling field as per TS38.214 Table 5.1.3.2-2, used to derive parameter \f$S\f$.
/// \return number of PRBs.
unsigned get_nof_prbs(unsigned payload_size,
                      unsigned nof_symb_sh,
                      unsigned nof_dmrs_prb,
                      unsigned nof_oh_prb,
                      unsigned mod_order,
                      float    target_code_rate,
                      unsigned nof_layers,
                      unsigned tb_scaling_field = 0);

} // namespace srsgnb

#endif // SRSGNB_GET_NOF_PRBS_H
