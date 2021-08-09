#include "FlowRpcProcessor.h"


#include <iostream>
#include <zconf.h>
#include <fstream>

bool SendMethod(const std::string& data){


    //TODO 此处定义网络发送函数


    //此处暂时定义模拟发送函数, 模拟成功失败的概率
    struct timeb timeSeed;
    std::cout<<"in SendMethod"<<std::endl;
    ftime(&timeSeed);
    srand(timeSeed.time * 1000 + timeSeed.millitm);  // milli time
    int r = rand() %11;
    if (r % 7 == 0)
        return true;

    return false;
}

int main() {

    std::string data ="{\"type\":0,\"tbname\":\"trajectory\",\"dbname\":\"lan\",\"data\":{\"x\":\"105\",\"timestamp\":1625122637500,\"sn\":\"2020\",\"id\":\"298\",\"y\":\"408\",\"url\":\"001.jpg\"}}";

    auto func = std::bind(&SendMethod, std::placeholders::_1);
    FlowRpcProcessor frpc(func);


    std::thread t([&](){
        for (int idx = 0; idx < 100; ++idx){
            frpc.SendToFlow(data);
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    std::thread t1([&](){
        frpc.run();
    });
    t.detach();
    t1.detach();

    // while(true)
        sleep(10);
    // sleep(500);
    frpc.sys_quit = true;
    sleep(10);


    return 0;
}
