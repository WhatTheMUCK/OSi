#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
using namespace std;

string backFile = "main1.back";
string semFile = "main1.semaphore";
int accessPerm = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH;

int main(int argc, char const *argv[])
{
    int state = 0;
    sem_t *semaphore = sem_open(semFile.c_str(), O_CREAT, accessPerm, 1);
    if (semaphore == SEM_FAILED){
	    perror("sem_open");
            exit(EXIT_FAILURE);
    }
    if (sem_getvalue(semaphore, &state) == -1){
	    perror("sem_getvalue");
	    exit(EXIT_FAILURE);
    }
    int semWaitErrCheck = sem_wait(semaphore);
    if (semWaitErrCheck == -1){
	    perror("sem_wait");
            exit(EXIT_FAILURE);
    }
    if (sem_getvalue(semaphore, &state) == -1){
	    perror("sem_getvalue");
	    exit(EXIT_FAILURE);
    }
    string str;
    float res, a;
    bool flag = true, zeroflag = true;
    ifstream in_file;
    in_file.open(argv[0]);
    string line, answer = "";
    while (getline(in_file, line) && zeroflag){
	if (!flag){
	    answer = answer + to_string(res) + "\n";
	    flag = true;
	}
	stringstream ss(line);
	while(ss >> a && zeroflag){
		if (flag){
			flag = false;
			res = a;
		} else {
			if (a == 0){
				zeroflag = false;
				break;
			}
			res /=  a;
		}
	}
    }
    if (zeroflag)
    	answer = answer + to_string(res) + "\n";
    if (!zeroflag)
	answer = answer + "division by 0\n";
    int mapSize = answer.size();
    int fd = shm_open(backFile.c_str(), O_RDWR, accessPerm);
    ftruncate(fd, mapSize);
    // Устанавливает длину файла с файловым дескриптором fd в mapSize байт
    if (fd == -1){
	    perror("shm_open");
	    exit(EXIT_FAILURE);
    }
    char *mapped = (char *)mmap(NULL,
                                mapSize,
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED,
                                fd,
                                0);
    //MAP_SHARED - задаёт опции отражения (разделение использования этого отражения с другими процессами)
    if (mapped == MAP_FAILED){
	    perror("mmap");
            exit(EXIT_FAILURE);
    }   
    memset(mapped, '\0', mapSize);
    for (int i = 0; i < answer.size(); ++i) {
        mapped[i] = answer[i];
    }
    close(fd);
    usleep(00150000);
    sem_post(semaphore);
    sem_close(semaphore);
}

