#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


set(SOURCES f1c_configuration_helpers.cpp)

add_library(du_manager_converters STATIC ${SOURCES})
target_link_libraries(du_manager_converters rrc_nr_asn1 f1ap_asn1)
