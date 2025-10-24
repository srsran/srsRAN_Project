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
Test Iperf
"""

import logging
from time import sleep
from typing import Optional, Sequence, Tuple, Union

import pytest
from google.protobuf.empty_pb2 import Empty
from pytest import mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.utils import configure_artifacts, param
from retina.protocol.base_pb2 import Metrics, PLMN
from retina.protocol.channel_emulator_pb2 import EphemerisInfoType, NtnScenarioDefinition, NtnScenarioType
from retina.protocol.channel_emulator_pb2_grpc import ChannelEmulatorStub
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ric_pb2_grpc import NearRtRicStub
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto
from retina.protocol.ue_pb2_grpc import UEStub

from .steps.configuration import (
    _get_dl_arfcn,
    _get_ul_arfcn,
    configure_test_parameters,
    get_minimum_sample_rate_for_bandwidth,
    nr_arfcn_to_freq,
)
from .steps.iperf_helpers import (
    assess_iperf_bitrate,
    get_maximum_throughput,
    HIGH_BITRATE,
    LOW_BITRATE,
    MEDIUM_BITRATE,
    SHORT_DURATION,
    TINY_DURATION,
)
from .steps.stub import (
    get_ntn_configs,
    INTER_UE_START_PERIOD,
    iperf_parallel,
    is_ntn_channel_emulator,
    ric_validate_e2_interface,
    start_and_attach,
    start_kpm_mon_xapp,
    start_ntn_channel_emulator,
    start_rc_xapp,
    stop,
    stop_kpm_mon_xapp,
    stop_rc_xapp,
)

ZMQ_ID = "band:%s-scs:%s-bandwidth:%s-bitrate:%s"


@mark.parametrize(
    "direction",
    (
        param(IPerfDir.DOWNLINK, id="downlink", marks=mark.downlink),
        param(IPerfDir.UPLINK, id="uplink", marks=mark.uplink),
        param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),
    ),
)
@mark.parametrize(
    "protocol",
    (
        param(IPerfProto.UDP, id="udp", marks=mark.udp),
        param(IPerfProto.TCP, id="tcp", marks=mark.tcp),
    ),
)
@mark.parametrize(
    "band, common_scs, bandwidth",
    (param(3, 15, 10, id="band:%s-scs:%s-bandwidth:%s"),),
)
@mark.zmq_srsue
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_srsue(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,  # pylint: disable=invalid-name
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    ZMQ IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue,),
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=11520000,
        iperf_duration=SHORT_DURATION,
        protocol=protocol,
        bitrate=MEDIUM_BITRATE,
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration=0,
        always_download_artifacts=True,
        common_search_space_enable=True,
        prach_config_index=1,
        pdsch_mcs_table="qam64",
        pusch_mcs_table="qam64",
    )


@mark.parametrize(
    "direction",
    (param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),),
)
@mark.parametrize(
    "protocol",
    (param(IPerfProto.UDP, id="udp", marks=mark.udp),),
)
@mark.parametrize(
    "band, common_scs, bandwidth",
    (param(3, 15, 10, id="band:%s-scs:%s-bandwidth:%s"),),
)
@mark.zmq_ric
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_ric(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,  # pylint: disable=invalid-name
    fivegc: FiveGCStub,
    gnb: GNBStub,
    ric: NearRtRicStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    ZMQ IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue,),
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=11520000,
        iperf_duration=SHORT_DURATION,
        protocol=protocol,
        bitrate=LOW_BITRATE,
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration=0,
        always_download_artifacts=True,
        common_search_space_enable=True,
        prach_config_index=1,
        pdsch_mcs_table="qam64",
        pusch_mcs_table="qam64",
        ric=ric,
    )


@mark.parametrize(
    "direction",
    (param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),),
)
@mark.parametrize(
    "protocol",
    (param(IPerfProto.UDP, id="udp", marks=mark.udp),),
)
@mark.parametrize(
    "band, common_scs, bandwidth, enable_feeder_link",
    (
        param(256, 15, 5, False, id="band:%s-scs:%s-bandwidth:%s-fl:%s"),
        param(256, 15, 5, True, id="band:%s-scs:%s-bandwidth:%s-fl:%s"),
    ),
)
@mark.zmq_ntn
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_ntn(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,  # pylint: disable=invalid-name
    fivegc: FiveGCStub,
    gnb: GNBStub,
    channel_emulator: ChannelEmulatorStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    enable_feeder_link: bool,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    NTN ZMQ Iperf
    """
    ntn_scenario_def = NtnScenarioDefinition()
    ntn_scenario_def.scenario_type = NtnScenarioType.GEO
    ntn_scenario_def.ephemeris_info_type = EphemerisInfoType.ECEF
    ntn_scenario_def.min_sat_elevation_deg = 20
    ntn_scenario_def.pass_start_offset_s = 10
    ntn_scenario_def.delay_offset_us = 20
    ntn_scenario_def.sample_rate = 5760000
    ntn_scenario_def.enable_feeder_link = enable_feeder_link
    ntn_scenario_def.enable_doppler = True
    ntn_scenario_def.access_link_dl_freq_hz = nr_arfcn_to_freq(_get_dl_arfcn(band))
    ntn_scenario_def.access_link_ul_freq_hz = nr_arfcn_to_freq(_get_ul_arfcn(band))
    ntn_scenario_def.feeder_link_dl_freq_hz = nr_arfcn_to_freq(_get_dl_arfcn(band))
    ntn_scenario_def.feeder_link_ul_freq_hz = nr_arfcn_to_freq(_get_ul_arfcn(band))

    # Define min DL/UL data rates to be achieved.
    min_dl_bitrate = 20e6 * 0.9
    min_ul_bitrate = 18e6 * 0.9

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=[ue],
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=5760000,
        iperf_duration=SHORT_DURATION,
        protocol=protocol,
        bitrate=HIGH_BITRATE,
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration=0,
        warning_as_errors=False,
        always_download_artifacts=True,
        common_search_space_enable=False,
        prach_config_index=31,
        pdsch_mcs_table="qam256",
        pusch_mcs_table="qam256",
        channel_emulator=channel_emulator,
        ntn_scenario_def=ntn_scenario_def,
        assess_bitrate=True,
        min_dl_bitrate=min_dl_bitrate,
        min_ul_bitrate=min_ul_bitrate,
    )


