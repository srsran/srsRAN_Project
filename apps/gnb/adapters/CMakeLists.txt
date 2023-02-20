#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES f1u_connector/f1u_local_connector.cpp)
add_library(srsgnbapp_f1u_connector STATIC ${SOURCES})
target_link_libraries(srsgnbapp_f1u_connector srsgnb_f1u_cu_up)
