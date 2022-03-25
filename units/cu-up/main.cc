#include "adapters.h"
#include "srsgnb/pdcp/pdcp_factory.h"
#include "srsgnb/sdap/sdap_factory.h"

using namespace cu_up;

namespace {

/// This class creates fake data packets and routes them to the F1u interface through the F1u_input_gateway interface.
/// No concrete class dependencies.
class fake_receiver
{
  srsgnb::pdcp_pdu_handler& notifier;

public:
  explicit fake_receiver(srsgnb::pdcp_pdu_handler& notifier) : notifier(notifier) {}

  void receive()
  {
    // Create a fake data packet.
    srsgnb::byte_buffer v;
    v.append(std::array<uint8_t, 22>{0});

    std::printf("[Network] Receiving a fake packet of size = %u\n", (unsigned)v.length());

    // Send the packet to the PDCP interface.
    notifier.handle_pdu(std::move(v));
  }
};

class fake_sdap_to_ngu_relay : public srsgnb::sdap_sdu_rx_notifier
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

  fake_sdap_to_ngu_relay ngap_tx;
  auto                   sdap = srsgnb::create_sdap(ngap_tx);
  sdap_packet_handler    pdcp_sdap_adapter(*sdap);
  auto                   pdcp = srsgnb::create_pdcp(pdcp_sdap_adapter);

  fake_receiver rx(*pdcp);
  rx.receive();

  return 0;
}
