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

#include "../support/uplink_context_repo.h"

namespace srsran {
namespace ofh {

struct uplane_message_decoder_results;

/// \brief Open Fronthaul User-Plane uplink received symbol data flow writer.
///
/// Writes IQ data received in an Open Fronthaul message to the corresponding resource grid.
class uplane_rx_symbol_data_flow_writer
{
public:
  uplane_rx_symbol_data_flow_writer(span<const unsigned>                 ul_eaxc_,
                                    srslog::basic_logger&                logger_,
                                    std::shared_ptr<uplink_context_repo> ul_context_repo_) :
    ul_eaxc(ul_eaxc_.begin(), ul_eaxc_.end()),
    logger(logger_),
    ul_context_repo_ptr(ul_context_repo_),
    ul_context_repo(*ul_context_repo_ptr)
  {
    srsran_assert(!ul_eaxc.empty(), "Invalid number of uplink eAxCs");
    srsran_assert(ul_context_repo_ptr, "Invalid uplink context repository");
  }

  /// Writes the given decoder results in the corresponding resource grid using the given eAxC.
  void write_to_resource_grid(unsigned eaxc, const uplane_message_decoder_results& results);

private:
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
  srslog::basic_logger&                                 logger;
  std::shared_ptr<uplink_context_repo>                  ul_context_repo_ptr;
  uplink_context_repo&                                  ul_context_repo;
};

} // namespace ofh
} // namespace srsran
