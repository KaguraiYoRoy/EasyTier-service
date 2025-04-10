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

    // ʹ������豸�������ӣ�Mersenne Twister�������������
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    // ���16�ֽڵ��������
    for (auto& byte : data) {
        byte = static_cast<uint8_t>(dis(gen));
    }

    // ���ð汾��Ϊ4��UUID�汾4��
    data[6] = (data[6] & 0x0F) | 0x40; // ����λ��Ϊ0100
    // ���ñ����ʶΪRFC 4122
    data[8] = (data[8] & 0x3F) | 0x80; // ����λ��Ϊ10

    // ������������ת��Ϊʮ�������ַ���
    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    // ��UUID��ʽƴ�Ӹ�����
    // ��һ���֣�4�ֽ� (0-3)
    for (int i = 0; i < 4; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    ss << '-';

    // �ڶ����֣�2�ֽ� (4-5)
    for (int i = 4; i < 6; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    ss << '-';

    // �������֣�2�ֽ� (6-7)
    for (int i = 6; i < 8; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    ss << '-';

    // ���Ĳ��֣�2�ֽ� (8-9)
    for (int i = 8; i < 10; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    ss << '-';

    // ���岿�֣�6�ֽ� (10-15)
    for (int i = 10; i < 16; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }

    return ss.str();
}