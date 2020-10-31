#include <string>
#include <semaphore.h>
#include <cstring>

struct SubInitArgs {
    sem_t *server_sub_empty;
    int *thread_state;
    sem_t *server_main_empty;
    sem_t *client_main_empty;
};


void *subtraction(void *args) {
    std::printf("subtraction thread start\n");

    int sock_out, sock_in, state;
    struct sockaddr_in addr_out, addr_in;
    int *threadState;
    SubInitArgs arguments = * (SubInitArgs * ) args;
    Variables *value;

    while (1) {
        sem_wait(arguments.server_sub_empty);
        if (*arguments.thread_state == 0) break;
        sock_in = socket(AF_INET, SOCK_DGRAM, 0);
        addr_in.sin_family = AF_INET;
        addr_in.sin_port = htons(SUB_SERVER_PORT);
        addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
        bind(sock_in, (struct sockaddr *)&addr_in, sizeof(addr_in));
        sem_post(arguments.client_main_empty);
        int ConnectFD = accept(sock_in, NULL, NULL);
        printf("server 'subtraction' receive data\n");
        recvfrom(sock_in, reinterpret_cast<void *> (&value), sizeof(value), 0, NULL, NULL);
        Variables args = * (Variables *) value;
//        printf("%f %f\n", args.a, args.b);
        close(ConnectFD);
        close(sock_in);
        // client socket
        double result = args.a - args.b;

//        sem_post(arguments.client_sum_empty);
        sem_wait(arguments.server_main_empty);
        sock_out = socket(AF_INET, SOCK_DGRAM, 0);
        addr_out.sin_family = AF_INET;
        addr_out.sin_port = htons(MAIN_SERVER_PORT);
        addr_out.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        // sem_wait(arguments.semaphoreEmpty);
        // отправить результат
        printf("client 'subtraction' send data %f \n", result);
        connect(sock_out, (struct sockaddr *)&addr_out, sizeof(addr_out));
        send(sock_out, &result, sizeof(result), 0);
        close(sock_out);
        sem_post(arguments.server_main_empty);
    }


    // sem_post(arguments.semaphoreFull);

    std::printf("subtraction thread end\n");
    return 0;
}