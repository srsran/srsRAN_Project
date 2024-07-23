#
# Copyright 2021-2024 Software Radio Systems Limited
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
Handover Tests
"""
import logging
from contextlib import contextmanager
from time import sleep
from typing import Dict, Generator, Optional, Sequence, Tuple, Union

import pytest
from pytest import mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.public import MetricsSummary
from retina.launcher.utils import configure_artifacts, param
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2 import UEAttachedInfo
from retina.protocol.ue_pb2_grpc import UEStub

from .steps.configuration import configure_test_parameters
from .steps.kpis import get_kpis
from .steps.stub import (
    ping_start,
    ping_wait_until_finish,
    start_network,
    stop,
    ue_expect_handover,
    ue_move,
    ue_start_and_attach,
    ue_validate_no_reattaches,
)


@mark.zmq
@mark.smoke
def test_smoke_sequentially(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_2: UEStub,
    fivegc: FiveGCStub,
    gnb: GNBStub,
):
    """
    ZMQ Handover tests
    """
    _handover_sequentially(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_2,
        fivegc=fivegc,
        gnb=gnb,
        metrics_summary=None,
        band=41,
        common_scs=30,
        bandwidth=50,
        noise_spd=0,
        always_download_artifacts=False,
    )


@mark.parametrize(
    "band, common_scs, bandwidth, noise_spd",
    (
        param(3, 15, 50, 0, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
        param(41, 30, 50, 0, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
        # param(3, 15, 50, -74, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
        # param(41, 30, 50, -74, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
    ),
)
@mark.zmq
@mark.flaky(reruns=2, only_rerun=["failed to start", "Attach timeout reached", "StatusCode.ABORTED"])
# pylint: disable=too-many-arguments
def test_zmq_handover_sequentially(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_8: UEStub,
    fivegc: FiveGCStub,
    gnb: GNBStub,
    metrics_summary: MetricsSummary,
    band: int,
    common_scs: int,
    bandwidth: int,
    noise_spd: int,
):
    """
    ZMQ Handover tests
    """
    _handover_sequentially(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_8,
        fivegc=fivegc,
        gnb=gnb,
        metrics_summary=metrics_summary,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        noise_spd=noise_spd,
    )


# pylint: disable=too-many-arguments,too-many-locals
def _handover_sequentially(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_array: UEStub,
    fivegc: FiveGCStub,
    gnb: GNBStub,
    metrics_summary: Optional[MetricsSummary],
    band: int,
    common_scs: int,
    bandwidth: int,
    noise_spd: int,
    always_download_artifacts: bool = True,
):
    with _handover_multi_ues(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_array,
        gnb=gnb,
        fivegc=fivegc,
        metrics_summary=metrics_summary,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        global_timing_advance=0,
        time_alignment_calibration=0,
        always_download_artifacts=always_download_artifacts,
        noise_spd=noise_spd,
        warning_as_errors=True,
    ) as (ue_attach_info_dict, movements, traffic_seconds):

        for ue_stub, ue_attach_info in ue_attach_info_dict.items():
            logging.info(
                "Zigzag HO for UE [%s] (%s) + Pings running in background for all UEs",
                id(ue_stub),
                ue_attach_info.ipv4,
            )

            ping_task_array = ping_start(ue_attach_info_dict, fivegc, traffic_seconds)

            for from_position, to_position, movement_steps, sleep_between_movement_steps in movements:
                _do_ho((ue_stub,), from_position, to_position, movement_steps, sleep_between_movement_steps)

            ping_wait_until_finish(ping_task_array)


@mark.parametrize(
    "band, common_scs, bandwidth, noise_spd",
    (
        param(3, 15, 50, 0, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
        param(41, 30, 50, 0, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
        # param(3, 15, 50, -74, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
        # param(41, 30, 50, -74, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
    ),
)
@mark.zmq
@mark.flaky(reruns=2, only_rerun=["failed to start", "Attach timeout reached", "StatusCode.ABORTED"])
# pylint: disable=too-many-arguments,too-many-locals
def test_zmq_handover_parallel(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_8: UEStub,
    fivegc: FiveGCStub,
    gnb: GNBStub,
    metrics_summary: MetricsSummary,
    band: int,
    common_scs: int,
    bandwidth: int,
    noise_spd: int,
):
    """
    ZMQ Handover tests
    """
    with _handover_multi_ues(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_8,
        gnb=gnb,
        fivegc=fivegc,
        metrics_summary=metrics_summary,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        global_timing_advance=0,
        time_alignment_calibration=0,
        always_download_artifacts=True,
        noise_spd=noise_spd,
        warning_as_errors=True,
    ) as (ue_attach_info_dict, movements, traffic_seconds):

        logging.info(
            "Zigzag HO for all UEs + Pings running in background for all UEs",
        )

        ping_task_array = ping_start(ue_attach_info_dict, fivegc, traffic_seconds)

        for from_position, to_position, movement_steps, sleep_between_movement_steps in movements:
            _do_ho(ue_8, from_position, to_position, movement_steps, sleep_between_movement_steps)

        ping_wait_until_finish(ping_task_array)


# pylint: disable=too-many-arguments,too-many-locals
@contextmanager
def _handover_multi_ues(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_array: Sequence[UEStub],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    metrics_summary: Optional[MetricsSummary],
    band: int,
    common_scs: int,
    bandwidth: int,
    sample_rate: Optional[int],
    global_timing_advance: int,
    time_alignment_calibration: Union[int, str],
    always_download_artifacts: bool,
    noise_spd: int,
    warning_as_errors: bool = True,
    movement_steps: int = 10,
    sleep_between_movement_steps: int = 2,
    cell_position_offset: Tuple[float, float, float] = (1000, 0, 0),
) -> Generator[
    Tuple[
        Dict[UEStub, UEAttachedInfo],
        Tuple[Tuple[Tuple[float, float, float], Tuple[float, float, float], int, int], ...],
        int,
    ],
    None,
    None,
]:
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
        noise_spd=noise_spd,
        num_cells=2,
        cell_position_offset=cell_position_offset,
        log_ip_level="debug",
    )

    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=always_download_artifacts,
    )

    start_network(ue_array, gnb, fivegc, gnb_post_cmd=("log --cu_level=debug", "log --mac_level=debug"))

    ue_attach_info_dict = ue_start_and_attach(ue_array, gnb, fivegc)

    try:
        # HO while pings
        movement_duration = (movement_steps + 1) * sleep_between_movement_steps
        movements: Tuple[Tuple[Tuple[float, float, float], Tuple[float, float, float], int, int], ...] = (
            (original_position, cell_position_offset, movement_steps, sleep_between_movement_steps),
            (cell_position_offset, original_position, movement_steps, sleep_between_movement_steps),
            (original_position, cell_position_offset, movement_steps, sleep_between_movement_steps),
            (cell_position_offset, original_position, movement_steps, sleep_between_movement_steps),
        )
        traffic_seconds = (len(movements) * movement_duration) + len(ue_array)

        yield ue_attach_info_dict, movements, traffic_seconds

        # Pings after handover
        logging.info("Starting Pings after all HO have been completed")
        ping_wait_until_finish(ping_start(ue_attach_info_dict, fivegc, movement_duration))

        for ue_stub in ue_array:
            ue_validate_no_reattaches(ue_stub)

        stop(ue_array, gnb, fivegc, retina_data, warning_as_errors=warning_as_errors)
    finally:
        get_kpis(gnb, ue_array=ue_array, metrics_summary=metrics_summary)


def _do_ho(
    ue_array: Tuple[UEStub, ...],
    from_position: Tuple[float, float, float],
    to_position: Tuple[float, float, float],
    steps: int,
    sleep_between_steps: int,
    extra_time: int = 10,
):
    for ue_stub in ue_array:
        logging.info("Moving UE [%s] from %s to %s", id(ue_stub), from_position, to_position)

    ho_task_array = [
        ue_expect_handover(ue_stub, ((steps + 1) * sleep_between_steps) + extra_time) for ue_stub in ue_array
    ]

    for i in range(steps + 1):
        for ue_stub in ue_array:
            ue_move(
                ue_stub,
                (int(round(from_position[0] + (i * (to_position[0] - from_position[0]) / steps)))),
                (int(round(from_position[1] + (i * (to_position[1] - from_position[1]) / steps)))),
                (int(round(from_position[2] + (i * (to_position[2] - from_position[2]) / steps)))),
            )
        sleep(sleep_between_steps)

    # We check again the future's result here so it can raise an exception if the HO failed
    if not all((task.result().status for task in ho_task_array)):
        pytest.fail("Handover UE failed")
