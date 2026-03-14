/**
 * Copyright (c) 2015-2019 Shanghai Fullhan Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-01-24     tangyh    the first version
 *
 */

#include "dma_mem.h"

#include "types/bufCtrl.h"
/*****************************************************************************
 * Define section
 * add all #define here
 *****************************************************************************/
/* #define FH_TEST_DMA_MEM */

/****************************************************************************
 * ADT section
 *  add definition of user defined Data Type that only be used in this file here
 ***************************************************************************/

/******************************************************************************
 * Function prototype section
 * add prototypes for all functions called by this file,execepting those
 * declared in header file
 *****************************************************************************/

/*****************************************************************************
 * Global variables section - Exported
 * add declaration of global variables that will be exported here
 * e.g.
 *  int8_t foo;
 ****************************************************************************/

/*****************************************************************************

 *  static fun;
 *****************************************************************************/

/*****************************************************************************
 * Global variables section - Local
 * define global variables(will be refered only in this file) here,
 * static keyword should be used to limit scope of local variable to this file
 * e.g.
 *  static uint8_t ufoo;
 *****************************************************************************/

/* static struct rt_memheap dma_heap = {0}; */

static cmm_handle dma_handle = 0;

/* function body */
/*****************************************************************************
 * Description:
 *      add funtion description here
 * Parameters:
 *      description for each argument, new argument starts at new line
 * Return:
 *      what does this function returned?
 *****************************************************************************/
rt_err_t fh_dma_mem_init(rt_uint32_t *mem_start, rt_uint32_t size)
{
    /* return rt_memheap_init(&dma_heap,"dma_heap",mem_start,size); */
    dma_handle = cmm_init((unsigned char *)mem_start, size);
    return dma_handle == 0;
}

void *fh_dma_mem_malloc_align(rt_uint32_t size, rt_uint32_t align, char *name)
{
    MEM_DESC md;

    if (!dma_handle)
        return 0;
    if (cmm_malloc(dma_handle, &md, size, align, name) == 0)
        return md.vbase;
    else
        return 0;
}

void *fh_dma_mem_malloc(rt_uint32_t size)
{
    return fh_dma_mem_malloc_align(size, 4, "dma_heap");
    /* return rt_memheap_alloc(&dma_heap, size); */
}

void fh_dma_mem_free(void *ptr)
{
    /* rt_memheap_free(ptr); */
    /* fixme: use uinit to free all; */
}

void fh_dma_mem_uninit(void) { cmm_free(dma_handle); }
#ifdef FH_TEST_DMA_MEM
int dma_mem_debug(void *ptr)
{
    /* rt_memheap_free(ptr); */
    rt_kprintf("dma mem start 0x%08x\n", (rt_uint32_t)dma_heap.start_addr);
    rt_kprintf("dma mem total size 0x%08x\n", dma_heap.pool_size);
    rt_kprintf("dma mem left size 0x%08x\n", dma_heap.available_size);
    rt_kprintf("dma mem max use size 0x%08x\n", dma_heap.max_used_size);
    return 0;
}
#endif

#ifdef RT_USING_FINSH
#include <finsh.h>
#ifdef FH_TEST_DMA_MEM
FINSH_FUNCTION_EXPORT(dma_mem_debug, dma_start & left size & max_use);
#endif
#endif

