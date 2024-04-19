#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Handover Tests
"""
import logging
from time import sleep
from typing import Optional, Sequence, Tuple, Union

from pytest import mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.utils import configure_artifacts, param
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2_grpc import UEStub

from .steps.configuration import configure_test_parameters
from .steps.stub import (
    ping_start,
    ping_wait_until_finish,
    start_network,
    stop,
    ue_expect_handover,
    ue_move,
    ue_start_and_attach,
)


@mark.parametrize(
    "band, common_scs, bandwidth, enable_channel_noise",
    (
        param(3, 15, 50, False, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
        param(41, 30, 50, False, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
        param(3, 15, 50, True, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
        param(41, 30, 50, True, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
    ),
)
@mark.zmq
# @mark.flaky(reruns=2, only_rerun=["failed to start", "Attach timeout reached", "StatusCode.ABORTED"])
# pylint: disable=too-many-arguments
def test_zmq_handover(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_8: UEStub,
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    enable_channel_noise: bool,
):
    """
    ZMQ Handover tests
    """
    _handover_multi_ues(
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
        enable_channel_noise=enable_channel_noise,
        warning_as_errors=True,
    )


# pylint: disable=too-many-arguments,too-many-locals
def _handover_multi_ues(
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
    enable_channel_noise: bool,
    warning_as_errors: bool = True,
    movement_steps: int = 10,
    sleep_between_movement_steps: int = 1,
    cell_position_offset: Tuple[float, float, float] = (1000, 0, 0),
):
    logging.info("Hanover Test")

    original_position = (0, 0, 0)

    configure_test_parameters(
        retina_manager=retina_manager,
        retina_data=retina_data,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=sample_rate,
        global_timing_advance=global_timing_advance,
        time_alignment_calibration=time_alignment_calibration,
        enable_channel_noise=enable_channel_noise,
        num_cells=2,
        cell_position_offset=cell_position_offset,
    )

    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=always_download_artifacts,
    )

    start_network(ue_array, gnb, fivegc)

    ue_attach_info_dict = ue_start_and_attach(ue_array, gnb, fivegc)

    # HO while pings
    movement_duration = (movement_steps + 1) * sleep_between_movement_steps
    movements = (
        (original_position, cell_position_offset),
        (cell_position_offset, original_position),
        (original_position, cell_position_offset),
        (cell_position_offset, original_position),
    )

    for index, ue_stub in enumerate(ue_array):
        logging.info(
            "Zigzag HO for UE [%s] (%s) + Pings running in background for all UEs",
            id(ue_stub),
            ue_attach_info_dict[ue_stub].ipv4,
        )
        ping_task_array = ping_start(ue_attach_info_dict, fivegc, (len(movements) * movement_duration) + len(ue_array))
        for from_position, to_position in movements:
            _do_ho(ue_stub, from_position, to_position, movement_steps, sleep_between_movement_steps)
        ping_wait_until_finish(ping_task_array, index)

    # Pings after reest
    logging.info("Starting Pings after all HO have been completed")
    ping_wait_until_finish(ping_start(ue_attach_info_dict, fivegc, movement_duration))

    stop(ue_array, gnb, fivegc, retina_data, warning_as_errors=warning_as_errors)


def _do_ho(
    ue_stub: UEStub,
    from_position: Tuple[float, float, float],
    to_position: Tuple[float, float, float],
    steps: int,
    sleep_between_steps: int,
):
    logging.info("Moving UE [%s] from %s to %s", id(ue_stub), from_position, to_position)

    ho_task = ue_expect_handover(ue_stub, ((steps + 1) * sleep_between_steps))

    for i in range(steps + 1):
        ue_move(
            ue_stub,
            (int(round(from_position[0] + (i * (to_position[0] - from_position[0]) / steps)))),
            (int(round(from_position[1] + (i * (to_position[1] - from_position[1]) / steps)))),
            (int(round(from_position[2] + (i * (to_position[2] - from_position[2]) / steps)))),
        )
        sleep(sleep_between_steps)

    # We check again the future's result here so it can raise an exception if the HO failed
    ho_task.result()
