#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        backend_pcap_writer.cpp
        dlt_pcap_impl.cpp
        mac_pcap_impl.cpp
        pcap_file_writer.cpp
        rlc_pcap_impl.cpp
        pcap_rlc_pdu_context.cpp
)

add_library(srsran_pcap STATIC ${SOURCES})
