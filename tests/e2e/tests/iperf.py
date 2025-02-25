#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Test Iperf
"""

import logging
from collections import defaultdict
from enum import Enum
from time import sleep
from typing import Optional, Sequence, Tuple, Union

import pytest
from google.protobuf.empty_pb2 import Empty
from pytest import mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.utils import configure_artifacts, param
from retina.protocol.base_pb2 import Metrics, PLMN
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ric_pb2_grpc import NearRtRicStub
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto
from retina.protocol.ue_pb2_grpc import UEStub

from .steps.configuration import configure_test_parameters, get_minimum_sample_rate_for_bandwidth, is_tdd
from .steps.stub import (
    INTER_UE_START_PERIOD,
    iperf_parallel,
    ric_validate_e2_interface,
    start_and_attach,
    start_kpm_mon_xapp,
    start_rc_xapp,
    stop,
    stop_kpm_mon_xapp,
    stop_rc_xapp,
)

TINY_DURATION = 10
SHORT_DURATION = 20
LONG_DURATION = 2 * 60
LOW_BITRATE = int(1e6)
MEDIUM_BITRATE = int(15e6)
HIGH_BITRATE = int(50e6)
MAX_BITRATE = int(600e6)

ZMQ_ID = "band:%s-scs:%s-bandwidth:%s-bitrate:%s"


class QAMTable(Enum):
    QAM64 = 0
    QAM256 = 1


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


def get_maximum_throughput_tdd(bandwidth: int, direction: IPerfDir, protocol: IPerfProto) -> int:
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


def get_maximum_throughput_fdd(bandwidth: int, direction: IPerfDir, protocol: IPerfProto) -> int:
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


def get_maximum_throughput(bandwidth: int, band: int, direction: IPerfDir, protocol: IPerfProto) -> int:
    """
    Get the maximum E2E throughput for bandwidth, duplex-type, direction and transport protocol
    """
    if is_tdd(band):
        return get_maximum_throughput_tdd(bandwidth, direction, protocol)
    return get_maximum_throughput_fdd(bandwidth, direction, protocol)


def get_max_theoretical_bitrate(
    bandwidth: int,
    band: int,
    nof_antennas: int,
    direction: IPerfDir,
    qam: QAMTable,
):
    """
    Get the expected bitrate for bandwidth, band, direction and protocol
    """
    max_nof_res = 273 * 12  # 273 resource blocks, 12 subcarriers per resource block
    max_bw = 100
    nof_symbols_per_ms = 28  # 14 slots per subframe, 2 subframes per ms
    # This is the number of DL symbols per TDD period, with 7 DL slots and 8 DL symbols
    if direction == IPerfDir.DOWNLINK:
        perc_dl_symb_over_tdd_period = (14 * 6 + 8) / (14 * 10) if is_tdd(band) else 1.0
    elif direction == IPerfDir.UPLINK:
        perc_dl_symb_over_tdd_period = 14 * 3 / (14 * 10) if is_tdd(band) else 1.0
    else:
        raise ValueError(
            "Input paramer direction:{direction} invalid. Must be DOWNLINK {IPerfDir.DOWNLINK} or UPLINK {IPerfDir.UPLINK}"
        )
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
    iperf_duration: int,
    metrics: Metrics,
):
    """
    Get the correct peak average bitrate reported metric based on the given iperf duration
    """

    if iperf_duration == TINY_DURATION:
        return metrics.total.dl_bitrate_peak_av.av_5_samples, metrics.total.ul_bitrate_peak_av.av_5_samples
    elif iperf_duration == SHORT_DURATION:
        return metrics.total.dl_bitrate_peak_av.av_15_samples, metrics.total.ul_bitrate_peak_av.av_15_samples
    else:
        return metrics.total.dl_bitrate_peak_av.av_30_samples, metrics.total.ul_bitrate_peak_av.av_30_samples


@mark.parametrize(
    "direction",
    (
        param(IPerfDir.DOWNLINK, id="downlink", marks=mark.downlink),
        param(IPerfDir.UPLINK, id="uplink", marks=mark.uplink),
        param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),
    ),
)
@mark.parametrize(
    "protocol",
    (
        param(IPerfProto.UDP, id="udp", marks=mark.udp),
        param(IPerfProto.TCP, id="tcp", marks=mark.tcp),
    ),
)
@mark.parametrize(
    "band, common_scs, bandwidth",
    (param(3, 15, 10, id="band:%s-scs:%s-bandwidth:%s"),),
)
@mark.zmq_srsue
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_srsue(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,  # pylint: disable=invalid-name
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    ZMQ IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue,),
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=11520000,
        iperf_duration=SHORT_DURATION,
        protocol=protocol,
        bitrate=MEDIUM_BITRATE,
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration=0,
        always_download_artifacts=True,
        common_search_space_enable=True,
        prach_config_index=1,
        pdsch_mcs_table="qam64",
        pusch_mcs_table="qam64",
    )


@mark.parametrize(
    "direction",
    (param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),),
)
@mark.parametrize(
    "protocol",
    (param(IPerfProto.UDP, id="udp", marks=mark.udp),),
)
@mark.parametrize(
    "band, common_scs, bandwidth",
    (param(3, 15, 10, id="band:%s-scs:%s-bandwidth:%s"),),
)
@mark.zmq_ric
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_ric(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,  # pylint: disable=invalid-name
    fivegc: FiveGCStub,
    gnb: GNBStub,
    ric: NearRtRicStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    ZMQ IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue,),
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=11520000,
        iperf_duration=SHORT_DURATION,
        protocol=protocol,
        bitrate=LOW_BITRATE,
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration=0,
        always_download_artifacts=True,
        common_search_space_enable=True,
        prach_config_index=1,
        pdsch_mcs_table="qam64",
        pusch_mcs_table="qam64",
        ric=ric,
    )


@mark.parametrize(
    "direction",
    (
        param(IPerfDir.DOWNLINK, id="downlink", marks=mark.downlink),
        param(IPerfDir.UPLINK, id="uplink", marks=mark.uplink),
        param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),
    ),
)
@mark.parametrize(
    "protocol",
    (
        param(IPerfProto.UDP, id="udp", marks=mark.udp),
        param(IPerfProto.TCP, id="tcp", marks=mark.tcp),
    ),
)
@mark.parametrize(
    "band, common_scs, bandwidth",
    (
        param(3, 15, 10, id="band:%s-scs:%s-bandwidth:%s"),
        param(78, 30, 20, id="band:%s-scs:%s-bandwidth:%s"),
    ),
)
@mark.android
@mark.flaky(
    reruns=2,
    only_rerun=["failed to start", "Exception calling application", "Attach timeout reached", "Some packages got lost"],
)
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_android(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,  # pylint: disable=invalid-name
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    Android IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue,),
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=get_minimum_sample_rate_for_bandwidth(bandwidth),
        iperf_duration=SHORT_DURATION,
        protocol=protocol,
        bitrate=get_maximum_throughput(bandwidth, band, direction, protocol),
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration="auto",
        always_download_artifacts=True,
        warning_as_errors=False,
    )


@mark.parametrize(
    "direction",
    (
        param(IPerfDir.DOWNLINK, id="downlink", marks=mark.downlink),
        param(IPerfDir.UPLINK, id="uplink", marks=mark.uplink),
        param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),
    ),
)
@mark.parametrize(
    "protocol",
    (
        param(IPerfProto.UDP, id="udp", marks=mark.udp),
        param(IPerfProto.TCP, id="tcp", marks=mark.tcp),
    ),
)
@mark.parametrize(
    "band, common_scs, bandwidth",
    (
        param(7, 15, 20, id="band:%s-scs:%s-bandwidth:%s"),
        param(78, 30, 50, id="band:%s-scs:%s-bandwidth:%s"),
    ),
)
@mark.android_hp
@mark.flaky(
    reruns=2,
    only_rerun=["failed to start", "Exception calling application", "Attach timeout reached", "Some packages got lost"],
)
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_android_hp(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,  # pylint: disable=invalid-name
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    Android high performance IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue,),
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,
        iperf_duration=SHORT_DURATION,
        protocol=protocol,
        bitrate=get_maximum_throughput(bandwidth, band, direction, protocol),
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration="auto",
        always_download_artifacts=True,
        warning_as_errors=False,
    )


@mark.parametrize(
    "direction",
    (param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),),
)
@mark.parametrize(
    "protocol",
    (param(IPerfProto.UDP, id="udp", marks=mark.udp),),
)
@mark.parametrize(
    "band, common_scs, bandwidth",
    (param(41, 30, 20, id="band:%s-scs:%s-bandwidth:%s"),),
)
@mark.zmq_2x2_mimo
@mark.flaky(reruns=2, only_rerun=["failed to start", "Attach timeout reached", "5GC crashed"])
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_zmq_2x2_mimo(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_32: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    ZMQ 2x2 mimo IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_32,
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,
        iperf_duration=SHORT_DURATION,
        protocol=protocol,
        bitrate=MEDIUM_BITRATE,
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration=0,
        always_download_artifacts=True,
        rx_to_tx_latency=2,
        enable_dddsu=True,
        nof_antennas_dl=2,
        nof_antennas_ul=2,
        inter_ue_start_period=1.5,  # Due to uesim
        assess_bitrate=False,
    )


@mark.parametrize(
    "direction",
    (param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),),
)
@mark.parametrize(
    "protocol",
    (param(IPerfProto.UDP, id="udp", marks=mark.udp),),
)
@mark.parametrize(
    "band, common_scs, bandwidth",
    (param(41, 30, 50, id="band:%s-scs:%s-bandwidth:%s"),),
)
@mark.zmq_64_ues
@mark.flaky(reruns=2, only_rerun=["failed to start", "Attach timeout reached", "5GC crashed"])
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_zmq_64_ues(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_64: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    ZMQ 2x2 mimo IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_64,
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,
        iperf_duration=SHORT_DURATION,
        protocol=protocol,
        bitrate=MEDIUM_BITRATE,
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration=0,
        always_download_artifacts=False,
        rx_to_tx_latency=2,
        enable_dddsu=False,
        nof_antennas_dl=2,
        nof_antennas_ul=2,
        inter_ue_start_period=1.5,  # Due to uesim
        assess_bitrate=True,
    )


@mark.parametrize(
    "direction",
    (
        param(IPerfDir.DOWNLINK, id="downlink", marks=mark.downlink),
        param(IPerfDir.UPLINK, id="uplink", marks=mark.uplink),
        param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),
    ),
)
@mark.parametrize(
    "protocol",
    (
        param(IPerfProto.UDP, id="udp", marks=mark.udp),
        param(IPerfProto.TCP, id="tcp", marks=mark.tcp),
    ),
)
@mark.parametrize(
    "band, common_scs, bandwidth",
    (param(41, 30, 20, id="band:%s-scs:%s-bandwidth:%s"),),
)
@mark.zmq_4x4_mimo
@mark.flaky(reruns=2, only_rerun=["failed to start", "Attach timeout reached", "5GC crashed"])
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_zmq_4x4_mimo(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,  # pylint: disable=invalid-name
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    ZMQ 4x4 mimo IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue,),
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,
        iperf_duration=SHORT_DURATION,
        protocol=protocol,
        bitrate=MEDIUM_BITRATE,
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration=0,
        always_download_artifacts=False,
        nof_antennas_dl=4,
        nof_antennas_ul=4,
        ue_stop_timeout=90,
    )


@mark.parametrize(
    "direction, nof_antennas",
    (
        param(IPerfDir.DOWNLINK, 1, id="downlink", marks=mark.downlink),
        param(IPerfDir.UPLINK, 1, id="uplink", marks=mark.uplink),
        param(IPerfDir.BIDIRECTIONAL, 4, id="bidirectional 4x4 mimo", marks=mark.bidirectional),
    ),
)
@mark.zmq
@mark.smoke
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_smoke(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_4: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    direction: IPerfDir,
    nof_antennas: int,
):
    """
    ZMQ IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_4,
        gnb=gnb,
        fivegc=fivegc,
        band=41,
        common_scs=30,
        bandwidth=20,
        sample_rate=None,  # default from testbed
        iperf_duration=TINY_DURATION,
        bitrate=LOW_BITRATE,
        protocol=IPerfProto.UDP,
        direction=direction,
        nof_antennas_dl=nof_antennas,
        nof_antennas_ul=nof_antennas,
        global_timing_advance=0,
        time_alignment_calibration=0,
        always_download_artifacts=False,
        bitrate_threshold=0,
        ue_stop_timeout=30,
        gnb_post_cmd=("", "metrics --enable_log_metrics=True"),
    )


