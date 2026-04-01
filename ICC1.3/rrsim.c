#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

#define MAX_PROCS 10 // Max number of processes
#define QUANTUM 40 // Time quantum, ms
#define MIN(x,y) ((x)<(y)?(x):(y)) // Compute the minimum

int clock = 0;

/**
 * Process information.
 */
struct process {
    int pid;
    int time_awake_remaining;
};

/**
 * The process table.
 */
struct process table[MAX_PROCS];

/**
 * Initialize the process table.
 */
void init_proc_table(void)
{
    for (int i = 0; i < MAX_PROCS; i++) {
        table[i].pid = i;
        table[i].time_awake_remaining = 0;
    }
}

/**
 * Parse the command line.
 */
void parse_command_line(int argc, char **argv)
{
    for(int i = 0; i < argc-1; i++){
        table[i].time_awake_remaining = atoi(argv[i+1]);
    }
}

/**
 * Main.
 */
int main(int argc, char **argv)
{
    struct queue *q = queue_new();

    init_proc_table();

    parse_command_line(argc, argv);

    int num_ready = argc-1;
    int quantum = 40;

    for(int i = 0; i < argc-1; i++){
        queue_enqueue(q, table + i);
    }

    while(num_ready != 0){
        printf("=== Clock %d ms ===\n", clock);
        
        struct process *p = queue_dequeue(q);
        printf("PID %d: Running\n", p->pid);

        // Check if time awake remaining is less than the time slice. If so, only subtract however much time is remaining
        if(table[p->pid].time_awake_remaining < quantum){
            printf("PID %d: Ran for %d ms\n", p->pid, table[p->pid].time_awake_remaining);
            clock += table[p->pid].time_awake_remaining;
            table[p->pid].time_awake_remaining = 0;    
        }
        // Subtract the time slice from time remaining
        else{
            printf("PID %d: Ran for %d ms\n", p->pid, quantum);
            clock += quantum;
            table[p->pid].time_awake_remaining -= quantum;
        }

        // If the process is done, mark it done and don't re-queue it
        if(table[p->pid].time_awake_remaining == 0){
            num_ready -= 1;
        }
        // If the process isn't done, re-queue it
        else{
            queue_enqueue(q, p);
        }
    }

    queue_free(q);
}
