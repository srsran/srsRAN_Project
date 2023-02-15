#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(procedures)
add_library(srsgnb_f1ap_du
        f1ap_du_impl.cpp
        f1ap_du_factory.cpp
        ue_context/f1ap_du_ue.cpp
        ue_context/ue_bearer_manager.cpp
        ue_context/f1c_du_bearer_impl.cpp
        ue_context/f1ap_du_ue_config_update.cpp)
target_include_directories(srsgnb_f1ap_du PRIVATE ..)
target_link_libraries(srsgnb_f1ap_du f1ap_asn1 srsgnb_f1ap_common srsgnb_f1ap_du_procedures srsgnb_f1u_du)
