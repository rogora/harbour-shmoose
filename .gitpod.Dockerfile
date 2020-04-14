FROM gitpod/workspace-full

# Install custom tools, runtime, etc.
RUN sudo apt-get update \
    && sudo apt-get install -y qt5-qmake qtbase5-dev qt5-default \
    && sudo rm -rf /var/lib/apt/lists/*
