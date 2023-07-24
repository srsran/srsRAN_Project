#
# Copyright 2021-2023 Software Radio Systems Limited
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

set(SOURCES
        accumulate.cpp
        add.cpp
        aligned_vec.cpp
        bit.cpp
        clip.cpp
        compare.cpp
        conversion.cpp
        division.cpp
        dot_prod.cpp
        modulus_square.cpp
        prod.cpp
        sc_prod.cpp
        subtract.cpp
        )

add_library(srsvec STATIC ${SOURCES})
target_link_libraries(srsvec srslog)

install(TARGETS srsvec
        EXPORT srsran_export)