#
# Copyright 2021-2025 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

"""
Test Iperf
"""

import logging
from collections import defaultdict
from enum import Enum

import pytest
from retina.protocol.base_pb2 import Metrics
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto

from .configuration import is_tdd


class QAMTable(Enum):
    """
    Defines the QAM table options
    """

    QAM64 = 0
    QAM256 = 1


TINY_DURATION = 10
SHORT_DURATION = 20
LONG_DURATION = 2 * 60
LOW_BITRATE = int(1e6)
MEDIUM_BITRATE = int(15e6)
HIGH_BITRATE = int(50e6)
MAX_BITRATE = int(600e6)

# TDD throughput (empirical measurements, might become outdated if RF conditions change)
tdd_ul_udp = defaultdict(
    lambda: MAX_BITRATE,
    {
        20: int(16e6),
        50: int(33e6),
        90: int(58e6),
    },
)
tdd_dl_udp = defaultdict(
    lambda: MAX_BITRATE,
    {
        20: int(45e6),
        50: int(156e6),
        90: int(247e6),
    },
)
tdd_ul_tcp = defaultdict(
    lambda: MAX_BITRATE,
    {
        20: int(16e6),
        50: int(29e6),
        90: int(56e6),
    },
)
tdd_dl_tcp = defaultdict(
    lambda: MAX_BITRATE,
    {
        20: int(43e6),
        50: int(153e6),
        90: int(124e6),
    },
)

# FDD throughput (empirical measurements, might become outdated if RF conditions change)
fdd_ul_udp = defaultdict(
    lambda: MAX_BITRATE,
    {
        10: int(32e6),
        20: int(71e6),
    },
)
fdd_dl_udp = defaultdict(
    lambda: MAX_BITRATE,
    {
        10: int(35e6),
        20: int(97e6),
    },
)
fdd_ul_tcp = defaultdict(
    lambda: MAX_BITRATE,
    {
        10: int(30e6),
        20: int(69e6),
    },
)
fdd_dl_tcp = defaultdict(
    lambda: MAX_BITRATE,
    {
        10: int(35e6),
        20: int(96e6),
    },
)


def get_maximum_throughput_tdd(
    *, bandwidth: int, direction: IPerfDir, protocol: IPerfProto  # The "*" enforces keyword-only arguments
) -> int:
    """
    Get the maximum E2E TDD throughput for bandwidth, direction and transport protocol
    """
    if direction in (IPerfDir.UPLINK, IPerfDir.BIDIRECTIONAL):
        if protocol == IPerfProto.UDP:
            return tdd_ul_udp[bandwidth]
        if protocol == IPerfProto.TCP:
            return tdd_ul_tcp[bandwidth]
    elif direction == IPerfDir.DOWNLINK:
        if protocol == IPerfProto.UDP:
            return tdd_dl_udp[bandwidth]
        if protocol == IPerfProto.TCP:
            return tdd_dl_tcp[bandwidth]
    return 0


def get_maximum_throughput_fdd(
    *, bandwidth: int, direction: IPerfDir, protocol: IPerfProto  # The "*" enforces keyword-only arguments
) -> int:
    """
    Get the maximum E2E FDD throughput for bandwidth, direction and transport protocol
    """
    if direction in (IPerfDir.UPLINK, IPerfDir.BIDIRECTIONAL):
        if protocol == IPerfProto.UDP:
            return fdd_ul_udp[bandwidth]
        if protocol == IPerfProto.TCP:
            return fdd_ul_tcp[bandwidth]
    elif direction == IPerfDir.DOWNLINK:
        if protocol == IPerfProto.UDP:
            return fdd_dl_udp[bandwidth]
        if protocol == IPerfProto.TCP:
            return fdd_dl_tcp[bandwidth]
    return 0


def get_maximum_throughput(
    *, bandwidth: int, band: int, direction: IPerfDir, protocol: IPerfProto  # The "*" enforces keyword-only arguments
) -> int:
    """
    Get the maximum E2E throughput for bandwidth, duplex-type, direction and transport protocol
    """
    if is_tdd(band):
        return get_maximum_throughput_tdd(bandwidth=bandwidth, direction=direction, protocol=protocol)
    return get_maximum_throughput_fdd(bandwidth=bandwidth, direction=direction, protocol=protocol)


