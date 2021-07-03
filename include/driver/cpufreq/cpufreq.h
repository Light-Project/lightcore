/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  lightcore/include/driver/cpufreq/cpufreq.h
 *  Cpufreq framework
 *  (C) 2020 wzl
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-01-24      first version 
 * 
 */

#ifndef _CPUFREQ_H_ 
#define _CPUFREQ_H_

#include <types.h>
#include <state.h>

typedef struct cpufreq{
    uint64_t (*freq_get)(void);
    state (*freq_set)(void);
    uint64_t min_freq;
    uint64_t max_freq;
} cpufreq_t;

uint64_t cpufreq_read(size_t cpu);
uint64_t cpufreq_write(uint64_t freq);

uint64_t cpufreq_min_read(size_t cpu);
state cpufreq_min_write(size_t cpu, uint64_t freq);

uint64_t cpufreq_max_read(size_t cpu);
state cpufreq_max_write(size_t cpu, uint64_t freq);

#endif
