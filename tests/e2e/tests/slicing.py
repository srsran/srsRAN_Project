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
from pytest import fail, mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.protocol.base_pb2 import Metrics
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto
from retina.protocol.ue_pb2_grpc import UEStub

from tests.steps.stub import iperf_parallel, start_network, stop, ue_start_and_attach

from .steps.configuration import configure_test_parameters, get_minimum_sample_rate_for_bandwidth


# pylint: disable=too-many-arguments,too-many-positional-arguments,too-many-locals
@mark.zmq
def test_slicing(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_4: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
):
    """
    Test PUCCH (Amarisoft, ZMQ)
    """

    band = 78
    common_scs = 30
    bandwidth = 50
    iperf_duration = 60
    iperf_bitrate = int(100e6)

    configure_test_parameters(
        retina_manager=retina_manager,
        retina_data=retina_data,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=get_minimum_sample_rate_for_bandwidth(bandwidth),
        global_timing_advance=0,
        time_alignment_calibration=0,
        slices=[
            {"sd": 1, "min_prb_policy_ratio": 50, "max_prb_policy_ratio": 100},
            {"sd": 2, "min_prb_policy_ratio": 0, "max_prb_policy_ratio": 100},
        ],
        # Assign 1 UE to slice 1 and the rest to slice 2
        ue_sds=["000001", "000002", "000002", "000002"],
    )

    logging.info("Slicing Test")

    start_network(ue_array=ue_4, gnb=gnb, fivegc=fivegc)
    ue_attach_info_dict = ue_start_and_attach(ue_array=ue_4, du_definition=[gnb.GetDefinition(Empty())], fivegc=fivegc)
    slice1_ue_rnti = ue_attach_info_dict[ue_4[0]].rnti

    # DL iperf test
    iperf_parallel(
        ue_attach_info_dict=ue_attach_info_dict,
        fivegc=fivegc,
        protocol=IPerfProto.UDP,
        direction=IPerfDir.BIDIRECTIONAL,
        iperf_duration=iperf_duration,
        bitrate=iperf_bitrate,
    )

    stop(
        ue_array=ue_4,
        gnb=gnb,
        fivegc=fivegc,
        retina_data=retina_data,
        fail_if_kos=True,
    )

    metrics: Metrics = gnb.GetMetrics(Empty())
    # The thresholds values are set empirically, the objective is just checking that no UE gets starved.
    thresholds = {
        "000001": {
            "dl": 50e6,
            "ul": 25e6,
        },
        "000002": {
            "dl": 15e6,
            "ul": 8e6,
        },
    }
    assessment_msgs = []
    for ue_metrics in metrics.ue_array:
        if ue_metrics.rnti == slice1_ue_rnti:
            sd = "000001"
        else:
            sd = "000002"
        dl_bitrate = ue_metrics.dl_bitrate_peak_av.av_30_samples
        ul_bitrate = ue_metrics.ul_bitrate_peak_av.av_30_samples
        dl_brate_assessment_ok = dl_bitrate >= thresholds[sd]["dl"]
        ul_brate_assessment_ok = ul_bitrate >= thresholds[sd]["ul"]
        dl_pass_msg = "PASSED" if dl_brate_assessment_ok else "FAILED"
        ul_pass_msg = "PASSED" if ul_brate_assessment_ok else "FAILED"
        msg = (
            f"Bitrate assessment for UE in slice {sd}:"
            f" DL={dl_bitrate} ({dl_pass_msg}, target {thresholds[sd]['dl']})"
            f" UL={ul_bitrate} ({ul_pass_msg}, target {thresholds[sd]['ul']})"
        )
        assessment_msgs.append(msg)
        logging.info(msg)

    for msg in assessment_msgs:
        if "FAILED" in msg:
            fail(msg)
