#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define EXEC_FAILED "error: cannot execute "

void putstr(char *str) {
	int i = 0;
	while (str[i])
		i++;
	write(2, str, i);
}

void fatal(void) {
	putstr("error: fatal\n");
	exit(1);
}

void	dup2_ch(int fd1, int fd2) {
	if (dup2(fd1, fd2))
		fatal();
}

int main(int argc, char **argv, char **env) {
	char previous = ';';
	//int forked = 0;
	int pid;
	int stat_loc;
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
			if (i - start_part > 1)
				putstr("error: cd: bad arguments\n");
			else
				chdir(argv[start_part + 1]);
			start_part = i;
			continue;
		}
		if (previous == '|')
			fdsl = fdsr[0];
		if (next == '|')
			if (pipe(fdsr))
				fatal();
		//forked ++;
		pid = fork();
		if (!pid) {
			//printf("executing %s, previous is %c\n", argv[start_part], previous);
			argv[i] = NULL;
			if (next == '|')
			{
				dup2_ch(fdsr[1], STDOUT_FILENO);
				close(fdsr[0]);
			}
			else if (previous == '|')
				dup2_ch(fdsl, STDIN_FILENO);
			execve(argv[start_part], argv + start_part, env);
			putstr(EXEC_FAILED);
			putstr(argv[start_part]);
			putstr("\n");
			exit(127);
		}
		if (pid == -1)
			fatal();
		close(fdsr[1]);
		if (previous == '|')
			close(fdsl);
		start_part = i;
		previous = next;
	}
	waitpid(pid, &stat_loc, 0);
	return (WEXITSTATUS(stat_loc));
	//system("lsof -c microshell");
}
