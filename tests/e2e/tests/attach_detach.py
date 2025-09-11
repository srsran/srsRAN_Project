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
Attach / Detach Tests
"""
import logging
from time import sleep
from typing import Optional, Sequence, Tuple, Union

from google.protobuf.empty_pb2 import Empty
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


@mark.zmq
@mark.smoke
def test_smoke(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_4: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
):
    """
    Smoke ZMQ Attach / Detach
    """
    _attach_and_detach_multi_ues(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_4,
        gnb=gnb,
        fivegc=fivegc,
        band=41,
        common_scs=30,
        bandwidth=50,
        sample_rate=None,
        bitrate=HIGH_BITRATE,
        protocol=IPerfProto.UDP,
        direction=IPerfDir.BIDIRECTIONAL,
        global_timing_advance=0,
        time_alignment_calibration=0,
        ue_stop_timeout=15,
        always_download_artifacts=False,
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
        param(3, 15, 50, id="band:%s-scs:%s-bandwidth:%s"),
        param(41, 30, 50, id="band:%s-scs:%s-bandwidth:%s"),
    ),
)
@mark.zmq
@mark.flaky(reruns=3, only_rerun=["failed to start", "IPerf Data Invalid"])
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_zmq(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_8: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    ZMQ Attach / Detach
    """

    _attach_and_detach_multi_ues(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_8,
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        bitrate=HIGH_BITRATE,
        protocol=protocol,
        direction=direction,
        global_timing_advance=0,
        time_alignment_calibration=0,
        always_download_artifacts=False,
        ue_stop_timeout=45,
        ue_settle_time=45,
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
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_rf_udp(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_4: Tuple[UEStub, ...],
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
        ue_array=ue_4,
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        protocol=IPerfProto.UDP,
        bitrate=HIGH_BITRATE,
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration="264",
        always_download_artifacts=always_download_artifacts,
        warning_as_errors=False,
    )


# pylint: disable=too-many-arguments,too-many-positional-arguments,too-many-locals
def _attach_and_detach_multi_ues(
    *,  # This enforces keyword-only arguments
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_array: Sequence[UEStub],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    sample_rate: Optional[int],
    bitrate: int,
    protocol: IPerfProto,
    direction: IPerfDir,
    global_timing_advance: int,
    time_alignment_calibration: Union[int, str],
    always_download_artifacts: bool,
    warning_as_errors: bool = True,
    reattach_count: int = 1,
    ue_stop_timeout: int = 30,
    ue_settle_time: int = 0,
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
    )
    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=always_download_artifacts,
    )

    start_network(ue_array=ue_array, gnb=gnb, fivegc=fivegc)
    ue_attach_info_dict = ue_start_and_attach(
        ue_array=ue_array, du_definition=[gnb.GetDefinition(Empty())], fivegc=fivegc
    )

    ue_array_to_iperf = ue_array[::2]
    ue_array_to_attach = ue_array[1::2]

    iperf_duration = reattach_count * ((ue_stop_timeout * len(ue_array_to_attach)) + ue_settle_time)

    # Starting iperf in half of the UEs
    iperf_array = []
    for ue_stub in ue_array_to_iperf:
        iperf_array.append(
            (
                ue_attach_info_dict[ue_stub],
                *iperf_start(
                    ue_stub=ue_stub,
                    ue_attached_info=ue_attach_info_dict[ue_stub],
                    fivegc=fivegc,
                    protocol=protocol,
                    direction=direction,
                    duration=iperf_duration,
                    bitrate=bitrate,
                ),
            )
        )

    # Stop and attach half of the UEs while the others are connecting and doing iperf
    for _ in range(reattach_count):
        ue_stop(ue_array=ue_array_to_attach, retina_data=retina_data, ue_stop_timeout=ue_stop_timeout)
        sleep(ue_settle_time)
        ue_attach_info_dict = ue_start_and_attach(
            ue_array=ue_array_to_attach, du_definition=[gnb.GetDefinition(Empty())], fivegc=fivegc
        )
    # final stop will be triggered by teardown

    # Stop and validate iperfs
    for ue_attached_info, task, iperf_request in iperf_array:
        iperf_wait_until_finish(
            ue_attached_info=ue_attached_info,
            fivegc=fivegc,
            task=task,
            iperf_request=iperf_request,
            bitrate_threshold_ratio=BITRATE_THRESHOLD,
        )

    stop(
        ue_array=ue_array,
        gnb=gnb,
        fivegc=fivegc,
        retina_data=retina_data,
        ue_stop_timeout=ue_stop_timeout,
        warning_as_errors=warning_as_errors,
    )
