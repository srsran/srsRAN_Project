# srsRAN Project Docker

To build and launch the gnb, please run:

```bash
docker compose -f docker/docker-compose.yml up
```

If you want to force a new build of the container, please add a `--build` flag to the previous command.

For more options, check `docker compose --help`

## Customizations

- Default docker compose uses `configs/gnb_rf_b200_tdd_n78_20mhz.yml` config file. You can change it by setting the variable `${GNB_CONFIG_PATH}` in the shell, in the `docker compose up` command line or using an external env-file. More info about how to do it in docker documentation here: <https://docs.docker.com/compose/environment-variables/set-environment-variables/>

- Network: If you are using an existing core-network on same machine, then you can comment the `5gc` service section and also link your srsran container to some existing AMF N1/N2 subnet, doing something like this:

```yml
  gnb: ...
    networks:
      network1:
          ipv4_address: 192.168.70.163 # Setting a fixed IP in the "network1" net

networks:
  network1:
    name: my-pre-existing-network
    external: true
```

More info here: <https://docs.docker.com/compose/networking/>

## Advanced Usage

To only run either the srsRAN Project gNB or Open5GS from the docker-compose run:

```bash
docker compose -f docker/docker-compose.yml up <service>
```

where service is either `gnb` for the srsRAN Project gNB or `5gc` for Open5GS.

### Advanced Open5GS Container Parameters

For a more advanced parametrization of the Open5GS container, e.g. to load subscriber data from a csv-file the `open5gs.env` file in `srsgnb/docker/open5gs` can be used by running:

```bash
docker compose -f docker/docker-compose.yml --env-file docker/open5gs/open5gs.env up
```

In [open5gs.env](open5gs/open5gs.env) the following parameters can be set:

- MONGODB_IP (default: 127.0.0.1): This is the IP of the mongodb to use. 127.0.0.1 is the mongodb that runs inside this container.
- SUBSCRIBER_DB (default: "001010123456780,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8000,10.45.1.2"): This adds subscriber data for a single or multiple users to the Open5GS mongodb. It contains either:
  - Comma separated string with information to define a subscriber
  - `subscriber_db.csv`. This is a csv file that contains entries to add to open5gs mongodb. Each entry will represent a subscriber. It must be stored in `srsgnb/docker/open5gs/`
- OPEN5GS_IP: This must be set to the IP of the container (here: 10.53.1.2).
- UE_IP_BASE: Defines the IP base used for connected UEs (here: 10.45.0).
- DEBUG (default: false): This can be set to true to run Open5GS in debug mode.
