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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/byte_buffer_slice_chain.h"
#include "srsgnb/pdcp/pdcp_config.h"
#include "srsgnb/security/security.h"
#include "srsgnb/srslog/srslog.h"

/*
 * This file will hold the interfaces and notifiers for the PDCP entity.
 * They follow the following nomenclature:
 *
 *   pdcp_{tx/rx}_{lower/upper}_{[control/data]}_{interface/notifier}
 *
 * 1. TX/RX indicates whether the interface is intended for the
 *    TX or RX side of the entity
 * 2. Lower/Upper indicates whether the interface/notifier interacts
 *    with the upper or lower layers.
 * 3. Control/Data: indicates whether this interface is necessary for "control"
 *    purposes (e.g., notifying the RRC of an integrity failure, or that we are
 *    near max HFN) or "data" purposes (e.g. handling SDUs).
 *    This distinction is only necessary when interfacing with the upper layers,
 *    and as such, we omit it in the interfaces with the lower layers.
 * 4. Interface/Notifier: whether this is an interface the PDCP entity will
 *    inherit or if a notifier that the PDCP will keep as a member.
 *
 */
namespace srsgnb {

/// This interface represents the data entry point of the receiving side of a PDCP entity.
/// The lower-layers will use this class to pass PDUs into the PDCP.
class pdcp_rx_lower_interface
{
public:
  pdcp_rx_lower_interface()                                           = default;
  virtual ~pdcp_rx_lower_interface()                                  = default;
  pdcp_rx_lower_interface(const pdcp_rx_lower_interface&)             = delete;
  pdcp_rx_lower_interface& operator=(const pdcp_rx_lower_interface&)  = delete;
  pdcp_rx_lower_interface(const pdcp_rx_lower_interface&&)            = delete;
  pdcp_rx_lower_interface& operator=(const pdcp_rx_lower_interface&&) = delete;

  virtual void handle_pdu(byte_buffer_slice_chain pdu) = 0; ///< Handle the incoming PDU.
};

/// This interface represents the data exit point of the receiving side of a PDCP entity.
/// The PDCP will use this class to pass SDUs to the upper-layers.
class pdcp_rx_upper_data_notifier
{
public:
  pdcp_rx_upper_data_notifier()                                               = default;
  virtual ~pdcp_rx_upper_data_notifier()                                      = default;
  pdcp_rx_upper_data_notifier(const pdcp_rx_upper_data_notifier&)             = delete;
  pdcp_rx_upper_data_notifier& operator=(const pdcp_rx_upper_data_notifier&)  = delete;
  pdcp_rx_upper_data_notifier(const pdcp_rx_upper_data_notifier&&)            = delete;
  pdcp_rx_upper_data_notifier& operator=(const pdcp_rx_upper_data_notifier&&) = delete;

  /// Pass SDU to higher layers.
  virtual void on_new_sdu(byte_buffer sdu) = 0;
};

/// This interface represents the control upper layer that the
/// RX PDCP bearer must notify in case of integrity errors or protocol failures.
class pdcp_rx_upper_control_notifier
{
public:
  pdcp_rx_upper_control_notifier()                                                  = default;
  virtual ~pdcp_rx_upper_control_notifier()                                         = default;
  pdcp_rx_upper_control_notifier(const pdcp_rx_upper_control_notifier&)             = delete;
  pdcp_rx_upper_control_notifier& operator=(const pdcp_rx_upper_control_notifier&)  = delete;
  pdcp_rx_upper_control_notifier(const pdcp_rx_upper_control_notifier&&)            = delete;
  pdcp_rx_upper_control_notifier& operator=(const pdcp_rx_upper_control_notifier&&) = delete;

  virtual void on_protocol_failure()  = 0;
  virtual void on_integrity_failure() = 0;
};

/// This interface represents the control SAP of the receiving side of a PDCP entity.
/// The RRC will use this interface to configure security keys and enable/disable
/// integrity and ciphering.
class pdcp_rx_upper_control_interface
{
public:
  pdcp_rx_upper_control_interface()                                                   = default;
  virtual ~pdcp_rx_upper_control_interface()                                          = default;
  pdcp_rx_upper_control_interface(const pdcp_rx_upper_control_interface&)             = delete;
  pdcp_rx_upper_control_interface& operator=(const pdcp_rx_upper_control_interface&)  = delete;
  pdcp_rx_upper_control_interface(const pdcp_rx_upper_control_interface&&)            = delete;
  pdcp_rx_upper_control_interface& operator=(const pdcp_rx_upper_control_interface&&) = delete;

  /// Handle the incoming SDU.
  virtual void set_as_security_config(sec_128_as_config sec_cfg)                                       = 0;
  virtual void enable_or_disable_security(pdcp_integrity_enabled integ, pdcp_ciphering_enabled cipher) = 0;
};

/*
 * PDCP RX metrics interface and structures.
 */
/// This struct will hold relevant metrics for the PDCP RX
struct pdcp_rx_metrics_container {
  uint32_t num_pdus;
  uint32_t num_pdu_bytes;
  uint32_t num_dropped_pdus;
  uint32_t num_sdus;
  uint32_t num_sdu_bytes;
  uint32_t integrity_failures;
  uint32_t nof_t_reordering_timeouts;
};

/// This interface will allow the relevant entities to query PDCP RX
/// metrics (and possibly reset them).
class pdcp_rx_metrics_interface
{
public:
  pdcp_rx_metrics_interface()                                                   = default;
  virtual ~pdcp_rx_metrics_interface()                                          = default;
  pdcp_rx_metrics_interface(const pdcp_rx_upper_control_interface&)             = delete;
  pdcp_rx_metrics_interface& operator=(const pdcp_rx_upper_control_interface&)  = delete;
  pdcp_rx_metrics_interface(const pdcp_rx_upper_control_interface&&)            = delete;
  pdcp_rx_metrics_interface& operator=(const pdcp_rx_upper_control_interface&&) = delete;

  /// Metrics interfaces.
  virtual pdcp_rx_metrics_container get_metrics()           = 0;
  virtual pdcp_rx_metrics_container get_metrics_and_reset() = 0;
  virtual void                      reset_metrics()         = 0;
};

} // namespace srsgnb

namespace fmt {
// PDCP RX metrics formatter
template <>
struct formatter<srsgnb::pdcp_rx_metrics_container> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::pdcp_rx_metrics_container m, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "num_sdus={}, num_sdu_bytes={}, num_dropped_pdus={}, num_pdus={}, num_pdu_bytes={}",
                     m.num_sdus,
                     m.num_sdu_bytes,
                     m.num_dropped_pdus,
                     m.num_pdus,
                     m.num_pdu_bytes);
  }
};
} // namespace fmt

