#include "IncludesAndDefines.h"
#if !defined(WIN32)

#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

#endif

#include <iostream>
#include "SharedMemory.hpp"
#include "SerialPort.h"
#include "FileUtils.h"
#include <ctime>

using namespace std;

struct Measurements {

};

DWORD WINAPI MeasurementsReader(SerialPort* sp) {
    char *buffer = static_cast<char *>(calloc(sizeof(char), 1024));
    memset(buffer, 0, sizeof(char)*1024);
    int remainder = 0;

    int read_count;
    int full_length;

    while (true) {
        if (remainder != 0 && read_count != 0) {
            memmove(buffer, buffer + full_length - remainder, remainder);
            memset(buffer+remainder, 0, sizeof(char)*(1024-remainder));
        }
        read_count = sp->read(buffer+remainder, 1024-remainder);
        full_length = read_count + remainder;
        if (read_count != 0) {
            memset(buffer+remainder+read_count, 0, sizeof(1024-remainder-read_count));
            char* current_line = buffer;
            char* current_line_end = strstr(current_line, "\n");
            while (current_line_end != nullptr) {
                int temp = 0;
                int timestamp = 0;

                sscanf(current_line, "%d %d\n", &temp, &timestamp);

                cout << temp << " " << timestamp << endl; //TODO: Обработать вывод измерений

                if (distance(buffer, current_line_end+1) < read_count+remainder) {
                    current_line = current_line_end + 1;
                    current_line_end = strstr(current_line, "\n");
                } else {
                    current_line = buffer + read_count;
                    break;
                }
            }
            remainder = distance(current_line, buffer+full_length);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "Usage: ./ShikataGaNai <serial port> <baud rate>" << endl;
        return 0;
    }

    auto shmem = new SharedMemory<int>("test_name");
    shmem->Lock();
    shmem->content->data=1;
    shmem->Unlock();

    auto serialPortParams = SerialPort::Parameters(
            argv[1],
            argv[2],
            Parity::ParityNone,
            StopBits::StopBitsOne
    );

    auto sp = SerialPort(serialPortParams);

    sp.connect();

    MeasurementsReader(&sp);

    char* buffer = static_cast<char *>(::calloc(sizeof(char), 1024));

    sp.read(buffer, 1024);

    cout << buffer;
    
    return 0;
}
