#include "console.h"

#define PAGE_SIZE 4096
#define PT_SIZE (1024 * sizeof(unsigned int))

unsigned int ptd[1024] __attribute__((aligned(PAGE_SIZE))); // Page directory
unsigned int pte[1024][1024] __attribute__((aligned(PAGE_SIZE))); // Array of page tables

static inline void lcr3(unsigned int val)
{
    asm volatile("movl %0,%%cr3" : : "r" (val));
}

static inline void halt(void)
{
    asm volatile("hlt" : : );
}

int main(void)
{
    int i, j;
    int sum = 0;


    uartinit(); 

    printk("Hello from C\n");

    
//     unsigned int phys_addr = 0;
for (i = 0; i < 64; i++) {
    ptd[i] = ((unsigned int)&pte[i]) | 0b00000111; // Set present, writable, and user bits
    for (j = 1024*i; j < 1024*(i+1); j++) {
       	int PAGE_SIZE1 =0x1000; // Map virtual page to physical page
		PAGE_SIZE1 = PAGE_SIZE1 *j;
		PAGE_SIZE1 = PAGE_SIZE1 | 0b00000111;
		pte[i][j-(1024*i)] =PAGE_SIZE1;   
    }
}

    // Load the page directory into CR3
    lcr3((unsigned int)ptd);

    // Access the first 256MB of virtual memory
    for (i = 0; i < 64; i++) {
        int *p = (int *)(i * PAGE_SIZE); 
        sum += *p;
    }

    halt();
    return sum;
}
