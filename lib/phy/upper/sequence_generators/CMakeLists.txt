#
# Copyright 2021-2024 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
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

install(TARGETS srsran_sequence_generators
        EXPORT srsran_export)