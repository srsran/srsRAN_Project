#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        ecpri_factories.cpp
        ecpri_packet_builder_impl.cpp
        ecpri_packet_decoder_impl.cpp)

add_library(srsran_ofh_ecpri STATIC ${SOURCES})
target_link_libraries(srsran_ofh_ecpri srslog)
