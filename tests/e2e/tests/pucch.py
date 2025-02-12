#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
PUCCH Tests
"""
import logging
from typing import Tuple

from google.protobuf.empty_pb2 import Empty
from pytest import fail, mark, param
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.protocol.base_pb2 import Metrics
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto
from retina.protocol.ue_pb2_grpc import UEStub

from tests.steps.stub import iperf_start, iperf_wait_until_finish, start_network, stop, ue_start_and_attach

from .steps.configuration import configure_test_parameters, get_minimum_sample_rate_for_bandwidth


@mark.pucch
@mark.parametrize(
    "use_format_0, pucch_set1_format",
    (
        param(True, 2, id="f0_f2"),
        param(False, 2, id="f1_f2"),
        param(False, 3, id="f1_f3"),
        param(False, 4, id="f1_f4"),
    ),
)
# pylint: disable=too-many-arguments,too-many-positional-arguments,too-many-locals
def test_pucch(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_32: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    use_format_0: bool,
    pucch_set1_format: int,
):
    """
    Test PUCCH (Amarisoft, ZMQ)
    """

    ue_array = ue_32

    band = 78
    common_scs = 30
    bandwidth = 50
    iperf_duration = 10
    iperf_bitrate = int(1e6)

    configure_test_parameters(
        retina_manager=retina_manager,
        retina_data=retina_data,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=get_minimum_sample_rate_for_bandwidth(bandwidth),
        global_timing_advance=0,
        time_alignment_calibration=0,
        use_format_0=use_format_0,
        pucch_set1_format=pucch_set1_format,
    )

    logging.info("PUCCH F%d+F%d Test", 0 if use_format_0 else 1, pucch_set1_format)

    start_network(ue_array, gnb, fivegc)
    ue_attach_info_dict = ue_start_and_attach(ue_array, gnb, fivegc)

    # DL iperf test
    iperf_array = []
    for ue_stub in ue_array:
        iperf_array.append(
            (
                ue_attach_info_dict[ue_stub],
                *iperf_start(
                    ue_stub,
                    ue_attach_info_dict[ue_stub],
                    fivegc,
                    duration=iperf_duration,
                    direction=IPerfDir.DOWNLINK,
                    protocol=IPerfProto.UDP,
                    bitrate=iperf_bitrate,
                ),
            )
        )

    for ue_attached_info, task, iperf_request in iperf_array:
        iperf_wait_until_finish(ue_attached_info, fivegc, task, iperf_request)

    # Bidirectional iperf test
    iperf_array = []
    for ue_stub in ue_array:
        iperf_array.append(
            (
                ue_attach_info_dict[ue_stub],
                *iperf_start(
                    ue_stub,
                    ue_attach_info_dict[ue_stub],
                    fivegc,
                    duration=iperf_duration,
                    direction=IPerfDir.BIDIRECTIONAL,
                    protocol=IPerfProto.UDP,
                    bitrate=iperf_bitrate,
                ),
            )
        )

    for ue_attached_info, task, iperf_request in iperf_array:
        iperf_wait_until_finish(ue_attached_info, fivegc, task, iperf_request)

    stop(
        ue_array,
        gnb,
        fivegc,
        retina_data,
        fail_if_kos=True,
    )

    metrics: Metrics = gnb.GetMetrics(Empty())
    nof_pucch_f0f1_invalid_harqs = 0
    nof_pucch_f2f3f4_invalid_harqs = 0
    nof_pucch_f2f3f4_invalid_csis = 0
    for ue_info in metrics.ue_array:
        nof_pucch_f0f1_invalid_harqs += ue_info.nof_pucch_f0f1_invalid_harqs
        nof_pucch_f2f3f4_invalid_harqs += ue_info.nof_pucch_f2f3f4_invalid_harqs
        nof_pucch_f2f3f4_invalid_csis += ue_info.nof_pucch_f2f3f4_invalid_csis

    if nof_pucch_f0f1_invalid_harqs > 0:
        fail(f"There are invalid PUCCH F{0 if use_format_0 else 1} HARQ-ACK transmissions")
    if nof_pucch_f2f3f4_invalid_harqs > 0:
        fail(f"There are invalid PUCCH F{pucch_set1_format} HARQ-ACK transmissions")
    if nof_pucch_f2f3f4_invalid_csis > 0:
        fail(f"There are invalid PUCCH F{pucch_set1_format} CSI transmissions")
