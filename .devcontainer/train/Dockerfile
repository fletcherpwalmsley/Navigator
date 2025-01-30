FROM tensorflow/tensorflow:latest-gpu

RUN apt-get update && apt-get install -y \
    sudo \
    git \
    curl \
    && rm -rf /var/lib/apt/lists/*

# ARG uid=1000
# ARG gid=1000

# Add a 'builder' user with the same uid/gid as the user running the container.
# Setup up the builder home directory, and ensure the it has the correct ownership.

ARG ps1="\[\033[0;32m\]\u@\h\[\033[m\]:\[\033[0;34m\]\w\[\033[m\]\[\033[0;36m\] \$(__git_ps1 "%s")\n\[\033[m\]$ "

# RUN groupadd -r -g ${gid} builder && \
#     useradd -rM -g builder -u ${uid} builder && \
#     mkdir -p /home/builder && \
#     echo "source /etc/bash_completion.d/git-prompt" >> /home/builder/.bashrc && \
#     echo "source /usr/share/bash-completion/completions/git" >> /home/builder/.bashrc && \
#     echo "PS1='${ps1}'" >> /home/builder/.bashrc && \
#     chown -R builder:builder /home/builder

# RUN usermod -aG sudo builder
# WORKDIR /home/builder/Navigator
# USER builder


# Create a new user 'newuser' and set a password
RUN useradd -m builder && echo "builder:password" | chpasswd

# Add the new user to the sudo group
RUN usermod -aG sudo builder video

# Allow the new user to use sudo without a password
RUN echo "builder ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# Switch to the new user
USER builder

# Set the working directory
WORKDIR /home/builder/Navigator