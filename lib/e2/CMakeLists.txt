#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#
set(SOURCES
    common/e2ap_asn1_packer.cpp
)


add_library(srsgnb_e2 STATIC ${SOURCES})
target_link_libraries(srsgnb_e2 e2ap_asn1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
