FROM ghcr.io/fletcherpwalmsley/navigator-base:latest

RUN apt-get update && apt-get install -y \
    sudo \
    git \
    curl \
    gstreamer1.0-tools \
    gstreamer1.0-libav \
    gstreamer1.0-plugins-ugly \
    && rm -rf /var/lib/apt/lists/*

ARG uid=1000
ARG gid=1000

# Add a 'builder' user with the same uid/gid as the user running the container.
# Setup up the builder home directory, and ensure the it has the correct ownership.

ARG ps1="\[\033[0;32m\]\u@\h\[\033[m\]:\[\033[0;34m\]\w\[\033[m\]\[\033[0;36m\] \$(__git_ps1 "%s")\n\[\033[m\]$ "

RUN groupadd -r -g ${gid} builder && \
    useradd -rM -g builder -u ${uid} builder && \
    mkdir -p /home/builder && \
    echo "source /etc/bash_completion.d/git-prompt" >> /home/builder/.bashrc && \
    echo "source /usr/share/bash-completion/completions/git" >> /home/builder/.bashrc && \
    echo "PS1='${ps1}'" >> /home/builder/.bashrc && \
    chown -R builder:builder /home/builder

RUN usermod -aG sudo builder
RUN usermod -aG video builder
WORKDIR /home/builder/Navigator
USER builder