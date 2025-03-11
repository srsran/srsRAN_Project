/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
