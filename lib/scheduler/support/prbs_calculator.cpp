/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "prbs_calculator.h"
#include "tbs_calculator.h"

using namespace srsgnb;

unsigned srsgnb::get_nof_prbs(const prbs_calculator_pdsch_config& pdsch_cfg)
{
  // Convert size into bits, as per TS procedures for TBS.
  unsigned payload_size = pdsch_cfg.payload_size_bytes * 8;

  // This function is limited to payload_size <= 3824 bits.
  if (payload_size > table_valid_tbs.back()) {
    return -1;
  }

  auto less_than_eq_to = [](const unsigned& size, const unsigned& tbs) { return size <= tbs; };

  // Get TBS greater or equal to payload_size.
  const unsigned* it = std::upper_bound(table_valid_tbs.begin(), table_valid_tbs.end(), payload_size, less_than_eq_to);
  srsran_assert(it != table_valid_tbs.end(), "Estimate of nof_info exceeds the maximum (3824).");

  // This is an estimate of the N_info (as per Section 5.1.3.2, TS 38.214), approximated as the TBS that is greater or
  // equal to the payload size. This guarantees that the estimated num. of PRBs we obtained yields a TBS that is
  // greater or equal to the payload.
  float nof_info_estimate = static_cast<float>(*it);

  // Get N_re (as per Section 5.1.3.2, TS 38.214) from N_info.
  float nof_re = nof_info_estimate / (pdsch_cfg.target_code_rate * static_cast<float>(pdsch_cfg.mod_order) *
                                      static_cast<float>(pdsch_cfg.nof_layers) *
                                      tbs_calculator_pdsch_get_scaling_factor(pdsch_cfg.tb_scaling_field));

  // N_info_prime as per Section 5.1.3.2, TS 38.214.
  unsigned nof_re_prime = static_cast<unsigned>(srsgnb::NOF_SUBCARRIERS_PER_RB) * pdsch_cfg.nof_symb_sh -
                          pdsch_cfg.nof_dmrs_prb - pdsch_cfg.nof_oh_prb;

  // Get the estimated number of PRBs from the N_re and N_info_prime.
  unsigned nof_prbs_estimate =
      static_cast<unsigned>(std::ceil(nof_re / static_cast<float>(std::min(nof_re_prime, 156U))));

  // If the number of PRBs is 1, there is no point in trying to reduce this number.
  if (nof_prbs_estimate == 1) {
    return nof_prbs_estimate;
  }

  // Due to the way this procedure is derived (inversion of non-invertible functions), the estimated num of PRBs can be
  // 1 PRB greater than required to fit the payload size. Thus, we compute the TBS for "nof_prbs_estimate - 1"; if
  // TBS(nof_prbs_estimate - 1) is enough to accommodate the payload_size, then we return nof_prbs_estimate - 1.
  unsigned tbs = tbs_calculator_pdsch_calculate(tbs_calculator_pdsch_configuration{pdsch_cfg.nof_symb_sh,
                                                                                   pdsch_cfg.nof_dmrs_prb,
                                                                                   pdsch_cfg.nof_oh_prb,
                                                                                   pdsch_cfg.target_code_rate,
                                                                                   pdsch_cfg.mod_order,
                                                                                   pdsch_cfg.nof_layers,
                                                                                   pdsch_cfg.tb_scaling_field,
                                                                                   nof_prbs_estimate - 1});
  return tbs < payload_size ? nof_prbs_estimate : nof_prbs_estimate - 1;
}