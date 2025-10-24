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
Ping + Reestablishment Tests
"""
import logging
from contextlib import contextmanager
from typing import Dict, Generator, Optional, Sequence, Tuple, Union

import pytest
from google.protobuf.empty_pb2 import Empty
from pytest import mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.public import MetricsSummary
from retina.launcher.utils import configure_artifacts, param
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto, UEAttachedInfo
from retina.protocol.ue_pb2_grpc import UEStub

from .steps.configuration import configure_test_parameters
from .steps.kpis import get_kpis
from .steps.stub import (
    iperf_parallel,
    iperf_sequentially,
    iperf_start,
    iperf_wait_until_finish,
    ping_start,
    ping_wait_until_finish,
    start_network,
    stop,
    ue_reestablishment,
    ue_reestablishment_parallel,
    ue_start_and_attach,
    ue_validate_no_reattaches,
)

_ONLY_RERUN = [
    "failed to start",
    "Attach timeout reached",
    "StatusCode.ABORTED",
    "socket is already closed",
    "StatusCode.UNKNOWN",
]


@mark.zmq
@mark.smoke
@mark.flaky(reruns=2, only_rerun=["StatusCode.UNKNOWN"])
def test_smoke_sequentially(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_2: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
):
    """
    ZMQ Ping + Reestablishment
    """
    _reestablishment_sequentially_ping(
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
        param(3, 15, 50, -74, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
        param(41, 30, 50, -74, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
    ),
)
@mark.zmq
@mark.flaky(reruns=2, only_rerun=_ONLY_RERUN)
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_zmq_sequentially(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_8: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    metrics_summary: MetricsSummary,
    band: int,
    common_scs: int,
    bandwidth: int,
    noise_spd: int,
):
    """
    ZMQ Ping + Reestablishment
    """
    _reestablishment_sequentially_ping(
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


# pylint: disable=too-many-arguments,too-many-positional-arguments,too-many-locals
def _reestablishment_sequentially_ping(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_array: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    metrics_summary: Optional[MetricsSummary],
    band: int,
    common_scs: int,
    bandwidth: int,
    noise_spd: int,
    always_download_artifacts: bool = True,
):
    """
    ZMQ Ping + Reestablishment
    """
    traffic_duration = 15
    reestablishment_interval = 5

    for reest_ue_attach_info_dict, other_ue_attach_info_dict in _iterator_over_attached_ues(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_array,
        fivegc=fivegc,
        gnb=gnb,
        metrics_summary=metrics_summary,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,
        global_timing_advance=0,
        time_alignment_calibration=0,
        always_download_artifacts=always_download_artifacts,
        noise_spd=noise_spd,
        log_ip_level="debug",
        warning_as_errors=True,
    ):
        # Launch pings
        ping_task_array = ping_start(
            ue_attach_info_dict={**reest_ue_attach_info_dict, **other_ue_attach_info_dict},
            fivegc=fivegc,
            ping_count=traffic_duration,
        )

        # Trigger reestablishments
        for ue_stub in reest_ue_attach_info_dict:
            for _ in range(int(traffic_duration / reestablishment_interval)):
                ue_reestablishment(ue_stub=ue_stub, reestablishment_interval=reestablishment_interval)

        # Wait and validate pings
        ping_wait_until_finish(ping_task_array)


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
@mark.flaky(reruns=2, only_rerun=_ONLY_RERUN)
# pylint: disable=too-many-arguments,too-many-positional-arguments,too-many-locals
def test_zmq_sequentially_full_rate(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_8: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    metrics_summary: MetricsSummary,
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
    traffic_duration = 15
    reestablishment_interval = 5

    for reest_ue_attach_info_dict, other_ue_attach_info_dict in _iterator_over_attached_ues(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_8,
        fivegc=fivegc,
        gnb=gnb,
        metrics_summary=metrics_summary,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        global_timing_advance=0,
        time_alignment_calibration=0,
        always_download_artifacts=True,
        noise_spd=noise_spd,
        log_ip_level="",
        warning_as_errors=True,
    ):
        # Launch iperf for all UEs
        iperf_dict = tuple(
            (
                ue_attached_info,
                *iperf_start(
                    ue_stub=ue_stub,
                    ue_attached_info=ue_attached_info,
                    fivegc=fivegc,
                    protocol=protocol,
                    direction=direction,
                    duration=traffic_duration,
                    bitrate=0,
                ),
            )
            for ue_stub, ue_attached_info in {**reest_ue_attach_info_dict, **other_ue_attach_info_dict}.items()
        )

        # Trigger reestablishments
        for ue_stub in reest_ue_attach_info_dict:
            for _ in range(int(traffic_duration / reestablishment_interval)):
                ue_reestablishment(ue_stub=ue_stub, reestablishment_interval=reestablishment_interval)

        # Wait for reestablished UEs
        for ue_attached_info, task, iperf_request in iperf_dict:
            iperf_wait_until_finish(
                ue_attached_info=ue_attached_info, fivegc=fivegc, task=task, iperf_request=iperf_request
            )


@mark.zmq
@mark.flaky(reruns=2, only_rerun=_ONLY_RERUN)
def test_zmq_sequentially_full_rate_verify_bitrate(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    metrics_summary: MetricsSummary,
):
    """
    ZMQ IPerf + Reestablishment + Check Bitrate
    """

    protocol = IPerfProto.UDP
    direction = IPerfDir.UPLINK
    bitrate = int(45e6)
    bitrate_threshold = 0.8
    traffic_duration = 15
    reestablishment_interval = 5

    with _test_reestablishments(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=[ue],
        fivegc=fivegc,
        metrics_summary=metrics_summary,
        gnb=gnb,
        band=3,
        common_scs=15,
        bandwidth=50,
        sample_rate=None,
        global_timing_advance=0,
        time_alignment_calibration=0,
        always_download_artifacts=True,
        noise_spd=0,
        log_ip_level="",
        warning_as_errors=True,
    ) as ue_attach_info_dict:

        # Iperf before reestablishment
        _, result_before = iperf_sequentially(
            ue_stub=ue,
            ue_attached_info=ue_attach_info_dict[ue],
            fivegc=fivegc,
            protocol=protocol,
            direction=direction,
            iperf_duration=traffic_duration,
            bitrate=bitrate,
        )

        # Reestablishment
        for _ in range(int(traffic_duration / reestablishment_interval)):
            ue_reestablishment(ue_stub=ue, reestablishment_interval=reestablishment_interval)

        # Iperf after reestablishment
        _, result_after = iperf_sequentially(
            ue_stub=ue,
            ue_attached_info=ue_attach_info_dict[ue],
            fivegc=fivegc,
            protocol=protocol,
            direction=direction,
            iperf_duration=traffic_duration,
            bitrate=bitrate,
        )

        # Validate bitrate
        bitrate_criteria = True
        if direction in (IPerfDir.DOWNLINK, IPerfDir.BIDIRECTIONAL):
            if result_after.downlink.bits_per_second / result_before.downlink.bits_per_second < bitrate_threshold:
                bitrate_criteria = False
                logging.error(
                    "DL Bitrate after reestablishment is too low: %s < %s",
                    result_after.downlink.bits_per_second,
                    result_before.downlink.bits_per_second,
                )
        if direction in (IPerfDir.UPLINK, IPerfDir.BIDIRECTIONAL):
            if result_after.uplink.bits_per_second / result_before.uplink.bits_per_second < bitrate_threshold:
                bitrate_criteria = False
                logging.error(
                    "UL Bitrate after reestablishment is too low: %s < %s",
                    result_after.uplink.bits_per_second,
                    result_before.uplink.bits_per_second,
                )

        if not bitrate_criteria:
            pytest.fail("Bitrate criteria not met after reestablishment.")


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
@mark.flaky(reruns=2, only_rerun=_ONLY_RERUN)
# pylint: disable=too-many-arguments,too-many-positional-arguments,too-many-locals
def test_zmq_parallel(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_8: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    metrics_summary: MetricsSummary,
    band: int,
    common_scs: int,
    bandwidth: int,
    noise_spd: int,
):
    """
    ZMQ Ping + Reestablishment
    """
    number_of_reestablishments = 10
    reestablishment_time = 10

    with _test_reestablishments(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_8,
        fivegc=fivegc,
        gnb=gnb,
        metrics_summary=metrics_summary,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        global_timing_advance=0,
        time_alignment_calibration=0,
        always_download_artifacts=True,
        noise_spd=noise_spd,
        log_ip_level="debug",
        warning_as_errors=False,
    ) as ue_attach_info_dict:

        for i in range(number_of_reestablishments):
            logging.info("Starting Reestablishment for all UEs + Traffic running in background. Iteration %s", i + 1)
            ping_task_array = ping_start(
                ue_attach_info_dict=ue_attach_info_dict, fivegc=fivegc, ping_count=reestablishment_time
            )
            ue_reestablishment_parallel(ue_array=ue_8, reestablishment_interval=reestablishment_time)
            ping_wait_until_finish(ping_task_array)

        logging.info("Starting traffic after all reestablishments have been completed")
        ping_task_array = ping_start(
            ue_attach_info_dict=ue_attach_info_dict, fivegc=fivegc, ping_count=reestablishment_time
        )
        ping_wait_until_finish(ping_task_array)


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
@mark.flaky(reruns=2, only_rerun=_ONLY_RERUN)
# pylint: disable=too-many-arguments,too-many-positional-arguments,too-many-locals
def test_zmq_parallel_full_rate(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_8: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    metrics_summary: MetricsSummary,
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
    number_of_reestablishments = 10
    reestablishment_time = 10

    with _test_reestablishments(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_8,
        fivegc=fivegc,
        gnb=gnb,
        metrics_summary=metrics_summary,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        global_timing_advance=0,
        time_alignment_calibration=0,
        always_download_artifacts=True,
        noise_spd=noise_spd,
        log_ip_level="",
        warning_as_errors=True,
    ) as ue_attach_info_dict:

        for _ in range(number_of_reestablishments):
            logging.info(
                "Starting Reestablishment for all UEs + Traffic running in background",
            )
            iperf_dict = tuple(
                (
                    ue_attached_info,
                    *iperf_start(
                        ue_stub=ue_stub,
                        ue_attached_info=ue_attached_info,
                        fivegc=fivegc,
                        protocol=protocol,
                        direction=direction,
                        duration=reestablishment_time,
                        bitrate=0,
                    ),
                )
                for ue_stub, ue_attached_info in ue_attach_info_dict.items()
            )
            ue_reestablishment_parallel(ue_array=ue_8, reestablishment_interval=reestablishment_time)
            for ue_attached_info, task, iperf_request in iperf_dict:
                iperf_wait_until_finish(
                    ue_attached_info=ue_attached_info, fivegc=fivegc, task=task, iperf_request=iperf_request
                )

        logging.info("Starting traffic after all reestablishments have been completed")
        iperf_parallel(
            ue_attach_info_dict=ue_attach_info_dict,
            fivegc=fivegc,
            protocol=protocol,
            direction=direction,
            iperf_duration=reestablishment_time,
            bitrate=0,
            parallel_iperfs=len(ue_8),
        )


# pylint: disable=too-many-arguments,too-many-positional-arguments,too-many-locals
def _iterator_over_attached_ues(
    *,  # This enforces keyword-only arguments
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
    log_ip_level: str,
    warning_as_errors: bool = True,
) -> Generator[Tuple[Dict[UEStub, UEAttachedInfo], Dict[UEStub, UEAttachedInfo]], None, None]:

    with _test_reestablishments(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_array,
        fivegc=fivegc,
        metrics_summary=metrics_summary,
        gnb=gnb,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=sample_rate,
        global_timing_advance=global_timing_advance,
        time_alignment_calibration=time_alignment_calibration,
        always_download_artifacts=always_download_artifacts,
        noise_spd=noise_spd,
        log_ip_level=log_ip_level,
        warning_as_errors=warning_as_errors,
    ) as ue_attach_info_dict:

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


# pylint: disable=too-many-arguments,too-many-positional-arguments,too-many-locals
@contextmanager
def _test_reestablishments(
    *,  # This enforces keyword-only arguments
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
    log_ip_level: str,
    warning_as_errors: bool = True,
) -> Generator[Dict[UEStub, UEAttachedInfo], None, None]:

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
        log_ip_level=log_ip_level,
    )

    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=always_download_artifacts,
    )

    start_network(
        ue_array=ue_array,
        gnb=gnb,
        fivegc=fivegc,
        gnb_post_cmd=(
            "log --cu_level=debug",
            "log --mac_level=debug cell_cfg pdcch common --ss1_n_candidates=0 0 2 0 0"
            " pdsch --max_consecutive_kos=200 pusch --max_consecutive_kos=200",
        ),
    )

    ue_attach_info_dict = ue_start_and_attach(
        ue_array=ue_array, du_definition=[gnb.GetDefinition(Empty())], fivegc=fivegc
    )

    try:
        yield ue_attach_info_dict

        for ue_stub in ue_array:
            ue_validate_no_reattaches(ue_stub)

        stop(ue_array=ue_array, gnb=gnb, fivegc=fivegc, retina_data=retina_data, warning_as_errors=warning_as_errors)

    finally:
        get_kpis(du_or_gnb_array=[gnb], ue_array=ue_array, metrics_summary=metrics_summary)
