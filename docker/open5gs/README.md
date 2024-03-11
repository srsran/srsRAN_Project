This is a all-in-one Docker container for Open5GS. At build, the container will use the specified version of the open5gs repository (default v2.6.6
). To run the latest tag of the open5gs repository (<https://github.com/open5gs/open5gs/tags>), line 51 and 52 in .Dockerfile
```
# get latest open5gs tag (must be stored in a file, because docker does not allow to use the return value directly)
# RUN git ls-remote --tags https://github.com/open5gs/open5gs | sort -t '/' -k 3 -V | awk -F/ '{ print $3 }' | awk '!/\^\{\}/' | tail -n 1 > ./open5gsversion
```
must be uncommented.


# Container Parameters

In [open5gs.env](open5gs.env) the following parameters can be set:

- MONGODB_IP (default: 127.0.0.1): This is the IP of the mongodb to use. 127.0.0.1 is the mongodb that runs inside this container.
- SUBSCRIBER_DB (default: "001010123456780,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8000,10.45.1.2"): This adds subscriber data for a single or multiple users to the Open5GS mongodb. It contains either:
  - Comma separated string with information to define a subscriber
  - `subscriber_db.csv`. This is a csv file that contains entries to add to open5gs mongodb. Each entry will represent a subscriber. It must be stored in `srsgnb/docker/open5gs/`
- OPEN5GS_IP: This must be set to the IP of the container (here: 10.53.1.2).
- UE_IP_BASE: Defines the IP base used for connected UEs (here: 10.45.0).
- DEBUG (default: false): This can be set to true to run Open5GS in debug mode.

```
# Kept in the following format: "Name,IMSI,Key,OP_Type,OP/OPc,AMF,QCI,IP_alloc"
#
# Name:     Human readable name to help distinguish UE's. Ignored by the HSS
# IMSI:     UE's IMSI value
# Key:      UE's key, where other keys are derived from. Stored in hexadecimal
# OP_Type:  Operator's code type, either OP or OPc
# OP/OPc:   Operator Code/Cyphered Operator Code, stored in hexadecimal
# AMF:      Authentication management field, stored in hexadecimal
# QCI:      QoS Class Identifier for the UE's default bearer.
# IP_alloc: IP allocation stratagy for the SPGW.
#           With 'dynamic' the SPGW will automatically allocate IPs
#           With a valid IPv4 (e.g. '10.45.0.2') the UE will have a statically assigned IP.
#
# Note: Lines starting by '#' are ignored and will be overwritten
# List of UEs with IMSI, and key increasing by one for each new UE. Useful for testing with AmariUE simulator and ue_count option
ue01,001010123456789,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,9001,9,10.45.1.2
ue02,001010123456790,00112233445566778899aabbccddef00,opc,63bfa50ee6523365ff14c1f45f88737d,9001,9,10.45.2.2
ue03,001010123456791,00112233445566778899aabbccddef01,opc,63bfa50ee6523365ff14c1f45f88737d,9001,9,10.45.3.2
```

# Open5GS Parameters

Open5GS can be setup using [open5gs-5gc.yml](open5gs-5gc.yml).

# Usage

Create a Docker network to assign a specified IP to the Open5GS conainer (here: 10.53.1.2):

`docker network create --subnet=10.53.1.0/16 open5gsnet`

Build the Docker container using:

`docker build --target open5gs -t open5gs-docker .`

You can overwrite open5gs version by adding `--build-arg OPEN5GS_VERSION=v2.6.6`

Then run the docker container with:

`docker run --net open5gsnet --ip 10.53.1.2 --env-file open5gs.env --privileged --publish 3000:3000 open5gs-docker ./build/tests/app/5gc -c open5gs-5gc.yml`

To use this container with srsgnb, the `addr` option under `amf` section in gnb configuration must be set OPEN5GS_IP (here: 10.53.1.2).
It could also be required to modify `bind_addr` option under `amf` section in gnb configuration to the local ethernet/wifi IP address for the host or container where gnb is running, not a localhost IP.

To ping a connected UE setup the necessary route to the UE_IP_BASE + ".0/24" (here: 10.45.0) via the OPEN5GS_IP (here: 10.53.1.2) using:

`sudo ip ro add 10.45.0.0/16 via 10.53.1.2`

## Note

The Open5GS WebUI to manually add/change UEs to the mongodb can be accessed at [localhost:3000](localhost:3000).
