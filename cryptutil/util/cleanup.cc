#include "util/cleanup.h"


void daemon_cleanup(char *address) {
    unlink(address);
    cryptography_cleanup();
}