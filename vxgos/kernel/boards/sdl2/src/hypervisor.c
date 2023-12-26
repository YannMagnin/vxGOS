#include <vhex/hypervisor.h>

/* hypervisor_init() : initialize the hypervisor module */
void hypervisor_init(void)
{
	hyp_world_t fake = hypervisor_world_new("fake");
	hyp_world_t vhex = hypervisor_world_new("vhex");

	hypervisor_world_switch(fake, vhex);
}

/* hypervisor_quit() : quit the hypervisor module */
void hypervisor_quit(void)
{
	hyp_world_t fake = hypervisor_world_find("fake");
	hyp_world_t vhex  = hypervisor_world_find("vhex");

	hypervisor_world_switch(vhex, fake);

	hypervisor_world_delete(fake);
	hypervisor_world_delete(vhex);
}