@mark.parametrize(
    "direction",
    (
        param(IPerfDir.DOWNLINK, id="downlink", marks=mark.downlink),
        param(IPerfDir.UPLINK, id="uplink", marks=mark.uplink),
        param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),
    ),
)
@mark.parametrize(
    "protocol",
    (
        param(IPerfProto.UDP, id="udp", marks=mark.udp),
        param(IPerfProto.TCP, id="tcp", marks=mark.tcp),
    ),
)
@mark.parametrize(
    "band, common_scs, bandwidth",
    (
        param(3, 15, 10, id="band:%s-scs:%s-bandwidth:%s"),
        param(78, 30, 20, id="band:%s-scs:%s-bandwidth:%s"),
    ),
)
@mark.android
@mark.flaky(
    reruns=2,
    only_rerun=["failed to start", "Exception calling application", "Attach timeout reached", "Some packages got lost"],
)
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_android(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,  # pylint: disable=invalid-name
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    Android IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue,),
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=get_minimum_sample_rate_for_bandwidth(bandwidth),
        iperf_duration=SHORT_DURATION,
        protocol=protocol,
        bitrate=get_maximum_throughput(bandwidth=bandwidth, band=band, direction=direction, protocol=protocol),
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration="auto",
        always_download_artifacts=True,
        warning_as_errors=False,
    )


@mark.parametrize(
    "direction",
    (param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),),
)
@mark.parametrize(
    "protocol",
    (param(IPerfProto.UDP, id="udp", marks=mark.udp),),
)
@mark.parametrize(
    "band, common_scs, bandwidth",
    (param(78, 30, 20, id="band:%s-scs:%s-bandwidth:%s"),),
)
@mark.android
@mark.flaky(
    reruns=2,
    only_rerun=["failed to start", "Exception calling application", "Attach timeout reached", "Some packages got lost"],
)
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_android_interleaving(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,  # pylint: disable=invalid-name
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    Android IPerfs Interleaving
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue,),
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=get_minimum_sample_rate_for_bandwidth(bandwidth),
        iperf_duration=SHORT_DURATION,
        protocol=protocol,
        bitrate=get_maximum_throughput(bandwidth=bandwidth, band=band, direction=direction, protocol=protocol),
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration="auto",
        always_download_artifacts=True,
        warning_as_errors=False,
        pdsch_interleaving_bundle_size=2,
    )


