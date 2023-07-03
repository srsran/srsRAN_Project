#!/usr/bin/env python3

import bson
import click
import pymongo
import random

from misc.db.python.Open5GS import Open5GS

def add_user(imsi, key="00112233445566778899aabbccddeeff", op=None,
             opc="63bfa50ee6523365ff14c1f45f88737d", amf="9001", apn="srsapn", ip_alloc=""):
    '''Add UE data to Open5GS mongodb'''

    if op is not None:
        opc = None

    slice_data = [
        {
        "sst": 1,
        "default_indicator": True,
        "session": [
            {
            "name": apn,
            "type": 3, "pcc_rule": [], "ambr": {"uplink": {"value": 1, "unit": 3}, "downlink": {"value": 1, "unit": 3}},
            "qos": {
                "index": 9,
                "arp": {"priority_level": 8, "pre_emption_capability": 1, "pre_emption_vulnerability": 1}
            },
            "ue": {
                "addr": ip_alloc
            }
            },
            {
            "name": "ims",
            "type": 3, "pcc_rule": [], "ambr": {"uplink": {"value": 1, "unit": 3}, "downlink": {"value": 1, "unit": 3}},
            "qos": {
                "index": 5,
                "arp": {"priority_level": 8, "pre_emption_capability": 1, "pre_emption_vulnerability": 1}
            }
            }
        ]
        }
    ]

    sub_data = {
    "imsi": imsi,
    "subscribed_rau_tau_timer": 12,
    "network_access_mode": 2,
    "subscriber_status": 0,
    "access_restriction_data": 32,
    "slice" : slice_data,
    "ambr": {"uplink": {"value": 1, "unit": 3}, "downlink": {"value": 1, "unit": 3}},
    "security": {
        "k": key,
        "amf": amf,
        'op': op,
        "opc": opc
    },
    "schema_version": 1,
    "__v": 0
    }

    return sub_data


def read_from_user_db(db_file):
    '''Read UE data from user_db.csv. Note that apn is not included in the default srs user_db file'''
    db_data = []
    db_file = open(db_file, "r")

    for line in db_file:
        if line.startswith("#"):
            pass
        else:
            name, auth, imsi, key, op_type, op_c, amf, sqn, qci, ip_alloc = line.split(',')
            opc = op_c
            op = None
            if op_type == "op":
                op = op_c
                opc = None

            db_data.append({"imsi": imsi, "key": key, "op": op, "opc": opc, "amf": amf, "ip_alloc": ip_alloc.rstrip()})

    return db_data


@click.command()
@click.option("--mongodb", default="127.0.0.1", help="IP address or hostname of the mongodb instance.")
@click.option("--mongodb_port", default=27017, help="Port to connect to the mongodb instance.")
@click.option("--db_file", default="user_db.csv", help="Full path of the CSV user db file (SRS formatted).")
def main(mongodb, mongodb_port, db_file):

    open5gs_client = Open5GS(mongodb, mongodb_port)

    db_data = read_from_user_db(db_file)

    for ue in db_data:
        try:
            sub_data = add_user(**ue)
            print(open5gs_client.AddSubscriber(sub_data))  # Add Subscriber using dict of sub_data
        except pymongo.errors.DuplicateKeyError:
            print(f"UE (IMSI={ue['imsi']}) already exists, updating it.")
            sub_data = add_user(**ue)
            print(open5gs_client.UpdateSubscriber(ue['imsi'], sub_data))  # Update Subscriber using dict of sub_data


if __name__ == "__main__":
    main()
