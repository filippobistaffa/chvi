#!/usr/bin/env bash

bin="./chvi"
pprof="google-pprof"
trace="trace"

${bin} $*
${pprof} --gv ${bin} ${trace}.prof
