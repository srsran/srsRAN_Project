#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


add_library(srsran_f1ap_cu
        f1ap_cu_impl.cpp
        f1ap_cu_factory.cpp
        procedures/f1_setup_procedure.cpp
        procedures/f1_removal_procedure.cpp
        procedures/f1ap_stop_procedure.cpp
        procedures/gnb_cu_configuration_update_procedure.cpp
        procedures/ue_context_setup_procedure.cpp
        procedures/ue_context_modification_procedure.cpp
        procedures/ue_context_release_procedure.cpp
        ue_context/f1ap_cu_ue_context.cpp
        ue_context/ue_ul_bearer_manager.cpp)

target_include_directories(srsran_f1ap_cu PRIVATE ..)
target_link_libraries(srsran_f1ap_cu srsran_support srsran_ran srsran_f1ap_common f1ap_asn1)
