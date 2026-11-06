#include "src/backend/store.h"
#include "src/ui/ui.h"

int main(void)
{
    store_t *store = ioopm_store_create();

    event_loop(store);
    
   return ioopm_store_quit(store);


}