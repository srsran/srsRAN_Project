#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
    procedures/e2_setup_procedure.cpp
    common/e2ap_asn1_packer.cpp
    common/e2_factory.cpp
    common/e2_impl.cpp
 )

add_library(srsran_e2 STATIC ${SOURCES})
target_link_libraries(srsran_e2 e2ap_asn1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
