extern "C" {
    #include "libc/system.h"
    #include "memory/memory.h"
    #include "common.h"
    #include "interrupts.h"
    #include "input.h"
    #include "song/song.h"
}

#include <cstddef>  // for std::size_t
#include <cstdlib>  // for std::malloc and std::free

// Existing global operator new overloads
void* operator new(std::size_t size) {
    return std::malloc(size);
}

void* operator new[](std::size_t size) {
    return std::malloc(size);
}

// Existing global operator delete overloads
void operator delete(void* ptr) noexcept {
    std::free(ptr);
}

void operator delete[](void* ptr) noexcept {
    std::free(ptr);
}

// Add sized-deallocation functions
void operator delete(void* ptr, std::size_t size) noexcept {
    (void)size; // Size parameter is unused, added to match required signature
    std::free(ptr);
}

void operator delete[](void* ptr, std::size_t size) noexcept {
    (void)size; // Size parameter is unused, added to match required signature
    std::free(ptr);
}

SongPlayer* create_song_player() {
    auto* player = new SongPlayer();
    player->play_song = play_song_impl;
    return player;
}

extern "C" int kernel_main(void);

int kernel_main() {
    // Set up interrupt handlers
    register_interrupt_handler(3, [](registers_t* regs, void* context) {
        printf("Interrupt 3 - OK\n");
    }, NULL);

    register_interrupt_handler(4, [](registers_t* regs, void* context) {
        printf("Interrupt 4 - OK\n");
    }, NULL);

    register_interrupt_handler(14, [](registers_t* regs, void* context) {
        uint32_t faulting_address;
        __asm__ __volatile__("mov %%cr2, %0" : "=r" (faulting_address));

        int32_t present = !(regs->err_code & 0x1); // Page not present
        int32_t rw = regs->err_code & 0x2;           // Write operation?
        int32_t us = regs->err_code & 0x4;           // Processor was in user-mode?
        int32_t reserved = regs->err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
        int32_t id = regs->err_code & 0x10;          // Caused by an instruction fetch?

        printf("Page fault! (");
        if (present)
            printf("present ");
        if (rw)
            printf("read-only ");
        if (us)
            printf("user-mode ");
        if (reserved)
            printf("reserved ");
        printf(")\n\n");
    }, NULL);

    // Trigger interrupts to test the handlers
    asm volatile("int $0x3");
    asm volatile("int $0x4");

    // Enable interrupts
    asm volatile("sti");

    // Register IRQ handler for keyboard (IRQ1)
    register_irq_handler(IRQ1, [](registers_t*, void*) {
        // Read from the keyboard
        unsigned char scancode = inb(0x60);
        char f = scancode_to_ascii(&scancode);

        // Print the character
        printf("%c", f);

        // Acknowledge the IRQ
        asm volatile("cli");
    }, NULL);

    Song* songs[] = {
        /*
        new Song({battlefield_1942_theme, sizeof(battlefield_1942_theme) / sizeof(Note)}),
        new Song({starwars_theme, sizeof(starwars_theme) / sizeof(Note)}),
        new Song({music_1, sizeof(music_1) / sizeof(Note)}),
        new Song({music_6, sizeof(music_6) / sizeof(Note)}),
        new Song({music_5, sizeof(music_5) / sizeof(Note)}),
        new Song({music_4, sizeof(music_4) / sizeof(Note)}),
        new Song({music_3, sizeof(music_3) / sizeof(Note)}),
        new Song({music_2, sizeof(music_2) / sizeof(Note)})
        */
        new Song({bbl, sizeof(bbl) / sizeof(Note)}),
    };
    uint32_t n_songs = sizeof(songs) / sizeof(Song*);

    // Create a song player and play each song
    SongPlayer* player = create_song_player();
    for (uint32_t i = 0; i < n_songs; i++) {
        printf("Playing Song...\n");
        player->play_song(songs[i]);
        printf("Finished playing the song.\n");
    }

    // Main loop
    printf("Kernel main loop\n");
    while (true) {
        // Kernel main tasks
    }

    // This part of the code will never be reached
    printf("Done!!!\n");
    return 0;
}