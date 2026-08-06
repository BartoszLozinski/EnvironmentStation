#pragma once

#include <concepts>
#include <cstdint>
#include <functional>


template<typename T>
concept TimerConcept = requires(T t)
{
    t.Reset();
    { t.IsExpired() } -> std::convertible_to<bool>;
};

// TODO: add interfaces for timer and task instead of concepts
// TODO: support task with message buffer which is cleared periodically
// instead of allocating inside the function

template<TimerConcept TimerType>
class Task
{
protected:
    TimerType timer;
    std::function<void()> function;
public:

    Task(const uint32_t periodMs, const std::function<void()>& function_)
        : timer(TimerType{ periodMs }), function(function_)
    {}

    void Update()
    {
        if (timer.IsExpired())
        {
            function();
            timer.Reset();
        }
    }
};
