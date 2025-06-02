#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char **argv)
{	
	int start, status, pid, n;
	int buffer[1];

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}

    /* Parsing of arguments */
	
	n = atoi(argv[1]);
	buffer[0] = atoi(argv[2]);
	start = atoi(argv[3]);

	if (n < 3 || start < 0 || start >= n || buffer[0] < 0) {
		printf("argumentos inválidos.\n");
        exit(EXIT_FAILURE);
    }

  	/* TO COMPLETE */
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], start);
    
   	/* You should start programming from here... */

    int end = (start - 1 + n) % n;

    int pipe_start[2]; // padre -> primer hijo
    int pipe_end[2];   // último hijo -> padre
    int child_pipes[n][2]; // entre hijos

    for (int i = 0; i < n; i++) {
        if (pipe(child_pipes[i]) == -1) {
            perror("pipe hijo");
            exit(1);
        }
    }

    if (pipe(pipe_start) == -1 || pipe(pipe_end) == -1) {
        perror("pipe padre");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(1);
        }

        if (pid == 0) {
            // hijo i
            int valor;
            int anterior = (i - 1 + n) % n;

            for (int j = 0; j < n; j++) {
                if (j != anterior) close(child_pipes[j][0]); 
                if (j != i)        close(child_pipes[j][1]); 
            }

			//no uso
            close(pipe_start[1]); 
            close(pipe_end[0]);   

           
            if (i == start) {
                read(pipe_start[0], &valor, sizeof(valor));
            } else {
                read(child_pipes[anterior][0], &valor, sizeof(valor));
            }

            valor++;

            if (i == end) {
                write(pipe_end[1], &valor, sizeof(valor));
            } else {
                write(child_pipes[i][1], &valor, sizeof(valor));
            }

           
            close(child_pipes[anterior][0]);
            close(child_pipes[i][1]);
            close(pipe_start[0]);
            close(pipe_end[1]);

            exit(0);
        }
    }

    // padre
    close(pipe_start[0]);
    close(pipe_end[1]);   

    write(pipe_start[1], &buffer[0], sizeof(buffer[0]));
    close(pipe_start[1]);

	//espero
    for (int i = 0; i < n; i++) {
        wait(&status);
    }

    int resultado;
    read(pipe_end[0], &resultado, sizeof(resultado));
    close(pipe_end[0]);

    printf("valor final: %d\n", resultado);

    return 0;
}