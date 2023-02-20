#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES du_cell_config_validation.cpp)
add_library(srsgnb_du_config_validators STATIC ${SOURCES})
