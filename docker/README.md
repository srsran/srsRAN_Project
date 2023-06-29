# SRSRAN Project Docker

To build and launch the gnb, please run:

```bash
docker compose -f docker/docker-compose.yml up
```

If you want to force a new build of the container, please add a `--build` flag to the previous command.

For more options, check `docker compose --help`

## Customizations

- Default docker compose uses `configs/gnb_rf_b200_tdd_n78_20mhz.yml` config file. You can change it by modifying the config or adding a new one.
- Network: the default mode is `host`, so the container can access the host network. If you are using a core-network on same machine, then you can you link the container to some existing AMF N1/N2 subnet, doing something like this:

```yml
    networks:
      network1:
          ipv4_address: 192.168.70.163 # Setting a fixed IP in the "network1" net

networks:
  network1:
    name: my-pre-existing-network
    external: true
```

More info here: <https://docs.docker.com/compose/networking/>
