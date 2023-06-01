/* Global variable for producer consumer */
extern int n; /* this is just declaration */
extern sid32 r_sem_id;
extern sid32 w_sem_id;
extern sid32 processes_sem_id;

/* Function Prototype */
void consumer(int count);
void producer(int count);
