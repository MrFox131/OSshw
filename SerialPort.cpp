//
// Created by mrfox131 on 02.01.24.
//

// serialport.cpp

#include <cstdlib>
#include "SerialPort.h"
#include "IncludesAndDefines.h"
#include "string"

SerialPort::SerialPort(const Parameters& params)
        : params(params) {
    // Инициализация дополнительных переменных
#if defined(WIN32)
    serialHandle = INVALID_HANDLE_VALUE;
#else
    serialHandle = -1;
#endif
}

SerialPort::~SerialPort() {
    // Дополнительные операции при уничтожении объекта
    disconnect();
}

bool SerialPort::connect() {
    // Реализация метода connect
#if defined(WIN32)
    std::string portName("\\\\.\\");
    portName += params.portName;
    serialHandle = CreateFile(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (serialHandle == INVALID_HANDLE_VALUE) {
        return false;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(serialHandle, &dcbSerialParams)) {
        CloseHandle(serialHandle);
        return false;
    }

    dcbSerialParams.BaudRate = static_cast<DWORD>(params.baudRate);
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = static_cast<BYTE>(params.stopBits);
    dcbSerialParams.Parity = static_cast<BYTE>(params.parity);

    if (!SetCommState(serialHandle, &dcbSerialParams)) {
        CloseHandle(serialHandle);
        return false;
    }

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(serialHandle, &timeouts)) {
        CloseHandle(serialHandle);
        return false;
    }
#else
    serialHandle = open(params.portName, O_RDWR | O_NOCTTY | O_NDELAY);
    if (serialHandle == -1) {
        return false;
    }

    struct termios tty;
    if (tcgetattr(serialHandle, &tty) != 0) {
        close(serialHandle);
        return false;
    }

    cfsetospeed(&tty, static_cast<speed_t>(params.baudRate));
    cfsetispeed(&tty, static_cast<speed_t>(params.baudRate));

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag |= static_cast<unsigned int>(params.parity);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag |= static_cast<unsigned int>(params.stopBits);
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(serialHandle, TCSANOW, &tty) != 0) {
        close(serialHandle);
        return false;
    }

    usleep(10000);
#endif

    return true;
}

void SerialPort::disconnect() {
    // Реализация метода disconnect
#if defined(WIN32)
    if (serialHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(serialHandle);
        serialHandle = INVALID_HANDLE_VALUE;
    }
#else
    if (serialHandle != -1) {
        close(serialHandle);
        serialHandle = -1;
    }
#endif
}

int SerialPort::read(char* buffer, int bufferSize) {
    // Реализация метода read
#if defined(WIN32)
    DWORD bytesRead;
    if (ReadFile(serialHandle, buffer, bufferSize, &bytesRead, NULL)) {
        return static_cast<int>(bytesRead);
    }
#else
    int bytesRead = ::read(serialHandle, buffer, bufferSize);
    if (bytesRead != -1) {
        return bytesRead;
    }
#endif

    return 0; // Ошибка при чтении или нулевой объем данных
}

BaudRate SerialPort::convertBaudRate(const char* baudRateStr) {
    // Попробуем конвертировать строку в число
    int baudRateInt = atoi(baudRateStr);

    // Если конвертация не удалась или число не соответствует BaudRate, возвращаем BaudInvalid
    if (baudRateInt <= 0 || baudRateInt >= 1000000) {
        return BaudRate::BaudInvalid;
    }

    // Возвращаем соответствующий BaudRate
    return static_cast<BaudRate>(baudRateInt);
}
