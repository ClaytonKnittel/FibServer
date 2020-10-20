#pragma once

#include <cilk/cilk_priority.h>



typedef cilk::Low ClientP;


CilkPrioCommandPrototype(void, handle_client_conn, struct server *, int);
CilkPrioCommandForwardDeclareAtPrio(handle_client_conn, ClientP);


