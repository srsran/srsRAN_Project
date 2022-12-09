#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        ngc_factory.cpp
        ngc_impl.cpp
        ngc_asn1_packer.cpp
        procedures/ng_setup_procedure.cpp
        procedures/ng_initial_context_setup_procedure.cpp
        procedures/ng_pdu_session_resource_setup_procedure.cpp
    )

add_library(srsgnb_ngap STATIC ${SOURCES})
target_link_libraries(srsgnb_ngap ngap_asn1 srsgnb_cu_cp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})