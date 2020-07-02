#!/bin/bash
CURRENT_DIR=$(dirname $(readlink -f $0))
VER=760de0a9ea0fec980a5244604ffa5b1acff2db7a
IMPL_DIR="${CURRENT_DIR}/impl"

rm -rf "${IMPL_DIR}"
mkdir -p "${IMPL_DIR}"

wget "https://raw.githubusercontent.com/ricab/scope_guard/${VER}/scope_guard.hpp" -O "${IMPL_DIR}/scope_guard.hpp"
