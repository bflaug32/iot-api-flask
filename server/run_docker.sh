#!/usr/bin/env bash

# setup environment variables in keys
cd /src
. ./config.sh

# run memcached (for temporary storage)
memcached -u root &

# run the server
python server.py
