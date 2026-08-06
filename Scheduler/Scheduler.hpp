#pragma once

#include <concepts>
#include <cstdint>


template<typename T>
concept TaskConcept = requires(T t)
{
    t.Update();
};

template<TaskConcept TaskType, std::size_t MaxTasks>
class Scheduler
{
private:
    std::array<TaskType, MaxTasks> tasks;
public:
    Scheduler(std::array<TaskType, MaxTasks> tasks_)
        : tasks(std::move(tasks_))
    {}

    void Run()
    {
        for (auto& task : tasks)
        {
            task.Update();
        }
    }
};