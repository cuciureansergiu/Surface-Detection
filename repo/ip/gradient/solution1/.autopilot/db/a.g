#!/bin/sh
lli=${LLVMINTERP-lli}
exec $lli \
    /home/sergiu/workspace/gradient/hls/gradient/solution1/.autopilot/db/a.g.bc ${1+"$@"}
