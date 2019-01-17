#pragma once 

#ifndef __TP_HPP__
#define __TP_HPP__  

#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <queue>

using namespace std;

typedef int (*hander_t)(int);

//加减乘除
class Task 
{
  private:
    int sock;
    hander_t hander;
  public:
    Task(int sock_ = -1,hander_t h = nullptr)
      :sock(sock_)
      ,hander(h)
  {}

    void Run()
    {
      hander(sock);
    }
    ~Task()
    {}
};
class ThreadPool
{
  private:
    int thread_num;
    int idle_nums;  //空闲
    bool is_stop;
    queue<Task> t_queue;
    pthread_mutex_t lock;
    pthread_cond_t cond;

  private:
    static void* Thread_rontine(void*arg)  //在C++中要使用线程函数必须要将该函数设置为static类型的,如果不加，这里会有两个参数，但是线程函数只有一个参数
    {
      pthread_detach(pthread_self());  //没写线程等待就要将线程分离
      ThreadPool* tp = (ThreadPool*)arg; //tp == this

      while(1)
      {
       tp-> LockQueue();
        //判断当前是否有任务
        while(tp->IsEmpty())
        {
          tp->IdleThread();  //说明当前线程已经被唤醒，有任务待处理
        }
        //取任务
        Task t = tp->GetTask();

        //处理任务
        tp->UnlockQueue();
        t.Run();
        //cout<<"thread "<<pthread_self()<<" hander done..." <<"Result is "<< ret <<endl; //查看结果
      }
    }
    void NotifyOneThtread()  //唤醒线程
    {
      pthread_cond_signal(&cond);
    }
    void NotifyAllThread()
    {
      pthread_cond_broadcast(&cond);
    }
  public:
    ThreadPool(int num)
      :thread_num(num)
      ,idle_nums(0)
      ,is_stop(false)
    {}
    void LockQueue()
    {
      pthread_mutex_lock(&lock);
    }
    void UnlockQueue()
    {
      pthread_mutex_unlock(&lock);
    }
    void IdleThread()
    {
      if(is_stop)
      {
        //退出之前先把锁释放
        UnlockQueue();
        pthread_exit((void*)0); //线程退出
        cout<< "pthread "<< pthread_self() <<"quit..."<<endl;
      }
      pthread_cond_wait(&cond,&lock);
    }
    bool IsEmpty()
    {
      return t_queue.size() == 0 ? true : false;
    }
    void InitThreadPool()
    {
      pthread_mutex_init(&lock,NULL);
      pthread_cond_init(&cond,NULL);
      for(int i=0;i<thread_num;++i)
      {
        pthread_t tid;
        pthread_create(&tid,NULL,Thread_rontine,(void*)this);
      }
    }
    //生产者
    void AddTask(Task& t)
    {
      LockQueue();
      if(is_stop)
      {
        UnlockQueue();
        return;
      }
      t_queue.push(t);

      //唤醒一个线程 
      NotifyOneThtread();

      UnlockQueue();
    }
    Task GetTask()
    {
      Task t = t_queue.front();
        t_queue.pop();
      return t;
    }
    void StopThread()
    {
      LockQueue();
      is_stop = true;
      UnlockQueue();
      while(thread_num>0)
      {
        NotifyAllThread();
      }
    }
    ~ThreadPool()
    {
      pthread_mutex_destroy(&lock);
      pthread_cond_destroy(&cond);
    }

};
#endif
