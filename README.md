# rtos-scheduling
Operating Systems Project: Real-Time Task Scheduling Simulation for RTOS Analysis

Abstract— This C++ simulator, running on Linux, allows users to define tasks and explore
real-time scheduling algorithms with threading. The simulator provides metrics (execution order,
response times, deadline misses) for analyzing scheduling behavior and schedulability. The
simulator supports multiple scheduling algorithms and allows users to configure task parameters
(arrival times, execution times, deadlines) and thread counts. The simulator's threading
capabilities enable the simulation of concurrent task execution, allowing users to analyze the
impact of threading on real-time scheduling performance.

Keywords— C++ simulator, Real-time scheduling, Threading, Concurrent task execution,Arrival
times,Execution time.

**1. Introduction**
This project simulates a real-time scheduling system using POSIX threads and
semaphores in C++. It implements a basic Rate Monotonic Scheduling (RMS) algorithm to
manage tasks of different types—PING, MESSAGE, and RESPONSE—each with specific
execution periods and deadlines. The tasks interact through pipes and shared
resources, demonstrating essential concepts in real-time operating systems such as task
synchronization, mutexes, and condition variables. The scheduler coordinates
the execution of tasks, ensuring that deadlines are met and handling
preemptions as necessary.
**2. Methodology**
Methodology of the project is explained in further steps as follow :
  **2.1. Task Definition**
  We defined an array of Task objects, each representing a unique task with
  specific attributes like id, arrival_time, execution_time, period, type, and
  relevant commands or messages.These tasks are characterized to simulate
  different types of activities such as pinging, sending messages, and handling
  responses, thereby showcasing various real-time task behaviors.
  **2.2. Synchronization Setup**
  Synchronization is crucial in real-time systems to prevent race conditions
  and ensure consistent task execution. We initialized a mutex
  (pthread_mutex_t mtx) and a condition variable (pthread_cond_t cv) to
  coordinate thread activities. Additionally, a global clock was managed
  alongside a flag (new_task_arrived) to track the state of the system and
  detect new task arrivals.
  **2.3. Task Execution**
  The executeTask function simulates the execution of each task. By locking
  and unlocking the mutex, we protected shared resources and synchronized
  access. The function checks and updates task attributes based on the global
  clock and handles different task types (PING, MESSAGE, RESPONSE) with
  their respective behaviors. Preemption checks and deadline management
  were integrated within the task loop to ensure timely task completion.
  **2.4. Scheduler Implementation**
  The scheduler function manages the global clock and orchestrates task
  scheduling. It periodically increments the global clock, monitors task
  completions, and notifies all threads of new task arrivals using the condition
  variable. This function ensures that tasks are executed in a timely manner,
  adhering to their specified periods and deadlines.
  **2.5. Main Function**
  In the main function, we initialized the mutex and condition variable, created
  threads for each task using pthread_create, and started the scheduler
  thread. The function waits for all task and scheduler threads to complete
  using pthread_join, ensuring that all tasks are executed as planned. Finally,
  the mutex and condition variable are destroyed to clean up resources,
  marking the end of the program.
  **2.6. Pipes and I/O**
  Inter-task communication was set up using named pipes (pipe_fd), allowing
  tasks to write to and read from pipes based on their types and behaviors.
  This mechanism facilitated the exchange of messages and responses
  between tasks, simulating real-world scenarios where tasks interact and
  share information in a concurrent environment.
  **2.7. Compilation and Execution**
  The program was compiled using appropriate flags to link the POSIX thread
  library (-lpthread). Running the program demonstrated the task scheduling,
  execution, and interaction among tasks, providing a practical implementation
  of real-time scheduling concepts using POSIX threads and synchronization
  mechanisms.