@mark.parametrize(
    "direction",
    (
        param(IPerfDir.DOWNLINK, id="downlink", marks=mark.downlink),
        param(IPerfDir.UPLINK, id="uplink", marks=mark.uplink),
        param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),
    ),
)
@mark.parametrize(
    "protocol",
    (
        param(IPerfProto.UDP, id="udp", marks=mark.udp),
        param(IPerfProto.TCP, id="tcp", marks=mark.tcp),
    ),
)
@mark.parametrize(
    "band, common_scs, bandwidth, bitrate",
    (
        # ZMQ
        param(3, 15, 5, MEDIUM_BITRATE, id=ZMQ_ID),
        param(3, 15, 10, MEDIUM_BITRATE, id=ZMQ_ID),
        param(3, 15, 20, MEDIUM_BITRATE, id=ZMQ_ID),
        param(3, 15, 50, MEDIUM_BITRATE, id=ZMQ_ID),
        param(41, 30, 10, MEDIUM_BITRATE, id=ZMQ_ID),
        param(41, 30, 20, MEDIUM_BITRATE, id=ZMQ_ID),
        param(41, 30, 50, MEDIUM_BITRATE, id=ZMQ_ID),
    ),
)
@mark.zmq
@mark.flaky(
    reruns=2,
    only_rerun=[
        "failed to start",
        "Attach timeout reached",
        "iperf did not achieve the expected data rate",
        "socket is already closed",
        "failed to connect to all addresses",
        "5GC crashed",
    ],
)
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_zmq(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_32: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    bitrate: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    ZMQ IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_32,
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        iperf_duration=SHORT_DURATION,
        bitrate=bitrate,
        protocol=protocol,
        direction=direction,
        global_timing_advance=0,
        time_alignment_calibration=0,
        always_download_artifacts=False,
        bitrate_threshold=0,
        ue_stop_timeout=1,
        gnb_post_cmd=("log --hex_max_size=32 cu_cp --inactivity_timer=600", ""),
    )


