This is a all-in-one Docker container for Open5GS. At build, the container will use the latest tag of the open5gs repository (<https://github.com/open5gs/open5gs/tags>). To run a specific version of open5gs, line 48 in .Dockerfile (`# RUN echo "v2.4.3" > ./open5gsversion)` must be uncommented.

# Container Parameters

In [open5gs.env](open5gs.env) the following parameters can be set:

- MONGODB_IP (default: 127.0.0.1): This is the IP of the mongodb to use. 127.0.0.1 is the mongodb that runs inside this container.
- OPEN5GS_IP: This must be set to the IP of the container (here: 172.18.0.2).
- UE_IP_BASE: Defines the IP base used for connected UEs (here: 10.45.0).
- DEBUG (default: false): This can be set to true to run Open5GS in debug mode.

# Open5GS Parameters

Open5GS can be setup using [open5gs-5gc.yml](open5gs-5gc.yml).

# Usage

Create a Docker network to assign a specified IP to the Open5GS conainer (here: 172.18.0.2):

`docker network create --subnet=172.18.0.0/16 open5gsnet`

Build the Docker container using:

`docker build --target open5gs -t open5gs-docker .`

Then run the docker container with:

`docker run --net open5gsnet --ip 172.18.0.2 --env-file open5gs.env --privileged --publish 3000:3000 open5gs-docker ./build/tests/app/5gc -c open5gs-5gc.yml`

To use this container with srsgnb, the `mme_addr` in `enb.conf` must be set to the OPEN5GS_IP (here: 172.18.0.2). Furthermore, the `gtp_bind_addr` and `s1c_bind_addr` must be set to the local ethernet/wifi IP address, not a localhost IP.

To ping a connected UE setup the necessary route to the UE_IP_BASE + ".0/24" (here: 10.45.0) via the OPEN5GS_IP (here: 172.18.0.2) using:

`sudo ip ro add 10.45.0.0/24 via 172.18.0.2`

## Note

At startup of the container, all entries from the [user_db.csv](user_db.csv) will be added to the specified mongodb. If this is not desired comment `python3 add_users.py` in [open5gs_entrypoint.sh](open5gs_entrypoint.sh).

The Open5GS WebUI to manually add/change UEs to the mongodb can be accessed at [localhost:3000](localhost:3000).
