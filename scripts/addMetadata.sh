#!/bin/bash
# Script to add document metadata from the Gcatalog file using dclient.
# Usage: scripts/addMetadata
# Check if exactly zero arguments (the input file) is provided
if [ "$#" -ne 0 ]; then
    echo "Usage: scripts/$0"
    exit 1
fi

INPUT_FILE="dataset/Gcatalog.tsv"

# Check if input file exists before proceeding
if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: File '$INPUT_FILE' not found."
    exit 1
fi

# Initialize a counter for processing documents
COUNT=0

# Read the input file line by line, using tab ('\t') as a delimiter
# The first line (header) is skipped
while IFS=$'\t' read -r filename title year authors; do
    COUNT=$((COUNT + 1))

    # Print document metadata being processed
    echo "------------------------"
    echo "Filename: $filename"
    echo "Title: $title"
    echo "Year: $year"
    echo "Authors: $authors"

    # Call the dclient program with extracted metadata
    bin/dclient -a "$title" "$authors" $year "$filename"

done < <(tail -n +2 "$INPUT_FILE")

echo -e "\nAdded metadata for $COUNT files."