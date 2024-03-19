/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/gtpu/n3_connection_factory.h"
#include "srsran/gateways/udp_network_gateway_factory.h"
#include "srsran/support/io/io_broker.h"

using namespace srsran;
using namespace srs_cu_up;

class udp_n3_connection final : public n3_connection
{
  // private ctor.
  udp_n3_connection(network_gateway_data_notifier_with_src_addr& data_notifier_, io_broker& io_brk_) :
    io_brk(io_brk_), data_notifier(data_notifier_), logger(srslog::fetch_basic_logger("GTPU"))
  {
  }

public:
  udp_n3_connection(udp_n3_connection&& other) noexcept                 = default;
  udp_n3_connection(const udp_n3_connection& other) noexcept            = delete;
  udp_n3_connection& operator=(const udp_n3_connection& other) noexcept = delete;
  udp_n3_connection& operator=(udp_n3_connection&& other) noexcept      = delete;
  ~udp_n3_connection() override
  {
    if (udp_gw != nullptr and udp_gw->get_socket_fd() >= 0) {
      // Deregister UDP gateway from IO broker.
      if (not io_brk.unregister_fd(udp_gw->get_socket_fd())) {
        logger.warning("Failed to stop NG-U gateway socket");
      }

      // Closes socket.
      udp_gw.reset();
    }
  }

  static std::unique_ptr<udp_n3_connection> create(const udp_network_gateway_config&            cfg,
                                                   network_gateway_data_notifier_with_src_addr& data_notifier,
                                                   io_broker&                                   io_brk,
                                                   task_executor&                               io_tx_executor)
  {
    std::unique_ptr<udp_n3_connection> conn(new udp_n3_connection(data_notifier, io_brk));

    // Create a new UDP network gateway instance.
    conn->udp_gw = create_udp_network_gateway(udp_network_gateway_creation_message{cfg, *conn, io_tx_executor});

    // Bind/open the gateway, start handling of incoming traffic from UPF, e.g. echo
    if (not conn->udp_gw->create_and_bind()) {
      conn->logger.error("Failed to create and connect N3 gateway");
    }
    bool success = io_brk.register_fd(conn->udp_gw->get_socket_fd(),
                                      [udp_gw_ptr = conn->udp_gw.get()](int fd) { udp_gw_ptr->receive(); });
    if (!success) {
      conn->logger.error("Failed to register N3 (GTP-U) network gateway at IO broker. socket_fd={}",
                         conn->udp_gw->get_socket_fd());
      return nullptr;
    }

    return conn;
  }

  void handle_pdu(byte_buffer pdu, const sockaddr_storage& dest_addr) override
  {
    // Forward PDU to UDP interface.
    udp_gw->handle_pdu(std::move(pdu), dest_addr);
  }

  void on_new_pdu(byte_buffer pdu, const sockaddr_storage& src_addr) override
  {
    // Forward PDU to data notifier.
    data_notifier.on_new_pdu(std::move(pdu), src_addr);
  }

  optional<uint16_t> get_n3_bind_port() override { return udp_gw->get_bind_port(); }

private:
  io_broker&                                   io_brk;
  network_gateway_data_notifier_with_src_addr& data_notifier;
  srslog::basic_logger&                        logger = srslog::fetch_basic_logger("CU-UP");

  std::unique_ptr<udp_network_gateway> udp_gw;
};

namespace {

class udp_n3_connection_factory : public n3_connection_factory
{
public:
  udp_n3_connection_factory(const udp_network_gateway_config& cfg_,
                            io_broker&                        io_brk_,
                            task_executor&                    io_tx_executor_) :
    cfg(cfg_), io_brk(io_brk_), io_tx_executor(io_tx_executor_)
  {
  }

  std::unique_ptr<n3_connection> create(network_gateway_data_notifier_with_src_addr& data_notifier) override
  {
    return udp_n3_connection::create(cfg, data_notifier, io_brk, io_tx_executor);
  }

private:
  const udp_network_gateway_config cfg;
  io_broker&                       io_brk;
  task_executor&                   io_tx_executor;
};

} // namespace

std::unique_ptr<n3_connection_factory>
srsran::srs_cu_up::create_udp_n3_connection_factory(const udp_network_gateway_config& config,
                                                    io_broker&                        io_brk,
                                                    task_executor&                    io_tx_executor)
{
  return std::make_unique<udp_n3_connection_factory>(config, io_brk, io_tx_executor);
}

/* ---- No UPF version ---- */

class no_upf_n3_connection final : public n3_connection
{
public:
  no_upf_n3_connection(local_upf_stub& upf_stub_, network_gateway_data_notifier_with_src_addr& data_notifier_) :
    upf_stub(upf_stub_), data_notifier(data_notifier_)
  {
    upf_stub.register_tx_pdu_handler(*this);
  }

  void handle_pdu(byte_buffer pdu, const sockaddr_storage& dest_addr) override
  {
    upf_stub.handle_pdu(std::move(pdu), dest_addr);
  }

  void on_new_pdu(byte_buffer pdu, const sockaddr_storage& src_addr) override
  {
    // Forward PDU to registered data notifier.
    data_notifier.on_new_pdu(std::move(pdu), src_addr);
  }

  optional<uint16_t> get_n3_bind_port() override { return nullopt; }

private:
  local_upf_stub&                              upf_stub;
  network_gateway_data_notifier_with_src_addr& data_notifier;
};

namespace {

class no_upf_n3_connection_factory : public n3_connection_factory
{
public:
  no_upf_n3_connection_factory(local_upf_stub& upf_stub_) : upf_stub(upf_stub_) {}

  std::unique_ptr<n3_connection> create(network_gateway_data_notifier_with_src_addr& data_notifier) override
  {
    return std::make_unique<no_upf_n3_connection>(upf_stub, data_notifier);
  }

private:
  local_upf_stub& upf_stub;
};

} // namespace

std::unique_ptr<n3_connection_factory> srsran::srs_cu_up::create_no_upf_n3_connection_factory(local_upf_stub& upf_stub)
{
  return std::make_unique<no_upf_n3_connection_factory>(upf_stub);
}
