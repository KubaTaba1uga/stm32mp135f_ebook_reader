#!/bin/bash
set -xeu

BUILD_DIR="$1"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cp "$SCRIPT_DIR"/*.env "$BUILD_DIR/board/st/stm32mp1/"