@mark.parametrize(
    "direction",
    (
        param(IPerfDir.DOWNLINK, id="downlink", marks=mark.downlink),
        param(IPerfDir.UPLINK, id="uplink", marks=mark.uplink),
        param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),
    ),
)
@mark.parametrize(
    "protocol",
    (
        param(IPerfProto.UDP, id="udp", marks=mark.udp),
        param(IPerfProto.TCP, id="tcp", marks=mark.tcp),
    ),
)
@mark.parametrize(
    "band, common_scs, bandwidth",
    (
        param(7, 15, 20, id="band:%s-scs:%s-bandwidth:%s"),
        param(78, 30, 50, id="band:%s-scs:%s-bandwidth:%s"),
    ),
)
@mark.android_hp
@mark.flaky(
    reruns=2,
    only_rerun=[
        "failed to start",
        "Exception calling application",
        "Attach timeout reached",
        "Some packages got lost",
        "Failed to connect to remote host",
    ],
)
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_android_hp(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,  # pylint: disable=invalid-name
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    Android high performance IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue,),
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,
        iperf_duration=SHORT_DURATION,
        protocol=protocol,
        bitrate=get_maximum_throughput(bandwidth=bandwidth, band=band, direction=direction, protocol=protocol),
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration="auto",
        always_download_artifacts=True,
        warning_as_errors=False,
        gnb_post_cmd=("ru_sdr expert_cfg --low_phy_dl_throttling=0.5",),
    )


@mark.parametrize(
    "direction",
    (param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),),
)
@mark.parametrize(
    "protocol",
    (param(IPerfProto.UDP, id="udp", marks=mark.udp),),
)
@mark.parametrize(
    "band, common_scs, bandwidth",
    (param(41, 30, 20, id="band:%s-scs:%s-bandwidth:%s"),),
)
@mark.zmq_2x2_mimo
@mark.flaky(reruns=2, only_rerun=["failed to start", "Attach timeout reached", "5GC crashed", "StatusCode.UNKNOWN"])
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_zmq_2x2_mimo(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_32: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    ZMQ 2x2 mimo IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_32,
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,
        iperf_duration=SHORT_DURATION,
        protocol=protocol,
        bitrate=MEDIUM_BITRATE,
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration=0,
        always_download_artifacts=True,
        rx_to_tx_latency=2,
        enable_dddsu=True,
        nof_antennas_dl=2,
        nof_antennas_ul=2,
        inter_ue_start_period=1.5,  # Due to uesim
        assess_bitrate=False,
    )


