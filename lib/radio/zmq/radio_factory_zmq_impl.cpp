
#include "radio_factory_zmq_impl.h"
#include "radio_session_zmq_impl.h"

using namespace srsgnb;

std::unique_ptr<radio_session> radio_factory_zmq_impl::create(const radio_configuration::radio& config,
                                                              task_executor&                    async_task_executor,
                                                              radio_notification_handler&       notifier)
{
  // Create radio session based on ZMQ.
  std::unique_ptr<radio_session_zmq_impl> session =
      std::make_unique<radio_session_zmq_impl>(config, async_task_executor, notifier);

  // Check session was created successfully.
  if (!session->is_successful()) {
    return nullptr;
  }

  // Otherwise, return the instance.
  return std::move(session);
}
