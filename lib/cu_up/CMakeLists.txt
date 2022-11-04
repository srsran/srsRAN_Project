#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
      cu_up_factory.cpp
      cu_up_impl.cpp
)

add_library(srsgnb_cu_up STATIC ${SOURCES})

target_link_libraries(srsgnb_cu_up srslog srsgnb_support srsgnb_pdcp srsgnb_security)
