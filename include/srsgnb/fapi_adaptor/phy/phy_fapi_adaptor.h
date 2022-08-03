/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsgnb {

namespace fapi {
class slot_message_gateway;
class slot_time_message_notifier;
} // namespace fapi

class upper_phy_timing_notifier;

namespace fapi_adaptor {

/// \brief Interface to the PHY side of the FAPI adaptor object.
///
/// This interface gives access to the interfaces needed to interconnect the adaptor with the PHY layer.
///
/// \note This object has the ownership of all the components of the adaptor.
class phy_fapi_adaptor
{
public:
  virtual ~phy_fapi_adaptor() = default;

  /// Returns the adaptor's upper PHY timing notifier.
  virtual upper_phy_timing_notifier& get_upper_phy_timing_notifier() = 0;

  /// Returns the adaptor's slot message gateway.
  virtual fapi::slot_message_gateway& get_slot_message_gateway() = 0;

  /// \brief Configures the adaptor's FAPI slot message notifier to the given one.
  ///
  /// \param fapi_time_notifier FAPI slot message notifier to set in the adaptor.
  virtual void set_fapi_slot_time_message_notifier(fapi::slot_time_message_notifier& fapi_time_notifier) = 0;
};

} // namespace fapi_adaptor
} // namespace srsgnb
