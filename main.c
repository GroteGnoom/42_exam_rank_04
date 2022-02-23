#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv, char **env) {
	char previous = ';';
	for (int start_part = 1; start_part < argc; start_part++) {
		//printf("starting loop at %d, %s\n", start_part, argv[start_part]);
		int i;
		char next = 0;
		int fdsl;
		int fdsr[2];
		for (i = start_part; i < argc; i++) {
			if (!strcmp(argv[i], "|") || !strcmp(argv[i], ";"))
			{
				next = argv[i][0];
				break;
			}
		}
		if (i == start_part)
		{
			//printf("found empty part at %d, %s\n", start_part, argv[start_part]);
			continue;
		}
		if (!strncmp(argv[start_part], "cd", 2))
		{
			chdir(argv[start_part + 1]);
			start_part = i;
			continue;
		}
		if (previous == '|')
			fdsl = fdsr[0];
		if (next == '|')
				pipe(fdsr);
		if (!fork()) {
			//printf("executing %s, previous is %c\n", argv[start_part], previous);
			argv[i] = NULL;
			if (next == '|')
			{
				dup2(fdsr[1], STDOUT_FILENO);
				close(fdsr[0]);
			}
			else if (previous == '|')
			{
				dup2(fdsl, STDIN_FILENO);
			}
			execve(argv[start_part], argv + start_part, env);
			perror(argv[start_part]);
			exit(1);
		}
		close(fdsr[1]);
		if (previous == '|')
			close(fdsl);
		start_part = i;
		previous = next;
	}
	//system("lsof -c microshell");
}
