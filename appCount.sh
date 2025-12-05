#!/bin/bash

# Syslog identifier
logId="AppCounter"

# TotalCount of File Descriptors (FDs)
totalCount=0

# Get all PIDs of app /usr/sbin/sshd
allPids=$(pgrep /usr/sbin/sshd)

if [ -z "$allPids" ]; then
    logger -t "$logId" "No processes found"
    exit 0
fi

# Loop through each PID
for pid in $allPids; do

    # Run ./app -a $pid and capture output, count lines
    count=$(./appList -a $pid | wc -l)

    # Add to total
    totalCount=$((totalCount + count))
done

# Log total count
logger -t "$logId" "Total FDs across all PIDs: $totalCount"