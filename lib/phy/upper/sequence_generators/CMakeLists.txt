#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        low_papr_sequence_collection_impl.cpp
        low_papr_sequence_generator_impl.cpp
        pseudo_random_generator_impl.cpp
        sequence_generator_factories.cpp
        )

include_directories(../../../../include/srsgnb/phy/upper/sequence_generators)

add_library(srsran_sequence_generators STATIC ${SOURCES})
target_link_libraries(srsran_sequence_generators srsran_support srsvec)

install(TARGETS srsran_sequence_generators
        EXPORT srsran_export)