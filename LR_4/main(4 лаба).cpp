#include <iostream>
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
// S_IWURS | S_IRUS | S_TRGRP | S_IROTH - пользователь имеет права на запись информации в файл | на чтение файла | группа имеет права на чтение файла | все остальные имеют права на чтение файла

int main(int argc, char const *argv[])
{
    string in;
    cin >> in;
    int file = open(in.c_str(), O_WRONLY);
    sem_t *semaphore = sem_open(semFile.c_str(), O_CREAT, accessPerm, 1);
    // Создаёт новый семафор с именем semFilde.c_str() ("main1.semaphore") (O_CREAT - создаётся если его ещё не существует), с правами доступа accesPerm, с начальным значением 1 нового семафора
    if (semaphore == SEM_FAILED){
	perror("sem_open");
        exit(EXIT_FAILURE);
    }
    int state = 0;
    int semErrCheck = sem_getvalue(semaphore, &state);
    if (semErrCheck == -1){
	perror("sem_getvalue");
        exit(EXIT_FAILURE);
    }
    while (state++ < 1) {
        sem_post(semaphore);
    }
    while (state-- > 2) {
        sem_wait(semaphore);
    }
    pid_t child = fork();
    if (child == -1){
		perror("fork");
		exit(EXIT_FAILURE);
    } else if (child == 0) {
	char *Child_argv[]={(char*)in.c_str(), NULL};
        execv("child", Child_argv);
		perror("execl");
		exit(EXIT_FAILURE);
    } else {
        int fd = shm_open(backFile.c_str(), O_RDWR | O_CREAT, accessPerm);
	// backFilde.c_str() ("main1.back") - определяет создаваемый объект разделяемой памяти для создания или открытия
	// O_RDWR | O_CREAT - Открывает объект для чтения и записи | Создаёт объект разделяемой памяти, если он ещё не существует
	// accesPerm - права доступа
        while (1) {
       		semErrCheck = sem_getvalue(semaphore, &state);
       		if (semErrCheck == -1){
            		perror("sem_getvalue");
            		exit(EXIT_FAILURE);
        	}
        	if (state == 0) {
            		int semWaitErrCheck = sem_wait(semaphore);
            		if (semWaitErrCheck == -1){
                		perror("sem_wait");
                		exit(EXIT_FAILURE);
            		}

            		struct stat statBuf;
            		fstat(fd, &statBuf);
			// Возвращает информацию об опрашиваемом файле (заданного в виде файлового дескриптора fd) в буфер, на который указывает statbuf
            		int mapSize = statBuf.st_size;
            		char *mapped = (char *) mmap(NULL,
                                		     mapSize,
                                 		     PROT_READ | PROT_WRITE,
                                 		     MAP_SHARED,
                                 		     fd,
                                		     0);
			// mapped отображает mapSize байт, начиная со смещения 0 файла, определённого файловым описателем fd, в память начиная с адреса NULL.
			// PROT_READ | PROT_WRITE описывают желаемый режим защиты памяти (Данные можно читать | В эту область можно записывать информацию)
			// MAP_SHARED опция отражения (Разделение использования этого отражения с другими процессами)
                cout << "Answer is:\n";
                for (int i = 0; i < mapSize; ++i) {
                    cout << mapped [i];
                }
            		return 0;
        	}
   	 }
    }
    return 0;
}


