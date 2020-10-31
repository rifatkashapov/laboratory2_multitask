/**
 * Kashapov R.R. 6122
 * 6 вариант
 * BDJN
 * B – потоки одного процесса;
 * D – семафоры;
 * J – сокеты;
 * N – выборочная дисперсия n чисел;
 **/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>
#include "summa.cpp"
#include "division.cpp"
#include "power.cpp"
#include "subtraction.cpp"

// main
sem_t client_main_empty, server_main_empty, server_main_full;

// summa
sem_t client_sum_empty, server_sum_empty, sum_state;

// division
sem_t client_div_empty, server_div_empty;

// power
sem_t client_pow_empty, server_pow_empty;

// sub
sem_t client_sub_empty, server_sub_empty;

double result = 0.0;

client_struct clientStruct;

void handleError(int code) {
    if (code != 0) {
        printf("error");
    }
}

void *serverThread(void *args) {
    int sock;
    struct sockaddr_in addr;
    double bytes_read, total = 0;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(MAIN_SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(sock, (struct sockaddr *) &addr, sizeof(addr));
    sem_post(&server_main_empty);
    while (1) {
        int ConnectFD = accept(sock, NULL, NULL);
        bytes_read = recvfrom(sock, &result, sizeof(result), 0, NULL, NULL);
        close(ConnectFD);
        sem_post(&server_main_full);
    }
}

void *clientMainThread(void *args) {
    int sock_out;
    struct sockaddr_in addr_out;
    // client socket
    while (1) {
        sem_wait(&client_main_empty);
        sock_out = socket(AF_INET, SOCK_DGRAM, 0);
        addr_out.sin_family = AF_INET;
        addr_out.sin_port = htons(clientStruct.port);
        addr_out.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        // отправить результат
        connect(sock_out, (struct sockaddr *) &addr_out, sizeof(addr_out));
        send(sock_out, &clientStruct.values, sizeof(clientStruct.values), 0);
        close(sock_out);
//        sem_post(&client_main_full);
    }
}


double executeAction(Operations operations, Variables *args) {
    clientStruct.values = args;
    switch (operations) {
        case ADD:
            clientStruct.port = SUMMA_SERVER_PORT;
            sem_post(&server_sum_empty);
            break;
        case DIV:
            clientStruct.port = DIVISION_SERVER_PORT;
            sem_post(&server_div_empty);
            break;
        case POW:
            clientStruct.port = POW_SERVER_PORT;
            sem_post(&server_pow_empty);
            break;
        case SUB:
            clientStruct.port = SUB_SERVER_PORT;
            sem_post(&server_sub_empty);
            break;
        default:
            break;
    }

    sem_wait(&server_main_full);
    return result;
}

int main() {
    std::printf("main thread start\n");

    pthread_t threadSum, threadDiv, threadPow, threadSub, threadMainServer, threadMainClient;
    int status1;
    int status_addr;

    int one = 1;
    int *sum_thread_state = &one;
    int *div_thread_state = &one;
    int *pow_thread_state = &one;
    int *sub_thread_state = &one;

    sem_init(&server_sum_empty, PTHREAD_PROCESS_SHARED, 0); // Установлен в 0
    sem_init(&client_sum_empty, PTHREAD_PROCESS_SHARED, 0); // Установлен в 0
    sem_init(&server_main_empty, PTHREAD_PROCESS_SHARED, 0); // Установлен в 0
    sem_init(&client_main_empty, PTHREAD_PROCESS_SHARED, 0); // Установлен в 0
    sem_init(&server_div_empty, PTHREAD_PROCESS_SHARED, 0); // Установлен в 0
    sem_init(&client_div_empty, PTHREAD_PROCESS_SHARED, 0); // Установлен в 0
    sem_init(&server_pow_empty, PTHREAD_PROCESS_SHARED, 0); // Установлен в 0
    sem_init(&client_pow_empty, PTHREAD_PROCESS_SHARED, 0); // Установлен в 0
    sem_init(&server_sub_empty, PTHREAD_PROCESS_SHARED, 0); // Установлен в 0
    sem_init(&client_sub_empty, PTHREAD_PROCESS_SHARED, 0); // Установлен в 0

    sem_init(&sum_state, PTHREAD_PROCESS_SHARED, 0); // Установлен в 0

    SummaInitArgs summaInitArgs = {&server_sum_empty, sum_thread_state, &server_main_empty, &client_main_empty};
    DivisionInitArgs divInitArgs = {&server_div_empty, div_thread_state, &server_main_empty, &client_main_empty};
    PowerInitArgs powInitArgs = {&server_pow_empty, pow_thread_state, &server_main_empty, &client_main_empty};
    SubInitArgs subInitArgs = {&server_sub_empty, sub_thread_state, &server_main_empty, &client_main_empty};

    int array_size = 0;
    std::printf("please input array size: ");
    std::cin >> array_size;

    std::printf("\nplease input array values: \n");
    std::vector<double> arr(array_size);

    for (int i = 0; i < array_size; i++) {
        std::cin >> arr[i];
    }

    status1 = pthread_create(&threadMainServer, NULL, serverThread, NULL);
    handleError(status1);
    status1 = pthread_create(&threadSum, NULL, summa, &summaInitArgs);
    handleError(status1);
    status1 = pthread_create(&threadDiv, NULL, division, &divInitArgs);
    handleError(status1);
    status1 = pthread_create(&threadPow, NULL, power, &powInitArgs);
    handleError(status1);
    status1 = pthread_create(&threadSub, NULL, subtraction, &subInitArgs);
    handleError(status1);
    status1 = pthread_create(&threadMainClient, NULL, clientMainThread, NULL);
    handleError(status1);

    double res = 0.0;
    Variables args;

    for (int i = 0; i < array_size; i++) {
        args = {res, arr[i]};
        res = executeAction(ADD, &args);
    }

    args = {res, (array_size) * 1.0};
    double X_vector = executeAction(DIV, &args);

    res = 0.0;
    double temp;
    for (int i = 0; i < array_size; i++) {
        args = {arr[i], X_vector};
        temp = executeAction(SUB, &args);
        args = {temp, 2};
        temp = executeAction(POW, &args);
        args = {res, temp};
        res = executeAction(ADD, &args);
    }
    args = {res, array_size * 1.0};
    res = executeAction(DIV, &args);

    *sum_thread_state = 0;
    *div_thread_state = 0;
    *pow_thread_state = 0;
    *sub_thread_state = 0;
    sem_post(&server_sum_empty);
    sem_post(&server_div_empty);
    sem_post(&server_pow_empty);
    sem_post(&server_sub_empty);

    status1 = pthread_join(threadSum, (void **) &status_addr);
    handleError(status1);
    status1 = pthread_join(threadDiv, (void **) &status_addr);
    handleError(status1);
    status1 = pthread_join(threadPow, (void **) &status_addr);
    handleError(status1);
    status1 = pthread_join(threadSub, (void **) &status_addr);
    handleError(status1);
    printf("Результат фукнции выборочной дисперсии %f \n", res);
    std::printf("main thread stop\n");
    return 0;
}
