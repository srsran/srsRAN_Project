/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_up/cu_up.h"
#include "srsran/gtpu/ngu_gateway.h"

namespace srsran {

/// \brief CU-UP wrapper. Wraps a CU-UP interface and a NGU gateway.
///
/// The wrapper purpose is to manage the life of the given NGU gateway and the CU-UP interface.
class cu_up_wrapper : public srs_cu_up::cu_up_interface
{
public:
  cu_up_wrapper(std::unique_ptr<srs_cu_up::ngu_gateway> gateway_, std::unique_ptr<srs_cu_up::cu_up_interface> cu_up_);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  std::optional<uint16_t> get_n3_bind_port() override;

private:
  std::unique_ptr<srs_cu_up::ngu_gateway>     gateway;
  std::unique_ptr<srs_cu_up::cu_up_interface> cu_up;
};

} // namespace srsran
