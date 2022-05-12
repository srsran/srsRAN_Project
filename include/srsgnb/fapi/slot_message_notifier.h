/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_FAPI_SLOT_MESSAGE_NOTIFIER_H
#define SRSGNB_FAPI_SLOT_MESSAGE_NOTIFIER_H

namespace srsgnb {
namespace fapi {

struct crc_indication_message;
struct dl_tti_response_message;
struct rach_indication_message;
struct rx_data_indication_message;
struct slot_indication_message;
struct srs_indication_message;
struct uci_indication_message;

/// This interface notifies the reception of FAPI slot based messages from the underlying PHY.
class slot_message_notifier
{
public:
  virtual ~slot_message_notifier() = default;

  /// \brief Callback to notify the reception of a downlink TTI response message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void on_dl_tti_response(const dl_tti_response_message& msg) = 0;

  /// \brief Callback to notify the reception of a slot indication message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void on_slot_indication(const slot_indication_message& msg) = 0;

  /// \brief Callback to notify the reception of a RX data indication message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void on_rx_data_indication(const rx_data_indication_message& msg) = 0;

  /// \brief Callback to notify the reception of a CRC indication message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void on_crc_indication(const crc_indication_message& msg) = 0;

  /// \brief Callback to notify the reception of a UCI indication message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void on_uci_indication(const uci_indication_message& msg) = 0;

  /// \brief Callback to notify the reception of a SRS indication message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void on_srs_indication(const srs_indication_message& msg) = 0;

  /// \brief Callback to notify the reception of a RACH indication message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void on_rach_indication(const rach_indication_message& msg) = 0;
};

} // namespace fapi
} // namespace srsgnb

#endif // SRSGNB_FAPI_SLOT_MESSAGE_NOTIFIER_H
