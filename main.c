
int main(void)
{
    store_t *store = store_create();

    event_loop(store);

    return store_quit();
}