@mark.zmq
@mark.flaky(reruns=2, only_rerun=["failed to start", "Attach timeout reached", "5GC crashed", "StatusCode.UNKNOWN"])
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_zmq_64_ues(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_64: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
):
    """
    ZMQ 2x2 mimo IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_64,
        gnb=gnb,
        fivegc=fivegc,
        band=41,
        common_scs=30,
        bandwidth=50,
        sample_rate=None,
        iperf_duration=SHORT_DURATION,
        protocol=IPerfProto.UDP,
        bitrate=MEDIUM_BITRATE,
        direction=IPerfDir.BIDIRECTIONAL,
        global_timing_advance=-1,
        time_alignment_calibration=0,
        always_download_artifacts=False,
        rx_to_tx_latency=2,
        enable_dddsu=False,
        nof_antennas_dl=2,
        nof_antennas_ul=2,
        inter_ue_start_period=1.5,  # Due to uesim
        assess_bitrate=True,
    )


@mark.parametrize(
    "direction",
    (
        param(IPerfDir.DOWNLINK, id="downlink", marks=mark.downlink),
        param(IPerfDir.UPLINK, id="uplink", marks=mark.uplink),
        param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),
    ),
)
@mark.parametrize(
    "protocol",
    (
        param(IPerfProto.UDP, id="udp", marks=mark.udp),
        param(IPerfProto.TCP, id="tcp", marks=mark.tcp),
    ),
)
@mark.parametrize(
    "band, common_scs, bandwidth",
    (param(41, 30, 20, id="band:%s-scs:%s-bandwidth:%s"),),
)
@mark.zmq_4x4_mimo
@mark.flaky(reruns=2, only_rerun=["failed to start", "Attach timeout reached", "5GC crashed", "StatusCode.UNKNOWN"])
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_zmq_4x4_mimo(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,  # pylint: disable=invalid-name
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    ZMQ 4x4 mimo IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue,),
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,
        iperf_duration=SHORT_DURATION,
        protocol=protocol,
        bitrate=MEDIUM_BITRATE,
        direction=direction,
        global_timing_advance=-1,
        time_alignment_calibration=0,
        always_download_artifacts=False,
        nof_antennas_dl=4,
        nof_antennas_ul=4,
        ue_stop_timeout=90,
    )


@mark.parametrize(
    "direction, nof_antennas",
    (
        param(IPerfDir.DOWNLINK, 1, id="downlink", marks=mark.downlink),
        param(IPerfDir.UPLINK, 1, id="uplink", marks=mark.uplink),
        param(IPerfDir.BIDIRECTIONAL, 4, id="bidirectional 4x4 mimo", marks=mark.bidirectional),
    ),
)
@mark.zmq
@mark.smoke
@mark.flaky(reruns=2, only_rerun=["StatusCode.UNKNOWN"])
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_smoke(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_4: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    direction: IPerfDir,
    nof_antennas: int,
):
    """
    ZMQ IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_4,
        gnb=gnb,
        fivegc=fivegc,
        band=41,
        common_scs=30,
        bandwidth=20,
        sample_rate=None,  # default from testbed
        iperf_duration=TINY_DURATION,
        bitrate=LOW_BITRATE,
        protocol=IPerfProto.UDP,
        direction=direction,
        nof_antennas_dl=nof_antennas,
        nof_antennas_ul=nof_antennas,
        global_timing_advance=0,
        time_alignment_calibration=0,
        always_download_artifacts=False,
        bitrate_threshold=0,
        ue_stop_timeout=30,
        gnb_post_cmd=("", "metrics --enable_log=True"),
    )


@mark.parametrize(
    "direction",
    (
        param(IPerfDir.DOWNLINK, id="downlink", marks=mark.downlink),
        param(IPerfDir.UPLINK, id="uplink", marks=mark.uplink),
        param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),
    ),
)
@mark.parametrize(
    "protocol",
    (
        param(IPerfProto.UDP, id="udp", marks=mark.udp),
        param(IPerfProto.TCP, id="tcp", marks=mark.tcp),
    ),
)
@mark.parametrize(
    "band, common_scs, bandwidth, bitrate",
    (
        # ZMQ
        param(3, 15, 5, MEDIUM_BITRATE, id=ZMQ_ID),
        param(3, 15, 10, MEDIUM_BITRATE, id=ZMQ_ID),
        param(3, 15, 20, MEDIUM_BITRATE, id=ZMQ_ID),
        param(3, 15, 50, MEDIUM_BITRATE, id=ZMQ_ID),
        param(41, 30, 10, MEDIUM_BITRATE, id=ZMQ_ID),
        param(41, 30, 20, MEDIUM_BITRATE, id=ZMQ_ID),
        param(41, 30, 50, MEDIUM_BITRATE, id=ZMQ_ID),
    ),
)
@mark.zmq
@mark.flaky(
    reruns=2,
    only_rerun=[
        "failed to start",
        "Attach timeout reached",
        "iperf did not achieve the expected data rate",
        "socket is already closed",
        "failed to connect to all addresses",
        "5GC crashed",
        "StatusCode.UNKNOWN",
    ],
)
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_zmq(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_32: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    bitrate: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    ZMQ IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_32,
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        iperf_duration=SHORT_DURATION,
        bitrate=bitrate,
        protocol=protocol,
        direction=direction,
        global_timing_advance=0,
        time_alignment_calibration=0,
        always_download_artifacts=False,
        bitrate_threshold=0,
        ue_stop_timeout=1,
        gnb_post_cmd=("log --hex_max_size=32 cu_cp --inactivity_timer=600", ""),
    )


