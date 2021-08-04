FROM ubuntu:18.04
LABEL vendor="Eaglys Inc." \
      version="1.0.0" \
      seal_version="3.6.5" \
      release-date="2021-02-20" \
      description="This image is for capusleflow using Microsoft/seal"
ENV DEBIAN_FRONTEND=noninteractive \
    SEAL_VERSION=3.6.5 \
    CMAKE_VERSION=3.19.5 \ 
    PYTHON_VERSION=3.6.9 \ 
    GOOGLETEST_VERSION=1.10.0

RUN apt-get update && \
    apt-get install -y build-essential \
        sudo \
        wget \
        m4 \
        libgmp-dev \
        file \
        git \
        vim \
        libomp-dev \
        libssl-dev \
        zlib1g-dev \
        libbz2-dev \
        libreadline-dev \
        libsqlite3-dev \
        curl \
        llvm \
        libncurses5-dev \
        xz-utils \
        tk-dev \
        libxml2-dev \
        libxmlsec1-dev \
        libffi-dev \
        liblzma-dev \
        libeigen3-dev && \
        apt-get clean && \
        rm -rf /var/lib/apt-get/lists/*

# CMake Installation 
WORKDIR /include
RUN wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}.tar.gz && tar -zxvf ./cmake-${CMAKE_VERSION}.tar.gz && \
    cd cmake-${CMAKE_VERSION} && \
    ./bootstrap --prefix=/opt/cmake -- -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_USE_OPENSSL=OFF && \
    make -j4 && make install && make clean \
    && rm  /include/cmake-${CMAKE_VERSION}.tar.gz

# Google test installaiton
RUN wget https://github.com/google/googletest/archive/release-${GOOGLETEST_VERSION}.tar.gz && tar -xzvf release-${GOOGLETEST_VERSION}.tar.gz &&\
    cd googletest-release-${GOOGLETEST_VERSION} && mkdir build && cd build && \
    /opt/cmake/bin/cmake .. && make -j4 && make install && make clean && \
    rm /include/release-${GOOGLETEST_VERSION}.tar.gz

# Install Python by pyenv and pyenv-virtualenv 
SHELL ["/bin/bash", "-c"]

## SEAL installation
#RUN wget https://github.com/microsoft/SEAL/archive/v${SEAL_VERSION}.tar.gz && \
#    tar -xzvf v${SEAL_VERSION}.tar.gz && \
#    rm v${SEAL_VERSION}.tar.gz && \
#    cd SEAL-${SEAL_VERSION} && \
#    /opt/cmake/bin/cmake -S . -B build && \
#    /opt/cmake/bin/cmake --build build -- -j4 && \
#    /opt/cmake/bin/cmake --install build && \
#    echo export PATH="$PATH:/opt/cmake/bin" > ~/.bashrc 

# SEAL installation
RUN git clone https://github.com/kenmaro3/SEAL.git && \
    cd SEAL && \
    git checkout -b ckks_coeff_365 origin/ckks_coeff_365 &&\
    /opt/cmake/bin/cmake . -DCMAKE_BUILD_TYPE=Release -DSEAL_BUILD_EXAMPLES=OFF -DSEAL_BUILD_TESTS=OFF -DSEAL_BUILD_BENCH=OFF -DSEAL_BUILD_DEPS=ON -DSEAL_USE_INTEL_HEXL=ON -DSEAL_USE_MSGSL=ON -DSEAL_USE_ZLIB=ON -DSEAL_USE_ZSTD=ON -DBUILD_SHARED_LIBS=OFF -DSEAL_BUILD_SEAL_C=ON -DSEAL_USE_CXX17=ON -DSEAL_USE_INTRIN=ON &&\
    make -j4 && make install &&\
    #/opt/cmake/bin/cmake --build build -- -j4 && \
    #/opt/cmake/bin/cmake --install build && \
    echo export PATH="$PATH:/opt/cmake/bin" > ~/.bashrc 
    
# python installation
RUN git clone https://github.com/pyenv/pyenv.git ~/.pyenv &&\
    echo 'export PYENV_ROOT="$HOME/.pyenv"' >> ~/.bashrc &&\
    echo 'export PATH="$PYENV_ROOT/bin:$PATH"' >> ~/.bashrc &&\
    echo "export PYENV_VIRTUALENV_DISABLE_PROMPT=1" >> ~/.bashrc &&\
    echo "export SEAL_VERSION="${SEAL_VERSION} >> ~/.bashrc &&\
    echo -e 'if command -v pyenv 1>/dev/null 2>&1; then\n eval "$(pyenv init -)"\nfi' >> ~/.bashrc &&\
    source ~/.bashrc &&\
    pyenv install ${PYTHON_VERSION} &&\
    git clone https://github.com/pyenv/pyenv-virtualenv.git $(pyenv root)/plugins/pyenv-virtualenv &&\
    echo 'eval "$(pyenv virtualenv-init -)"' >> ~/.bashrc && source ~/.bashrc

### Install Capsulflow by "python setup.py install"
COPY requirements.txt /include/requirements.txt 
COPY . /ml_seal 
#WORKDIR /ml_seal
#RUN source ~/.bashrc &&\
#    pyenv virtualenv ${PYTHON_VERSION} myenv && pyenv global ${PYTHON_VERSION} && source activate myenv && \
#    pip install --no-cache-dir --upgrade pip setuptools && pip install --no-cache-dir -r requirements.txt && \
#    python setup.py build_ext --inplace

WORKDIR /ml_seal
RUN source ~/.bashrc &&\
    pyenv virtualenv ${PYTHON_VERSION} myenv && pyenv global ${PYTHON_VERSION} && source activate myenv

RUN source ~/.bashrc && source activate myenv && ~/.pyenv/versions/myenv/bin/pip install --upgrade pip setuptools && ~/.pyenv/versions/myenv/bin/pip install -r requirements.txt
RUN source ~/.bashrc && source activate myenv && ~/.pyenv/versions/myenv/bin/python setup.py install && ~/.pyenv/versions/myenv/bin/python setup.py build_ext --inplace

## user add 
#RUN useradd -m coder && echo "coder:coder" | chpasswd && adduser coder sudo
#USER coder