@mark.parametrize(
    "direction",
    (
        param(IPerfDir.DOWNLINK, id="downlink", marks=mark.downlink),
        param(IPerfDir.UPLINK, id="uplink", marks=mark.uplink),
        param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),
    ),
)
@mark.parametrize(
    "protocol",
    (
        param(IPerfProto.UDP, id="udp", marks=mark.udp),
        param(IPerfProto.TCP, id="tcp", marks=mark.tcp),
    ),
)
@mark.parametrize(
    "band, common_scs, bandwidth",
    (
        param(3, 15, 10, id="band:%s-scs:%s-bandwidth:%s"),
        param(41, 30, 10, id="band:%s-scs:%s-bandwidth:%s"),
    ),
)
@mark.rf
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_rf(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_4: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    RF IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_4,
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        iperf_duration=LONG_DURATION,
        protocol=protocol,
        bitrate=MEDIUM_BITRATE,
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration="264",
        always_download_artifacts=False,
        warning_as_errors=False,
    )


# pylint: disable=too-many-arguments,too-many-positional-arguments, too-many-locals
def _iperf(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_array: Sequence[UEStub],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    sample_rate: Optional[int],
    iperf_duration: int,
    bitrate: int,
    protocol: IPerfProto,
    direction: IPerfDir,
    global_timing_advance: int,
    time_alignment_calibration: Union[int, str],
    always_download_artifacts: bool,
    warning_as_errors: bool = True,
    bitrate_threshold: float = 0,  # bitrate != 0
    gnb_post_cmd: Tuple[str, ...] = tuple(),
    plmn: Optional[PLMN] = None,
    common_search_space_enable: bool = False,
    prach_config_index=-1,
    ue_stop_timeout: int = 0,
    rx_to_tx_latency: int = -1,
    enable_dddsu: bool = False,
    nof_antennas_dl: int = 1,
    nof_antennas_ul: int = 1,
    pdsch_mcs_table: str = "qam256",
    pusch_mcs_table: str = "qam256",
    inter_ue_start_period=INTER_UE_START_PERIOD,
    ric: Optional[NearRtRicStub] = None,
    assess_bitrate: bool = False,
):
    wait_before_power_off = 5

    logging.info("Iperf Test")

    configure_test_parameters(
        retina_manager=retina_manager,
        retina_data=retina_data,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=sample_rate,
        global_timing_advance=global_timing_advance,
        time_alignment_calibration=time_alignment_calibration,
        common_search_space_enable=common_search_space_enable,
        prach_config_index=prach_config_index,
        rx_to_tx_latency=rx_to_tx_latency,
        enable_dddsu=enable_dddsu,
        nof_antennas_dl=nof_antennas_dl,
        nof_antennas_ul=nof_antennas_ul,
        pdsch_mcs_table=pdsch_mcs_table,
        pusch_mcs_table=pusch_mcs_table,
    )

    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=always_download_artifacts,
    )

    ue_attach_info_dict = start_and_attach(
        ue_array,
        gnb,
        fivegc,
        gnb_post_cmd=gnb_post_cmd,
        plmn=plmn,
        inter_ue_start_period=inter_ue_start_period,
        ric=ric,
    )

    if ric:
        start_rc_xapp(ric, control_service_style=2, action_id=6)
        start_kpm_mon_xapp(ric, report_service_style=1, metrics="DRB.UEThpDl,DRB.UEThpUl")

    iperf_parallel(
        ue_attach_info_dict,
        fivegc,
        protocol,
        direction,
        iperf_duration,
        bitrate,
        bitrate_threshold,
    )

    if ric:
        stop_rc_xapp(ric)
        stop_kpm_mon_xapp(ric)

    sleep(wait_before_power_off)
    if ric:
        ric_validate_e2_interface(ric, kpm_expected=True, rc_expected=True)

    stop(
        ue_array,
        gnb,
        fivegc,
        retina_data,
        ue_stop_timeout=ue_stop_timeout,
        warning_as_errors=warning_as_errors,
        ric=ric,
    )

    metrics: Metrics = gnb.GetMetrics(Empty())

    if metrics.total.dl_bitrate + metrics.total.ul_bitrate <= 0:
        pytest.fail("No traffic detected in GNB metrics")

    if assess_bitrate and protocol == IPerfProto.UDP:

        min_dl_bitrate = get_max_theoretical_bitrate(
            bandwidth,
            band,
            nof_antennas_dl,
            IPerfDir.DOWNLINK,
            QAMTable.QAM256 if pdsch_mcs_table == "qam256" else QAMTable.QAM64,
        )

        min_ul_bitrate = get_max_theoretical_bitrate(
            bandwidth,
            band,
            1,
            IPerfDir.UPLINK,
            QAMTable.QAM256 if pusch_mcs_table == "qam256" else QAMTable.QAM64,
        )

        # Defines the actual DL/UL bitrate as a percentage of the max theoretical bitrates
        effective_rate_coeff_dl = 0.66  # empirical value
        effective_rate_coeff_ul = 0.46  # empirical value
        # Defines the expected bitrate to be achived as a percentage of the DL/UL bitrates
        expected_to_max_coeff = 0.9

        dl_brate_threshold = min_dl_bitrate * effective_rate_coeff_dl * expected_to_max_coeff
        ul_brate_threshold = min_ul_bitrate * effective_rate_coeff_ul * expected_to_max_coeff

        dl_bitrate_peak_average, ul_bitrate_peak_average = get_peak_average_bitrate(iperf_duration, metrics)
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
            pytest.fail(
                f"Bitrate assessment: DL={"PASSED" if dl_brate_assessment_ok else "FAILED"} UL={"PASSED" if ul_brate_assessment_ok else "FAILED"}"
            )
