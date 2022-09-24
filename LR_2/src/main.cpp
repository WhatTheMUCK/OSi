#include <unistd.h>
#include <iostream>
#include <fcntl.h>
using namespace std;

int main(){
	string name;
	cin >> name;
	int file = open(name.c_str(), O_RDONLY);
	int pipefd[2];
	int pip = pipe(pipefd);
	if (pip == -1){
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	int child = fork();
	if (child == -1){
		perror("fork");
		exit(EXIT_FAILURE);
	} else if (child == 0){
		close(pipefd[0]);
		if (execlp("./child", to_string(file).c_str(), to_string(pipefd[0]).c_str(), to_string(pipefd[1]).c_str(), NULL) == -1){
			perror("execlp");
			exit(EXIT_FAILURE);
		}
	} else {
		close(pipefd[1]);
		char ch;
		while (read(pipefd[0], &ch, sizeof(char)) > 0 && ch != '\0'){
			putchar(ch);
		}
		cout << "\n";
		close(pipefd[0]);
	}
	close(file);
	return 0;	
}

