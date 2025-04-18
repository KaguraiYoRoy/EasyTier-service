﻿// EasyTier-service.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <json/json.h>
#include <curl/curl.h>

#include "clipp.h"

#include "service.h"
#ifdef _WIN32
#include "service-windows.h"
#elif defined(__linux__)
#include "service-linux.h"
#endif

extern BlockingQueue<std::string> m_queueMsg;
extern Log service_log;

int main(int argc, char* argv[]) {
    CliArgs args;
    ServiceCmd service_cmd;
    std::string service_subcmd;

    auto serve_mode = (
        clipp::command("serve").set(args.action, Action::Serve)
        );

    auto service_commands = (
        clipp::command("start").set(service_cmd, ServiceCmd::Start) |
        clipp::command("stop").set(service_cmd, ServiceCmd::Stop) |
        clipp::command("enable").set(service_cmd, ServiceCmd::Enable) |
        clipp::command("disable").set(service_cmd, ServiceCmd::Disable),
        clipp::value("instance-id", args.instance_id)
        );

    auto set_mode = (
        clipp::command("set").set(args.action, Action::SetParam),
        clipp::value("instance-id", args.instance_id),
        clipp::value("param", args.param_name),
        clipp::value("value", args.param_value)
        );

    auto install_mode = (
        clipp::command("install").set(args.action, Action::Install),
        clipp::option("--instance-id") & clipp::value("instance-id", args.instance_id)
        );

    auto cli = (
        serve_mode |
        (clipp::command("service").set(args.action, Action::ManageService), service_commands) |
        set_mode |
        install_mode
        );

    if (!clipp::parse(argc, argv, cli)) {
        std::cout << clipp::make_man_page(cli, argv[0]) << std::endl;
        return 1;
    }

    if (args.action == Action::ManageService) {
        args.service_cmd = service_cmd;
    }

    switch (args.action) {
    case Action::Serve:

        register_service();

        while (!exit_requested.load()) {

        }
        
        clean_service();

        break;

    case Action::ManageService:
        switch (args.service_cmd) {
        case ServiceCmd::Start:

            break;
        case ServiceCmd::Stop:

            break;
        case ServiceCmd::Enable:

            break;
        case ServiceCmd::Disable:

            break;
        default:
            service_log.push(LEVEL_ERROR, "Unknown service command");
            return 1;
        }
        break;

    case Action::SetParam:
        if (args.instance_id.empty() || args.param_name.empty() || args.param_value.empty()) {
            std::cerr << "Error: " << std::endl;
            service_log.push(LEVEL_ERROR, "Missing parameters. Required: instance-id, param-name, param-value");
            return 1;
        }

        break;

    case Action::Install:
        if (args.instance_id.empty()) {
            args.instance_id = generate_guid();
            service_log.push(LEVEL_INFO, "Instance id is missing. Auto generated: %s.", args.instance_id.c_str());
        }
        break;

    default:
        service_log.push(LEVEL_ERROR, "Unknown command or invalid arguments");
        std::cout << clipp::make_man_page(cli, argv[0]) << std::endl;
        return 1;
    }

    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
