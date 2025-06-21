
/**
 * File: thread-pool.cc
 * --------------------
 * Presents the implementation of the ThreadPool class.
 */

#include "thread-pool.h"
#include <queue>
using namespace std;

ThreadPool::ThreadPool(size_t numThreads) : wts(numThreads), done(false) {
    for (size_t i = 0; i < numThreads; i++) {
        wts[i].ts = thread([this, i] { worker(i); });
    }
    
    // inicio el thread
    dt = thread([this] { dispatcher(); });
}

void ThreadPool::schedule(const function<void(void)>& thunk) {
    if (done) {  
        throw runtime_error("no se puede asignar una tarea a un thread destruido");
    }
    if (!thunk) {  
        throw runtime_error("no se puede asignar si la función es null");
    }
    {
        lock_guard<mutex> lock(queueLock);
        tasks.push(thunk);
        // uso condition variable para notificar al dispatcher
        dispatcherCV.notify_one();
    }
    
}

void ThreadPool::wait() {
    unique_lock<mutex> lock(queueLock);
    // espero hasta que no haya tareas en la cola y todos los workers estén inactivos
    waitCV.wait(lock, [this] { 
        return tasks.empty() && activeWorkers == 0; 
    });
}

void ThreadPool::dispatcher() {
    while (true) {
        unique_lock<mutex> lock(queueLock);
        
        // espero hasta que haya trabajo disponible
        dispatcherCV.wait(lock, [this] { return !tasks.empty() || done; });
        
        if (done && tasks.empty()) {
            for (auto& worker : wts) {
                worker.workerCV.notify_one();
            }
            break;
        }
        
        if (!tasks.empty()) {
            // busco un worker disponible
            for (size_t i = 0; i < wts.size(); i++) {
                if (wts[i].available) {
                    wts[i].thunk = tasks.front();
                    tasks.pop();
                    wts[i].available = false;
                    activeWorkers++;
                    
                    wts[i].workerCV.notify_one();
                    break;
                }
            }
        }
    }
}

void ThreadPool::worker(int id) {
    while (true) {
        unique_lock<mutex> lock(queueLock);
        
        // espero
        wts[id].workerCV.wait(lock, [this, id] { 
            return wts[id].thunk != nullptr || done; 
        });
        
        if (done && wts[id].thunk == nullptr) {
            break; 
        }
        
        function<void(void)> taskToExecute = wts[id].thunk;
        wts[id].thunk = nullptr;
        
        lock.unlock();
        
        // ejecuto la tarea
        if (taskToExecute) {
            taskToExecute();
        }
        
        lock.lock();
        
        wts[id].available = true;
        activeWorkers--;
        
        if (activeWorkers == 0 && tasks.empty()) {
            waitCV.notify_all();
        }
    }
}

ThreadPool::~ThreadPool() {
    wait();
    
    {
        lock_guard<mutex> lock(queueLock);
        done = true;
    
    
        dispatcherCV.notify_all();
        for (auto& worker : wts) {
            worker.workerCV.notify_all();
        }
    }
    // espero a que todos los hilos terminen
    if (dt.joinable()) {
        dt.join();
    }
    
    for (auto& worker : wts) {
        if (worker.ts.joinable()) {
            worker.ts.join();
        }
    }
}