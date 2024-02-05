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

set(SOURCES 
  bbdev_acc.cpp
  bbdev_acc_factory.cpp
  bbdev.cpp
  bbdev_op_pool_factory.cpp
  ldpc/bbdev_ldpc_decoder.cpp
  ldpc/bbdev_ldpc_encoder.cpp
)

add_library(hal_bbdev STATIC ${SOURCES})
target_compile_options(hal_bbdev PRIVATE ${DPDK_CFLAGS})
target_link_libraries(hal_bbdev srslog ${DPDK_LIBRARIES})
