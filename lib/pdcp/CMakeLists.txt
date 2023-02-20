#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES pdcp_factory.cpp pdcp_entity_tx.cpp pdcp_entity_rx.cpp pdcp_pdu.cpp)

add_library(srsran_pdcp STATIC ${SOURCES})
target_link_libraries(srsran_pdcp srsran_security srsran_support)
