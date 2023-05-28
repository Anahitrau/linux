#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_KEY 1234
#define SEM_KEY 5678

struct SharedMemory {
    int function;
    int operand1;
    int operand2;
    int result;
};

void semWait(int semId) {
    struct sembuf semBuf;
    semBuf.sem_num = 0;
    semBuf.sem_op = -1;
    semBuf.sem_flg = 0;
    semop(semId, &semBuf, 1);
}

void semSignal(int semId) {
    struct sembuf semBuf;
    semBuf.sem_num = 0;
    semBuf.sem_op = 1;
    semBuf.sem_flg = 0;
    semop(semId, &semBuf, 1);
}

int main() {
    // Stanal SharedMemory i hatvacy
    int shmId = shmget(SHM_KEY, sizeof(SharedMemory), 0666);
    if (shmId == -1) {
        std::cerr << "Failed!" << std::endl;
        return 1;
    }

    SharedMemory* sharedMemory = static_cast<SharedMemory*>(shmat(shmId, nullptr, 0));
    if (sharedMemory == reinterpret_cast<SharedMemory*>(-1)) {
        std::cerr << "Failed !" << std::endl;
        return 1;
    }

    // stanal semaphore
    int semId = semget(SEM_KEY, 1, 0666);
    if (semId == -1) {
        std::cerr << "Failed!" << std::endl;
        return 1;
    }

    while (true) {
        // mutqagrum enq function i hamary, operandnery, ev save enq anum shared memory i mej
        int function;
        std::cout << "Enter the function number (0 - add, 1 - sub, 2 - div, 3 -mul): ";
        std::cin >> function;

        int operand1, operand2;
        std::cout << "Enter 2 integers: ";
        std::cin >> operand1 >> operand2;

        sharedMemory->function = function;
        sharedMemory->operand1 = operand1;
        sharedMemory->operand2 = operand2;

        // azdanshan enq talis server in  vor data n patrast e
        semSignal(semId);

        // spasum enq server in vor verjacni gorcy
        semWait(semId);

        // tpum enq resulty
        std::cout << "Result: " << sharedMemory->result << std::endl;
    }
    //anjatum enq shared memory i hatvacy
    if (shmdt(sharedMemory) == -1) {
        std::cerr << "Failed!" << std::endl;
        return 1;
    }

    return 0;
}
