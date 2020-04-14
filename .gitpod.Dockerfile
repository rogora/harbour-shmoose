FROM gitpod/workspace-full

USER root

# Install custom tools, runtime, etc. using apt-get
# For example, the command below would install "bastet" - a command line tetris clone:
#
RUN apt-get update \
	&& apt-get install -y qt5-default libqt5sql5-sqlite libswiften-dev qtdeclarative5-dev qml-module-qtquick2 qml-module-qtquick-controls \
	&& apt-get clean && rm -rf /var/cache/apt/*
