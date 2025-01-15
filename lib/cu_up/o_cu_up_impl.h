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
