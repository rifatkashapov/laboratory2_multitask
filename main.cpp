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
#include <sstream>
#include <pthread.h>
#include <signal.h>
#include "summa.cpp"
#include "division.cpp"
#include "power.cpp"
#include "subtraction.cpp"

// main
sem_t client_main_empty, client_main_full;
sem_t server_main_empty, server_main_full;

// summa
sem_t client_sum_empty, client_sum_full;
sem_t server_sum_empty, server_sum_full;

// division
sem_t client_div_empty, client_div_full;
sem_t server_div_empty, server_div_full;

// power
sem_t client_pow_empty, client_pow_full;
sem_t server_pow_empty, server_pow_full;

// sub
sem_t client_sub_empty, client_sub_full;
sem_t server_sub_empty, server_sub_full;

double result = 0.0;



client_struct clientStruct;

void *serverThread(void *args) {
    int sock;
    struct sockaddr_in addr;
    double bytes_read, total = 0;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(MAIN_SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    sem_post(&server_main_empty);
    while (1) {
        int ConnectFD = accept(sock, NULL, NULL);
        bytes_read = recvfrom(sock, &result, sizeof(result), 0, NULL, NULL);
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

    pthread_t threadSum, threadDiv, threadPow, threadSub, threadMainServer, threadMainClient;

    int status1;
    int status_addr;
    int status_addr2;
    int a, b, c;

//    std::cin >> a >> b >> c;

//    std::printf("%d %d %d", a, b, c);

    SummaInitArgs summaInitArgs = {&server_sum_empty, &client_sum_empty, &server_main_empty, &client_main_empty};
    DivisionInitArgs divInitArgs = {&server_div_empty, &client_div_empty, &server_main_empty, &client_main_empty};
    PowerInitArgs powInitArgs = {&server_pow_empty, &client_pow_empty, &server_main_empty, &client_main_empty};
    SubInitArgs subInitArgs = {&server_sub_empty, &client_sub_empty, &server_main_empty, &client_main_empty};

    sem_init(&server_sum_empty,PTHREAD_PROCESS_SHARED,0); // Установлен в 0
    sem_init(&client_sum_empty,PTHREAD_PROCESS_SHARED,0); // Установлен в 0
    sem_init(&server_main_empty,PTHREAD_PROCESS_SHARED,0); // Установлен в 0
    sem_init(&client_main_empty,PTHREAD_PROCESS_SHARED,0); // Установлен в 0
    sem_init(&server_div_empty,PTHREAD_PROCESS_SHARED,0); // Установлен в 0
    sem_init(&client_div_empty,PTHREAD_PROCESS_SHARED,0); // Установлен в 0
    sem_init(&server_pow_empty,PTHREAD_PROCESS_SHARED,0); // Установлен в 0
    sem_init(&client_pow_empty,PTHREAD_PROCESS_SHARED,0); // Установлен в 0
    sem_init(&server_sub_empty,PTHREAD_PROCESS_SHARED,0); // Установлен в 0
    sem_init(&client_sub_empty,PTHREAD_PROCESS_SHARED,0); // Установлен в 0

    status1 = pthread_create(&threadMainServer, NULL, serverThread, NULL); handleError(status1);
    status1 = pthread_create(&threadSum, NULL, summa, &summaInitArgs); handleError(status1);
    status1 = pthread_create(&threadDiv, NULL, division, &divInitArgs); handleError(status1);
    status1 = pthread_create(&threadPow, NULL, power, &powInitArgs); handleError(status1);
    status1 = pthread_create(&threadSub, NULL, subtraction, &subInitArgs); handleError(status1);
    status1 = pthread_create(&threadMainClient, NULL, clientMainThread, NULL); handleError(status1);


    SummaArgs sumArgs = {a: 10, b: 55};
    clientStruct.values = &sumArgs;
    clientStruct.port = SUMMA_SERVER_PORT;
    sem_post(&server_sum_empty);

    sem_wait(&server_main_full);
    printf("%f\n", result);

    sumArgs = {a: 16, b: 22};
    clientStruct.values = &sumArgs;
    clientStruct.port = SUMMA_SERVER_PORT;
    sem_post(&server_sum_empty);

    sem_wait(&server_main_full);
    printf("%f\n", result);

    ///

    DivisionArgs divArgs = {a: 25, b: 5};
    clientStruct.values = &divArgs;
    clientStruct.port = DIVISION_SERVER_PORT;
    sem_post(&server_div_empty);

    sem_wait(&server_main_full);
    printf("%f\n", result);

    ///

    PowerArgs powArgs = {a: 2, b: 8};
    clientStruct.values = &powArgs;
    clientStruct.port = POW_SERVER_PORT;
    sem_post(&server_pow_empty);

    sem_wait(&server_main_full);
    printf("%f\n", result);

    ///

    SubArgs subArgs = {a: 36, b: 42};
    clientStruct.values = &subArgs;
    clientStruct.port = SUB_SERVER_PORT;
    sem_post(&server_sub_empty);

    sem_wait(&server_main_full);
    printf("%f\n", result);

    ///

    status1 = pthread_join(threadMainServer, (void**)&status_addr);
    handleError(status1);
    status1 = pthread_join(threadSum, (void**)&status_addr);
    handleError(status1);
    status1 = pthread_join(threadMainClient, (void**)&status_addr);
    handleError(status1);

//    pthread_kill(threadMainServer, SIGUSR1);
//    pthread_kill(threadSum, SIGUSR1);
//    pthread_kill(threadMainClient, SIGUSR1);
////
//    handleError(status1);
//    handleError(status2);


    std::printf("main thread stop\n");
    return 0;
}
