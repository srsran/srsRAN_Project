/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "adapters.h"
#include "srsgnb/pdcp/pdcp_factory.h"
#include "srsgnb/sdap/sdap_factory.h"

using namespace srsgnb;

namespace {

/// This class creates fake data packets and routes them to the F1u interface through the F1u_input_gateway
/// interface. No concrete class dependencies.
class fake_receiver
{
  srsgnb::pdcp_rx_lower_interface& pdcp_if;

public:
  explicit fake_receiver(srsgnb::pdcp_rx_lower_interface& pdcp_if) : pdcp_if(pdcp_if) {}

  void receive()
  {
    // Create a fake data packet.
    srsgnb::byte_buffer buf;
    buf.append(std::array<uint8_t, 22>{0});

    std::printf("[Network] Receiving a fake packet of size = %u\n", (unsigned)buf.length());

    // Send the packet to the PDCP interface.
    pdcp_if.handle_pdu(srsgnb::byte_buffer_slice_chain{std::move(buf)});
  }
};

class fake_sdap_to_ngu_relay : public srsgnb::srs_cu_up::sdap_sdu_rx_notifier
{
public:
  void on_new_sdu(srsgnb::byte_buffer data) override
  {
    std::printf("[SDAP-NGU-RELAY] Receiving a fake packet of size = %u\n", (unsigned)data.length());
    std::printf("  Forwarding packet to NG-U to be delivered to UPF\n");
  }
};

} // namespace

int main()
{
  /// Creation order is from upper to lower layer. Dependency injection is done at object construction, in case of
  /// further flexibility, setters can be implemented instead.

  fake_sdap_to_ngu_relay                               ngap_tx;
  std::unique_ptr<srsgnb::srs_cu_up::sdap_pdu_handler> sdap = srsgnb::srs_cu_up::create_sdap(ngap_tx);
  srsgnb::srs_cu_up::sdap_packet_handler               pdcp_sdap_adapter(*sdap);
  srsgnb::pdcp_entity_creation_message                 pdcp_msg = {};
  pdcp_msg.rx_upper_dn                                          = &pdcp_sdap_adapter;
  std::unique_ptr<srsgnb::pdcp_entity> pdcp                     = srsgnb::create_pdcp_entity(pdcp_msg);

  fake_receiver rx(pdcp->get_rx_lower_interface());
  rx.receive();

  return 0;
}
