#
# Copyright 2013-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Test ping
"""
import logging

from pytest import mark
from retina.launcher.test_base import BaseTest
from retina.launcher.utils import param
from retina.protocol.base_pb2 import Empty, PingRequest, UInteger
from retina.protocol.epc_pb2 import EPCStartInfo
from retina.protocol.gnb_pb2 import GNBStartInfo
from retina.protocol.ue_pb2 import UEStartInfo

from .utils import ATTACH_TIMEOUT, DEFAULT_MCS, STARTUP_TIMEOUT, get_ue_gnb_epc

PING_COUNT = 10


class TestPing(BaseTest):
    ID = "band:%s-scs:%s-bandwidth:%s-log_search:%s-timing_advance:%s-calibration:%s"

    @mark.parametrize(
        "ue_count",
        (
            param(1, id="singleue"),
            param(4, id="multiue", marks=mark.multiue),
        ),
    )
    @mark.parametrize(
        "band, common_scs, bandwidth, log_search, global_timing_advance, time_alignment_calibration",
        (
            param(3, 15, 5, True, 0, 0, marks=mark.zmq, id=ID),
            param(3, 15, 10, False, -1, "auto", marks=(mark.zmq, mark.rf), id=ID),
            param(3, 15, 20, True, 0, 0, marks=(mark.zmq, mark.test), id=ID),
            param(3, 15, 50, True, 0, 0, marks=mark.zmq, id=ID),
            param(41, 30, 10, False, -1, "auto", marks=(mark.zmq, mark.rf), id=ID),
            param(41, 30, 20, True, 0, 0, marks=mark.zmq, id=ID),
            param(41, 30, 50, True, 0, 0, marks=mark.zmq, id=ID),
        ),
    )
    def test(
        self,
        extra,
        band,
        common_scs,
        bandwidth,
        log_search,
        global_timing_advance,
        time_alignment_calibration,
        ue_count,
        mcs=DEFAULT_MCS,
        startup_timeout=STARTUP_TIMEOUT,
        attach_timeout=ATTACH_TIMEOUT,
        ping_count=PING_COUNT,
    ):
        logging.info("Ping Test")

        with get_ue_gnb_epc(
            self,
            extra,
            band=band,
            common_scs=common_scs,
            bandwidth=bandwidth,
            mcs=mcs,
            log_search=log_search,
            global_timing_advance=global_timing_advance,
            time_alignment_calibration=time_alignment_calibration,
            ue_count=ue_count,
        ) as items:
            ue, gnb, epc = items

            ue_def = ue.GetDefinition(Empty())
            gnb_def = gnb.GetDefinition(Empty())
            epc_def = epc.GetDefinition(Empty())

            for subscriber in ue_def.subscriber_list:
                epc.AddUESubscriber(subscriber)

            epc.Start(EPCStartInfo())
            logging.info("EPC started")

            gnb.Start(
                GNBStartInfo(
                    ue_definition=ue_def,
                    epc_definition=epc_def,
                    timeout=startup_timeout,
                )
            )
            logging.info("GNB started")

            ue.Start(
                UEStartInfo(
                    gnb_definition=gnb_def,
                    epc_definition=epc_def,
                    timeout=startup_timeout,
                )
            )
            logging.info("UE started")

            ue_attached_info_list = ue.WainUntilAttached(UInteger(value=attach_timeout))
            logging.info("UEs attached %s", ue_attached_info_list)

            for ue_attached_info in ue_attached_info_list.value:
                ping_task_dict = {}
                ping_task_dict[f"UE {ue_attached_info.ipv4} -> EPC {ue_attached_info.ipv4_gateway}"] = ue.Ping.future(
                    PingRequest(address=ue_attached_info.ipv4_gateway, count=ping_count)
                )
                ping_task_dict[f"EPC {ue_attached_info.ipv4_gateway} -> UE {ue_attached_info.ipv4}"] = epc.Ping.future(
                    PingRequest(address=ue_attached_info.ipv4, count=ping_count)
                )

                ping_result_dict = {key: ping_task.result() for key, ping_task in ping_task_dict.items()}

                for key, value in ping_result_dict.items():
                    logging.info("Ping %s: %s", key, value)

                assert all(map(lambda r: r.status, ping_result_dict.values())) is True, "Ping failed!"
