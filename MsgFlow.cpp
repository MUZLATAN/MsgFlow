//
// Created by z on 2021/7/5.
//



#include "MsgFlow.h"

void MsgFlow::SendToFlow(const std::string& eventStr){
    std::unique_lock<std::mutex> qlock(m_qmutex, std::defer_lock);

    qlock.lock();
    {
        if (eventStr.empty()){
            qlock.unlock();
            return;
        }

        m_queue.push(eventStr);
        std::cout<<"push "<<m_queue.size()<<std::endl;
    }
    qlock.unlock();
}

void MsgFlow::run() {

    LoadFileNames("./");


    //新开一个线程, 来检查m_queue的状态
    //m_queue 为空 就向里面塞数据
    //fail_queue 数据过多, 存入磁盘
    //注意内存对齐  暂时未做
    std::thread t1([&](){
        MoveData();
    });
    t1.detach();
    std::unique_lock<std::mutex> m_qlock(m_qmutex, std::defer_lock);
    std::unique_lock<std::mutex> m_fqlock(m_fqmutex, std::defer_lock);

    //计算发送失败的次数
    int count = 0;
    int cnt = 0;
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (sys_quit) {
            dump();
            return;
        }
        //不断取数据
        if (m_queue.empty()){
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        m_qlock.lock();
        auto info = m_queue.front();
        m_queue.pop();
        m_qlock.unlock();


        while (cnt++ < loop_times ){
            //不断发送数据
            if (SendMessage(info)){
                cnt = 0;
                success_flag=true;
                std::cout<<"send successfully "<<count++<<std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        

        if (cnt >= loop_times){
            success_flag=false;
            m_fqlock.lock();
            m_fail_queue.push(info);
            cnt = 0;
            m_fqlock.unlock();
        }

        
        if (m_fail_queue.size() >= 5){
            if (files.size()> 100)
            {
                auto top = files.front();
                files.erase(files.begin());
                //删除文件
                if(::remove(top.first.c_str())!=0){
                    std::cout<<"delete file failed!"<<std::endl;
                }
            }

            long int time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
            std::string name = sys_data_path+std::to_string(time)+".data.over";
            
            files.push_back(std::pair<std::string, long int>(name, time));
            std::ofstream ofs(name, std::ios::app);
            m_fqlock.lock();
            while(m_fail_queue.size() > 0){
                ofs<<m_fail_queue.front()<<SEPARATION;
                m_fail_queue.pop();
            }
            ofs.close();
            std::cout<<"m_faile_queue size is greater than 5!!! save in file and m_faile_queue size is: "<<m_fail_queue.size()<<name;
            m_fqlock.unlock();
        }
        
    }
}
void MsgFlow::SaveDatatoFile() {
    std::unique_lock<std::mutex> m_fqlock(m_fqmutex);
    if (m_fail_queue.size() >= 5){
        if (files.size()> 100)
        {
            auto top = files.front();
            files.erase(files.begin());
            //删除文件
            if(::remove(top.first.c_str())!=0){
                std::cout<<"delete file failed!"<<std::endl;
            }
        }

        long int time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
        std::string name = sys_data_path+std::to_string(time)+".data.over";

        files.push_back(std::pair<std::string, long int>(name, time));
        std::ofstream ofs(name, std::ios::app);
        while(m_fail_queue.size() > 0){
            ofs<<m_fail_queue.front()<<SEPARATION;
            m_fail_queue.pop();
        }
        ofs.close();
        std::cout<<"m_faile_queue size is greater than 5!!! save in file and m_faile_queue size is: "<<m_fail_queue.size()<<name<<std::endl;
    }
    m_fqlock.unlock();
}
bool MsgFlow::SendMessage(const std::string& data){
    //使用回调函数
    if (callfunc(data)){
        return true;
    } else
        return false;

}
void MsgFlow::Split(const std::string& data){
    int index = 0;
    int dataidx = 0;
    while ((dataidx = data.find(SEPARATION, index)) <= data.length())
    {
        std::string str = data.substr(index, dataidx);
        m_queue.push(str);
        index = dataidx+strlen(SEPARATION);
    }
    if (data.find_last_of(SEPARATION) != (data.length()-1))
        std::cout<<"not end of "<<SEPARATION<<", the lastest record maybe wrong";

}

void MsgFlow::MoveData() {
    std::unique_lock<std::mutex> m_qlock(m_qmutex, std::defer_lock);
    std::unique_lock<std::mutex> m_fqlock(m_fqmutex, std::defer_lock);
    while (true){
        if (sys_quit)
        {
            return;
        }
        if (m_queue.empty() && !success_flag)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

        if (m_queue.empty()){
            if (!m_fail_queue.empty() && success_flag){
                m_fqlock.lock();
                std::swap(m_fail_queue, m_queue);
                m_fqlock.unlock();
            }
        }
     

        
        if (!m_fail_queue.empty() && success_flag){
            //failed queue pop;
            std::cout <<"data from failed queue lock";
            m_fqlock.lock();
            auto info = m_fail_queue.front();
            m_fail_queue.pop();
            m_fqlock.unlock();
            std::cout<<"data from failed queue ulock";
            //data queue enqueue;
            if (!info.empty()){
                std::cout<<"failed queue to data queue lock";
                m_qlock.lock();
                m_queue.push(info);
                m_qlock.unlock();
                std::cout<<"failed queue to data queue unlock";
            }
                
        }
        

        if (m_fail_queue.empty() && m_queue.empty() && success_flag){


            //读本地文件 files 按时间升序 排列 ,第一个即历史最久的文件
            if (files.size() > 0){

                auto top = files.front();
                std::ifstream in(top.first);
                std::ostringstream oss;
                oss<< in.rdbuf();
                std::string str = oss.str();
                std::cout<<str<<std::endl;
                //将str分开存入m_queuue
                m_qlock.lock();
                Split(str);
                m_qlock.unlock();
                //从维护的vector 中删除记录
                files.erase(files.begin());
                std::cout<<files.begin()->first<<std::endl;
                for (auto fileit = files.begin(); fileit != files.end(); fileit++)
                    std::cout<<fileit->first<<std::endl;

                //删除文件
                if(::remove(top.first.c_str())!=0){
                    std::cout<<"delete file failed!"<<std::endl;
                }
            }
            success_flag = false;
        }
    }
}


void MsgFlow::LoadFileNames(const std::string& path){
    boost::filesystem::path p (path);
    try{
        if (exists(p)){
            if (is_regular_file(p))
                return ;

            if (is_directory(p)){
                for (boost::filesystem::directory_entry& x : boost::filesystem::directory_iterator(p)){
                    if (is_regular_file(x.path()) && x.path().string().find(".data.over") != std::string::npos){
                        std::cout << "   " << x.path() << '\n';
                        long int t =  static_cast<long int> (boost::filesystem::last_write_time(x));
                        std::cout << "time:" << t <<std::endl;
                        files.push_back(std::pair<std::string, long int>(x.path().string(),t));
                    }
                }
            }
            else
                std::cout <<"empty directory\n";
        }
        else
            std::cout << p << " does not exist\n";

        // 将文件按时间排序
        std::sort(files.begin(), files.end(),
                  [&](std::pair<std::string, long int> a, std::pair<std::string, long int> b){
                      return a.second < b.second;});

        return ;
    }
    catch (const boost::filesystem::filesystem_error& ex)
    {
        std::cout << ex.what() << '\n';
    }
}
void MsgFlow::dump(){
    //if this processor exit
    // dump the queue's content
    long int time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
    std::string name = std::to_string(time)+".data.over";
    std::ofstream ofs(name, std::ios::app);

    if (!m_queue.empty()){
        std::cout<<"m_queue not empty!"<<std::endl;
        while(m_queue.size()> 0){
            ofs<<m_queue.front()<<SEPARATION;
            m_queue.pop();
        }
    }

    // dump the failed queue's content
    if (!m_fail_queue.empty()){
        std::cout<<"m_fail_queue not empty"<<std::endl;
        while(m_fail_queue.size() > 0){
            ofs<<m_fail_queue.front()<<SEPARATION;
            m_fail_queue.pop();
        }
    }
    ofs.close();
}
void MsgFlow::print() {
    auto p_queue = m_queue;
    while(p_queue.size() > 0)
    {
        auto it = p_queue.front();
        p_queue.pop();
        std::cout<<it<<std::endl;
    }

}
