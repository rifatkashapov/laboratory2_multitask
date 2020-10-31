/**
 * Kashapov R.R. 6122
 * BDJN
 * B – потоки одного процесса;
 * D – семафоры;
 * J – сокеты;
 * N – выборочная дисперсия трех чисел;
 **/

#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>
#include <sstream>
#include "summa.cpp"
#include <pthread.h>
#include <signal.h>

#define MAIN_SERVER_PORT 6969
#define BUFSIZE 11
int sock;
struct sockaddr_in addr;
char buf[BUFSIZE];

sem_t client_main_empty, client_main_full;
sem_t server_main_empty, server_main_full;

sem_t client_sum_empty, client_sum_full;
sem_t server_sum_empty, server_sum_full;



double result = 0.0;

struct client_struct {
    void *values;
    int port;
};

client_struct clientStruct;

void *serverThread(void *args) {
    double bytes_read, total = 0;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(MAIN_SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    double value;
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    sem_post(&server_main_empty);
    while (1) {
        int ConnectFD = accept(sock, NULL, NULL);
        bytes_read = recvfrom(sock, &value, sizeof(value), 0, NULL, NULL);
        printf("%f\n", value);
        close(ConnectFD);
        sem_post(&server_main_full);
    }

    printf("\nTotal %d bytes received", total);
    return 0;
}

void *clientMainThread(void *args) {
    int sock_out;
    struct sockaddr_in addr_out;
    // client socket
    while(1) {
        sem_wait(&client_main_empty);
        sock_out = socket(AF_INET, SOCK_DGRAM, 0);
        addr_out.sin_family = AF_INET;
        addr_out.sin_port = htons(clientStruct.port);
        addr_out.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        // отправить результат

        connect(sock_out, (struct sockaddr *)&addr_out, sizeof(addr_out));
        send(sock_out, &clientStruct.values, sizeof(clientStruct.values), 0);
        close(sock_out);
//        sem_post(&client_main_full);
    }

}

void handleError(int code) {
    if (code != 0) {
        printf("error");
    }
}

int main() {
    std::printf("main thread start\n");

    pthread_t threadSum, threadMainServer, threadMainClient;

    int status1, status2;
    int status_addr;
    int status_addr2;
    int a, b, c;

//    std::cin >> a >> b >> c;

//    std::printf("%d %d %d", a, b, c);

    SummaInitArgs summaArgs = {&server_sum_empty, &client_sum_empty, &server_main_empty, &client_main_empty};

    sem_init(&server_sum_empty,PTHREAD_PROCESS_SHARED,0); // Установлен в 0
    sem_init(&client_sum_empty,PTHREAD_PROCESS_SHARED,0); // Установлен в 0
    sem_init(&server_main_empty,PTHREAD_PROCESS_SHARED,0); // Установлен в 0
    sem_init(&client_main_empty,PTHREAD_PROCESS_SHARED,0); // Установлен в 0

    status2 = pthread_create(&threadMainServer, NULL, serverThread, NULL);
    status1 = pthread_create(&threadSum, NULL, summa, &summaArgs);
    status1 = pthread_create(&threadMainClient, NULL, clientMainThread, &summaArgs);


    SummaArgs args = {a: 10, b: 55};
    clientStruct.values = &args;
    clientStruct.port = 6970;
    sem_post(&server_sum_empty);

    sem_wait(&server_main_full);
    args = {a: 16, b: 22};
    clientStruct.values = &args;
    clientStruct.port = 6970;
    sem_post(&server_sum_empty);

    handleError(status1);
    handleError(status2);

//    while(1) {
//        sem_wait(&full);
//        printf("testtest\n");
//        sem_post(&empty);
//    }


    status2 = pthread_join(threadMainServer, (void**)&status_addr);
    status1 = pthread_join(threadSum, (void**)&status_addr);
    status1 = pthread_join(threadMainClient, (void**)&status_addr);
//    pthread_kill(threadMainServer, SIGUSR1);
//    pthread_kill(threadSum, SIGUSR1);
//    pthread_kill(threadMainClient, SIGUSR1);
////
//    handleError(status1);
//    handleError(status2);


    std::printf("main thread stop\n");
    return 0;
}
