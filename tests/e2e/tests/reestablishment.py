#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Ping + Reestablishment Tests
"""
import logging
from contextlib import suppress
from typing import Optional, Sequence, Union

from _pytest.outcomes import Failed
from pytest import mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.utils import configure_artifacts, param
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto
from retina.protocol.ue_pb2_grpc import UEStub

from .steps.configuration import configure_test_parameters
from .steps.stub import (
    iperf_start,
    iperf_wait_until_finish,
    ping_start,
    ping_wait_until_finish,
    start_network,
    stop,
    ue_reestablishment,
    ue_start_and_attach,
    ue_validate_no_reattaches,
)


@mark.parametrize(
    "band, common_scs, bandwidth, noise_spd",
    (
        param(3, 15, 50, 0, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
        param(41, 30, 50, 0, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
        param(3, 15, 50, -74, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
        param(41, 30, 50, -74, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
    ),
)
@mark.zmq
@mark.flaky(reruns=2, only_rerun=["failed to start", "Attach timeout reached", "StatusCode.ABORTED"])
# pylint: disable=too-many-arguments
def test_zmq_reestablishment(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_32: UEStub,
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    noise_spd: int,
):
    """
    ZMQ Ping + Reestablishment
    """

    _ping_and_reestablishment_multi_ues(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_32,
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        global_timing_advance=0,
        time_alignment_calibration=0,
        always_download_artifacts=True,
        noise_spd=noise_spd,
        traffic_duration=50,
        reestablishment_interval=10,
        warning_as_errors=True,
    )


@mark.parametrize(
    "direction",
    (
        param(IPerfDir.DOWNLINK, id="downlink", marks=mark.downlink),
        param(IPerfDir.UPLINK, id="uplink", marks=mark.uplink),
    ),
)
@mark.parametrize(
    "protocol",
    (param(IPerfProto.UDP, id="udp", marks=mark.udp),),
)
@mark.parametrize(
    "band, common_scs, bandwidth, noise_spd",
    (param(3, 15, 50, 0, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),),
)
@mark.zmq
@mark.flaky(reruns=2, only_rerun=["failed to start", "Attach timeout reached", "StatusCode.ABORTED"])
# pylint: disable=too-many-arguments
def test_zmq_reestablishment_full_rate(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_8: UEStub,
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    noise_spd: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    ZMQ IPerf + Reestablishment
    """

    _iperf_and_reestablishment_multi_ues(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_8,
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        global_timing_advance=0,
        time_alignment_calibration=0,
        always_download_artifacts=True,
        noise_spd=noise_spd,
        protocol=protocol,
        direction=direction,
        traffic_duration=50,
        reestablishment_interval=10,
        warning_as_errors=True,
    )


# pylint: disable=too-many-arguments,too-many-locals
def _ping_and_reestablishment_multi_ues(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_array: Sequence[UEStub],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    sample_rate: Optional[int],
    global_timing_advance: int,
    time_alignment_calibration: Union[int, str],
    always_download_artifacts: bool,
    noise_spd: int,
    traffic_duration: int,
    reestablishment_interval: int,
    warning_as_errors: bool,
):

    for reest_ue_attach_info_dict, other_ue_attach_info_dict in _iterator_over_attached_ues(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_array,
        fivegc=fivegc,
        gnb=gnb,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=sample_rate,
        global_timing_advance=global_timing_advance,
        time_alignment_calibration=time_alignment_calibration,
        always_download_artifacts=always_download_artifacts,
        noise_spd=noise_spd,
        warning_as_errors=warning_as_errors,
    ):
        # Launch reestablished UEs
        ping_task_array_reest = ping_start(reest_ue_attach_info_dict, fivegc, traffic_duration, time_step=0)
        # Launch other UEs
        ping_task_array_other = ping_start(other_ue_attach_info_dict, fivegc, traffic_duration, time_step=0)

        # Trigger reestablishments
        for ue_stub in reest_ue_attach_info_dict:
            for _ in range(int(traffic_duration / reestablishment_interval)):
                ue_reestablishment(ue_stub, reestablishment_interval)

        # Wait and ignore reestablished UEs
        with suppress(Failed):
            ping_wait_until_finish(ping_task_array_reest)

        # Wait and validate other UEs
        ping_wait_until_finish(ping_task_array_other)


# pylint: disable=too-many-arguments,too-many-locals
def _iperf_and_reestablishment_multi_ues(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_array: Sequence[UEStub],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    sample_rate: Optional[int],
    global_timing_advance: int,
    time_alignment_calibration: Union[int, str],
    always_download_artifacts: bool,
    noise_spd: int,
    protocol: IPerfProto,
    direction: IPerfDir,
    traffic_duration: int,
    reestablishment_interval: int,
    warning_as_errors: bool,
):

    for reest_ue_attach_info_dict, other_ue_attach_info_dict in _iterator_over_attached_ues(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_array,
        fivegc=fivegc,
        gnb=gnb,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=sample_rate,
        global_timing_advance=global_timing_advance,
        time_alignment_calibration=time_alignment_calibration,
        always_download_artifacts=always_download_artifacts,
        noise_spd=noise_spd,
        warning_as_errors=warning_as_errors,
    ):
        # Launch iperf for all UEs
        iperf_dict = tuple(
            (
                ue_attached_info,
                *iperf_start(ue_stub, ue_attached_info, fivegc, protocol, direction, traffic_duration, 0),
            )
            for ue_stub, ue_attached_info in {**reest_ue_attach_info_dict, **other_ue_attach_info_dict}.items()
        )

        # Trigger reestablishments
        for ue_stub in reest_ue_attach_info_dict:
            for _ in range(int(traffic_duration / reestablishment_interval)):
                ue_reestablishment(ue_stub, reestablishment_interval)

        # Wait for reestablished UEs
        for ue_attached_info, task, iperf_request in iperf_dict:
            iperf_wait_until_finish(ue_attached_info, fivegc, task, iperf_request)


def _iterator_over_attached_ues(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_array: Sequence[UEStub],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    sample_rate: Optional[int],
    global_timing_advance: int,
    time_alignment_calibration: Union[int, str],
    always_download_artifacts: bool,
    noise_spd: int,
    warning_as_errors: bool = True,
):

    logging.info("Reestablishment Test")

    configure_test_parameters(
        retina_manager=retina_manager,
        retina_data=retina_data,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=sample_rate,
        global_timing_advance=global_timing_advance,
        time_alignment_calibration=time_alignment_calibration,
        noise_spd=noise_spd,
        enable_qos_reestablishment=True,
    )

    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=always_download_artifacts,
    )

    start_network(ue_array, gnb, fivegc, gnb_post_cmd="log --mac_level=debug --cu_level=debug")

    ue_attach_info_dict = ue_start_and_attach(ue_array, gnb, fivegc)

    # Reestablishment while traffic
    for ue_stub in ue_array:
        logging.info(
            "Starting Reestablishment for UE [%s] (%s) + Traffic running in background for all UEs",
            id(ue_stub),
            ue_attach_info_dict[ue_stub].ipv4,
        )
        other_ue_attach_info_dict = dict(ue_attach_info_dict)
        other_ue_attach_info_dict.pop(ue_stub)

        yield {ue_stub: ue_attach_info_dict[ue_stub]}, other_ue_attach_info_dict

    # Pings after reest
    logging.info("Starting traffic after all reestablishments have been completed")
    yield {}, ue_attach_info_dict

    for ue_stub in ue_array:
        ue_validate_no_reattaches(ue_stub)

    stop(ue_array, gnb, fivegc, retina_data, warning_as_errors=warning_as_errors)
