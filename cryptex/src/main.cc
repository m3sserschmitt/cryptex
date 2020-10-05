#include "../include/cryptex.h"
#include "../include/util.h"
#include "../include/help.h"

#include <string.h>


using namespace std;

int main(int argc, char **argv)
{
    PLAINTEXT *argv_end = argv + argc;

    if (cmd_option_exists(argv, argv_end, "-help"))
    {
        printf("%s\n", help);
        return EXIT_SUCCESS;
    }

    if (cmd_option_exists(argv, argv_end, "-daemon"))
    {
        return service_daemon(argv, argv_end);
    }

    cryptography_init();

    SIZE size;
    BYTES in = read_file(argv, argv_end, size);

    printf("[+] Input %zi bytes.\n", size);

    if (not in)
    {
        return EXIT_FAILURE;
    }

    SIZE passwdlen;
    argv_end = argv + argc;
    PLAINTEXT password = read_password(argv, argv_end, passwdlen);

    BYTES out;
    SIZE outlen;

    add_command(argv, argc, "-daemon");
    argv_end = argv + argc;
    
    int result = cryptography(in, size, password, &out, outlen, argv, argv_end);

    printf("[+] Result: %i ", result);

    if (result < 0)
    {
        cryptography_free(in, out, password);
        printf("(FAILURE)\n");

        return EXIT_FAILURE;
    }

    printf("(SUCCESS)\n[+] Out: %zi bytes.\n", outlen);

    if (not write_file(out, outlen, argv, argv_end))
    {
        cryptography_free(in, out, password);
        return EXIT_FAILURE;
    }

    cryptography_free(in, out, password);
    cryptography_cleanup();

    return EXIT_SUCCESS;
}
