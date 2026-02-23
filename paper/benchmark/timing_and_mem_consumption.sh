#!/usr/bin/env bash

TLS_MS=(./Lidar/PRF133/TLS/PR133.in)
TLS=(./Lidar/ALS/SG_IR057.las ./Lidar/W01/MLS_W01.laz ./Lidar/MLS-b3p9sp11sf.laz)
TIME_CMD="/usr/bin/time -v"

echo "~~~~~~~~~~~~~~~~~~~~TLS-MS~~~~~~~~~~~~~~~~~~~~~~~~" | tee run.log

for f in $TLS_MS;
do
    REAL_F=$(realpath $f)
    BASE_DIR=$(dirname "$REAL_F")

    echo $REAL_F
    for v_size in 0.5 0.3 0.1;
    do
        $TIME_CMD -a -o run.log ./lvox "$REAL_F" -v "$v_size" -g "$BASE_DIR/out$v_size.h5" -j 12
        $TIME_CMD -a -o run.log ./lvox "$REAL_F" -v "$v_size" -g "$BASE_DIR/out$v_size.h5" -s -j 12
    done
done

echo "~~~~~~~~~~~~~~~~~~~~TLS~~~~~~~~~~~~~~~~~~~~~~~~~~" | tee --append run.log

for f in ${TLS[@]};
do
    REAL_F=$(realpath $f)
    BASE_DIR=$(dirname "$REAL_F")

    echo $REAL_F
    for v_size in 0.5 0.3 0.1;
    do
        $TIME_CMD -a -o run.log ./lvox "$REAL_F" -v "$v_size" -g "$BASE_DIR/out$v_size.h5" -j 12
        $TIME_CMD -a -o run.log ./lvox "$REAL_F" -v "$v_size" -g "$BASE_DIR/out$v_size.h5" -s -j 12
    done
done

echo "~~~~~~~~~~~~~~~~~~~~MLS~~~~~~~~~~~~~~~~~~~~~~~~~~" | tee --append run.log

REAL_F=./Lidar/PRF133/MLS/MLS-PRF133.laz
REAL_F_TRAJ=./Lidar/PRF133/MLS/MLS-PRF133-traj.txt
BASE_DIR=$(dirname "$REAL_F")

echo $REAL_F
for v_size in 0.5 0.3;
do
    $TIME_CMD -a -o run.log ./lvox "$REAL_F" -v "$v_size" -t "$REAL_F_TRAJ" -g "$BASE_DIR/out$v_size.h5" -j 12
    $TIME_CMD -a -o run.log ./lvox "$REAL_F" -v "$v_size" -t "$REAL_F_TRAJ" -g "$BASE_DIR/out$v_size.h5" -s -j 12
done
