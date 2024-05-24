#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


add_library(srsran_e1ap_cu_up
        e1ap_cu_up_factory.cpp
        e1ap_cu_up_impl.cpp
        e1ap_cu_up_connection_handler.cpp
        procedures/e1ap_cu_up_setup_procedure.cpp)
target_include_directories(srsran_e1ap_cu_up PRIVATE ..)
target_link_libraries(srsran_e1ap_cu_up e1ap_asn1 srsran_e1ap_common)
