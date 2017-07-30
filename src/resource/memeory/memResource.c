/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-02 19:08
* Last modified: 2017-04-02 19:08
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
#undef K

	return true;
}

int getProcessVmRSS(pid_t pid)
{
	struct task_struct *p = pid_task(find_vpid(pid), PIDTYPE_PID);
	int ret = -1;
	if(p != NULL)
	{
		//task_lock(p);
		struct mm_struct *mm = get_task_mm(p);
		if(mm)
			ret = get_mm_rss(p->mm) << (PAGE_SHIFT - 10);
		//task_unlock(p);
	}

	return ret;
}
