#include "control_socket_context.h"

// ✅ Global definition — visible to all linked units
std::atomic<int> control_client_socket_fd = -1;