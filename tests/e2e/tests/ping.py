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
from retina.launcher.utils import param
from retina.launcher.test_base import BaseTest
from retina.protocol.base_pb2 import Empty, Integer, PingRequest
from retina.protocol.epc_pb2 import EPCStartInfo
from retina.protocol.gnb_pb2 import GNBStartInfo
from retina.protocol.ue_pb2 import UEStartInfo

from .utils import get_ue_gnb_epc, ATTACH_TIMEOUT, STARTUP_TIMEOUT

PING_COUNT = 10

class TestPing(BaseTest):
    @mark.parametrize(
        "band, common_scs, bandwidth, ul_mcs, dl_mcs",
        (
            # Test
            param(3, 15, 20, 28, 28, marks=mark.test, id="band:%s-scs:%s-bandwidth:%s-dl_mcs:%s-ul_mcs:%s"),
            # Smoke
            param(3, 15, 20, 28, 28, marks=mark.smoke, id="band:%s-scs:%s-bandwidth:%s-dl_mcs:%s-ul_mcs:%s"),
            param(41, 30, 20, 28, 28, marks=mark.smoke, id="band:%s-scs:%s-bandwidth:%s-dl_mcs:%s-ul_mcs:%s"),
            # ZMQ intensive
            param(3, 15, 5, 28, 28, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s-dl_mcs:%s-ul_mcs:%s"),
            param(3, 15, 10, 28, 28, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s-dl_mcs:%s-ul_mcs:%s"),
            param(3, 15, 20, 28, 28, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s-dl_mcs:%s-ul_mcs:%s"),
            param(3, 15, 50, 28, 28, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s-dl_mcs:%s-ul_mcs:%s"),
            param(7, 15, 5, 28, 28, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s-dl_mcs:%s-ul_mcs:%s"),
            param(7, 15, 10, 28, 28, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s-dl_mcs:%s-ul_mcs:%s"),
            param(7, 15, 20, 28, 28, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s-dl_mcs:%s-ul_mcs:%s"),
            param(7, 15, 50, 28, 28, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s-dl_mcs:%s-ul_mcs:%s"),
            param(41, 30, 10, 28, 28, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s-dl_mcs:%s-ul_mcs:%s"),
            param(41, 30, 20, 28, 28, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s-dl_mcs:%s-ul_mcs:%s"),
            param(41, 30, 50, 28, 28, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s-dl_mcs:%s-ul_mcs:%s"),
            # RF intensive
            param(3, 15, 10, 10, 10, marks=[mark.rf, mark.xfail], id="band:%s-scs:%s-bandwidth:%s-dl_mcs:%s-ul_mcs:%s"),
            param(7, 15, 10, 10, 10, marks=[mark.rf, mark.xfail], id="band:%s-scs:%s-bandwidth:%s-dl_mcs:%s-ul_mcs:%s"),
            param(41, 30, 10, 10, 10, marks=[mark.rf, mark.xfail], id="band:%s-scs:%s-bandwidth:%s-dl_mcs:%s-ul_mcs:%s"),
        ),
    )
    def test(
        self,
        extra,
        band,
        common_scs,
        bandwidth,
        ul_mcs,
        dl_mcs,
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
        ) as items:

            ue, gnb, epc = items

            ue_def = ue.GetDefinition(Empty())
            gnb_def = gnb.GetDefinition(Empty())
            epc_def = epc.GetDefinition(Empty())

            epc.AddUESubscriber(ue_def)
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
                    timeout=startup_timeout,
                )
            )
            logging.info("UE started")

            ue_attached_info = ue.WainUntilAttached(Integer(value=attach_timeout))
            logging.info("UE Attached %s", ue_attached_info)

            ue_to_epc_ping_task = ue.Ping.future(PingRequest(address=epc_def.tun_ip, count=ping_count))
            epc_to_ue_ping_task = epc.Ping.future(PingRequest(address=ue_attached_info.ipv4, count=ping_count))

            ue_to_epc_ping_result = ue_to_epc_ping_task.result()
            epc_to_ue_ping_result = epc_to_ue_ping_task.result()

            logging.info("Ue to EPC Ping: %s", ue_to_epc_ping_result)
            logging.info("EPC to UE Ping: %s", epc_to_ue_ping_result)

            assert ue_to_epc_ping_result.status is True and epc_to_ue_ping_result.status is True
