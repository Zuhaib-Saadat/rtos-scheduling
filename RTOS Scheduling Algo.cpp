#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

#define MAX_TEXT_SIZE 128

enum TaskType {
    PING,
    PRODUCER,
    CONSUMER
};

struct Task {
    int id;
    int arrival_time; // in milliseconds
    int execution_time; // in milliseconds
    int period; // in milliseconds (used as the deadline in RMS)
    TaskType type; // Type of task
    string ping_command; // Ping command for the task (if type is PING)
    string message; // Message to send (if type is PRODUCER)
    int pipe_fd; // File descriptor for pipe (if type is CONSUMER)
    int remaining_time; // Remaining execution time in milliseconds
    bool completed; // Task completion status
    int next_arrival_time; // Next arrival time of the task
};

pthread_mutex_t mtx;
pthread_cond_t cv;
bool new_task_arrived = false;
int global_clock = 0;
const char* pipe_name = "/tmp/task_pipe";

// Define tasks globally
Task tasks[] = {
    {1, 0, 20, 1000, PING, "ping -c 1 google.com", "", -1, 20, false, 0},
    {2, 0, 18, 1500, PRODUCER, "", "Hello from Task 2", -1, 18, false, 0},
    {3, 0, 10, 2000, CONSUMER, "", "", -1, 10, false, 0}
};
const int num_tasks = sizeof(tasks) / sizeof(tasks[0]);

// Function to simulate a task
void* executeTask(void* arg) {
    Task* task = (Task*)arg;

    pthread_mutex_lock(&mtx);
    while (global_clock < task->arrival_time) {
        pthread_cond_wait(&cv, &mtx);
    }
    pthread_mutex_unlock(&mtx);

    while (!task->completed) {
        if (global_clock >= task->next_arrival_time) {
            task->remaining_time = task->execution_time;
            task->next_arrival_time += task->period;
        }

        while (task->remaining_time > 0 && global_clock < task->next_arrival_time) {
            if (task->type == PING) {
                // Execute ping command and print the result
                FILE* ping_output = popen(task->ping_command.c_str(), "r");
                if (ping_output) {
                    char buffer[MAX_TEXT_SIZE];
                    while (!feof(ping_output)) {
                        if (fgets(buffer, MAX_TEXT_SIZE, ping_output) != NULL) {
                            cout << "Task " << task->id << " ping result: " << buffer;
                        }
                    }
                    pclose(ping_output);
                }
            } else if (task->type == PRODUCER) {
                // Write message to pipe
                write(task->pipe_fd, task->message.c_str(), task->message.length() + 1);
                cout << "Task " << task->id << " produced: " << task->message << endl;
            } else if (task->type == CONSUMER) {
                // Read message from pipe
                char buffer[MAX_TEXT_SIZE];
                read(task->pipe_fd, buffer, MAX_TEXT_SIZE);
                cout << "Task " << task->id << " consumed: " << buffer << endl;
            }

            usleep(100000); // Simulate work for 100ms
            task->remaining_time -= 100;
            cout << "Task " << task->id << " executing. Remaining time: " << task->remaining_time << " ms\n";

            pthread_mutex_lock(&mtx);
            if (new_task_arrived) {
                // Sort tasks by period (Rate Monotonic Scheduling)
                for (int i = 0; i < num_tasks - 1; ++i) {
                    for (int j = 0; j < num_tasks - i - 1; ++j) {
                        if (tasks[j].period > tasks[j + 1].period) {
                            Task temp = tasks[j];
                            tasks[j] = tasks[j + 1];
                            tasks[j + 1] = temp;
                        }
                    }
                }
                new_task_arrived = false;
                pthread_cond_broadcast(&cv);
                pthread_cond_wait(&cv, &mtx);
            }
            pthread_mutex_unlock(&mtx);
        }

        if (global_clock >= task->next_arrival_time && task->remaining_time > 0) {
            cout << "Task " << task->id << " missed its deadline\n";
        }
    }

    return nullptr;
}

void* scheduler(void* arg) {
    while (true) {
        usleep(100000); // Sleep for 100ms
        global_clock += 100;

        // Check if all tasks are completed
        bool all_completed = true;
        pthread_mutex_lock(&mtx);
        for (int i = 0; i < num_tasks; ++i) {
            if (!tasks[i].completed) {
                all_completed = false;
                break;
            }
        }
        if (all_completed) {
            pthread_mutex_unlock(&mtx);
            break;
        }

        // Notify all threads
        new_task_arrived = true;
        pthread_cond_broadcast(&cv);
        pthread_mutex_unlock(&mtx);
    }

    return nullptr;
}

int main() {
    // Initialize mutex and condition variable
    pthread_mutex_init(&mtx, nullptr);
    pthread_cond_init(&cv, nullptr);

	char str[256] = "hello world";
    // Create pipe
    int fifo_write;
    //open "pipe_name" with WRITE only mode and return file descriptor
    fifo_write = mkfifo(pipe_name, 0666);
    int producer_fd = open(pipe_name, O_WRONLY); //write only
    int consumer_fd = open(pipe_name, O_RDONLY);

    // Update tasks with pipe file descriptors
    tasks[1].pipe_fd = producer_fd; // Producer task
    tasks[2].pipe_fd = consumer_fd; // Consumer task

    // Create task threads
    pthread_t task_threads[num_tasks];
    for (int i = 0; i < num_tasks; ++i) {
        pthread_create(&task_threads[i], nullptr, executeTask, (void*)&tasks[i]);
    }

    // Start the scheduler
    pthread_t scheduler_thread;
    pthread_create(&scheduler_thread, nullptr, scheduler, nullptr);

    // Wait for all threads to complete
    for (int i = 0; i < num_tasks; ++i) {
        pthread_join(task_threads[i], nullptr);
    }
    pthread_join(scheduler_thread, nullptr);

    // Close and unlink pipe
    close(producer_fd);
    close(consumer_fd);
    unlink(pipe_name);

    // Destroy mutex and condition variable
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cv);

    return 0;
}
