#include "pic.h"

#include "../drivers/ports.h"

#define PIC1 0x20  // master PIC
#define PIC2 0xA0  // slave PIC
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

#define ICW1_ICW4 0x01
#define ICW1_SINGLE 0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL 0x08
#define ICW1_INIT 0x10

#define ICW4_8086 0x01
#define ICW4_AUTO 0x02
#define ICW4_BUF_SLAVE 0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM 0x10

static inline void io_wait(void) { __asm__ __volatile__("outb %%al, $0x80" : : "a"(0)); }

static void PIC_remap(int master_off, int slave_off) {
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, master_off);  // ICW2: Master PIC
	io_wait();
	outb(PIC2_DATA, slave_off);  // ICW2: Slave PIC
	io_wait();
	outb(PIC1_DATA, 4);  // ICW3: tell Master PIC that there is a slave PIC at IRQ2
	io_wait();
	outb(PIC2_DATA, 2);  // ICW3: tell Slave PIC its cascade identity
	io_wait();

	outb(PIC1_DATA, ICW4_8086);  // ICW4: have the PICs use 8086 mode
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	// Unmask both PICs
	outb(PIC1_DATA, 0);
	outb(PIC2_DATA, 0);
}

void init_PIC(void) {
	__asm__ __volatile__("cli");  // disable interrupts
	PIC_remap(0x20, 0x28);
	__asm__ __volatile__("sti");  // enable interrupts
}
