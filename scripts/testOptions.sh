#!/bin/bash
# Script to test all dclient commands using Gcatalog file.
# Usage: scripts/addMetadata [max_tests]

if [ "$#" -gt 1 ]; then
    echo "Usage: scripts/$0 [max_tests]"
    exit 1
fi

INPUT_FILE="dataset/Gcatalog.tsv"

if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: File '$INPUT_FILE' not found."
    exit 1
fi

# Default: test all lines unless max_tests is provided
MAX_TESTS=0
if [ "$#" -eq 1 ]; then
    MAX_TESTS=$1
    if ! [[ "$MAX_TESTS" =~ ^[0-9]+$ ]] || [ "$MAX_TESTS" -le 0 ]; then
        echo "Error: max_tests must be a positive integer."
        exit 1
    fi
fi

COUNT=0

while IFS=$'\t' read -r filename title year authors; do
    COUNT=$((COUNT + 1))
    if [ "$MAX_TESTS" -ne 0 ] && [ "$COUNT" -gt "$MAX_TESTS" ]; then
        break
    fi

    echo "------------------------"

    document_id=$(($RANDOM % 2500))

    # 1. Consult document metadata
    echo "--> Running: bin/dclient -c $document_id"
    bin/dclient -c $document_id

    # 2. Lookup document by keyword (use second word of title as keyword)
    keyword=$(echo "$title" | awk '{print $2}')
    echo "--> Running: bin/dclient -l $document_id \"$keyword\""
    bin/dclient -l $document_id "$keyword"

    # 3. Search by keyword (use second word of title as keyword, n_procs=1)
    echo "--> Running: bin/dclient -s \"$keyword\" 12 (output truncated to 10 words)"
    bin/dclient -s "$keyword" 12 | tr -s '[:space:]' ' ' | cut -d' ' -f1-10

    # 4. Delete document
    echo "--> Running: bin/dclient -d $document_id"
    bin/dclient -d $document_id

done < <(tail -n +2 "$INPUT_FILE")

# 5. Delete document
echo "--> Running: bin/dclient -f"
bin/dclient -f

echo -e "\nTested all commands for $(( (MAX_TESTS>0 && COUNT>MAX_TESTS) ? MAX_TESTS : COUNT )) files."
