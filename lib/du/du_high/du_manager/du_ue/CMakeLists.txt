#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(du_ue du_bearer.cpp du_ue_adapters.cpp du_ue_bearer_manager.cpp du_ue_controller_impl.cpp du_ue_manager.cpp)
target_link_libraries(du_ue srsran_du_manager_procedures srsran_f1u_du srsran_gtpu)
