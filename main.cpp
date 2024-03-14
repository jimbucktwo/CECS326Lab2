#include <iostream>
#include <pthread.h>
#include <unistd.h>
using namespace std;

// enumerates states of the philosphers and num of philosophers
#define N 5
#define thinking 0
#define hungry 1
#define eating 2

//array of philosophers and how long to execute program
int philosophers[N];
int times = 10;

class monitor {
    //declare variables
    int state[N];
    pthread_mutex_t lock;
    pthread_cond_t cond[N];

    public:

        //constructor
        monitor() {
            for (int i = 0; i < N; i++) {
                state[i] = thinking;
                pthread_cond_init(&cond[i], NULL);
            }

            pthread_mutex_init(&lock, NULL);
        }

        // tests if left and right fork are available, if so then invoke a signal
        void test(int i) {
            if ((state[(i + 1) % 5] != eating) && (state[(i + 4) % 5] != eating) && (state[i] == hungry)) {
                state[i] = eating;

                pthread_cond_signal(&cond[i]);
            }
        }
        
        // picks up forks if available, otherwise wait
        void pickup_fork(int i) {
            pthread_mutex_lock(&lock);
            state[i] = hungry;
            test(i);
            if (state[i] != eating) {
                pthread_cond_wait(&cond[i], &lock);
            }

            cout << "Forks are with Philosopher #" << i << endl;
            cout << "Philosopher #" << i << " is eating" << endl;
            pthread_mutex_unlock(&lock);
        }

        //returns forks to pool and returns to thinking state. Tests adjacent philosophers
        void putdown_fork(int i) {
            pthread_mutex_lock(&lock);
            
            state[i] = thinking;
            cout << "Philosopher #" << i+1 << " is thinking" << endl;
            test((i + 4) % N);
            test((i + 1) % N);

            pthread_mutex_unlock(&lock);
        }

        
};

// creates global object of monitor
monitor phil;

// sequence of actions taken by each philosopher
void* philosopher(void* arg) {
    int c = 0;
    while (c < times) {
        int i = *(int*)arg;
        sleep(1);
        phil.pickup_fork(i);
        sleep(1);
        phil.putdown_fork(i);
        c++;
    }
}

int main() {
    //declare the num of threads for each philosopher
    pthread_t threadid[N];
    
    //assign each philosopher a number 0..4
    for (int i = 0; i < N; i++) {
        philosophers[i] = i;
    }

    //creates the threads for each philosopher
    for (int i = 0; i < N; i++) {
        pthread_create(&threadid[i], NULL, philosopher, &philosophers[i]);
        cout << "Philosopher #" << i+1 << " is thinking" << endl;

    }

    //joins the thread back to the main thread, signifies end of a thread process
    for (int i = 0; i < N; i++) {
        pthread_join(threadid[i], NULL);
    }

    return 0;
}
