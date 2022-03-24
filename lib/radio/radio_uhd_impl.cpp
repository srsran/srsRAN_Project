#include "radio_uhd_impl.h"

using namespace srsgnb;

std::unique_ptr<radio_session> radio_session_factory_uhd_impl::create(const radio_configuration::radio& config,
                                                                      radio_notifier&                   notifier)
{
  std::unique_ptr<uhd_session> session = std::make_unique<uhd_session>(config, notifier);
  if (!session->is_valid()) {
    return nullptr;
  }

  return std::move(session);
}

std::unique_ptr<radio_session_factory> srsgnb::create_radio_uhd_factory()
{
  return std::make_unique<radio_session_factory_uhd_impl>();
}
