#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


set(SOURCES
        common/e1ap_asn1_packer.cpp
        cu_cp/e1_cu_cp_factory.cpp
        cu_cp/e1_cu_cp_impl.cpp
        cu_cp/procedures/cu_cp_e1_setup_procedure.cpp
        cu_cp/procedures/e1_bearer_context_setup_procedure.cpp
        cu_cp/procedures/e1_bearer_context_release_procedure.cpp
        cu_cp/procedures/e1_bearer_context_modification_procedure.cpp
    )

add_library(srsgnb_e1 STATIC ${SOURCES})
target_link_libraries(srsgnb_e1 e1ap_asn1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
