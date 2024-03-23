#!/bin/bash

SCRIPT_DIR=$(pwd)
ET_SRC_DIR=$(pwd)/../executorch
ET_VENV=$ET_SRC_DIR/.executorch

cd $ET_SRC_DIR
git submodule sync
git submodule update --init
cd third-party/flatbuffers
git checkout 6ff9e90e7e399f3977e99a315856b57c8afe5b4d # flatbuffers version 24.3.7
git pull origin master

if [ -d "$ET_VENV" ] 
then
	echo "venv exists"
else
	python3 -m venv .executorch
fi

cd $ET_SRC_DIR
source .executorch/bin/activate

# Load toolchain
source ~/.bash_profile

# Fix error: Preparing metadata (pyproject.toml) did not run successfully.
pip cache purge
pip install meson
python -m pip install --upgrade pip
pip install importlib-metadata
python -m pip install --upgrade setuptools
pip install subprocess.run
pip3 install wheel setuptools pip --upgrade
pip install toml
pip install numpy --use-deprecated=legacy-resolver
pip install setuptools-metadata
# Fix error end #

pip install cmake
./install_requirements.sh

export PATH="$(pwd)/third-party/flatbuffers/cmake-out:${PATH}"
"$(pwd)/build/install_flatc.sh"