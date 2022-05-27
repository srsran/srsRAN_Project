/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_PHY_UPPER_CHANNEL_PROCESSORS_PRACH_DETECTOR_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_CHANNEL_PROCESSORS_PRACH_DETECTOR_IMPL_H

#include "srsgnb/phy/upper/channel_processors/prach_detector.h"

namespace srsgnb {

class prach_detector_impl : public prach_detector
{
  /// \brief Sequence generation as per TS38.211 Section 6.3.3.1.
  void sequence_generation();

  /// \brief Mapping to physical resources as per TS38.211 Section 6.3.3.2.
  void map_to_physical_resources();

public:

  prach_detector_impl(const prach_detector_factory::common_configuration &common_config);

  // See interface for documentation.
  detection_result detect(span<const cf_t> signal, const slot_configuration& config) override;
};

class prach_detector_factory_impl : public prach_detector_factory
{
public:
  std::unique_ptr<prach_detector> create_prach_detector(const common_configuration& common_config) override;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_CHANNEL_PROCESSORS_PRACH_DETECTOR_IMPL_H
