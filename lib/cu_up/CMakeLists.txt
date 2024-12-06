#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        cu_up_executor_mapper.cpp
        cu_up_factory.cpp
        cu_up_impl.cpp
        cu_up_manager_impl.cpp
        ue_manager.cpp
        pdu_session_manager_impl.cpp
        ngu_session_manager_impl.cpp
        routines/initial_cu_up_setup_routine.cpp
)

add_library(srsran_cu_up STATIC ${SOURCES})
target_link_libraries(srsran_cu_up
        srsran_e1ap_common
        srsran_e1ap_cu_up
        srsran_gtpu
        srsran_pdcp
        srsran_f1u_cu_up
        srsran_gateway
        e1ap_asn1
        srsran_sdap
        srsran_security
        srsran_support
        srslog)

add_library(srsran_o_cu_up STATIC
        o_cu_up_factory.cpp
        o_cu_up_impl.cpp)
target_link_libraries(srsran_o_cu_up PRIVATE srsran_cu_up srsran_e2)
