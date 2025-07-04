#ifndef __PLATFORM_H__
#define __PLATFORM_H__

// stack
#define _MAIN_STACK_SIZE 2048
#define _ISR_STACK_SIZE  512

//vector
#define VECTOR_ADDRESS ((uintptr_t)_vector_start)
#define VTOR_MASK      SCB_VTOR_TBLOFF_Msk

#endif