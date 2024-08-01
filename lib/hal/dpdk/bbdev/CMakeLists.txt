#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES 
  bbdev_acc_factory.cpp
  plugin_bbdev_acc_factory.cpp)

add_library(hal_bbdev_factory STATIC ${SOURCES})
target_compile_options(hal_bbdev_factory PRIVATE ${DPDK_CFLAGS})
target_link_libraries(hal_bbdev_factory srslog ${DPDK_LIBRARIES} dl)
