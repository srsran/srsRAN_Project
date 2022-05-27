/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "prach_detector_impl.h"

using namespace srsgnb;

prach_detector_impl::prach_detector_impl(const prach_detector_factory::common_configuration& common_config) {}

prach_detector::detection_result prach_detector_impl::detect(span<const cf_t>                          signal,
                                                             const prach_detector::slot_configuration& config)
{
  return detection_result();
}

std::unique_ptr<prach_detector>
prach_detector_factory_impl::create_prach_detector(const common_configuration& common_config)
{
  return std::make_unique<prach_detector_impl>(common_config);
}
