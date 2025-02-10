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
from pytest import mark, fail, param
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2_grpc import UEStub
from retina.protocol.base_pb2 import Metrics

from tests.steps.stub import ping_start, ping_start_from_5gc, ping_wait_until_finish, start_network, stop, ue_start_and_attach

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

    logging.info(f"PUCCH F{0 if use_format_0 else 1}+F{pucch_set1_format} Test")

    start_network(ue_array, gnb, fivegc)
    ue_attach_info_dict = ue_start_and_attach(ue_array, gnb, fivegc)

    ping_task_array = ping_start(ue_attach_info_dict, fivegc, 10)
    ping_wait_until_finish(ping_task_array)
    ping_task_array = ping_start_from_5gc(ue_attach_info_dict, fivegc, 10)
    ping_wait_until_finish(ping_task_array)

    stop(
        ue_array,
        gnb,
        fivegc,
        retina_data,
        fail_if_kos=True,
    )

    metrics : Metrics = gnb.GetMetrics(Empty())
    nof_pucch_f2f3f4_invalid_harqs = 0
    nof_pucch_f2f3f4_invalid_csis = 0
    for ue_info in metrics.ue_array:
        nof_pucch_f2f3f4_invalid_harqs += ue_info.nof_pucch_f2f3f4_invalid_harqs
        nof_pucch_f2f3f4_invalid_csis += ue_info.nof_pucch_f2f3f4_invalid_csis

    if nof_pucch_f2f3f4_invalid_csis > 0:
        fail(f"There are invalid PUCCH F{pucch_set1_format} CSI transmissions")
    if nof_pucch_f2f3f4_invalid_harqs > 0:
        fail("There are invalid PUCCH F{pucch_set1_format} HARQ-ACK transmissions")
