FROM tensorflow/tensorflow:latest-gpu

ARG uid=1000
ARG gid=1000

# Add a 'builder' user with the same uid/gid as the user running the container.
# Setup up the builder home directory, and ensure the it has the correct ownership.
RUN groupadd -r -g ${gid} builder && \
    useradd -rM -g builder -u ${uid} builder && \
    mkdir -p /home/builder && \
    echo "source /etc/bash_completion.d/git-prompt" >> /home/builder/.bashrc && \
    echo "source /usr/share/bash-completion/completions/git" >> /home/builder/.bashrc && \
    echo "PS1='${ps1}'" >> /home/builder/.bashrc && \
    chown -R builder:builder /home/builder

WORKDIR /home/builder/Navigator
USER builder

ENTRYPOINT ["/bin/bash"]