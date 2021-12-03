#include "adapters.h"
#include "srsgnb/F1_interface/F1u_factory.h"
#include "srsgnb/pdcp/pdcp_factory.h"

using namespace cu_up;

/// This class creates fake data packets and routes them to the F1u interface through the F1u_input_gateway interface.
/// No concrete class dependencies.
class fake_receiver
{
  srsgnb::F1u_input_gateway& notifier;

public:
  explicit fake_receiver(srsgnb::F1u_input_gateway& notifier) : notifier(notifier) {}

  void receive()
  {
    // Create a fake data packet.
    srsgnb::byte_buffer v(22);

    std::printf("[Network] Receiving a fake packet of size = %u\n", (unsigned)v.size());

    // Send the packet to the F1 interface.
    notifier.handle(v);
  }
};

int main()
{
  /// Creation order is from upper to lower layer. Dependency injection is done at object construction, in case of
  /// further flexibility, setters can be implemented instead.

  sdap_packet_handler pdcp_sdap_adapter;
  auto                pdcp = srsgnb::create_pdcp(pdcp_sdap_adapter);
  pdcp_packet_handler f1u_pdcp_adapter(*pdcp);
  auto                F1u = srsgnb::create_F1u_interface(f1u_pdcp_adapter);

  fake_receiver rx(*F1u);
  rx.receive();

  return 0;
}
