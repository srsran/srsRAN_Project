#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        ra_scheduler.cpp
        sib_scheduler.cpp
        ssb_scheduler.cpp
        prach_scheduler.cpp
        paging_scheduler.cpp
        csi_rs_scheduler.cpp)

add_library(common_sched OBJECT ${SOURCES})
