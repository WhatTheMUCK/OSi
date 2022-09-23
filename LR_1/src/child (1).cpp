#include <unistd.h>
#include <sstream>
#include <iostream>
#define STDIN 0
#define STDOUT 1
using namespace std;


int main(int argc, char *argv[]){
	int pipefd[2];
	pipefd[0] = atoi(argv[1]);
	pipefd[1] = atoi(argv[2]);
	dup2(atoi(argv[0]), STDIN);
	dup2(pipefd[1], STDOUT);
	close(pipefd[0]);
	float res, a;
	bool flag = true;
	string line;
	while (getline(cin, line)){
		if (!flag){
			cout << res << "\n";
			flag = true;
		}
		stringstream ss(line);
		while(ss >> a){
			if (flag){
				flag = false;
				res = a;
			} else {
				if (a == 0){
					exit(0);
				}
				res /=  a;
			}
		}
	}
	cout << res << "\0";
	close(pipefd[1]);	
}



