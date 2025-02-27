#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


add_subdirectory(e2)
add_subdirectory(f1u)
add_subdirectory(hal)
add_subdirectory(logger)
add_subdirectory(metrics)
add_subdirectory(network)

add_library(srsran_app_helpers INTERFACE)
target_link_libraries(srsran_app_helpers INTERFACE  srsran_e2_app_helpers
                                                    srsran_f1u_app_helpers
                                                    srsran_hal_app_helpers
                                                    srsran_logger_app_helpers
                                                    srsran_metrics_app_helpers
                                                    srsran_network_app_helpers)
