#include "FlowRpcProcessor.h"


#include <iostream>
#include <zconf.h>
#include <fstream>

int main() {
    std::cout << "Hello, World!" << std::endl;
//    std::string data ="{\"type\":0,\"table_name\":\"cv_algo_trajectory\",\"db_name\":\"lantern\",\"data\":{\"loc_x\":\"105\",\"timestamp\":1625122637500,\"sn\":\"WKA2020330000055C77\",\"traj_id\":\"1625122605093_298\",\"loc_y\":\"408\",\"image_url\":\"https://whale-cv-prod.oss-cn-hangzhou.aliyuncs.com/WOP202033000005C037/WKA2020330000055C77/image/traj/1625122605093_231_1625122637001.jpg\"}}";
//    std::cout<<data.length()<<std::endl;
//    std::string url =  "https://loggerconfig.meetwhale.com/api/v1/write";
//    std::string response;
//    HttpPost(url.c_str(), data, response, 3, 5);
//    std::cout<<response<<std::endl;


    std::string data ="{\"type\":0,\"tbname\":\"trajectory\",\"dbname\":\"lan\",\"data\":{\"x\":\"105\",\"timestamp\":1625122637500,\"sn\":\"WKA2020\",\"id\":\"298\",\"y\":\"408\",\"url\":\"001.jpg\"}}";


    FlowRpcProcessor frpc;
    std::string data1 = "{\"type\":0,\"tbname\":\"trajectory\",\"dbname\":\"lan\",\"data\":{\"x\":\"105\",\"timestamp\":1625122637500,\"sn\":\"WKA2020\",\"id\":\"298\",\"y\":\"408\",\"url\":\"001.jpg\"}}#&&#{\"type\":0,\"tbname\":\"trajectory\",\"dbname\":\"lan\",\"data\":{\"x\":\"105\",\"timestamp\":1625122637500,\"sn\":\"WKA2020\",\"id\":\"298\",\"y\":\"408\",\"url\":\"001.jpg\"}}#&&#{\"type\":0,\"tbname\":\"trajectory\",\"dbname\":\"lan\",\"data\":{\"x\":\"105\",\"timestamp\":1625122637500,\"sn\":\"WKA2020\",\"id\":\"298\",\"y\":\"408\",\"url\":\"001.jpg\"}}#&&#";
    std::cout<<data1.length()<<std::endl;




    std::thread t([&](){
        for (int idx = 0; idx < 100; ++idx){
            frpc.SendToFlow(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    std::thread t1([&](){
        frpc.run();
    });
    t.detach();
    t1.detach();

    while(true)
        sleep(10);
    sleep(500);
    frpc.sys_quit = true;


    return 0;
}
