#ifndef DESCRIPTOR_TABLES_H
#define DESCRIPTOR_TABLES_H

#include "libc/system.h"



// Number of GDT entries
#define GDT_ENTRIES 5

// Number of IDT entries
#define IDT_ENTRIES 256

// GDT entry structure
struct gdt_entry_t {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

// Structure of an IDT entry
struct idt_entry_t {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

// GDT and IDT pointer structures
struct gdt_ptr_t {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_ptr_t {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// GDT functions
void init_gdt();
void gdt_load();
void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

// IDT functions
void init_idt();
void idt_load();

// External declarations for the assembly functions
// extern void gdt_flush(uint32_t gdt_ptr);
// extern void idt_flush(uint32_t idt_ptr);

static struct gdt_entry_t gdt[GDT_ENTRIES];
static struct gdt_ptr_t gdt_ptr;
static struct idt_entry_t idt[IDT_ENTRIES];
static struct idt_ptr_t idt_ptr;

#endif // DESCRIPTOR_TABLES_H



