#!/usr/bin/env bash

./generate_database.sh
cd frontend
./compile.sh
./deploy.sh