@mark.parametrize(
    "direction",
    (
        param(IPerfDir.DOWNLINK, id="downlink", marks=mark.downlink),
        param(IPerfDir.UPLINK, id="uplink", marks=mark.uplink),
        param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),
    ),
)
@mark.parametrize(
    "protocol",
    (param(IPerfProto.TCP, id="tcp", marks=mark.tcp),),
)
@mark.parametrize(
    "band, common_scs, bandwidth, bitrate",
    (
        param(41, 30, 10, MEDIUM_BITRATE, id=ZMQ_ID),
        param(41, 30, 20, MEDIUM_BITRATE, id=ZMQ_ID),
        param(41, 30, 50, MEDIUM_BITRATE, id=ZMQ_ID),
    ),
)
@mark.zmq
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_zmq_precoding(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_32: Tuple[UEStub, ...],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    bitrate: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    ZMQ IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_32,
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        iperf_duration=SHORT_DURATION,
        bitrate=bitrate,
        protocol=protocol,
        direction=direction,
        global_timing_advance=0,
        time_alignment_calibration=0,
        always_download_artifacts=False,
        bitrate_threshold=0,
        ue_stop_timeout=1,
        gnb_post_cmd=(
            "log --hex_max_size=32 cu_cp --inactivity_timer=600",
            "cell_cfg pusch --enable_transform_precoding=true",
        ),
    )


@mark.parametrize(
    "direction",
    (param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),),
)
@mark.parametrize(
    "protocol",
    (param(IPerfProto.UDP, id="udp", marks=mark.udp),),
)
@mark.parametrize(
    "band, common_scs, bandwidth, bitrate",
    (
        param(41, 30, 50, int(600e6), id=ZMQ_ID),
        param(41, 30, 100, int(1.2e9), id=ZMQ_ID),
    ),
)
@mark.s72
@mark.flaky(
    reruns=2,
    only_rerun=["failed to start", "5GC crashed", "StatusCode.UNKNOWN"],
)
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_s72(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    bitrate: int,
    protocol: IPerfProto,
    direction: IPerfDir,
):
    """
    Amariue Split 7.2x IPerfs
    """

    _iperf(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=(ue,),
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        iperf_duration=10 * 60,
        bitrate=bitrate,
        protocol=protocol,
        direction=direction,
        gnb_post_cmd=(
            "expert_execution threads main_pool --task_queue_size=4096",
            "expert_phy --max_proc_delay=4",
        ),
        nof_antennas_dl=4,
        nof_antennas_ul=1,
        global_timing_advance=-1,
        prach_config_index=159,
        time_alignment_calibration=0,
        always_download_artifacts=False,
        warning_as_errors=False,
        bitrate_threshold=0,
        ue_stop_timeout=1,
        assess_bitrate=True,
        stop_gnb_first=True,
        packet_length=1400,
    )


