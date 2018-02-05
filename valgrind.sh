#!/bin/bash
valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=../valgrind.suppress --run-libc-freeres=yes ./test_datastore $@

