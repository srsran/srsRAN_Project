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

#include "srsran/cu_cp/cu_cp.h"
#include "srsran/cu_cp/cu_cp_operation_controller.h"
#include "srsran/cu_cp/o_cu_cp.h"
#include "srsran/e2/e2.h"
#include <memory>

namespace srsran {
namespace srs_cu_cp {

/// O-RAN CU-CP implementation.
class o_cu_cp_impl : public o_cu_cp, public cu_cp_operation_controller
{
public:
  explicit o_cu_cp_impl(std::unique_ptr<cu_cp> cu_cp_);

  // See interface for documentation.
  cu_cp& get_cu_cp() override { return *cu_impl; }

  // See interface for documentation.
  cu_cp_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  std::unique_ptr<cu_cp> cu_impl;
};

/// O-RAN CU-CP with E2 implementation.
class o_cu_cp_with_e2_impl : public o_cu_cp_impl
{
public:
  o_cu_cp_with_e2_impl(std::unique_ptr<e2_agent> e2agent_, std::unique_ptr<cu_cp> cu_cp_);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  std::unique_ptr<e2_agent> e2agent;
};

} // namespace srs_cu_cp
} // namespace srsran
