#include <vhex/driver/cpu.h>

volatile int cpu_sleep_block_counter = 0;

void cpu_sleep(void)
{
    if(cpu_sleep_block_counter <= 0) __asm__("sleep");
}

void cpu_sleep_block(void)
{
    cpu_atomic_start();
    cpu_sleep_block_counter++;
    cpu_atomic_end();
}

void cpu_sleep_unblock(void)
{
    cpu_atomic_start();
    cpu_sleep_block_counter--;
    cpu_atomic_end();
}
