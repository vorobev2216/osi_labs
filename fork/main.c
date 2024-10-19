#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

//состояния дочернего процесса
pid_t child_pid = -1;


void signal_handler(int signal) {
    switch (signal) {
        case SIGINT:
            printf("\nПолучен сигнал SIGINT (Ctrl+C)\n");

            if (child_pid > 0) {
                kill(child_pid, SIGTERM);
                printf("Дочерний процесс (PID: %d) завершен.\n", child_pid);
            }
            exit(EXIT_SUCCESS); // Завершаем родительский процесс
            break;
        case SIGTERM:
            printf("\nПолучен сигнал SIGTERM\n");
            exit(EXIT_SUCCESS); // Завершаем программу
            break;
        default:
            printf("\nПолучен сигнал: %d\n", signal);
            break;
    }
}


void exit_handler() {
    printf("Программа завершает работу\n");
}

int main() {
    
    signal(SIGINT, signal_handler); 
    struct sigaction sa;
    sa.sa_handler = signal_handler; 
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);

    
    atexit(exit_handler);

    
    child_pid = fork();

    if (child_pid < 0) {
       
        perror("Ошибка fork()");
        exit(EXIT_FAILURE);
    } else if (child_pid == 0) {
        // Код, выполняемый дочерним процессом
        printf("Дочерний процесс (PID: %d) создан.\n", getpid());
        for (int i = 0; i < 5; i++) {
            printf("Дочерний процесс: работа... (%d)\n", i + 1);
            sleep(5); 
        }
        exit(EXIT_SUCCESS); 
    } else {
        
        printf("Родительский процесс (PID: %d) создал дочерний процесс (PID: %d).\n", getpid(), child_pid);

        
        int status;
        waitpid(child_pid, &status, 0); 

       
        if (WIFEXITED(status)) {
            printf("Дочерний процесс завершился с кодом: %d\n", WEXITSTATUS(status));
        } else {
            printf("Дочерний процесс завершился ненормально.\n");
        }
    }

    return 0;
}
