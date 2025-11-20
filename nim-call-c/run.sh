#!/bin/bash

# Check if Nim is installed
if ! command -v nim &> /dev/null; then
    echo "Nim is not installed."
    exit 1
fi

# Get Nim version
nim_version=$(nim -v | head -n 1 | awk '{print $4}')
echo "Detected Nim version: $nim_version"

# Set download URL using Nim version
url="https://raw.githubusercontent.com/nim-lang/Nim/refs/tags/v${nim_version}/lib/nimbase.h"

# Set destination path
dest_path="./nim-call-c/nimbase.h"

# Download file
curl -L -o "$dest_path" "$url"

echo "Downloaded nimbase.h to $dest_path"

nim c --exceptions:quirky -d:release --gc:refc -d:useMalloc --noMain --app:lib --nimcache:./libsrc -o:libmylib.so e.nim