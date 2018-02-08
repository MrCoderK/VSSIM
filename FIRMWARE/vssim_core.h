// File: vssim_core.h
// Date: 21-Sep-2017
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr)
// Copyright(c)2017
// Hanyang University, Seoul, Korea
// Embedded Software Systems Laboratory. All right reserved

#ifndef _VSSIM_CORE_H_
#define _VSSIM_CORE_H_

#include <stdbool.h>
#include "firm_buffer_manager.h"

extern pthread_cond_t* ssd_io_ready;

typedef void CallbackFunc(void *opaque, int ret);

typedef struct core_req_entry
{
	enum vssim_io_type io_type;
	uint64_t sector_nb;
	uint32_t length;
	void* buf;
	event_queue_entry* parent;
	bool flush;
	bool is_trimmed;
	struct core_req_entry* next;
}core_req_entry;

typedef struct core_req_queue
{
	int entry_nb;
	core_req_entry* head;
	core_req_entry* tail;
	pthread_mutex_t lock;	
}core_req_queue;

typedef struct vssim_core
{
	/* Per core request queues */
	core_req_queue read_queue;
	core_req_queue* write_queue;
	core_req_queue discard_queue;

	int flash_index;
	flash_info* flash_i;

	/* the number of bggc candidate planes */
	uint32_t n_bggc_planes;
	pthread_mutex_t n_bggc_lock;

	/* the number of fggc candidate planes */
	uint32_t n_fggc_planes;
	pthread_mutex_t n_fggc_lock;
	
	/* lock for per core GC */
	pthread_mutex_t gc_lock;

	/* flush command arrived */
	bool flush_flag;
	pthread_mutex_t flush_lock;
	event_queue_entry* flush_event;

}vssim_core;

extern vssim_core* vs_core;

struct nvme_dsm_range {
	uint32_t	cattr;
	uint32_t	nlb;
	uint64_t	slba;
};

void MAKE_TIMEOUT(struct timespec *tsp, long timeout_usec);

/* Initialize vssim core structure */
void INIT_VSSIM_CORE(void);
void TERM_VSSIM_CORE(void);
void INIT_PER_CORE_REQUEST_QUEUE(core_req_queue* cr_queue);
void INIT_FLASH_LIST(int core_id);
int GET_NEXT_FLASH_LIST_INDEX(int core_id, int cur_flash_index);

/* Main loop for each thread */
void *FIRM_IO_BUF_THREAD_MAIN_LOOP(void *arg);
void *SSD_IO_THREAD_MAIN_LOOP(void *arg);
void *BACKGROUND_GC_THREAD_MAIN_LOOP(void *arg);

/* IO Buffer Processing */
void INSERT_NEW_PER_CORE_REQUEST(int core_id, event_queue_entry* eq_entry, 
			uint64_t sector_nb, uint32_t length, int w_buf_index);
void INSERT_RW_TO_PER_CORE_EVENT_QUEUE(event_queue_entry* eq_entry, 
							int w_buf_index);
void INSERT_DISCARD_TO_PER_CORE_EVENT_QUEUE(event_queue_entry* eq_entry);
void INSERT_FLUSH_TO_PER_CORE_EVENT_QUEUE(event_queue_entry* eq_entry);

/* IO Event Processing */
core_req_entry* GET_PER_CORE_EVENT(int core_id);
core_req_entry* CREATE_NEW_CORE_EVENT(event_queue_entry* eq_entry, int core_id, 
		uint64_t sector_nb, uint32_t length, bool flush);
void WAKEUP_ALL_IO_THREADS(void);

/* IO Post Processing */
void END_PER_CORE_READ_REQUEST(core_req_entry* cr_entry);
void END_PER_CORE_WRITE_REQUEST(core_req_entry* cr_entry, int w_buf_index);
void END_PER_CORE_DISCARD_REQUEST(core_req_entry* cr_entry);
void END_PER_CORE_FLUSH_REQUEST(int core_id);

/* IO Core flush flag */
void SET_FLUSH_FLAG(int core_id);
void RESET_FLUSH_FLAG(int core_id);
bool TEST_FLUSH_FLAG(int core_id); 

/* Manipulate GC related information */
uint32_t GET_N_BGGC_PLANES(int core_id);
uint32_t GET_N_FGGC_PLANES(int core_id);
void INCREASE_N_BGGC_PLANES(int core_id);
void DECREASE_N_BGGC_PLANES(int core_id);
void INCREASE_N_FGGC_PLANES(int core_id);
void DECREASE_N_FGGC_PLANES(int core_id);
#endif
