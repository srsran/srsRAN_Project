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

from typing import Tuple

from pytest import mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.utils import param
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ric_pb2_grpc import NearRtRicStub
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto
from retina.protocol.ue_pb2_grpc import UEStub

from .steps.configuration import get_minimum_sample_rate_for_bandwidth
from .steps.iperf_steps import (
    _iperf,
    get_maximum_throughput,
    LONG_DURATION,
    LOW_BITRATE,
    MEDIUM_BITRATE,
    SHORT_DURATION,
    TINY_DURATION,
)

ZMQ_ID = "band:%s-scs:%s-bandwidth:%s-bitrate:%s"


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
        gnb_post_cmd=("", "log --mac_level=info metrics --enable_log_metrics=True"),
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
