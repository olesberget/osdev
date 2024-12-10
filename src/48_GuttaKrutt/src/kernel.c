#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libc/stdbool.h"
#include "libc/system.h"
#include <multiboot2.h>
#include "descriptor_tables.h"  // Inkluderer header file for gdt filen
#include "common.h"
#include "monitor.h"
#include "interrupts.h"
#include "memory/memory.h"
#include "pit.h"


struct multiboot_info {
    uint32_t size;
    uint32_t reserved;
    struct multiboot_tag *first;
};

int kernel_main();

// End of the kernel image, defined elsewhere.
extern uint32_t end;

void isr_test_handler(registers_t* regs, void* ctx) {
    printf("Interrupt: %d\n", regs->int_no);
}

int main(uint32_t magic, struct multiboot_info* mb_info_addr) {


monitor_initialize();


//Initialiserer gdt man lagde tidligere
init_gdt();

// Initialiserer interrupt descriptor table (IDT)
init_idt();

// Initialiserer hardware interrupts
init_irq();

// Initialize the kernel's memory manager using the end address of the kernel.
init_kernel_memory(&end);

// Initialize paging for memory management.
init_paging();

// Print memory information.
print_memory_layout();

init_pit();

printf("Hello World\n"); // Printer ut Hello World

// Call cpp kernel_main (defined in kernel.cpp)
return kernel_main();
}

