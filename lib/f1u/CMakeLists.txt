#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_f1u_cu_up cu_up/f1u_bearer_impl.cpp cu_up/f1u_bearer_factory.cpp)
add_library(srsran_f1u_du du/f1u_bearer_impl.cpp du/f1u_bearer_factory.cpp)

# Local connector
add_library(srsran_f1u_connector local_connector/f1u_local_connector.cpp)
target_link_libraries(srsran_f1u_connector srsran_f1u_cu_up)

#Split connector
add_library(srsran_f1u_cu_up_split_connector cu_up/f1u_session_manager_impl.cpp cu_up/f1u_session_manager_factory.cpp cu_up/split_connector/f1u_split_connector_factory.cpp cu_up/split_connector/f1u_split_connector.cpp )
target_link_libraries(srsran_f1u_cu_up_split_connector srsran_f1u_cu_up)

add_library(srsran_f1u_du_split_connector du/split_connector/f1u_split_connector.cpp du/split_connector/f1u_split_connector_factory.cpp)
target_link_libraries(srsran_f1u_du_split_connector srsran_f1u_du)
