/*
#include "core/daemon.h"
#include "core/encryption_context.h"

#include "util/cmd.h"
#include "core/cryptex.h"
#include "util/util.h"
#include "util/log.h"
#include "util/cleanup.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

static int create_server(const char *address)
{
    struct sockaddr_un my_addr;

    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (sfd < 0)
    {
        return sfd;
    }

    int enable = 1;
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        return -1;
    }

    memset(&my_addr, 0, sizeof(struct sockaddr_un));

    my_addr.sun_family = AF_UNIX;
    strncpy(my_addr.sun_path, address, sizeof(my_addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_un)) < 0)
    {
        return -1;
    }

    if (listen(sfd, 1) < 0)
    {
        return -1;
    }

    return sfd;
}

static int start_server(PLAINTEXT *argv, PLAINTEXT *argv_end, PLAINTEXT *address)
{
    *address = (char *)malloc(PATH_MAX);

    if (not *address)
    {
        log(argv, argv_end, "[-] ERROR: start_server: memory allocation error: malloc.");
        return -1;
    }

    if (not cmd_option_exists(argv, argv_end, "-address"))
    {
        free_memory(*address);
        log(argv, argv_end, "[-] ERROR: start_server: no address available.");

        return -1;
    }

    strcpy(*address, get_cmd_option(argv, argv_end, "-address"));

    
    // char *rm_cmd = (char *)malloc(128);

    // if (not rm_cmd)
    // {
    //     log(argv, argv_end, "[-] ERROR: service_daemon: cannot remove address file (memory allocation failed).");
    //     return -1;
    // }
    // else
    // {
    //     strcat(rm_cmd, "rm ");
    //     strcat(rm_cmd, *address);

    //     system(rm_cmd);
    //     free_memory(rm_cmd);
    // }


    int sfd = create_server(*address);

    if (sfd < 0)
    {
        free_memory(*address);
        log(argv, argv_end, "[-] ERROR: service_daemon: Cannot start server.");

        return -1;
    }

    return sfd;
}

static bool read_password(PLAINTEXT *argv, PLAINTEXT *argv_end, int sock, ENCRYPTION_CONTEXT *ctx)
{
    BYTES key = (BYTES)malloc(1024);

    if (not *key)
    {
        log(argv, argv_end, "[-] ERROR: service_daemon: memory allocation error: malloc.");

        return false;
    }

    memset(key, 0, 1024);
    int keylen;

    if ((keylen = read(sock, key, 1024)) < 0)
    {

        log(argv, argv_end, "[-] ERROR: service_daemon: socket read failed.");

        return false;
    }

    ctx->aes_encr = AES_create_encrypt_ctx(key, keylen, nullptr, 15);
    ctx->aes_decr = AES_create_decrypt_ctx(key, keylen, nullptr, 15);

    return true;
}

int service_daemon(PLAINTEXT *argv, PLAINTEXT *argv_end)
{
    char *log_file = get_cmd_option(argv, argv_end, "-log");

    if (not log_file)
    {
        log(argv, argv_end, "[-] ERROR: No log file.");
        return EXIT_FAILURE;
    }

    if (daemon(1, 0) < 0)
    {
        log(argv, argv_end, "[-] ERROR: service_daemon: error starting daemon: daemon.");
        return EXIT_FAILURE;
    }

    log(argv, argv_end);
    log(argv, argv_end, "===============================");
    log(argv, argv_end, "===== [+] Daemon started. =====");
    log(argv, argv_end, "===============================", '\n');

    PLAINTEXT address;
    int sfd = start_server(argv, argv_end, &address);

    if (sfd < 0 or not address)
    {
        log(argv, argv_end, "[-] ERROR: service daemon: error starting server.");
        return EXIT_FAILURE;
    }

    sockaddr client_addr;
    socklen_t sock_len = sizeof(client_addr);
    int client = accept(sfd, &client_addr, &sock_len);

    if (client < 0)
    {
        log(argv, argv_end, "[-] ERROR: service_daemon: error accepting client.");
        return EXIT_FAILURE;
    }

    cryptography_init();

    log(argv, argv_end, "[+] New client connected.");

    ENCRYPTION_CONTEXT *ctx = new ENCRYPTION_CONTEXT;
    if (not read_password(argv, argv_end, client, ctx))
    {
        log(argv, argv_end, "[+] ERROR: service_daemon: client set up failed.");

        return EXIT_FAILURE;
    }

    PLAINTEXT command;
    allocate_memory(&command, 1024);

    if (not command)
    {
        log(argv, argv_end, "[-] ERROR: service_daemon: memory allocation error: malloc.");
        daemon_cleanup(address);

        return EXIT_FAILURE;
    }

    do
    {
        memset(command, 0, 1024);
        ssize_t bytes_read = read(client, command, 1024);

        if (bytes_read <= 0)
        {
            log(argv, argv_end, "[-] ERROR: service_daemon: socket read error.");
            daemon_cleanup(address, command);

            return EXIT_FAILURE;
        }

        int argc;
        parse_command(argv, argc, command, "-log", log_file, "-daemon");
        argv_end = argv + argc;

        size_t inlen;
        BYTES in = read_file(argv, argv_end, inlen);

        BYTES out;
        size_t outlen;

        int result = cryptography(in, inlen, ctx, &out, outlen, argv, argv_end);

        if (not result)
        {
            log(argv, argv_end, "[-] WARNING: service_daemon: no action passed or no data to process.");
        }
        else if (result < 0)
        {
            log(argv, argv_end, "[-] ERROR: service_daemon: encrypt / decrypt error code: ", result, ".");
        }
        else if (write(client, out, outlen) < 0)
        {
            log(argv, argv_end, "[-] ERROR: service_daemon: write socket error.");
            daemon_cleanup(address, command, in, out);

            return EXIT_FAILURE;
        }

        free_memory(in, out);
    } while (not strstr(command, "exit"));

    close(client);
    log(argv, argv_end, "[+] Client sent \"exit\" command.");
    daemon_cleanup(address, command);

    return EXIT_SUCCESS;
}
*/