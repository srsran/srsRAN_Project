#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Attach / Detach Tests
"""
import logging
from typing import Optional, Sequence, Union

from pytest import mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.utils import configure_artifacts, param
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto
from retina.protocol.ue_pb2_grpc import UEStub

from .steps.configuration import configure_test_parameters
from .steps.stub import iperf_start, iperf_wait_until_finish, start_network, stop, ue_start_and_attach, ue_stop

HIGH_BITRATE = int(15e6)
BITRATE_THRESHOLD: float = 0.1


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
    "band, common_scs, bandwidth, always_download_artifacts",
    (
        param(3, 15, 50, True, id="band:%s-scs:%s-bandwidth:%s-artifacts:%s"),
        param(41, 30, 50, False, id="band:%s-scs:%s-bandwidth:%s-artifacts:%s"),
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
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    ZMQ Attach / Detach
    """

    _attach_and_detach_multi_ues(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue_1, ue_2, ue_3, ue_4),
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        iperf_duration=30,
        bitrate=HIGH_BITRATE,
        protocol=protocol,
        direction=direction,
        global_timing_advance=0,
        time_alignment_calibration=0,
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
    "band, common_scs, bandwidth, always_download_artifacts",
    (
        param(3, 15, 10, True, id="band:%s-scs:%s-bandwidth:%s-artifacts:%s"),
        param(41, 30, 10, False, id="band:%s-scs:%s-bandwidth:%s-artifacts:%s"),
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
    always_download_artifacts: bool,
    direction: IPerfDir,
):
    """
    RF Attach / Detach
    """

    _attach_and_detach_multi_ues(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue_1, ue_2, ue_3, ue_4),
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        iperf_duration=120,
        protocol=IPerfProto.UDP,
        bitrate=HIGH_BITRATE,
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration="auto",
        always_download_artifacts=always_download_artifacts,
        warning_as_errors=False,
    )


# pylint: disable=too-many-arguments,too-many-locals
def _attach_and_detach_multi_ues(
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
    reattach_count: int = 1,
):
    logging.info("Attach / Detach Test")

    configure_test_parameters(
        retina_manager=retina_manager,
        retina_data=retina_data,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=sample_rate,
        global_timing_advance=global_timing_advance,
        time_alignment_calibration=time_alignment_calibration,
        pcap=False,
    )
    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=always_download_artifacts,
    )

    start_network(ue_array, gnb, fivegc)
    ue_attach_info_dict = ue_start_and_attach(ue_array, gnb, fivegc)

    ue_array_to_iperf = ue_array[::2]
    ue_array_to_attach = ue_array[1::2]

    # Starting iperf in half of the UEs
    iperf_array = []
    for ue_stub in ue_array_to_iperf:
        iperf_array.append(
            (
                ue_attach_info_dict[ue_stub],
                *iperf_start(
                    ue_stub,
                    ue_attach_info_dict[ue_stub],
                    fivegc,
                    duration=iperf_duration,
                    direction=direction,
                    protocol=protocol,
                    bitrate=bitrate,
                ),
            )
        )

    # Stop and attach half of the UEs while the others are connecting and doing iperf
    for _ in range(reattach_count):
        ue_stop(ue_array_to_attach, retina_data)
        ue_attach_info_dict = ue_start_and_attach(ue_array_to_attach, gnb, fivegc)
    # final stop will be triggered by teardown

    # Stop and validate iperfs
    for ue_attached_info, task, iperf_request in iperf_array:
        iperf_wait_until_finish(ue_attached_info, fivegc, task, iperf_request, BITRATE_THRESHOLD)

    stop(ue_array, gnb, fivegc, retina_data, warning_as_errors=warning_as_errors)
