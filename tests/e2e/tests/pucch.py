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

from tests.steps.stub import iperf_parallel, start_network, stop, ue_start_and_attach

from .steps.configuration import configure_test_parameters, get_minimum_sample_rate_for_bandwidth


@mark.zmq
@mark.parametrize(
    "use_format_0, pucch_set1_format, ul_noise_spd",
    (
        # PUCCH Format 0 decoder doesn't work with no noise.
        param(True, 2, -134, id="f0_f2"),
        param(False, 2, 0, id="f1_f2"),
        param(False, 3, 0, id="f1_f3"),
        param(False, 4, 0, id="f1_f4"),
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
    ul_noise_spd: int,
):
    """
    Test PUCCH (Amarisoft, ZMQ)
    """

    ue_array = ue_32

    band = 41
    common_scs = 30
    bandwidth = 50
    iperf_duration = 10
    iperf_bitrate = int(1e6)
    pucch_set0_format = 0 if use_format_0 else 1

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
        ul_noise_spd=ul_noise_spd,
        pucch_set1_format=pucch_set1_format,
    )

    logging.info("PUCCH F%d+F%d Test", pucch_set0_format, pucch_set1_format)

    start_network(ue_array=ue_array, gnb=gnb, fivegc=fivegc)
    ue_attach_info_dict = ue_start_and_attach(
        ue_array=ue_array, du_definition=[gnb.GetDefinition(Empty())], fivegc=fivegc
    )

    # DL iperf test
    iperf_parallel(
        ue_attach_info_dict=ue_attach_info_dict,
        fivegc=fivegc,
        protocol=IPerfProto.UDP,
        direction=IPerfDir.DOWNLINK,
        iperf_duration=iperf_duration,
        bitrate=iperf_bitrate,
    )

    # Bidirectional iperf test
    iperf_parallel(
        ue_attach_info_dict=ue_attach_info_dict,
        fivegc=fivegc,
        protocol=IPerfProto.UDP,
        direction=IPerfDir.BIDIRECTIONAL,
        iperf_duration=iperf_duration,
        bitrate=iperf_bitrate,
    )

    stop(
        ue_array=ue_array,
        gnb=gnb,
        fivegc=fivegc,
        retina_data=retina_data,
        fail_if_kos=True,
    )

    metrics: Metrics = gnb.GetMetrics(Empty())
    invalid_pucchs = (
        metrics.total.nof_pucch_f0f1_invalid_harqs > 0
        or metrics.total.nof_pucch_f2f3f4_invalid_harqs > 0
        or metrics.total.nof_pucch_f2f3f4_invalid_csis > 0
    )

    if invalid_pucchs:
        fail(
            f"Invalid PUCCH transmissions during the test: "
            f"harq_f{pucch_set0_format}={metrics.total.nof_pucch_f0f1_invalid_harqs} "
            f"harq_f{pucch_set1_format}={metrics.total.nof_pucch_f2f3f4_invalid_harqs} "
            f"csi_f{pucch_set1_format}={metrics.total.nof_pucch_f2f3f4_invalid_csis}"
        )
