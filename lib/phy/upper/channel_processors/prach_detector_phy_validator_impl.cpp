/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Definition of the validator of PRACH configurations at the PHY layer.
///
/// There is a huge number of possible PRACH configurations and the detector has not been fine-tuned for all of them.
/// The validator will pass if the configuration allows the detector to meet the requirements, will issue a warning if
/// the configuration will cause a slight degradation in the expected performance, and will issue an error if the
/// expected performance is too far from the requested one.

#include "prach_detector_generic_thresholds.h"
#include "srsran/phy/upper/channel_processors/prach_detector_phy_validator.h"

using namespace srsran;

error_type<std::string> srsran::validate_prach_detector_phy(prach_format_type        format,
                                                            prach_subcarrier_spacing scs,
                                                            unsigned                 zero_correlation_zone,
                                                            unsigned                 nof_rx_ports)
{
  detail::threshold_params th_params;
  th_params.nof_rx_ports          = nof_rx_ports;
  th_params.scs                   = scs;
  th_params.format                = format;
  th_params.zero_correlation_zone = zero_correlation_zone;
  th_params.combine_symbols       = true;

  auto flag = detail::get_threshold_flag(th_params);

  if (flag == detail::threshold_flag::red) {
    fmt::print("\nThe PRACH detector does not support the configuration {{Format {}, ZCZ {}, SCS {}, Rx ports {}}}.\n",
               to_string(format),
               zero_correlation_zone,
               to_string(scs),
               nof_rx_ports);
    return make_unexpected(fmt::format(
        "The PRACH detector does not support the configuration {{Format {}, ZCZ {}, SCS {}, Rx ports {}}}.\n",
        to_string(format),
        zero_correlation_zone,
        to_string(scs),
        nof_rx_ports));
  }

  return default_success_t();
}
