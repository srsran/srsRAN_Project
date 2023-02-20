#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(procedures)
add_library(srsran_f1ap_du
        f1ap_du_impl.cpp
        f1ap_du_factory.cpp
        ue_context/f1ap_du_ue.cpp
        ue_context/ue_bearer_manager.cpp
        ue_context/f1c_du_bearer_impl.cpp
        ue_context/f1ap_du_ue_config_update.cpp)
target_include_directories(srsran_f1ap_du PRIVATE ..)
target_link_libraries(srsran_f1ap_du f1ap_asn1 srsran_f1ap_common srsran_f1ap_du_procedures srsran_f1u_du)
