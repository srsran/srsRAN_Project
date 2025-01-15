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

namespace srsran {
namespace ofh {

class controller;
class error_notifier;
class receiver;
class transmitter;

/// \brief Open Fronthaul sector interface.
///
/// A sector represents the minimum entity to send and receive data from the Open Fronthaul.
class sector
{
public:
  virtual ~sector() = default;

  /// Returns the Open Fronthaul receiver of this sector.
  virtual receiver& get_receiver() = 0;

  /// Returns the Open Fronthaul transmitter of this sector.
  virtual transmitter& get_transmitter() = 0;

  /// Returns the Open Fronthaul controller of this sector.
  virtual controller& get_controller() = 0;

  /// Sets the error notifier of this sector to the given one.
  virtual void set_error_notifier(error_notifier& notifier) = 0;
};

} // namespace ofh
} // namespace srsran
