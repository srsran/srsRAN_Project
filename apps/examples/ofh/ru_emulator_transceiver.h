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

#include "srsran/ofh/ethernet/dpdk/dpdk_ethernet_port_context.h"
#include "srsran/ofh/ethernet/ethernet_controller.h"
#include "srsran/ofh/ethernet/ethernet_frame_notifier.h"
#include "srsran/ofh/ethernet/ethernet_gateway.h"
#include "srsran/ofh/ethernet/ethernet_gateway_config.h"
#include "srsran/ofh/ethernet/ethernet_properties.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/ofh/ethernet/ethernet_receiver_metrics_collector.h"
#include "srsran/ofh/ethernet/ethernet_transmitter_metrics_collector.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// Encapsulates Ethernet transmitter and receiver functionalities used by the RU emulator.
class ru_emulator_transceiver
{
public:
  ru_emulator_transceiver(std::unique_ptr<ether::receiver> receiver_, std::unique_ptr<ether::gateway> transmitter_) :
    receiver(std::move(receiver_)), transmitter(std::move(transmitter_))
  {
    srsran_assert(receiver, "Invalid Ethernet receiver passed to RU emulator");
    srsran_assert(transmitter, "Invalid Ethernet transmitter passed to RU emulator");
  }

  /// Starts the Ethernet receiver operation.
  void start(ether::frame_notifier& notifier) { receiver->get_operation_controller().start(notifier); }

  /// Stops the Ethernet receiver operation.
  void stop() { receiver->get_operation_controller().stop(); }

  /// Sends the given list of frames through the underlying Ethernet link.
  void send(span<span<const uint8_t>> frames) { transmitter->send(frames); }

private:
  std::unique_ptr<ether::receiver> receiver;
  std::unique_ptr<ether::gateway>  transmitter;
};

/// Ethernet receiver implementation based on DPDK library.
class ru_emu_dpdk_receiver : public ether::receiver, public ether::receiver_operation_controller
{
  enum class status { idle, running, stop_requested, stopped };

public:
  ru_emu_dpdk_receiver(srslog::basic_logger&                     logger_,
                       task_executor&                            executor_,
                       std::shared_ptr<ether::dpdk_port_context> port_ctx_ptr_) :
    logger(logger_), executor(executor_), port_ctx_ptr(port_ctx_ptr_), port_ctx(*port_ctx_ptr)
  {
    srsran_assert(port_ctx_ptr, "Invalid port context");
  }

  // See interface for documentation.
  ether::receiver_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start(ether::frame_notifier& notifier_) override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  ether::receiver_metrics_collector* get_metrics_collector() override { return nullptr; }

protected:
  /// Main receiving loop.
  void receive_loop();

  /// Receives new Ethernet frames from the socket.
  void receive();

private:
  srslog::basic_logger&                     logger;
  task_executor&                            executor;
  std::shared_ptr<ether::dpdk_port_context> port_ctx_ptr;
  ether::dpdk_port_context&                 port_ctx;
  ether::frame_notifier*                    notifier;
  std::atomic<status>                       trx_status{status::idle};
};

/// Ethernet transmitter implementation based on DPDK library.
class ru_emu_dpdk_transmitter : public ether::gateway
{
public:
  ru_emu_dpdk_transmitter(srslog::basic_logger& logger_, std::shared_ptr<ether::dpdk_port_context> port_ctx_ptr_) :
    logger(logger_), port_ctx_ptr(port_ctx_ptr_), port_ctx(*port_ctx_ptr)
  {
    srsran_assert(port_ctx_ptr, "Invalid port context");
  }

  // See interface for documentation.
  void send(span<span<const uint8_t>> frames) override;

  // See interface for documentation.
  ether::transmitter_metrics_collector* get_metrics_collector() override { return nullptr; }

private:
  srslog::basic_logger&                     logger;
  std::shared_ptr<ether::dpdk_port_context> port_ctx_ptr;
  ether::dpdk_port_context&                 port_ctx;
};

/// Ethernet receiver implementation based on regular UNIX sockets.
class ru_emu_socket_receiver : public ether::receiver, public ether::receiver_operation_controller
{
public:
  ru_emu_socket_receiver(srslog::basic_logger& logger_, task_executor& executor_, const ether::gw_config& cfg);

  // See interface for documentation.
  ether::receiver_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  ether::receiver_metrics_collector* get_metrics_collector() override { return nullptr; }

  // See interface for documentation.
  void start(ether::frame_notifier& notifier_) override;

  // See interface for documentation.
  void stop() override;

private:
  std::unique_ptr<ether::receiver> receiver;
};

/// Ethernet transmitter implementation based on regular UNIX sockets.
class ru_emu_socket_transmitter : public ether::gateway
{
public:
  ru_emu_socket_transmitter(srslog::basic_logger& logger_, const ether::gw_config& cfg);

  // See interface for documentation.
  ether::transmitter_metrics_collector* get_metrics_collector() override { return nullptr; }

  // See interface for documentation.
  void send(span<span<const uint8_t>> frames) override;

private:
  std::unique_ptr<ether::gateway> transmitter;
};

/// Creates RU emulator transceiver based on DPDK library.
std::unique_ptr<ru_emulator_transceiver>
ru_emu_create_dpdk_transceiver(srslog::basic_logger&                     logger,
                               task_executor&                            executor,
                               std::shared_ptr<ether::dpdk_port_context> context);

/// Creates RU emulator transceiver based on regular UNIX sockets.
std::unique_ptr<ru_emulator_transceiver>
ru_emu_create_socket_transceiver(srslog::basic_logger& logger, task_executor& executor, const ether::gw_config& config);

} // namespace srsran
