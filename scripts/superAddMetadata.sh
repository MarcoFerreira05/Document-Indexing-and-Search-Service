#!/bin/bash
# Script to execute the addMetadata script a lot of times
# Usage: scripts/supperAddMetadata.sh <number_additions>

# Check if exactly one argument (the number of additions) is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: scripts/$0 <number_additions>"
    exit 1
fi

NUMBER_CONSULTS=$1
COUNT=0

while [ $COUNT -lt $NUMBER_CONSULTS ]
do
    scripts/addMetadata.sh
    ((COUNT++))
done

echo "Did $COUNT additions."