# pylint: disable=too-many-arguments,too-many-positional-arguments, too-many-locals
def _iperf(
    *,  # This enforces keyword-only arguments
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_array: Sequence[UEStub],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    sample_rate: Optional[int],
    iperf_duration: int,
    bitrate: int,
    protocol: IPerfProto,
    direction: IPerfDir,
    global_timing_advance: int,
    time_alignment_calibration: Union[int, str],
    always_download_artifacts: bool,
    warning_as_errors: bool = True,
    bitrate_threshold: float = 0,  # bitrate != 0
    gnb_post_cmd: Tuple[str, ...] = tuple(),
    plmn: Optional[PLMN] = None,
    common_search_space_enable: bool = False,
    prach_config_index=-1,
    ue_stop_timeout: int = 0,
    rx_to_tx_latency: int = -1,
    enable_dddsu: bool = False,
    nof_antennas_dl: int = 1,
    nof_antennas_ul: int = 1,
    pdsch_mcs_table: str = "qam256",
    pusch_mcs_table: str = "qam256",
    inter_ue_start_period=INTER_UE_START_PERIOD,
    ric: Optional[NearRtRicStub] = None,
    assess_bitrate: bool = False,
    stop_gnb_first: bool = False,
    packet_length: int = 0,
    channel_emulator: Optional[ChannelEmulatorStub] = None,
    ntn_scenario_def: Optional[NtnScenarioDefinition] = None,
    min_dl_bitrate: float = 0,
    min_ul_bitrate: float = 0,
    pdsch_interleaving_bundle_size: int = 0,
):
    wait_before_power_off = 5

    logging.info("Iperf Test")

    ntn_config = None
    if channel_emulator and ntn_scenario_def:
        if not is_ntn_channel_emulator(channel_emulator):
            logging.info("The channel emulator is not a NTN emulator.")
            return
        start_ntn_channel_emulator(
            ue_array=ue_array, gnb=gnb, channel_emulator=channel_emulator, ntn_scenario_def=ntn_scenario_def
        )
        ntn_config = get_ntn_configs(channel_emulator)

    configure_test_parameters(
        retina_manager=retina_manager,
        retina_data=retina_data,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=sample_rate,
        global_timing_advance=global_timing_advance,
        time_alignment_calibration=time_alignment_calibration,
        common_search_space_enable=common_search_space_enable,
        prach_config_index=prach_config_index,
        rx_to_tx_latency=rx_to_tx_latency,
        enable_dddsu=enable_dddsu,
        nof_antennas_dl=nof_antennas_dl,
        nof_antennas_ul=nof_antennas_ul,
        pdsch_mcs_table=pdsch_mcs_table,
        pusch_mcs_table=pusch_mcs_table,
        ntn_config=ntn_config,
        pdsch_interleaving_bundle_size=pdsch_interleaving_bundle_size,
    )

    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=always_download_artifacts,
    )

    ue_attach_info_dict = start_and_attach(
        ue_array=ue_array,
        gnb=gnb,
        fivegc=fivegc,
        gnb_post_cmd=gnb_post_cmd,
        plmn=plmn,
        inter_ue_start_period=inter_ue_start_period,
        ric=ric,
        channel_emulator=channel_emulator,
    )

    if ric:
        start_rc_xapp(ric=ric, control_service_style=2, action_id=6)
        start_kpm_mon_xapp(ric=ric, report_service_style=1, metrics="DRB.UEThpDl,DRB.UEThpUl")

    iperf_parallel(
        ue_attach_info_dict=ue_attach_info_dict,
        fivegc=fivegc,
        protocol=protocol,
        direction=direction,
        iperf_duration=iperf_duration,
        bitrate=bitrate,
        packet_length=packet_length,
        bitrate_threshold_ratio=bitrate_threshold,
    )

    if ric:
        stop_rc_xapp(ric)
        stop_kpm_mon_xapp(ric)

    sleep(wait_before_power_off)
    if ric:
        ric_validate_e2_interface(ric=ric, kpm_expected=True, rc_expected=True)

    stop(
        ue_array=ue_array,
        gnb=gnb,
        fivegc=fivegc,
        retina_data=retina_data,
        ue_stop_timeout=ue_stop_timeout,
        warning_as_errors=warning_as_errors,
        ric=ric,
        stop_gnb_first=stop_gnb_first,
    )

    metrics: Metrics = gnb.GetMetrics(Empty())

    if metrics.total.dl_bitrate + metrics.total.ul_bitrate <= 0:
        pytest.fail("No traffic detected in GNB metrics")

    if assess_bitrate and protocol == IPerfProto.UDP:

        assess_iperf_bitrate(
            bw=bandwidth,
            band=band,
            nof_antennas_dl=nof_antennas_dl,
            pdsch_mcs_table=pdsch_mcs_table,
            pusch_mcs_table=pusch_mcs_table,
            iperf_duration=iperf_duration,
            metrics=metrics,
            dl_brate_threshold=min_dl_bitrate,
            ul_brate_threshold=min_ul_bitrate,
        )
