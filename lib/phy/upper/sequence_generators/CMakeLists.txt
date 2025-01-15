#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

# There is a compilation defect for CLang version 15 which makes the pseudo-random generator to create invalid
# sequences.
if ((CMAKE_CXX_COMPILER_ID MATCHES "Clang") AND (CMAKE_CXX_COMPILER_VERSION MATCHES "15"))
    message(WARNING "The selected compiler version is not supported with -O2 flag.")
    string(REPLACE "-O2" "-O3" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
endif ((CMAKE_CXX_COMPILER_ID MATCHES "Clang") AND (CMAKE_CXX_COMPILER_VERSION MATCHES "15"))

set(SOURCES
        low_papr_sequence_collection_impl.cpp
        low_papr_sequence_generator_impl.cpp
        pseudo_random_generator_impl.cpp
        sequence_generator_factories.cpp
        )

add_library(srsran_sequence_generators STATIC ${SOURCES})
target_link_libraries(srsran_sequence_generators srsran_support srsvec)

add_to_exported_libs(srsran_sequence_generators)
