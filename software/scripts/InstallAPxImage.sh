#!/bin/bash

# Check if the user provided a file as an argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <file-to-transfer>"
    exit 1
fi

# Variables
LOCAL_FILE=$1
FILE_NAME=$(basename "$LOCAL_FILE") # Extract the filename without the path
REMOTE_USER="bareese"
REMOTE_HOST="192.168.22.110"
REMOTE_PATH="~"

# Step 1: Copy the file to the remote server
echo "Copying $LOCAL_FILE to $REMOTE_USER@$REMOTE_HOST:$REMOTE_PATH"
scp "$LOCAL_FILE" "$REMOTE_USER@$REMOTE_HOST:$REMOTE_PATH"

# Step 2: SSH into the remote server and execute commands
echo "Logging into $REMOTE_USER@$REMOTE_HOST and executing commands"
ssh -tt "$REMOTE_USER@$REMOTE_HOST" << EOF
    # Check if the file is a .gz file
    if [[ "$FILE_NAME" == *.gz ]]; then
        # Unzip the file, keeping the original .gz file
        gunzip -c "$FILE_NAME" > "${FILE_NAME%.gz}"
        FILE_NAME_UNZIPPED="${FILE_NAME%.gz}" # Get the unzipped file name
    else
        FILE_NAME_UNZIPPED="$FILE_NAME"
    fi

    # Remove the existing symlink if it exists
    rm -f fpga.bit

    # Create a new symlink to the unzipped file or the original file
    ln -s "\$FILE_NAME_UNZIPPED" fpga.bit

    # Execute additional commands
    apx-prime -H localhost -c config.ini
    exit
EOF

echo "Script completed." 
