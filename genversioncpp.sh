#!/bin/sh

[ -z "$1" ] && exit 1

cat << EOF > src/sbo_version.cpp
#include <string>
#include "sbo_version.h"

string const sbosubmit_version = "$1";
EOF
