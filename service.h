#pragma once

#include <iostream>
#include <cstring>
#include <array>
#include <random>
#include <sstream>
#include <iomanip>
#include <string>

enum class Action {
    Serve,
    ManageService,
    SetParam,
    Install,
    Unknown
};

enum class ServiceCmd {
    Start,
    Stop,
    Enable,
    Disable,
    Unknown
};

struct CliArgs {
    Action action = Action::Unknown;
    ServiceCmd service_cmd = ServiceCmd::Unknown;
    std::string instance_id;
    std::string param_name;
    std::string param_value;
};

std::string generate_guid() {
    std::array<uint8_t, 16> data{};

    // 使用随机设备生成种子，Mersenne Twister引擎生成随机数
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    // 填充16字节的随机数据
    for (auto& byte : data) {
        byte = static_cast<uint8_t>(dis(gen));
    }

    // 设置版本号为4（UUID版本4）
    data[6] = (data[6] & 0x0F) | 0x40; // 高四位设为0100
    // 设置变体标识为RFC 4122
    data[8] = (data[8] & 0x3F) | 0x80; // 高两位设为10

    // 将二进制数据转换为十六进制字符串
    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    // 按UUID格式拼接各部分
    // 第一部分：4字节 (0-3)
    for (int i = 0; i < 4; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    ss << '-';

    // 第二部分：2字节 (4-5)
    for (int i = 4; i < 6; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    ss << '-';

    // 第三部分：2字节 (6-7)
    for (int i = 6; i < 8; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    ss << '-';

    // 第四部分：2字节 (8-9)
    for (int i = 8; i < 10; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    ss << '-';

    // 第五部分：6字节 (10-15)
    for (int i = 10; i < 16; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }

    return ss.str();
}