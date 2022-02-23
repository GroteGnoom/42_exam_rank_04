#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv, char **env) {
	char previous = ';';
	for (int start_part = 1; start_part < argc; start_part++) {
		int i;
		char next = 0;
		int fds[2];
		for (i = start_part; i < argc; i++) {
			if (!strcmp(argv[i], "|") || !strcmp(argv[i], ";"))
			{
				next = argv[i][0];
				break;
			}
		}
		if (next == '|')
				pipe(fds);
		if (!fork()) {
			printf("executing %s, previous is %c\n", argv[start_part], previous);
			argv[i] = NULL;
			if (next == '|')
			{
				dup2(fds[1], STDOUT_FILENO);
				close(fds[0]);
			}
			else if (previous == '|')
			{
				dup2(fds[0], STDIN_FILENO);
			}
			execve(argv[start_part], argv + start_part, env);
		}
		close(fds[1]);
		if (next == ';')
			break;
		start_part = i;
		previous = next;
	}
}
