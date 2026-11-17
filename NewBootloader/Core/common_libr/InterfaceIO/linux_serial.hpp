#ifndef __LINUX_SERIAL_HPP__
#define __LINUX_SERIAL_HPP__

#ifdef __linux__

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string>

#include "InterfaceIO.h"

class LinuxSerial : public InterfaceIO {
    int fd_ = -1;
    std::string device_path_;
    uint32_t baud_rate_;
public:
    uint8_t error = 0;
    // Constructor to store path and baud rate configuration
    LinuxSerial(const char* path, uint32_t baud) 
        : device_path_(path), baud_rate_(baud) {}
    ~LinuxSerial() { closePort(); }

    // Fulfill the pure virtual init() from InterfaceIO without parameters
    void init() override {
        fd_ = ::open(device_path_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd_ < 0)
        {
            error = 1;
            return;
        } 
            

        struct termios tty;
        if (tcgetattr(fd_, &tty) != 0) {
            closePort();
            error = 2;
            return;
        }

        cfmakeraw(&tty);
        
        speed_t speed = B115200;
        if (baud_rate_ == 9600) speed = B9600;
        else if (baud_rate_ == 115200) speed = B115200;
        
        cfsetospeed(&tty, speed);
        cfsetispeed(&tty, speed);

        tty.c_cflag |= (CLOCAL | CREAD);
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;

        tcsetattr(fd_, TCSANOW, &tty);
        error = 0;
    }

    void closePort() {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }

    // --- Interface Implementation ---
    void write(uint8_t byte) override {
        write(&byte, 1);
    }

    uint8_t receive() override {
        uint8_t byte = 0;
        read(fd_, &byte, 1);
        return byte;
    }

    void write(uint8_t* data, uint16_t length) override {
        if (fd_ < 0 || !data) return ;
        ::write(fd_, data, length);
    }

    uint32_t receive(uint8_t* buffer) override {
        if (fd_ < 0 || !buffer) return 0;
        
        ssize_t res = ::read(fd_, buffer, 1024);
        return (res > 0) ? static_cast<uint8_t>(res) : 0;
    }

    size_t available() const {
        if (fd_ < 0) return 0;
        int bytes = 0;
        ioctl(fd_, FIONREAD, &bytes);
        return (bytes > 0) ? static_cast<size_t>(bytes) : 0;
    }

    void flush() {
        if (fd_ >= 0) tcflush(fd_, TCIOFLUSH);
    }
};

#endif

#endif /* __LINUX_SERIAL_HPP__ */
