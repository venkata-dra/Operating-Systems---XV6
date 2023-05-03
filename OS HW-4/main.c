#include "console.h"

#define PAGE_SIZE 4096
#define PT_SIZE (1024 * sizeof(unsigned int))

unsigned int pt1[1024] __attribute__((__aligned__(PAGE_SIZE))); // PTD
unsigned int pt2[1024] __attribute__((__aligned__(PAGE_SIZE))); // PTE
unsigned int pt3[1024] __attribute__((__aligned__(PAGE_SIZE))); // PTE

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
    int i; 
    int sum = 0;

    // Initialize the console
    uartinit(); 

    printk("Hello from C\n");

    pt1[0] = (unsigned int)pt2 | 0b11;
    pt1[1] = (unsigned int)pt3 | 0b11;

    // Construct the page table
    for (i = 0; i < 1024; i++) {
        pt2[i] = (i * PAGE_SIZE) | 0b111; // Map virtual page to physical page with present and writable bits
        pt3[i] = (i*PAGE_SIZE + PAGE_SIZE * 1024) | 0b111;
    }

    // Load the page table into CR3
    lcr3((unsigned int)pt1);

    // Access the first 8MB of virtual memory
    for (i = 0; i < 32; i++) {
        int *p = (int *)(i * PAGE_SIZE);
        sum += *p; 
               
        printk("page\n"); 
    }
    halt(); 
    return sum; 
}