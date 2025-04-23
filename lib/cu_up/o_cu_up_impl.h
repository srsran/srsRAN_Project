/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include "srsran/cu_up/cu_up.h"
#include "srsran/cu_up/cu_up_operation_controller.h"
#include "srsran/cu_up/o_cu_up.h"
#include "srsran/e2/e2.h"
#include <memory>

namespace srsran {
namespace srs_cu_up {

/// O-RAN CU-UP implementation.
class o_cu_up_impl : public o_cu_up, public cu_up_operation_controller
{
public:
  explicit o_cu_up_impl(std::unique_ptr<cu_up_interface> cu_up_) : cu_up(std::move(cu_up_))
  {
    srsran_assert(cu_up, "Invalid CU-UP");
  }

  // See interface for documentation.
  cu_up_interface& get_cu_up() override { return *cu_up; }

  // See interface for documentation.
  cu_up_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  std::unique_ptr<cu_up_interface> cu_up;
};

/// O-RAN CU-UP implementation with E2.
class o_cu_up_with_e2_impl : public o_cu_up_impl
{
public:
  o_cu_up_with_e2_impl(std::unique_ptr<cu_up_interface> cu_up_, std::unique_ptr<e2_agent> e2agent_) :
    o_cu_up_impl(std::move(cu_up_)), e2agent(std::move(e2agent_))
  {
    srsran_assert(e2agent, "Invalid E2 agent");
  }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  std::unique_ptr<e2_agent> e2agent;
};

} // namespace srs_cu_up
} // namespace srsran
