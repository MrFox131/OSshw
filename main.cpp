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
#include <vector>
#include <algorithm>

using namespace std;

struct Measurements {

};

bool comp(pair<int, int> a, int b) {
    return a.second < b;
}

HANDLE all_fd, hour_fd, day_fd;

DWORD WINAPI MeasurementsReader(SerialPort* sp) {
    char *buffer = static_cast<char *>(calloc(sizeof(char), 1024));

    vector<pair<int,int>> measurements = {};
    vector<pair<double,int>> hour_measurements = {};
    vector<pair<double,int>> day_measurements = {};
    memset(buffer, 0, sizeof(char)*1024);
    int remainder = 0;

    int read_count;
    int full_length;

    while (true) {
        char *output = (char*)calloc(32, sizeof(char));
        if (remainder != 0 && read_count != 0) {
            memmove(buffer, buffer + full_length - remainder, remainder);
            memset(buffer+remainder, 0, sizeof(char)*(1024-remainder));
        }
        read_count = sp->read(buffer+remainder, 1024-remainder);
        full_length = read_count + remainder;
        if (read_count != 0) {
            memset(buffer+remainder+read_count, 0, sizeof(1024-remainder-read_count));
            char* current_line = buffer;
            char* current_line_end = strchr(current_line, '\n');
            while (current_line_end != nullptr) {
                int temp = 0;
                int timestamp = 0;

                sscanf(current_line, "%d %d\n", &temp, &timestamp);

                {
                    cout << temp << " " << timestamp << endl; //TODO: Обработать вывод измерений
                    measurements.emplace_back(temp, timestamp);
                    sprintf(output, "%d %d\n", temp, timestamp);

                    WriteLog(output, all_fd);

                    int overflow_index = -1;
                    while (measurements[overflow_index+1].second < timestamp - 24*60*60) {
                        overflow_index++;
                    }

                    if (overflow_index > -1) {
                        measurements.erase(measurements.begin(), measurements.begin() + overflow_index + 1);

                        sprintf(output, "%d %d\n", measurements.begin()->first, measurements.begin()->second);
                        WriteLog(output, all_fd, true);
                        for(auto measurement = measurements.begin()+1; measurement != measurements.end(); measurement++) {
                            sprintf(output, "%d %d\n", measurement->first, measurement->second);
                            WriteLog(output, all_fd);
                        }
                    }

                    if (measurements.size() > 1 && (measurements.end()-1)->second/(60*60) > (measurements.end()-2)->second/(60*60)) {
                        auto hour_start = lower_bound(measurements.begin(), measurements.end(), (measurements.end()-2)->second/(60*60)*60*60, comp);
                        auto hour_end = measurements.end()-1;
                        auto hour_sum = 0;
                        for (auto i = hour_start; i!=hour_end; i++) {
                            hour_sum += i->first;
                        }

                        auto hour_avg = (double)hour_sum/(double)distance(hour_start, hour_end);
                        hour_measurements.emplace_back(hour_avg, hour_start->second/(60*60)*(60*60));

                        sprintf(output, "%.2f %d\n", hour_avg, hour_start->second/(60*60)*(60*60));

                        WriteLog(output, hour_fd);

                        overflow_index = -1;
                        while (hour_measurements[overflow_index+1].second < (hour_measurements.end()-1)->second-24*60*60*30) {
                            overflow_index ++;
                        }

                        if (overflow_index > -1) {
                            hour_measurements.erase(hour_measurements.begin(), hour_measurements.begin()+overflow_index+1);
                            sprintf(output, "%.2f %d\n", hour_measurements.begin()->first, hour_measurements.begin()->second);
                            WriteLog(output, hour_fd, true);
                            for(auto hour = hour_measurements.begin()+1; hour != hour_measurements.end(); hour++) {
                                sprintf(output, "%.2f %d\n", hour->first, hour->second);
                                WriteLog(output, hour_fd);
                            }
                        }

                        if (hour_measurements.size() > 1 && (hour_measurements.end()-1)->second/(60*60*24) > (hour_measurements.end()-2)->second/(60*60*24)) {

                            auto day_start = lower_bound(hour_measurements.begin(), hour_measurements.end(), (hour_measurements.end()-2)->second/(60*60*24)*60*60*24, comp);
                            auto day_end = hour_measurements.end()-1;

                            auto day_sum = .0;

                            for (auto i = day_start; i!=day_end; i++) {
                                day_sum += i->first;
                            }

                            day_measurements.emplace_back(day_sum/distance(day_start, day_end), (measurements.end()-2)->second/(60*60*24)*60*60*24);
                            sprintf(output, "%.2f %d\n", day_sum/distance(day_start, day_end), (measurements.end()-2)->second/(60*60*24)*60*60*24);

                            WriteLog(output, day_fd);

                            overflow_index = -1;
                            while (day_measurements[overflow_index+1].second < (day_measurements.end()-1)->second-24*60*60*365) {
                                overflow_index ++;
                            }

                            if (overflow_index > -1) {
                                day_measurements.erase(day_measurements.begin(), day_measurements.begin() + overflow_index + 1);
                                sprintf(output, "%.2f %d\n", day_measurements.begin()->first, day_measurements.begin()->second);
                                WriteLog(output, day_fd, true);
                                for(auto day = day_measurements.begin()+1; day != day_measurements.end(); day++) {
                                    sprintf(output, "%.2f %d\n", day->first, day->second);
                                    WriteLog(output, day_fd);
                                }
                            }
                        }
                    }
                }

                if (distance(buffer, current_line_end+1) < full_length) {
                    current_line = current_line_end + 1;
                    current_line_end = strstr(current_line, "\n");
                } else {
                    current_line = buffer + full_length;
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

    all_fd = OpenFile("all.log", true);
    hour_fd = OpenFile("hour.log", true);
    day_fd = OpenFile("day.log", true);


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
