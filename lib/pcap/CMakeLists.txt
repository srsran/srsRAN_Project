#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        pcap.cpp
        backend_pcap_writer.cpp
        dlt_pcap_impl.cpp
        e2ap_pcap.cpp
        f1ap_pcap.cpp
        mac_pcap_impl.cpp
        pcap_file_base.cpp
        pcap_rlc_impl.cpp
        pcap_rlc_pdu_context.cpp
)

add_library(srsran_pcap STATIC ${SOURCES})
