#include <string>
#include <semaphore.h>
#include <cstring>

struct PowerInitArgs {
    sem_t *server_pow_empty;
    sem_t *client_pow_empty;
    sem_t *server_main_empty;
    sem_t *client_main_empty;
};

struct PowerArgs {
    double a;
    double b;
};

void *power(void *args) {
    std::printf("pow thread start\n");

    int sock_out, sock_in, state;
    struct sockaddr_in addr_out, addr_in;
    int *threadState;
    PowerInitArgs arguments = * (PowerInitArgs * ) args;
    PowerArgs *value;

    while (1) {
        sem_wait(arguments.server_pow_empty);
        sock_in = socket(AF_INET, SOCK_DGRAM, 0);
        addr_in.sin_family = AF_INET;
        addr_in.sin_port = htons(POW_SERVER_PORT);
        addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
        bind(sock_in, (struct sockaddr *)&addr_in, sizeof(addr_in));
        sem_post(arguments.client_main_empty);
        int ConnectFD = accept(sock_in, NULL, NULL);
        recvfrom(sock_in, reinterpret_cast<void *> (&value), sizeof(value), 0, NULL, NULL);
        PowerArgs args = * (PowerArgs *) value;
//        printf("%f %f\n", args.a, args.b);
        close(ConnectFD);
        close(sock_in);
        // client socket
        double result = 1;
        for (int i=0; i < args.b; i++) {
            result = result * args.a;
        }

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

    std::printf("pow thread end\n");
    return 0;
}