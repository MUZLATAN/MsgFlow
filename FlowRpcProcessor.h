//
// Created by z on 2021/7/5.
//

#ifndef BOOSTFILEPROCESS_FLOWRPCPROCESSOR_H
#define BOOSTFILEPROCESS_FLOWRPCPROCESSOR_H


#include "boost/filesystem.hpp"

#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <fstream>
#include <time.h>
#include <sys/timeb.h>

#define  SEPARATION "#&&#"

class FlowRpcProcessor {
public:
    FlowRpcProcessor(){
        loop_times = 5;
        sys_quit = false;
        sys_data_path="/home/z/CWorkspace/BoostFileProcess/cmake-build-debug/";

        LoadFileNames(sys_data_path);

        count_successfully = 0;
        count_failed = 0;
    };
    ~FlowRpcProcessor(){
        dump();
    }

    void SendToFlow(const std::string& eventStr);

    bool SendMessage(const std::string& data);

    void MoveData();

    void Split(const std::string& data);

    void LoadFileNames(const std::string& path);

    void run();

    void dump();

    void print();
private:
    bool success_flag;
    int loop_times;
    std::string sys_data_path;
    std::queue<std::string> m_queue; //data queue
    std::queue<std::string> m_fail_queue; //failed queue
    std::mutex m_qmutex;   // data queue mutex
    std::mutex m_fqmutex;  //failed queue mutex;
    std::vector<std::pair<std::string, long int>> files;


    int count_successfully;
    int count_failed;

public:
    bool sys_quit;
};
#endif //BOOSTFILEPROCESS_FLOWRPCPROCESSOR_H
