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
Test Iperf
"""

import logging
from time import sleep

from pytest import mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.utils import configure_artifacts, param
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto
from retina.protocol.ue_pb2_grpc import UEStub

from .steps.configuration import configure_test_parameters
from .steps.stub import iperf_parallel, start_and_attach, stop


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
    (param("log --hex_max_size=32", id="hex_max_size"),),
)
@mark.zmq_single_ue
# pylint: disable=too-many-arguments
def test_multiple_configs_zmq(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,  # pylint: disable=invalid-name
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
    wait_before_power_off = 2

    configure_test_parameters(
        retina_manager=retina_manager,
        retina_data=retina_data,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        global_timing_advance=0,
        time_alignment_calibration=0,
    )
    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=False,
    )

    ue_attach_info_dict = start_and_attach((ue,), gnb, fivegc, gnb_post_cmd=config)

    iperf_parallel(
        ue_attach_info_dict,
        fivegc,
        protocol,
        direction,
        iperf_duration,
        bitrate,
        bitrate_threshold_ratio=0,  # bitrate != 0
    )
    sleep(wait_before_power_off)
    stop((ue,), gnb, fivegc, retina_data, warning_as_errors=True, fail_if_kos=True)
