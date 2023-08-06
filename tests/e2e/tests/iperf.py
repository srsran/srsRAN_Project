#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Test Iperf
"""

import logging
from contextlib import suppress
from typing import Optional, Sequence, Union

from pytest import mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.utils import configure_artifacts, param
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto
from retina.protocol.ue_pb2_grpc import UEStub
from retina.reporter.metric_manager import MetricManager

from .steps.configuration import configure_test_parameters, get_minimum_sample_rate_for_bandwidth
from .steps.stub import iperf, start_and_attach, StartFailure, stop
from .utils import get_current_pytest_suite_name, get_current_pytest_test_name

TINY_DURATION = 5
SHORT_DURATION = 20
LONG_DURATION = 5 * 60
LOW_BITRATE = int(1e6)
MEDIUM_BITRATE = int(15e6)
HIGH_BITRATE = int(50e6)
MEDIUM_BITRATE_THRESHOLD: float = 0.1
HIGH_BITRATE_THRESHOLD: float = 0.5

ZMQ_ID = "band:%s-scs:%s-bandwidth:%s-bitrate:%s-artifacts:%s"


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
        # param(78, 30, 20, id="band:%s-scs:%s-bandwidth:%s"),
    ),
)
@mark.android
# pylint: disable=too-many-arguments
def test_android(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_1: UEStub,
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
        reporter=None,
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue_1,),
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=get_minimum_sample_rate_for_bandwidth(bandwidth),
        iperf_duration=SHORT_DURATION,
        protocol=protocol,
        bitrate=MEDIUM_BITRATE,
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
    (param(78, 30, 40, id="band:%s-scs:%s-bandwidth:%s"),),
)
@mark.android_hp
# pylint: disable=too-many-arguments
def test_android_2x2_mimo(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    reporter: MetricManager,
    ue_1: UEStub,
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
        reporter=reporter,
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue_1,),
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,
        antennas_dl=2,
        iperf_duration=SHORT_DURATION,
        protocol=protocol,
        bitrate=HIGH_BITRATE,
        bitrate_threshold=HIGH_BITRATE_THRESHOLD,
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
    "band, common_scs, bandwidth, bitrate, always_download_artifacts",
    (
        param(3, 15, 20, LOW_BITRATE, True, id=ZMQ_ID),
        param(41, 30, 20, LOW_BITRATE, True, id=ZMQ_ID),
    ),
)
@mark.zmq
@mark.smoke
# pylint: disable=too-many-arguments
def test_zmq_smoke(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_1: UEStub,
    ue_2: UEStub,
    ue_3: UEStub,
    ue_4: UEStub,
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    always_download_artifacts: bool,
    bitrate: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    ZMQ IPerfs
    """

    _iperf(
        reporter=None,
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue_1, ue_2, ue_3, ue_4),
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
        always_download_artifacts=always_download_artifacts,
        bitrate_threshold=0,
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
    "band, common_scs, bandwidth, bitrate, always_download_artifacts",
    (
        # ZMQ
        param(3, 15, 5, MEDIUM_BITRATE, False, id=ZMQ_ID),
        param(3, 15, 10, MEDIUM_BITRATE, False, id=ZMQ_ID),
        param(3, 15, 20, MEDIUM_BITRATE, False, id=ZMQ_ID),
        param(3, 15, 50, MEDIUM_BITRATE, True, id=ZMQ_ID),
        param(41, 30, 10, MEDIUM_BITRATE, False, id=ZMQ_ID),
        param(41, 30, 20, MEDIUM_BITRATE, False, id=ZMQ_ID),
        param(41, 30, 50, MEDIUM_BITRATE, True, id=ZMQ_ID),
    ),
)
@mark.zmq
# pylint: disable=too-many-arguments
def test_zmq(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_1: UEStub,
    ue_2: UEStub,
    ue_3: UEStub,
    ue_4: UEStub,
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    always_download_artifacts: bool,
    bitrate: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    ZMQ IPerfs
    """

    _iperf(
        reporter=None,
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue_1, ue_2, ue_3, ue_4),
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
        always_download_artifacts=always_download_artifacts,
        bitrate_threshold=0,
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
    "band, common_scs, bandwidth",
    (
        param(3, 15, 10, id="band:%s-scs:%s-bandwidth:%s"),
        param(41, 30, 10, id="band:%s-scs:%s-bandwidth:%s"),
    ),
)
@mark.rf
# pylint: disable=too-many-arguments
def test_rf_udp(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_1: UEStub,
    ue_2: UEStub,
    ue_3: UEStub,
    ue_4: UEStub,
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    direction: IPerfDir,
):
    """
    RF IPerfs
    """

    _iperf(
        reporter=None,
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue_1, ue_2, ue_3, ue_4),
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        iperf_duration=LONG_DURATION,
        protocol=IPerfProto.UDP,
        bitrate=MEDIUM_BITRATE,
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration="auto",
        always_download_artifacts=True,
        warning_as_errors=False,
    )


# pylint: disable=too-many-arguments, too-many-locals
def _iperf(
    reporter: Union[MetricManager, None],
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
    bitrate_threshold: float = MEDIUM_BITRATE_THRESHOLD,
    gnb_post_cmd: str = "",
    antennas_dl: int = 1,
):
    logging.info("Iperf Test")

    with suppress(StartFailure):
        configure_test_parameters(
            retina_manager=retina_manager,
            retina_data=retina_data,
            band=band,
            common_scs=common_scs,
            bandwidth=bandwidth,
            sample_rate=sample_rate,
            global_timing_advance=global_timing_advance,
            time_alignment_calibration=time_alignment_calibration,
            antennas_dl=antennas_dl,
            pcap=False,
        )
        configure_artifacts(
            retina_data=retina_data,
            always_download_artifacts=always_download_artifacts,
        )

        ue_attach_info_dict = start_and_attach(ue_array, gnb, fivegc, gnb_post_cmd=gnb_post_cmd)

        iperf_result = iperf(
            ue_attach_info_dict,
            fivegc,
            protocol,
            direction,
            iperf_duration,
            bitrate,
            bitrate_threshold,
        )

        if reporter is not None:
            test_name = get_current_pytest_test_name()
            test_suite = get_current_pytest_suite_name()

            for i, iperf_result_inst in enumerate(iperf_result):
                if direction in (IPerfDir.DOWNLINK, IPerfDir.BIDIRECTIONAL):
                    reporter.write_db_direct(
                        test_suite,
                        test_name,
                        f"ue_{i}",
                        "Downlink",
                        [str(iperf_result_inst.downlink.bits_per_second)],
                    )
                elif direction in (IPerfDir.UPLINK, IPerfDir.BIDIRECTIONAL):
                    reporter.write_db_direct(
                        test_suite,
                        test_name,
                        f"ue_{i}",
                        "Uplink",
                        [str(iperf_result_inst.uplink.bits_per_second)],
                    )

        stop(ue_array, gnb, fivegc, retina_data, warning_as_errors=warning_as_errors)
