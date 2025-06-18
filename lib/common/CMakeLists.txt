add_library(control_socket_context STATIC
  control_socket_context.cpp
)

target_include_directories(control_socket_context PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)