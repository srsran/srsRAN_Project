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

pdsch_prbs_tbs srsgnb::get_nof_prbs(const prbs_calculator_pdsch_config& pdsch_cfg)
{
  // Convert size into bits, as per TS procedures for TBS.
  unsigned payload_size = pdsch_cfg.payload_size_bytes * 8;

  const unsigned max_payload_size_bits = 3824;

  // This function is limited to payload_size <= 3824 bits.
  payload_size = std::min(max_payload_size_bits, payload_size);

  // This is an estimate of the N_info (as per Section 5.1.3.2, TS 38.214), approximated as the TBS that is greater or
  // equal to the payload size. This guarantees that the estimated num. of PRBs we obtained yields a TBS that is
  // greater or equal to the payload.
  float nof_info_estimate = static_cast<float>(tbs_calculator_table_find_smallest_not_less_than(payload_size));

  unsigned nof_bit_per_symbol = get_bits_per_symbol(pdsch_cfg.mcs_descr.modulation);

  // Get N_re (as per Section 5.1.3.2, TS 38.214) from N_info.
  float nof_re =
      nof_info_estimate /
      (pdsch_cfg.mcs_descr.get_normalised_target_code_rate() * static_cast<float>(nof_bit_per_symbol) *
       static_cast<float>(pdsch_cfg.nof_layers) * tbs_calculator_pdsch_get_scaling_factor(pdsch_cfg.tb_scaling_field));

  // N_info_prime as per Section 5.1.3.2, TS 38.214.
  unsigned nof_re_prime = static_cast<unsigned>(srsgnb::NOF_SUBCARRIERS_PER_RB) * pdsch_cfg.nof_symb_sh -
                          pdsch_cfg.nof_dmrs_prb - pdsch_cfg.nof_oh_prb;

  // Get the estimated number of PRBs from the N_re and N_info_prime.
  unsigned nof_prbs_estimate =
      static_cast<unsigned>(std::ceil(nof_re / static_cast<float>(std::min(nof_re_prime, 156U))));

  // Due to the way this procedure is derived (inversion of non-invertible functions), the estimated num of PRBs can be
  // 1 PRB greater than required to fit the payload size.
  // Thus, we compute the TBS for "nof_prbs_estimate - 1"; if TBS(nof_prbs_estimate - 1) is enough to accommodate the
  // payload_size, then we return nof_prbs_estimate - 1.
  unsigned tbs_bits_lb = tbs_calculator_calculate(tbs_calculator_configuration{pdsch_cfg.nof_symb_sh,
                                                                               pdsch_cfg.nof_dmrs_prb,
                                                                               pdsch_cfg.nof_oh_prb,
                                                                               pdsch_cfg.mcs_descr,
                                                                               pdsch_cfg.nof_layers,
                                                                               pdsch_cfg.tb_scaling_field,
                                                                               nof_prbs_estimate - 1});
  // TBS(nof_prbs_estimate-1) is big enough for payload_size; return the couple
  // {nof_prbs_estimate -1, TBS(nof_prbs_estimate-1)}
  if (tbs_bits_lb >= payload_size) {
    return {nof_prbs_estimate - 1, tbs_bits_lb / 8};
  }

  // TBS(nof_prbs_estimate-1) NOT big enough for payload_size; recompute the TBS for nof_prbs_estimate.
  unsigned tbs_bits_ub = tbs_calculator_calculate(tbs_calculator_configuration{pdsch_cfg.nof_symb_sh,
                                                                               pdsch_cfg.nof_dmrs_prb,
                                                                               pdsch_cfg.nof_oh_prb,
                                                                               pdsch_cfg.mcs_descr,
                                                                               pdsch_cfg.nof_layers,
                                                                               pdsch_cfg.tb_scaling_field,
                                                                               nof_prbs_estimate});

  // Handle an edge-case. It could happen that the TBS corresponding to the min. number of PRBs to fit the payload
  // exceeds the maximum value of 3824 bits or 478 bytes. This means that the function is working outside its validity
  // range. In this case, it returns the result {nof_prbs_estimate -1, TBS(nof_prbs_estimate-1)}, which won't be enough
  // to accommodate the payload. It's up the caller to handle this case.
  pdsch_prbs_tbs prbs_lbs_lb = {nof_prbs_estimate - 1, tbs_bits_lb / 8};
  pdsch_prbs_tbs prbs_lbs_up = {nof_prbs_estimate, tbs_bits_ub / 8};
  return tbs_bits_ub > max_payload_size_bits ? prbs_lbs_lb : prbs_lbs_up;
}
