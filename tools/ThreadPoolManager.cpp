// 线程池 c11

#include <functional>
#include <thread>
#include <condition_variable>
#include <future>
#include <atomic>
#include <vector>
#include <queue>


class ThreadPoolManager {
    using Task = std::function<void()>;
public:
    //成员函数
    void close();
    // 停止任务提交
    void shutdown();
    // 重启任务提交
    void restart();
//        // 提交一个任务

    // 提交一个任务
    template<class Func, class... Args>
    auto commit(Func&& func, Args&&... args) ->std::future<decltype(func(args...))> {
        if(_stop.load()){    // stop == true ??
            throw std::runtime_error("task executor have closed commit.");
        }
        using ResType =  decltype(func(args...));    // typename std::result_of<Func(Args...)>::type, 函数 func 的返回值类型
        auto task = std::make_shared<std::packaged_task<ResType()>>(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
        {    // 添加任务到队列
            std::lock_guard<std::mutex> lock {_task};
            _tasks.emplace([task](){ (*task)();}); // push(Task{...})
        }
        _cv_task.notify_one();    // 唤醒线程执行
        std::future<ResType> future = task->get_future();
        return future;
    }
private:
    //私有成员函数
    bool init();
    void initData();
    // 获取一个待执行的 task
    Task get_one_task();
    // 任务调度
    void schedual();

public:
    virtual ~ThreadPoolManager();
    
    static ThreadPoolManager* create(size_t size = 3);
    virtual const char* objectType() {return "ThreadPoolManager"; }
    
private:
    ThreadPoolManager(size_t size);
    
private:
    // 线程池
    std::vector<std::thread> _pool;
    // 任务队列
    std::queue<Task> _tasks;
    // 同步
    std::mutex _task;
    
    std::condition_variable _cv_task;
    // 是否关闭提交
    std::atomic<bool> _stop;
};
 

ThreadPoolManager* ThreadPoolManager::create(size_t size){
    ThreadPoolManager * instance = new ThreadPoolManager(size);
    
    if (instance->init())
    {
        return instance;
    }
    delete instance;
    return nullptr;
    
}

ThreadPoolManager::~ThreadPoolManager(){
    shutdown();
    
    for(std::thread& thread : _pool){
        thread.detach();    // 让线程“自生自灭”
        //thread.join();        // 等待任务结束， 前提：线程一定会执行完
    }
}

ThreadPoolManager::ThreadPoolManager(size_t size): _stop (false){
    size = size < 1 ? 1 : size;
    for(size_t i = 0; i< size; ++i){
        _pool.emplace_back(&ThreadPoolManager::schedual, this);    // push_back(std::thread{...})
    }
}

bool ThreadPoolManager::init(){
    initData();
    
    return true;
}

void ThreadPoolManager::initData(){
    
}

// 停止任务提交
void ThreadPoolManager::shutdown(){
    this->_stop.store(true);
}

// 重启任务提交
void ThreadPoolManager::restart(){
    this->_stop.store(false);
}


// 获取一个待执行的 task
ThreadPoolManager::Task ThreadPoolManager::get_one_task(){
    std::unique_lock<std::mutex> lock {_task};
    _cv_task.wait(lock, [this](){ return !_tasks.empty(); });     // wait 直到有 task
    Task task {std::move(_tasks.front())};    // 取一个 task
    _tasks.pop();
    return task;
}

// 任务调度
void ThreadPoolManager::schedual(){
    while(true){
        if(Task task = get_one_task()){
            task();    //
        }else{
            // return;    // done
        }
    }
}
    
int add(int x ,int y){
    return x + y;
}

int  main(){
    ThreadPoolManager* threadPool = ThreadPoolManager::create();

    std::function<int(int, int )> func = std::bind(add,std::placeholders::_1,std::placeholders::_2);
    std::future<int> fSum = threadPool->commit(func,1,3);
    printf("--->%d\n", fSum.get());

    delete threadPool;
    threadPool = nullptr;
}

    