#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Handover Tests
"""
import logging

from pytest import mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.public import MetricsSummary
from retina.launcher.utils import param
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto
from retina.protocol.ue_pb2_grpc import UEStub

from .handover import _do_ho, _handover_multi_ues_iperf, _handover_sequentially

HIGH_BITRATE = int(15e6)


@mark.parametrize(
    "band, common_scs, bandwidth, noise_spd",
    (
        param(3, 15, 50, 0, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
        param(41, 30, 50, 0, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
    ),
)
@mark.zmq
@mark.flaky(reruns=1, only_rerun=["failed to start", "Attach timeout reached", "StatusCode.ABORTED"])
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_zmq_sequentially(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_8: UEStub,
    fivegc: FiveGCStub,
    gnb_2: GNBStub,
    metrics_summary: MetricsSummary,
    band: int,
    common_scs: int,
    bandwidth: int,
    noise_spd: int,
):
    """
    ZMQ Inter-CU Handover tests
    """

    # _inter_du_handover_sequentially(
    _handover_sequentially(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_8,
        fivegc=fivegc,
        gnb_array=gnb_2,
        metrics_summary=metrics_summary,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        noise_spd=noise_spd,
        sleep_between_movement_steps=1,
    )


@mark.parametrize(
    "protocol",
    (
        param(IPerfProto.UDP, id="udp", marks=mark.udp),
        param(IPerfProto.TCP, id="tcp", marks=mark.tcp),
    ),
)
@mark.parametrize(
    "band, common_scs, bandwidth, noise_spd",
    (
        param(3, 15, 50, 0, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
        param(41, 30, 50, 0, id="band:%s-scs:%s-bandwidth:%s-noise:%s"),
    ),
)
@mark.zmq_single_ue
@mark.flaky(reruns=2, only_rerun=["failed to start", "Attach timeout reached", "StatusCode.ABORTED"])
# pylint: disable=too-many-arguments,too-many-positional-arguments,too-many-locals
def test_zmq_handover_iperf(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,
    fivegc: FiveGCStub,
    gnb_2: GNBStub,
    metrics_summary: MetricsSummary,
    band: int,
    common_scs: int,
    bandwidth: int,
    noise_spd: int,
    protocol: IPerfProto,
):
    """
    ZMQ Handover iperf test
    """

    with _handover_multi_ues_iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=[ue],
        gnb_array=gnb_2,
        fivegc=fivegc,
        metrics_summary=metrics_summary,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        bitrate=HIGH_BITRATE,
        protocol=protocol,
        direction=IPerfDir.BIDIRECTIONAL,
        sample_rate=None,  # default from testbed
        global_timing_advance=0,
        time_alignment_calibration=0,
        always_download_artifacts=True,
        noise_spd=noise_spd,
        sleep_between_movement_steps=1,
        warning_as_errors=True,
    ) as (ue_attach_info_dict, movements, _):

        for ue_stub, ue_attach_info in ue_attach_info_dict.items():
            logging.info(
                "Zigzag Inter-CU HO for UE [%s] (%s) + iPerf running in background for all UEs",
                id(ue_stub),
                ue_attach_info.ipv4,
            )

            for _from_position, _to_position, _movement_steps, _sleep_between_movement_steps in movements:
                _do_ho(
                    ue_array=(ue_stub,),
                    from_position=_from_position,
                    to_position=_to_position,
                    steps=_movement_steps,
                    sleep_between_steps=_sleep_between_movement_steps,
                )
