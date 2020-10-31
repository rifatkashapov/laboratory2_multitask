#define MAIN_SERVER_PORT 6969
#define SUMMA_SERVER_PORT 6970
#define DIVISION_SERVER_PORT 6971
#define POW_SERVER_PORT 6972
#define SUB_SERVER_PORT 6973

//int sum_thread_state;
//int div_thread_state;
//int pow_thread_state;
//int sub_thread_state;

struct client_struct {
    void *values;
    int port;
};