def get_max_theoretical_bitrate(
    *,  # This enforces keyword-only arguments
    bandwidth: int,
    band: int,
    nof_antennas: int,
    is_dl: bool,
    qam: QAMTable,
):
    """
    Get the expected bitrate for bandwidth, band, direction and protocol
    """
    max_nof_res = 273 * 12  # 273 resource blocks, 12 subcarriers per resource block
    max_bw = 100
    nof_symbols_per_ms = 28  # 14 slots per subframe, 2 subframes per ms
    # This is the number of DL symbols per TDD period, with 7 DL slots and 8 DL symbols
    if is_dl:
        perc_dl_symb_over_tdd_period = (14 * 6 + 8) / (14 * 10) if is_tdd(band) else 1.0
    else:
        perc_dl_symb_over_tdd_period = 14 * 3 / (14 * 10) if is_tdd(band) else 1.0

    # Theoretical maximum SE for QAM64 and QAM256, as per Table 5.1.3.1-1 and 5.1.3.1-2, TS 38.214.
    max_se = 7.4063 if qam == QAMTable.QAM256 else 5.5547
    # Theoretical maximum bitrate in bps. The factor 1000 is to convert the ms (ref. to nof_symbols_per_ms) into sec.
    phy_max_bitrate = (
        perc_dl_symb_over_tdd_period
        * max_nof_res
        * bandwidth
        / max_bw
        * nof_symbols_per_ms
        * max_se
        * nof_antennas
        * 1000
    )

    return phy_max_bitrate


def get_peak_average_bitrate(
    *,  # This enforces keyword-only arguments
    iperf_duration: int,
    metrics: Metrics,
):
    """
    Get the correct peak average bitrate reported metric based on the given iperf duration
    """

    if iperf_duration == TINY_DURATION:
        return metrics.total.dl_bitrate_peak_av.av_5_samples, metrics.total.ul_bitrate_peak_av.av_5_samples
    if iperf_duration == SHORT_DURATION:
        return metrics.total.dl_bitrate_peak_av.av_15_samples, metrics.total.ul_bitrate_peak_av.av_15_samples
    return metrics.total.dl_bitrate_peak_av.av_30_samples, metrics.total.ul_bitrate_peak_av.av_30_samples


# pylint: disable=too-many-arguments,too-many-positional-arguments,too-many-locals
def assess_iperf_bitrate(
    *,  # This enforces keyword-only arguments
    bw: int,
    band: int,
    nof_antennas_dl: int,
    pdsch_mcs_table: str,
    pusch_mcs_table: str,
    iperf_duration: int,
    metrics: Metrics,
    dl_brate_threshold: float = 0,
    ul_brate_threshold: float = 0,
):
    """
    Check if the achieved bitrate values are above the expected thresholds
    """
    # Defines the actual DL/UL bitrate as a percentage of the max theoretical bitrates
    effective_rate_coeff_dl = 0.66  # empirical value
    effective_rate_coeff_ul = 0.46  # empirical value
    # Defines the expected bitrate to be achived as a percentage of the DL/UL bitrates
    expected_to_max_coeff = 0.9

    if dl_brate_threshold == 0:
        min_dl_bitrate = get_max_theoretical_bitrate(
            bandwidth=bw,
            band=band,
            nof_antennas=nof_antennas_dl,
            is_dl=True,
            qam=QAMTable.QAM256 if pdsch_mcs_table == "qam256" else QAMTable.QAM64,
        )
        dl_brate_threshold = min_dl_bitrate * effective_rate_coeff_dl * expected_to_max_coeff

    if ul_brate_threshold == 0:
        min_ul_bitrate = get_max_theoretical_bitrate(
            bandwidth=bw,
            band=band,
            nof_antennas=1,
            is_dl=False,
            qam=QAMTable.QAM256 if pusch_mcs_table == "qam256" else QAMTable.QAM64,
        )
        ul_brate_threshold = min_ul_bitrate * effective_rate_coeff_ul * expected_to_max_coeff

    dl_bitrate_peak_average, ul_bitrate_peak_average = get_peak_average_bitrate(
        iperf_duration=iperf_duration, metrics=metrics
    )
    dl_brate_assessment_ok = dl_bitrate_peak_average >= dl_brate_threshold
    ul_brate_assessment_ok = ul_bitrate_peak_average >= ul_brate_threshold
    if dl_brate_assessment_ok:
        logging.info(
            "DL tot brate %.3f Mbps is above the %.3f Mbps threshold",
            dl_bitrate_peak_average * 1e-6,
            dl_brate_threshold * 1e-6,
        )
    else:
        logging.error(
            "DL tot brate %.3f Mbps is below the %.3f Mbps threshold",
            dl_bitrate_peak_average * 1e-6,
            dl_brate_threshold * 1e-6,
        )
    if ul_brate_assessment_ok:
        logging.info(
            "UL tot brate %.3f Mbps is above the %.3f Mbps threshold",
            ul_bitrate_peak_average * 1e-6,
            ul_brate_threshold * 1e-6,
        )
    else:
        logging.error(
            "UL tot brate %.3f Mbps is below %.3f Mbps threshold",
            ul_bitrate_peak_average * 1e-6,
            ul_brate_threshold * 1e-6,
        )
    if not dl_brate_assessment_ok or not ul_brate_assessment_ok:
        dl_pass_msg = "PASSED" if dl_brate_assessment_ok else "FAILED"
        ul_pass_msg = "PASSED" if ul_brate_assessment_ok else "FAILED"
        pytest.fail(f"Bitrate assessment: DL={dl_pass_msg} UL={ul_pass_msg}")
