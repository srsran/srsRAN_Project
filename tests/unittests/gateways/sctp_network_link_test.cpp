/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/adt/mutexed_mpmc_queue.h"
#include "srsran/gateways/sctp_network_client_factory.h"
#include "srsran/gateways/sctp_network_server_factory.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/io/io_broker_factory.h"
#include <condition_variable>
#include <gtest/gtest.h>
#include <mutex>

using namespace srsran;

class base_sctp_network_link_test
{
public:
  base_sctp_network_link_test(unsigned nof_clients) :
    logger([]() -> srslog::basic_logger& {
      srslog::init();
      return srslog::fetch_basic_logger("SCTP-GW");
    }()),
    server_broker(create_io_broker(srsran::io_broker_type::epoll)),
    client_broker(create_io_broker(srsran::io_broker_type::epoll)),
    assoc_factory(std::make_unique<server_assoc_handler_factory>(*this)),
    server_cfg([this]() {
      sctp_network_server_config cfg{{}, *server_broker, *assoc_factory};
      cfg.sctp.if_name      = "SERVER";
      cfg.sctp.ppid         = NGAP_PPID;
      cfg.sctp.bind_address = "127.0.0.1";
      cfg.sctp.bind_port    = 0;
      return cfg;
    }()),
    server(create_sctp_network_server(server_cfg))
  {
    report_fatal_error_if_not(server_broker != nullptr, "Failed to create IO broker");
    report_fatal_error_if_not(client_broker != nullptr, "Failed to create IO broker");
    report_fatal_error_if_not(server != nullptr, "Failed to create Server");

    report_fatal_error_if_not(server->listen(), "Failed to listen to connections");

    for (unsigned i = 0; i < nof_clients; i++) {
      // Create context instance.
      auto ret = client_associations.insert(std::make_pair(i, std::make_unique<sctp_client_association_context>()));
      report_fatal_error_if_not(ret.second, "Failed to insert Client Association");

      // Create client.
      sctp_network_client_config client_cfg{{}, *client_broker};
      client_cfg.sctp.if_name   = fmt::format("client{}", i);
      client_cfg.sctp.ppid      = NGAP_PPID;
      ret.first->second->client = create_sctp_network_client(client_cfg);
      report_fatal_error_if_not(ret.first->second->client != nullptr, "Failed to create Client");
    }

    // Connect Clients.
    for (auto& assoc : client_associations) {
      int server_port             = server->get_listen_port().value();
      assoc.second->client_sender = assoc.second->client->connect_to(
          "server", server_cfg.sctp.bind_address, server_port, create_client_receiver(assoc.first));
      report_fatal_error_if_not(assoc.second->client_sender != nullptr, "Failed to connect Client");
    }

    // Wait for associations to be made to the server.
    std::unique_lock<std::mutex> lock(assoc_creation_mutex);
    assoc_created_cvar.wait(lock, [this, nof_clients]() { return server_associations.size() == nof_clients; });

    logger.info("All UEs connected");
  }

protected:
  struct server_assoc_handler_factory : public sctp_network_association_factory {
    server_assoc_handler_factory(base_sctp_network_link_test& parent_) : parent(parent_) {}

    std::unique_ptr<sctp_association_sdu_notifier>
    create(std::unique_ptr<sctp_association_sdu_notifier> sctp_send_notifier) override
    {
      // Note: Called from within io_broker execution context.

      class server_recv_notifier : public sctp_association_sdu_notifier
      {
      public:
        server_recv_notifier(sctp_server_association_context& parent_) : parent(parent_) {}

        bool on_new_sdu(byte_buffer sdu) override { return parent.recv_data.try_push(std::move(sdu)); }

      private:
        sctp_server_association_context& parent;
      };

      // Create new SCTP server association test context.
      auto assoc_ctxt           = std::make_unique<sctp_server_association_context>();
      assoc_ctxt->server_sender = std::move(sctp_send_notifier);
      auto receiver             = std::make_unique<server_recv_notifier>(*assoc_ctxt);

      {
        std::lock_guard<std::mutex> lock(parent.assoc_creation_mutex);
        auto ret = parent.server_associations.insert(std::make_pair(next_server_assoc_key++, std::move(assoc_ctxt)));
        report_fatal_error_if_not(ret.second, "Failed to create SCTP association handler");
        parent.assoc_created_cvar.notify_one();
      }

      return receiver;
    }

