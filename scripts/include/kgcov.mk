# SPDX-License-Identifier: GPL-2.0-or-later
kgcov-cflags-y += -fprofile-arcs -ftest-coverage
kgcov-cflags-y += -fno-tree-loop-im
export CFLAGS_KGCOV := $(kgcov-cflags-y)
