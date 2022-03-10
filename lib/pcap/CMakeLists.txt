#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES pcap.cc f1ap_pcap.cc e1ap_pcap.cc)
add_library(srsgnb_pcap STATIC ${SOURCES})
INSTALL(TARGETS srsgnb_pcap DESTINATION ${LIBRARY_DIR})