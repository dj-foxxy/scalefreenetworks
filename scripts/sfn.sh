#!/bin/bash

set -o errexit
set -o nounset

readonly PROJ="$(readlink -f -- "$(dirname -- "${0}")/..")"

"${PROJ}/bin/sfn/sfn" "${@}"

