#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Test Iperf
"""

import logging
from contextlib import suppress

from pytest import mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.utils import configure_artifacts, param
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto
from retina.protocol.ue_pb2_grpc import UEStub

from .steps.configuration import configure_test_parameters
from .steps.stub import iperf, start_and_attach, StartFailure, stop


@mark.parametrize(
    "direction",
    (
        param(IPerfDir.DOWNLINK, id="downlink", marks=mark.downlink),
        param(IPerfDir.UPLINK, id="uplink", marks=mark.uplink),
        param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),
    ),
)
@mark.parametrize(
    "config",
    (param("", id=""),),
)
@mark.zmq_single_ue
# pylint: disable=too-many-arguments
def test_multiple_configs_zmq(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_1: UEStub,
    fivegc: FiveGCStub,
    gnb: GNBStub,
    config: str,
    direction: IPerfDir,
):
    """
    ZMQ IPerfs
    """

    logging.info("Iperf Test")

    band = 41
    common_scs = 30
    bandwidth = 20
    iperf_duration = 20
    bitrate = int(15e6)
    protocol = IPerfProto.TCP

    with suppress(StartFailure):
        configure_test_parameters(
            retina_manager=retina_manager,
            retina_data=retina_data,
            band=band,
            common_scs=common_scs,
            bandwidth=bandwidth,
            sample_rate=None,  # default from testbed
            global_timing_advance=0,
            time_alignment_calibration=0,
            pcap=False,
        )
        configure_artifacts(
            retina_data=retina_data,
            always_download_artifacts=True,
        )

        ue_attach_info_dict = start_and_attach((ue_1,), gnb, fivegc, gnb_post_cmd=config)

        iperf(
            ue_attach_info_dict,
            fivegc,
            protocol,
            direction,
            iperf_duration,
            bitrate,
            bitrate_threshold_ratio=0,
        )
        stop((ue_1,), gnb, fivegc, retina_data, warning_as_errors=True, fail_if_kos=True)
