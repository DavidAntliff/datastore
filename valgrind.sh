#!/bin/bash
#GEN_SUPPRESSIONS="--gen-suppressions=all"
valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=../valgrind.suppress --run-libc-freeres=yes ${GEN_SUPPRESSIONS} ./test_datastore $@

