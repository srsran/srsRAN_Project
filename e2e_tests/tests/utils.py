import logging
import math
from collections import defaultdict
from contextlib import contextmanager, suppress
from pprint import pformat

import grpc
from pytest_html import extras
from retina.protocol.base_pb2 import Empty


@contextmanager
def get_ue_gnb_epc(self, extra, band, common_scs, bandwidth):
    """
    Get test elements
    """
    try:
        test_config = {
            "ue": {
                "parameters": {
                    "band": band,
                    "dl_arfcn": get_dl_arfcn(band),
                    "ssb_arfcn": get_ssb_arfcn(band, bandwidth),
                    "common_scs": common_scs,
                    "bandwidth": bandwidth,
                }
            },
            "gnb": {
                "parameters": {
                    "band": band,
                    "dl_arfcn": get_dl_arfcn(band),
                    "common_scs": common_scs,
                    "bandwidth": bandwidth,
                }
            },
        }
        logging.info("Test config: \n%s", pformat(test_config))
        self.test_config = test_config
        self.retina_manager.parse_configuration(test_config)

        # Get clients
        ue = self.retina_manager.get_ue()
        gnb = self.retina_manager.get_gnb()
        epc = self.retina_manager.get_epc()

        yield ue, gnb, epc

        logging.info("Test passed")

    except Exception as err:
        logging.error("Test failed")
        raise err from None

    finally:

        with suppress(UnboundLocalError, NameError):
            extra.append(extras.url(self.relative_output_html_path, name="[[ Go to logs and configs ]]"))

        with suppress(NameError, grpc._channel._InactiveRpcError):
            return_code = gnb.Stop(Empty()).value
            if return_code:
                logging.warning("GNB stopped with invalid exit code %s", return_code)
        with suppress(NameError, grpc._channel._InactiveRpcError):
            return_code = epc.Stop(Empty()).value
        with suppress(NameError, grpc._channel._InactiveRpcError):
            return_code = ue.Stop(Empty()).value

def get_dl_arfcn(band):
    """
    Get dl arfcn
    """
    return {3: 368500, 7: 536020}[band]


def get_ssb_arfcn(band, bandwidth):
    """
    Get SSB arfcn
    """
    return {
        3: defaultdict(
            lambda: 368410,
            {
                30: 367450,
                40: 366490,
                50: 365530,
            },
        ),
        7: defaultdict(
            lambda: 535930,
            {
                30: 534970,
                40: 534010,
                50: 533050,
            },
        ),
    }[band][bandwidth]
