#include"ThreadPool.h"

ThreadPool::ThreadPool(size_t threadnum):stop_(false)
{

    for(int ii=0;ii<threadnum;ii++)
    {
        threads_.emplace_back([this]{
            printf("create thread %d\n",syscall(SYS_gettid));
 
            while(stop_==false)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex>lock(mutex_);
                    this->condition_.wait(lock,[this]{
                        return (this->stop_==true)||(this->taskqueue_.empty()==false);
                    });

                    if((this->stop_==true)&&(this->taskqueue_.empty()==true)) return;

                    task = this->taskqueue_.front();
                    this->taskqueue_.pop();
                }

                printf("thread %d execute\n",syscall(SYS_gettid));
                task();
            }

        });
    }

}

void ThreadPool::addtask(std::function<void()> fn)
{
    {
        std::lock_guard<std::mutex> gd(mutex_);
        taskqueue_.push(fn);
    }
    condition_.notify_one();
}

ThreadPool::~ThreadPool()
{
    stop_=true;

    condition_.notify_all();

    for(auto &aa:threads_)
        aa.join();
}

void show(int num,std::string name)
{
    printf("我是世界第%d帅比%s\n",num,name.c_str());
}

void test()
{
    printf("cnm\n");
}
class AA
{
public:
    void showw(std::string smth)
    {
        printf("AA:%s\n",smth.c_str());
    }
};

int main()
{
    ThreadPool threadpool(3);
    std::string name = "zz";
    sleep(1);
    threadpool.addtask(std::bind(show,8,name));
    sleep(1);
    threadpool.addtask(std::bind(test));
    sleep(1);
    threadpool.addtask(std::bind([]{printf("Lambda\n");}));
    sleep(1);
    AA aa;
    threadpool.addtask(std::bind(&AA::showw,&aa,"woshishabi"));
    sleep(100);
}
// g++ -g -pthread -o threadpool ThreadPool.cpp