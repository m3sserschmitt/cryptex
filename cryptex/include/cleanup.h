#include <cryptography.h>

#include <unistd.h>


void daemon_cleanup(char *address);

template <typename ...Args>
void daemon_cleanup(char *address, Args... args) {
    unlink(address);
    cryptography_free(args...);
    cryptography_cleanup();
}