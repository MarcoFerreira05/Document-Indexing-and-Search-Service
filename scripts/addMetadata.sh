#!/bin/bash
# Script to add document metadata from the Gcatalog file using dclient.
# Usage: ./addMetadata.sh <Gcatalog_file>

# Debug: Print the number of arguments
echo "Number of arguments: $#"
echo "Arguments: $@"

# Check if exactly one argument (the input file) is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <Gcatalog_file>"
    exit 1
fi

INPUT_FILE="$1"

# Debug: Print the input file path
echo "Input file: $INPUT_FILE"

# Check if input file exists before proceeding
if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: File '$INPUT_FILE' not found."
    exit 1
fi

# Initialize a counter for processing documents
COUNT=0

# Save the current directory
ORIGINAL_DIR=$(pwd)

# Debug: Print the original directory
echo "Original directory: $ORIGINAL_DIR"

# Change to the directory containing the dclient binary
BIN_DIR="$(dirname "$0")/../bin"
echo "Changing to directory: $BIN_DIR"
cd "$BIN_DIR" || { echo "Failed to change directory to $BIN_DIR"; exit 1; }

# Debug: Print the current directory after changing
echo "Current directory after change: $(pwd)"

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
    ./dclient -a "$title" "$authors" "$year" "$filename"

done < <(tail -n +2 "$INPUT_FILE")

# Change back to the original directory
cd "$ORIGINAL_DIR" || { echo "Failed to change back to the original directory"; exit 1; }

echo -e "\nAdded metadata for $COUNT files."
