#include <vhex/hypervisor.h>

/* hypervisor_init() : initialize the hypervisor module */
void hypervisor_init(void)
{
	hyp_world_t casio = hypervisor_world_new("casio");
	hyp_world_t vhex = hypervisor_world_new("vhex");

	hypervisor_world_switch(casio, vhex);
}

/* hypervisor_quit() : quit the hypervisor module */
void hypervisor_quit(void)
{
	hyp_world_t casio = hypervisor_world_find("casio");
	hyp_world_t vhex  = hypervisor_world_find("vhex");

	hypervisor_world_switch(vhex, casio);

	hypervisor_world_delete(casio);
	hypervisor_world_delete(vhex);
}
