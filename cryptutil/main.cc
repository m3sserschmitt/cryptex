#include "core/cryptex.h"
#include "util/util.h"
#include "util/help.h"

#include <crypto/mem.h>
#include <iostream>
#include <unistd.h>

#include <string.h>

using namespace std;

static bool silent = false;

int main(int argc, char **argv)
{
    PLAINTEXT *argv_end = argv + argc;

    if(cmd_option_exists(argv, argv_end, "-version"))
    {
        cout << "cryptutil version 0.0.2-beta\n";
        return EXIT_SUCCESS;
    }

    if (cmd_option_exists(argv, argv_end, "-help"))
    {
        cout << help << "\n";

        return EXIT_SUCCESS;
    }

    BYTES in;
    SIZE inlen;
    ENCRYPTION_CONTEXT *ctx = new ENCRYPTION_CONTEXT;

    silent = cmd_option_exists(argv, argv_end, "-silent");

    if (not cmd_option_exists(argv, argv_end, "-in"))
    {
        in = read_stdin(argv, argv_end, inlen, ctx);

        if (not in)
        {
            silent or cout << "[-] Cannot read stdin.\n";

            return EXIT_FAILURE;
        }
    }
    else
    {
        in = read_file(argv, argv_end, inlen);

        if (not in)
        {
            silent or cout << "[-] Error: cannot read file.\n";

            return EXIT_FAILURE;
        }

        if (not read_password(argv, argv_end, ctx))
        {
            silent or cout << "[-] ERROR: Cannot read password / key.\n";

            return EXIT_FAILURE;
        }
    }

    BYTES out;
    SIZE outlen;

    cryptography_init();
    int result = cryptography(in, inlen, ctx, &out, outlen, argv, argv_end);
    cryptography_cleanup();

    silent or cout << "[+] Input: " << inlen << " bytes.\n"
                   << "[+] Result: " << result;

    if (result < 0)
    {
        silent or cout << " (FAILURE).\n";

        return EXIT_FAILURE;
    }

    silent or cout << " (SUCCESS).\n"
                   << "[+] Out: " << outlen << " bytes.\n";

    if (not cmd_option_exists(argv, argv_end, "-out"))
    {
        write(1, out, outlen);
    }
    else if (not write_file(out, outlen, argv, argv_end))
    {
        silent or cout << "[-] Error: cannot write result into file.\n";

        return EXIT_FAILURE;
    }

    silent or cout << "[+] Done.\n";

    return EXIT_SUCCESS;
}
