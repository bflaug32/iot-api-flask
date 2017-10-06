#!/usr/bin/env bash

# setup environment variables in keys
cd /src
. ./config.sh

# run memcached (for temporary storage)
memcached -u root &

# run the server
gunicorn server:app -w 4 -b 0.0.0.0:5000

