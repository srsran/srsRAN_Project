#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_instrumentation
        traces/du_traces.cpp
        traces/ru_traces.cpp
        traces/up_traces.cpp
        traces/ofh_traces.cpp)
target_link_libraries(srsran_instrumentation srsran_support)

install(TARGETS srsran_instrumentation
        EXPORT srsran_export)
