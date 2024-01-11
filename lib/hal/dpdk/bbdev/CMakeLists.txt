#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES 
  bbdev_acc.cpp
  bbdev_acc_factory.cpp
  bbdev.cpp
  bbdev_op_pool_factory.cpp
  ldpc/bbdev_ldpc_decoder.cpp
)

add_library(hal_bbdev STATIC ${SOURCES})
target_compile_options(hal_bbdev PRIVATE ${DPDK_CFLAGS})
target_link_libraries(hal_bbdev srslog ${DPDK_LIBRARIES})
