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
from typing import Sequence, Union

from pytest import mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.utils import configure_artifacts, param
from retina.protocol.epc_pb2_grpc import EPCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto
from retina.protocol.ue_pb2_grpc import UEStub

from .steps.configuration import configure_test_parameters
from .steps.stub import iperf, start_and_attach

SHORT_DURATION = 20
LONG_DURATION = 5 * 60
LOW_BITRATE = int(1e6)
HIGH_BITRATE = int(15e6)
BITRATE_THRESHOLD: float = 0.1

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
    "band, common_scs, bandwidth, bitrate, always_download_artifacts",
    (
        # Smoke
        param(3, 15, 20, LOW_BITRATE, True, marks=(mark.smoke, mark.zmq), id=ZMQ_ID),
        param(41, 30, 20, LOW_BITRATE, True, marks=(mark.smoke, mark.zmq), id=ZMQ_ID),
        # ZMQ
        param(3, 15, 5, HIGH_BITRATE, False, marks=mark.zmq, id=ZMQ_ID),
        param(3, 15, 10, HIGH_BITRATE, False, marks=mark.zmq, id=ZMQ_ID),
        param(3, 15, 20, HIGH_BITRATE, False, marks=mark.zmq, id=ZMQ_ID),
        param(3, 15, 50, HIGH_BITRATE, True, marks=mark.zmq, id=ZMQ_ID),
        param(41, 30, 10, HIGH_BITRATE, False, marks=mark.zmq, id=ZMQ_ID),
        param(41, 30, 20, HIGH_BITRATE, False, marks=mark.zmq, id=ZMQ_ID),
        param(41, 30, 50, HIGH_BITRATE, True, marks=mark.zmq, id=ZMQ_ID),
    ),
)
# pylint: disable=too-many-arguments
def test_zmq(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_1: UEStub,
    ue_2: UEStub,
    ue_3: UEStub,
    ue_4: UEStub,
    epc: EPCStub,
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
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue_1, ue_2, ue_3, ue_4),
        gnb=gnb,
        epc=epc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        iperf_duration=SHORT_DURATION,
        bitrate=bitrate,
        protocol=protocol,
        direction=direction,
        global_timing_advance=0,
        time_alignment_calibration=0,
        log_search=True,
        always_download_artifacts=always_download_artifacts,
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
        param(3, 15, 10, marks=mark.rf, id="band:%s-scs:%s-bandwidth:%s"),
        param(41, 30, 10, marks=mark.rf, id="band:%s-scs:%s-bandwidth:%s"),
    ),
)
# pylint: disable=too-many-arguments
def test_rf_udp(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_1: UEStub,
    ue_2: UEStub,
    ue_3: UEStub,
    ue_4: UEStub,
    epc: EPCStub,
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
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue_1, ue_2, ue_3, ue_4),
        gnb=gnb,
        epc=epc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        iperf_duration=LONG_DURATION,
        protocol=IPerfProto.UDP,
        bitrate=HIGH_BITRATE,
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration="auto",
        log_search=False,
        always_download_artifacts=True,
    )


# pylint: disable=too-many-arguments, too-many-locals
def _iperf(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_array: Sequence[UEStub],
    epc: EPCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    iperf_duration: int,
    bitrate: int,
    protocol: IPerfProto,
    direction: IPerfDir,
    global_timing_advance: int,
    time_alignment_calibration: Union[int, str],
    log_search: bool,
    always_download_artifacts: bool,
):
    logging.info("Iperf Test")

    configure_test_parameters(
        retina_manager=retina_manager,
        retina_data=retina_data,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        global_timing_advance=global_timing_advance,
        time_alignment_calibration=time_alignment_calibration,
    )
    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=always_download_artifacts,
        log_search=log_search,
    )

    ue_attach_info_dict = start_and_attach(ue_array, gnb, epc)

    iperf(
        ue_attach_info_dict,
        epc,
        protocol,
        direction,
        iperf_duration,
        bitrate,
        BITRATE_THRESHOLD,
    )
