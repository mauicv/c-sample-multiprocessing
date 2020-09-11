#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#include "util.h"

#define TARGET 500000000
#define MAX_CONNECT 2


struct task {
  int task_id;
  int pid;
  int pipe;
  int pipes[MAX_CONNECT];
  int num_pipes;
};


void generate_child_process(struct task* task_data){
  int pid;
  int p[2];
  if (task_data -> pid != 0) return;
  if (pipe(p) < 0) exit(1);
  if (fcntl(p[0], F_SETFL, O_NONBLOCK) < 0) exit(2);

  if ((pid = fork()) == 0 ){
    // main process
    close(p[1]);
    task_data -> pid = pid;
    task_data -> pipes[task_data -> num_pipes++] = p[0];
  } else {
    // child process
    // do something...
    for(int i=0; i<TARGET; i++);

    close(p[0]);
    char result[20], pid_str[6];
    strcpy(result, "_");
    strcpy(pid_str, itoa(pid, 10));
    char* full_msg = strcat(pid_str, "_");
    write(p[1], full_msg, 15);
    exit(0);
  }
}

int active_processes(int* pipes, int len){
  for(int i=0; i<len; i++) if (pipes[i] == 1) return 1;
  return 0;
}

int main(void){
  struct task task_data;
  task_data.pid = 0;
  task_data.num_pipes = 0;

  clock_t start_t, end_t;
  start_t = clock();

  for(int i=0; i<MAX_CONNECT; i++){
    generate_child_process(&task_data);
  }

  char buffer[100];
  int nread, i, active_pipes[MAX_CONNECT];

  for (i=0; i<MAX_CONNECT; i++) active_pipes[i] = 1;
  while(active_processes(active_pipes, MAX_CONNECT)) {
    for (i = 0; i<MAX_CONNECT; i++) {
      if (active_pipes[i] == 1) {
        nread = read(task_data.pipes[i], &buffer, 100);
        switch (nread) {
          case -1:
            break;
          case 0:
            active_pipes[i] = 0;
            task_data.num_pipes--;
            break;
          default:
            break;
        }
      }
    }
  }

  end_t = clock();
  printf("\nTotal Time: %f\n", ((double) (end_t - start_t)) / CLOCKS_PER_SEC);

  start_t = clock();
  for(int i=0; i<MAX_CONNECT*TARGET; i++);
  end_t = clock();
  printf("\nTotal Time: %f\n", ((double) (end_t - start_t)) / CLOCKS_PER_SEC);


  exit(0);
}
