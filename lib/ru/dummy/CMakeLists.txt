#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        ru_dummy_factory.cpp
        ru_dummy_impl.cpp
        ru_dummy_metrics_collector.cpp)

add_library(srsran_ru_dummy STATIC ${SOURCES})
target_link_libraries(srsran_ru_dummy srslog)
