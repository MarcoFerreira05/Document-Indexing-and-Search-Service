#!/bin/bash
# Script to make a lot of consults in order to evaluate the impact of the cache
# Usage: scripts/testCache <number_consults>

if [ "$#" -ne 1 ]; then
    echo "Usage: scripts/$0 <number_consults> "
    exit 1
fi

NUMBER_CONSULTS=$1
COUNT=0
KEY=-1

while [ $COUNT -lt $NUMBER_CONSULTS ]
do
    echo "----"
    KEY=$COUNT
    echo "Consulting key $KEY:"
    bin/dclient -c "$KEY"
    ((COUNT++))
done

echo "Did $COUNT consults."