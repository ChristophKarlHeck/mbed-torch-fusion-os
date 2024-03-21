#!/bin/bash

cd ../executorch
git submodule sync
git submodule update --init
cd third-party/flatbuffers
git checkout 6ff9e90e7e399f3977e99a315856b57c8afe5b4d # flatbuffers version 24.3.7
git pull origin master
cd ../../