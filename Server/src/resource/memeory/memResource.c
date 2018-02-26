/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-02 19:08
* Last modified: 2017-08-16 05:40
* Email        : 374648064@qq.com
* Filename     : memResource.c
* Description  : 
******************************************************/

#include "resource/memeory/memResource.h"

bool getTotalPMDebug(MemInfo *totalMem, const char *file, const char *function, const int line)
{
	memset(totalMem, 0, sizeof(MemInfo));
	struct sysinfo i;
	long available;
	unsigned long pagecache;
	unsigned long wmark_low = 0;
	unsigned long pages[NR_LRU_LISTS];
	struct zone *zone;
	int lru;

/*
 * display in kilobytes.
 */
#define K(x) ((x) << (PAGE_SHIFT - 10))
	si_meminfo(&i);
	si_swapinfo(&i);

	for (lru = LRU_BASE; lru < NR_LRU_LISTS; lru++)
		pages[lru] = global_page_state(NR_LRU_BASE + lru);

	for_each_zone(zone)
		wmark_low += zone->watermark[WMARK_LOW];

	available = i.freeram - wmark_low;

	pagecache = pages[LRU_ACTIVE_FILE] + pages[LRU_INACTIVE_FILE];
	pagecache -= min(pagecache / 2, wmark_low);
	available += pagecache;

	available += global_page_state(NR_SLAB_RECLAIMABLE) -
		     min(global_page_state(NR_SLAB_RECLAIMABLE) / 2, wmark_low);

	if (available < 0)
		available = 0;

	totalMem->memTotal = K(i.totalram);
	totalMem->memAvailable = K(available);
	totalMem->totalswap = K(i.totalswap);
	totalMem->freeswap = K(i.freeswap);
#undef K

	return true;
}

unsigned long getProcessMAJ_FLT(pid_t pid)
{
	unsigned long maj_flt = 0;
	struct task_struct *p = pid_task(find_vpid(pid), PIDTYPE_PID);
	if(p != NULL)
	{
		task_lock(p);
		struct task_struct *t = p;
		do
		{
			maj_flt += t->maj_flt;
		}while_each_thread(p, t);

		maj_flt += p->signal->maj_flt;

		task_unlock(p);
	}
	return maj_flt;
}

bool getProcessMemInfo(pid_t pid, Process_Mem_Info *memInfo)
{
	memset(memInfo, 0, sizeof(Process_Mem_Info));
	struct task_struct *p = pid_task(find_vpid(pid), PIDTYPE_PID);
	if(p != NULL)
	{
		struct mm_struct *mm = get_task_mm(p);
		if(mm)
		{
			memInfo->rss = get_mm_rss(mm) << (PAGE_SHIFT - 10);
			memInfo->swap = get_mm_counter(mm, MM_SWAPENTS) << (PAGE_SHIFT - 10);
			mmput(mm);
		}

		return true;
	}

	return false;
}
