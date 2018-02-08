// File: ftl_gc_manager.h
// Date: 18-Sep-2017
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr)
// Copyright(c)2017
// Hanyang University, Seoul, Korea
// Embedded Software Systems Laboratory. All right reserved

#ifndef _GC_MANAGER_H_
#define _GC_MANAGER_H_

#define GC_THREAD_MIN_SLEEP_TIME	20	/* milliseconds */
#define GC_THREAD_MAX_SLEEP_TIME	200	/* milliseconds */

extern unsigned int gc_count;

int GET_GC_LOCK(plane_info* plane_i);
void RELEASE_GC_LOCK(plane_info* plane_i);

void FGGC_CHECK(int core_id);
void CHECK_EMPTY_BLOCKS(int core_id, pbn_t pbn);

int PLANE_GARBAGE_COLLECTION(plane_info* plane_i);
int GARBAGE_COLLECTION(block_entry* victim_entry);

block_entry* SELECT_VICTIM_BLOCK(void);
block_entry* SELECT_VICTIM_BLOCK_FROM_FLASH(flash_info* flash_i);
block_entry* SELECT_VICTIM_BLOCK_FROM_PLANE(plane_info* plane_i);

void INCREASE_SLEEP_TIME(long* t_sleep_ms);
void DECREASE_SLEEP_TIME(long* t_sleep_ms);

#endif
