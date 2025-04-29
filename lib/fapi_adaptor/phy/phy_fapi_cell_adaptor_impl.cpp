/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "phy_fapi_cell_adaptor_impl.h"
#include "phy_fapi_cell_config_validator.h"
#include "srsran/fapi/config_message_gateway.h"
#include "srsran/fapi/config_message_notifier.h"
#include "srsran/fapi/messages/config_messages.h"
#include "srsran/fapi/messages/error_indication.h"
#include "srsran/fapi/slot_error_message_notifier.h"

using namespace srsran;
using namespace fapi_adaptor;

namespace {

/// FAPI configuration message notifier dummy implementation.
class config_message_notifier_dummy : public fapi::config_message_notifier
{
public:
  void on_param_response(const fapi::param_response& msg) override
  {
    report_error("Dummy FAPI config message notifier cannot notify on PARAM.response");
  }

  void on_config_response(const fapi::config_response& msg) override
  {
    report_error("Dummy FAPI config message notifier cannot notify on CONFIG.response");
  }

  void on_stop_indication(const fapi::stop_indication& msg) override
  {
    report_error("Dummy FAPI config message notifier cannot notify on STOP.indication");
  }
};

class slot_error_message_notifier_dummy : public fapi::slot_error_message_notifier
{
public:
  void on_error_indication(const fapi::error_indication_message& msg) override
  {
    report_error("Dummy FAPI ERROR.indication notifier cannot notify errors");
  }
};
} // namespace

static config_message_notifier_dummy     dummy_msg_notifier;
static slot_error_message_notifier_dummy dummy_error_msg_notifier;

class phy_fapi_cell_adaptor_impl::phy_fapi_config_message_adaptor_impl : public phy_fapi_config_message_adaptor,
                                                                         public fapi::config_message_gateway
{
  enum class cell_status : uint8_t { IDLE, CONFIGURED, RUNNING };

  srslog::basic_logger&              logger;
  phy_fapi_cell_adaptor_impl&        parent;
  fapi_cell_config&                  cell_config;
  fapi::config_message_notifier*     notifier       = nullptr;
  fapi::slot_error_message_notifier* error_notifier = nullptr;
  cell_status                        status         = cell_status::IDLE;

public:
  phy_fapi_config_message_adaptor(phy_fapi_cell_adaptor_impl& parent_,
                                  fapi_cell_config&           cell_config_,
                                  srslog::basic_logger&       logger_) :
    logger(logger_),
    parent(parent_),
    cell_config(cell_config_),
    notifier(&dummy_msg_notifier),
    error_notifier(&dummy_error_msg_notifier)
  {
  }

  // See interface for documentation.
  fapi::config_message_gateway& get_config_message_gateway() override { return *this; }

  // See interface for documentation.
  void set_config_message_notifier(fapi::config_message_notifier& fapi_notifier) override { notifier = &fapi_notifier; }

  /// FAPI config message gateway methods.
  // See interface for documentation.
  void param_request(const fapi::param_request& msg) override
  {
    // Do nothing for the PARAM.response.
    fapi::param_response reponse;
    reponse.num_tlv = 0;

    // If current status is running, report back error code, as perCF-222 v4.0 section 3.3.1.3.
    reponse.error_code =
        (status == cell_status::RUNNING) ? fapi::error_code_id::msg_invalid_state : fapi::error_code_id::msg_ok;

    notifier->on_param_response(reponse);
  }

  // See interface for documentation.
  void config_request(const fapi::config_request& msg) override
  {
    cell_status current_status = status;

    bool config_updated = false;

    // [Implementation defined] Do not allow configure the cell in running state.
    if (current_status != cell_status::RUNNING) {
      config_updated = update_cell_config(msg);
    }

    fapi::config_response reponse;
    // :TODO: double check these counters, as if the message is marked as invalid config, I expect one of the counters
    // to increase.
    reponse.num_invalid_tlvs_can_conf_idle    = 0;
    reponse.num_invalid_tlvs_can_conf_running = 0;
    reponse.num_invalid_unsupported_tlvs      = 0;
    reponse.num_missing_tlvs                  = 0;
    reponse.error_code = config_updated ? fapi::error_code_id::msg_ok : fapi::error_code_id::msg_invalid_config;

    // Move cell to configured status if current status is idle and the configuration update was successful.
    if ((current_status == cell_status::IDLE) && config_updated) {
      status = cell_status::CONFIGURED;
    }

    notifier->on_config_response(reponse);
  }

  // See interface for documentation.
  void start_request(const fapi::start_request& msg) override
  {
    cell_status current_status = status;

    // Not expected status, notify error.
    if (current_status != cell_status::CONFIGURED) {
      fapi::error_indication_message indication;
      indication.error_code = fapi::error_code_id::msg_invalid_state;
      error_notifier->on_error_indication(indication);

      return;
    }

    // Status is configured. Create DU low and set the status to start.
    parent.start();

    // Set the cell status to running.
    status.store(cell_status::RUNNING, std::memory_order_relaxed);
  }

  // See interface for documentation.
  void stop_request(const fapi::stop_request& msg) override
  {
    cell_status current_status = status;

    // Not expected status, notify error.
    if (current_status != cell_status::RUNNING) {
      fapi::error_indication_message indication;
      indication.error_code = fapi::error_code_id::msg_invalid_state;
      error_notifier->on_error_indication(indication);

      return;
    }

    // Status is configured. Create DU low and set the status to start.
    parent.stop();

    // Set the cell status to configured.
    status = cell_status::CONFIGURED;

    fapi::stop_indication indication;
    notifier->on_stop_indication(indication);
  }

private:
  /// Updates the cell config using the given FAPI CONFIG.request message and returns true if the config was
  /// successfully updated, otherwise false.
  bool update_cell_config(const fapi::config_request& msg)
  {
    // Validate the config request message contents before updating.
    if (!validate_config_request_message(msg, logger)) {
      return false;
    }

    // Update the configuration.
    cell_config.carrier_cfg = msg.carrier_cfg;
    cell_config.cell_cfg    = msg.cell_cfg;
    cell_config.phy_cfg     = msg.phy_cfg;
    cell_config.prach_cfg   = msg.prach_cfg;
    cell_config.ssb_cfg     = msg.ssb_cfg;
    cell_config.tdd_cfg     = msg.tdd_cfg;

    return true;
  }
};

phy_fapi_cell_adaptor_impl::phy_fapi_config_message_adaptor_impl() :
  config_adaptor(
      std::make_unique<phy_fapi_cell_adaptor_impl::phy_fapi_config_message_adaptor_impl>(*this, cell_config, logger))
{
}

phy_fapi_config_message_adaptor& phy_fapi_cell_adaptor_impl::get_config_message_adaptor()
{
  return *config_adaptor;
}
