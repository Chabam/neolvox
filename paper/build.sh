#!/usr/bin/env bash

podman run --privileged \
       --rm \
       --volume $PWD:/data \
       --env JOURNAL=joss \
       docker.io/openjournals/inara \
       -o pdf \
       ./paper.md
