#!/bin/bash
set -ex
base_dir="$(cd "$(dirname "$0")" && pwd)"
ln -s "$base_dir"/pre-commit "$base_dir"/../../.git/hooks/