//
// Created by z on 2021/7/5.
//

#ifndef BOOSTFILEPROCESS_MsgFlow_H
#define BOOSTFILEPROCESS_MsgFlow_H


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

class MsgFlow {
public:
    MsgFlow(std::function<bool(std::string)> func){
        loop_times = 5;
        sys_quit = false;
        count_successfully = 0;
        count_failed = 0;
        sys_data_path="./";

        LoadFileNames(sys_data_path);



        callfunc = func;



    };
    ~MsgFlow(){
        dump();
    }

    void SendToFlow(const std::string& eventStr);

    bool SendMessage(const std::string& data);

    void MoveData();

    void Split(const std::string& data);

    void LoadFileNames(const std::string& path);

    void SaveDatatoFile();

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

    std::function<bool(std::string)> callfunc;


    int count_successfully;
    int count_failed;

public:
    bool sys_quit;
};
#endif //BOOSTFILEPROCESS_MsgFlow_H
