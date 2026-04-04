#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

#define MAX_PROG_LEN 20 // Max terms in a "program"
#define MAX_PROCS 10 // Max number of processes
#define QUANTUM 40 // Time quantum, ms

int clock = 0;

enum State { ready, sleeping, exited };

struct process {
    int pid;
    enum State state;
    int time_asleep_remaining;
    int time_awake_remaining;
    int pc;
    int instructions[MAX_PROG_LEN];
    int num_instructions;
    int just_woke;
};

struct process table[MAX_PROCS];
struct process *procs_sleeping[MAX_PROCS];
int num_sleeping = 0;

void init_proc_table(void)
{
    for (int i = 0; i < MAX_PROCS; i++) {
        table[i].pid = i;
        table[i].time_awake_remaining = 0;
        table[i].time_asleep_remaining = 0;
        table[i].pc = 0;
        table[i].num_instructions = 0;
        table[i].state = exited;
        table[i].just_woke = 0;
    }
}

/**
 * Parse the command line.
 */
void parse_command_line(int argc, char **argv)
{
    for(int i = 0; i < argc-1; i++) {
        char *arg = strdup(argv[i + 1]);
        char *token = strtok(arg, ",");
        int j = 0;

        while (token != NULL && j < MAX_PROG_LEN) {
            table[i].instructions[j++] = atoi(token);
            token = strtok(NULL, ",");
        }
        
        table[i].num_instructions = j;
        table[i].pc = 0;
        table[i].time_awake_remaining = table[i].instructions[0];
        table[i].state = ready;

        free(arg);
    }
}

void tick_sleeping(int elapsed, struct queue *q)
{
    int i = 0;
    while (i < num_sleeping) {
        struct process *p = procs_sleeping[i];
        p->time_asleep_remaining -= elapsed;
        if (p->time_asleep_remaining <= 0) {
            p->time_asleep_remaining = 0;

            p->pc += 1;

            if (p->pc < p->num_instructions) {
                p->time_awake_remaining = p->instructions[p->pc];
                p->state = ready;
                p->just_woke = 1;
                queue_enqueue(q, p);
            } 
            else {
                p->state = exited;
            }

            procs_sleeping[i] = procs_sleeping[--num_sleeping];
        } 
        else {
            i++;
        }
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
    
    int num_procs = argc-1;

    for(int i = 0; i < num_procs; i++) {
        queue_enqueue(q, table + i);
    }

    while(!queue_is_empty(q) || num_sleeping > 0) {
        
        if (queue_is_empty(q)) {
            // Nothing is ready, fast-forward
            int min_sleep = procs_sleeping[0]->time_asleep_remaining;
            for (int i = 1; i < num_sleeping; i++) {
                int t = procs_sleeping[i]->time_asleep_remaining;
                if (t < min_sleep) min_sleep = t;
            }
            clock+=min_sleep;
            tick_sleeping(min_sleep, q);
            continue;
        }

        printf("=== Clock %d ms ===\n", clock);

        for (int i = 0; i < MAX_PROCS; i++) {
            if (table[i].just_woke) {
                printf("PID %d: Waking up for %d ms\n", table[i].pid, table[i].time_awake_remaining);
                table[i].just_woke = 0;
            }
        }

        struct process *p = queue_dequeue(q);

        printf("PID %d: Running\n", p->pid);

        int run_time;
        if (p->time_awake_remaining <= QUANTUM) {
            run_time = p->time_awake_remaining;
        } 
        else {
            run_time = QUANTUM;
        }

        clock += run_time;
        p->time_awake_remaining -= run_time;
        
        int goes_to_sleep = 0;
        int next_pc = p->pc + 1;

        if (p->time_awake_remaining == 0) {
            if (next_pc < p->num_instructions) {
                goes_to_sleep = 1;
            } else {
                printf("PID %d: Exiting\n", p->pid);
                p->state = exited;
            }
        } else {
            queue_enqueue(q, p);
        }

        tick_sleeping(run_time, q);

        if (goes_to_sleep) {
            p->pc = next_pc;
            p->time_asleep_remaining = p->instructions[next_pc];
            p->state = sleeping;
            procs_sleeping[num_sleeping++] = p;
            printf("PID %d: Sleeping for %d ms\n", p->pid, p->time_asleep_remaining);
        }

        printf("PID %d: Ran for %d ms\n", p->pid, run_time);
    }

    queue_free(q);
    return 0;
}
