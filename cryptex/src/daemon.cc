#include "../include/daemon.h"
#include "../include/cmd.h"
#include "../include/cryptex.h"
#include "../include/util.h"
#include "../include/log.h"
#include "../include/cleanup.h"

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

static int start_server(char **argv, char **argv_end, char **address)
{
    *address = (char *)malloc(PATH_MAX);

    if (not *address)
    {
        log(argv, argv_end, "[-] ERROR: start_server: memory allocation error: malloc.");
        return -1;
    }

    if (not cmd_option_exists(argv, argv_end, "-address"))
    {
        cryptography_free(*address);
        log(argv, argv_end, "[-] ERROR: start_server: no address available.");

        return -1;
    }

    strcpy(*address, get_cmd_option(argv, argv_end, "-address"));

    /*char *rm_cmd = (char *)malloc(128);

    if (not rm_cmd)
    {
        log(argv, argv_end, "[-] ERROR: service_daemon: cannot remove address file (memory allocation failed).");
        return -1;
    }
    else
    {
        strcat(rm_cmd, "rm ");
        strcat(rm_cmd, *address);

        system(rm_cmd);
        free_memory(rm_cmd);
    }
*/

    int sfd = create_server(*address);

    if (sfd < 0)
    {
        cryptography_free(*address);
        log(argv, argv_end, "[-] ERROR: service_daemon: Cannot start server.");

        return -1;
    }

    return sfd;
}

static bool read_password(char **argv, char **argv_end, int sock, char **password)
{
    *password = (char *)malloc(1024);

    if (not *password)
    {
        log(argv, argv_end, "[-] ERROR: service_daemon: memory allocation error: malloc.");

        return false;
    }

    memset(*password, 0, 1024);

    if (read(sock, *password, 1024) < 0)
    {

        log(argv, argv_end, "[-] ERROR: service_daemon: socket read failed.");

        return false;
    }

    return true;
}

int service_daemon(char **argv, char **argv_end)
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

    char *address;
    int sfd = start_server(argv, argv_end, &address);

    if (sfd < 0 or not address)
    {
        return EXIT_FAILURE;
    }

    sockaddr client_addr;
    socklen_t sock_len = sizeof(client_addr);
    int client;

    while ((client = accept(sfd, &client_addr, &sock_len)) > 0)
    {
        cryptography_init();

        log(argv, argv_end, "[+] New client connected.");

        char *password;
        if (not read_password(argv, argv_end, client, &password))
        {
            log(argv, argv_end, "[+] ERROR: service_daemon: client set up failed.");

            return EXIT_FAILURE;
        }

        char *command = (char *)malloc(1024);

        if (not command)
        {
            log(argv, argv_end, "[-] ERROR: service_daemon: memory allocation error: malloc.");
            daemon_cleanup(address, password);

            return EXIT_FAILURE;
        }

        do
        {
            memset(command, 0, 1024);
            ssize_t bytes_read = read(client, command, 1024);

            if (bytes_read <= 0)
            {
                log(argv, argv_end, "[-] ERROR: service_daemon: socket read error.");
                daemon_cleanup(address, command, password);

                return EXIT_FAILURE;
            }

            int argc;
            parse_command(argv, argc, command, "-log", log_file, "-daemon");
            argv_end = argv + argc;

            size_t inlen;
            unsigned char *in = read_file(argv, argv_end, inlen);
            unsigned char *out;
            size_t outlen;

            int result = cryptography(in, inlen, (PLAINTEXT)password, &out, outlen, argv, argv_end);

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
                daemon_cleanup(address, command, password, in, out);

                return EXIT_FAILURE;
            }

            // free_memory(in, out);
        } while (not strstr(command, "exit"));

        close(client);
        log(argv, argv_end, "[+] Client sent \"exit\" command.");
        daemon_cleanup(address, command, password);

        return EXIT_SUCCESS;
    }

    log(argv, argv_end, "[+] ERROR: service_daemon: daemon stopped due to critical errors.");
    daemon_cleanup(address);

    return EXIT_FAILURE;
}