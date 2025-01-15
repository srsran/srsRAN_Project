#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES 
  bbdev_acc_factory.cpp)

add_library(srsran_hal_bbdev STATIC ${SOURCES})
target_compile_options(srsran_hal_bbdev PRIVATE ${DPDK_CFLAGS})
target_link_libraries(srsran_hal_bbdev srslog ${DPDK_LIBRARIES} dl)
