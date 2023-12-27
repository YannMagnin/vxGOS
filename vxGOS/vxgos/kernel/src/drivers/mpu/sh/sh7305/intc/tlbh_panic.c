
extern void sh7305_intc_exch_panic(void);

void sh7305_intc_tlbh_panic(void)
{
    sh7305_intc_exch_panic();
    while (1) { __asm__ volatile ("sleep"); }
}