  private:
    base_sctp_network_link_test& parent;
    unsigned                     next_server_assoc_key = 0;
  };

  struct sctp_client_association_context {
    // Data the client received.
    concurrent_queue<byte_buffer,
                     concurrent_queue_policy::locking_mpmc,
                     concurrent_queue_wait_policy::condition_variable>
        recv_data{1024};

    std::unique_ptr<sctp_network_client>           client;
    std::unique_ptr<sctp_association_sdu_notifier> client_sender;
  };

  struct sctp_server_association_context {
    // Data the server association received.
    concurrent_queue<byte_buffer,
                     concurrent_queue_policy::locking_mpmc,
                     concurrent_queue_wait_policy::condition_variable>
        recv_data{1024};

    std::unique_ptr<sctp_association_sdu_notifier> server_sender;
  };

  std::unique_ptr<sctp_association_sdu_notifier> create_client_receiver(unsigned key)
  {
    class client_recv_notifier : public sctp_association_sdu_notifier
    {
    public:
      client_recv_notifier(sctp_client_association_context& parent_) : parent(parent_) {}

      bool on_new_sdu(byte_buffer sdu) override { return parent.recv_data.try_push(std::move(sdu)); }

    private:
      sctp_client_association_context& parent;
    };

    return std::make_unique<client_recv_notifier>(*client_associations.at(key));
  }

  srslog::basic_logger&                             logger;
  std::unique_ptr<io_broker>                        server_broker;
  std::unique_ptr<io_broker>                        client_broker;
  std::unique_ptr<sctp_network_association_factory> assoc_factory;
  sctp_network_server_config                        server_cfg;

  std::unique_ptr<sctp_network_server> server;

  std::map<unsigned, std::unique_ptr<sctp_server_association_context>> server_associations;
  std::map<unsigned, std::unique_ptr<sctp_client_association_context>> client_associations;

  std::mutex              assoc_creation_mutex;
  std::condition_variable assoc_created_cvar;
};

namespace {

struct test_params {
  unsigned nof_clients = 1;
};

void PrintTo(const test_params& value, ::std::ostream* os)
{
  *os << fmt::format("nof_clients={}", value.nof_clients);
}

} // namespace

class sctp_network_link_test : public base_sctp_network_link_test, public ::testing::TestWithParam<test_params>
{
public:
  sctp_network_link_test() : base_sctp_network_link_test(GetParam().nof_clients) {}
  ~sctp_network_link_test() { srslog::flush(); }
};

static byte_buffer create_data(unsigned start_val, unsigned nof_vals)
{
  std::vector<uint8_t> vec(nof_vals);
  for (unsigned i = 0; i != vec.size(); ++i) {
    vec[i] = start_val + i;
  }
  return byte_buffer::create(vec).value();
}

TEST_P(sctp_network_link_test, multi_client_recv_data)
{
  unsigned pdu_len = 10;

  // Send data from each server association sender.
  unsigned i = 0;
  for (auto& assoc : server_associations) {
    byte_buffer pdu = create_data(i * pdu_len, pdu_len);
    ASSERT_TRUE(assoc.second->server_sender->on_new_sdu(std::move(pdu)));
  }

  // Check data received by client.
  for (auto& assoc : client_associations) {
    byte_buffer pdu;
    ASSERT_TRUE(assoc.second->recv_data.pop_blocking(pdu));
    EXPECT_EQ(pdu.length(), pdu_len);
  }
}

TEST_P(sctp_network_link_test, multi_client_send_data)
{
  unsigned pdu_len = 10;

  // Send data from each client association.
  unsigned i = 0;
  for (auto& assoc : client_associations) {
    byte_buffer pdu = create_data(i * pdu_len, pdu_len);
    ASSERT_TRUE(assoc.second->client_sender->on_new_sdu(std::move(pdu)));
  }

  // Check data received by each server association.
  for (auto& assoc : server_associations) {
    byte_buffer pdu;
    ASSERT_TRUE(assoc.second->recv_data.pop_blocking(pdu));
    EXPECT_EQ(pdu.length(), pdu_len);
  }
}

INSTANTIATE_TEST_SUITE_P(sctp_multi_client_test,
                         sctp_network_link_test,
                         ::testing::Values(test_params{1}, test_params{4}, test_params{8}, test_params{32}));
