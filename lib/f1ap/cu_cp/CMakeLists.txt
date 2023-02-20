#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


add_library(srsgnb_f1ap_cu
        f1ap_cu_impl.cpp
        f1ap_cu_factory.cpp
        procedures/ue_context_setup_procedure.cpp
        procedures/ue_context_modification_procedure.cpp
        procedures/ue_context_release_procedure.cpp)
target_include_directories(srsgnb_f1ap_cu PRIVATE ..)
target_link_libraries(srsgnb_f1ap_cu srsgnb_f1ap_common f1ap_asn1)
