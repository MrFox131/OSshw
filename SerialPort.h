// serialport.h

#pragma once

#include "IncludesAndDefines.h"

#if defined(WIN32)
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

// Определение перечисления для BaudRate
enum class BaudRate {
    BaudInvalid = -1,
#if defined(WIN32)
    Baud1200 = CBR_1200,
    Baud2400 = CBR_2400,
    Baud4800 = CBR_4800,
    Baud9600 = CBR_9600,
    Baud19200 = CBR_19200,
    Baud38400 = CBR_38400,
    Baud57600 = CBR_57600,
    Baud115200 = CBR_115200
#else
    Baud1200 = B1200,
    Baud2400 = B2400,
    Baud4800 = B4800,
    Baud9600 = B9600,
    Baud19200 = B19200,
    Baud38400 = B38400,
    Baud57600 = B57600,
    Baud115200 = B115200
#endif
};

// Определение перечисления для Parity
enum class Parity {
#if defined(WIN32)
    ParityNone = NOPARITY,
    ParityOdd = ODDPARITY,
    ParityEven = EVENPARITY
#else
    ParityNone = 0,
    ParityOdd = PARENB | PARODD,
    ParityEven = PARENB
#endif
};

// Определение перечисления для StopBits
enum class StopBits {
#if defined(WIN32)
    StopBitsOne = ONESTOPBIT,
    StopBitsTwo = TWOSTOPBITS
#else
    StopBitsOne = 0,
    StopBitsTwo = CSTOPB
#endif
};

class SerialPort {
public:
    // Внутренний класс для параметров установки соединения
    class Parameters {
    public:
        Parameters(const char* portName, const char* baudRate, Parity parity, StopBits stopBits)
                : portName(portName), baudRate(convertBaudRate(baudRate)), parity(parity), stopBits(stopBits) {}

    private:
        const char* portName;
        BaudRate baudRate;
        Parity parity;
        StopBits stopBits;

        friend class SerialPort;
    };

    // Конструктор и деструктор
    SerialPort(const Parameters& params);
    ~SerialPort();

    // Методы для работы с портом
    bool connect();
    void disconnect();
    int read(char* buffer, int bufferSize);

private:
    // Приватные методы
    static BaudRate convertBaudRate(const char* baudRateStr);

    // Дополнительные приватные методы или переменные могут быть добавлены
    HANDLE serialHandle;

    Parameters params;
};
