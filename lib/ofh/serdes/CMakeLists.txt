#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        ofh_cplane_message_builder_dynamic_compression_impl.cpp
        ofh_cplane_message_builder_impl.cpp
        ofh_cplane_message_builder_static_compression_impl.cpp
        ofh_serdes_factories.cpp
        ofh_uplane_message_builder_impl.cpp
        ofh_uplane_message_builder_dynamic_compression_impl.cpp
        ofh_uplane_message_builder_static_compression_impl.cpp
        ofh_uplane_message_decoder_impl.cpp
        ofh_uplane_message_decoder_dynamic_compression_impl.cpp
        ofh_uplane_message_decoder_static_compression_impl.cpp)

add_library(srsran_ofh_message_serdes STATIC ${SOURCES})
target_link_libraries(srsran_ofh_message_serdes srslog)
