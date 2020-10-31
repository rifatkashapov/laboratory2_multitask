#include <string>
#include <semaphore.h>
#include <cstring>

struct DivisionInitArgs {
    sem_t *server_div_empty;
    sem_t *client_div_empty;
    sem_t *server_main_empty;
    sem_t *client_main_empty;
};

struct DivisionArgs {
    double a;
    double b;
};

void *division(void *args) {
    std::printf("division thread start\n");

    int sock_out, sock_in;
    struct sockaddr_in addr_out, addr_in;

    DivisionInitArgs arguments = * (DivisionInitArgs * ) args;
    DivisionArgs *value;
    //server socket
    while (1) {
        sem_wait(arguments.server_div_empty);
        sock_in = socket(AF_INET, SOCK_DGRAM, 0);
        addr_in.sin_family = AF_INET;
        addr_in.sin_port = htons(DIVISION_SERVER_PORT);
        addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
        bind(sock_in, (struct sockaddr *)&addr_in, sizeof(addr_in));
        sem_post(arguments.client_main_empty);
        int ConnectFD = accept(sock_in, NULL, NULL);
        recvfrom(sock_in, reinterpret_cast<void *> (&value), sizeof(value), 0, NULL, NULL);
        DivisionArgs args = * (DivisionArgs *) value;
//        printf("%f %f\n", args.a, args.b);
        close(ConnectFD);
        close(sock_in);
        // client socket

        double result = args.a / args.b;

//        sem_post(arguments.client_sum_empty);
        sem_wait(arguments.server_main_empty);
        sock_out = socket(AF_INET, SOCK_DGRAM, 0);
        addr_out.sin_family = AF_INET;
        addr_out.sin_port = htons(MAIN_SERVER_PORT);
        addr_out.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        // sem_wait(arguments.semaphoreEmpty);
        // отправить результат

        connect(sock_out, (struct sockaddr *)&addr_out, sizeof(addr_out));
        send(sock_out, &result, sizeof(result), 0);
        close(sock_out);
        sem_post(arguments.server_main_empty);
    }


    // sem_post(arguments.semaphoreFull);

    std::printf("division thread end\n");
    return 0;
}