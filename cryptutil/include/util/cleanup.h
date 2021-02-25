#include <crypto/cryptography.h>
#include <crypto/mem.h>

#include <unistd.h>


void daemon_cleanup(char *address);

template <typename ...Args>
void daemon_cleanup(char *address, Args... args) {
    unlink(address);
    free_memory(args...);
    cryptography_cleanup();
}