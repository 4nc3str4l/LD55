#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <functional>

struct ScheduledTask {
    std::function<void()> function;
    float delay;
    bool toBeExecuted;
};

class Scheduler {
public:

    // Static list of tasks
    inline static std::vector<ScheduledTask> tasks;

    // Schedule a function to be called after a delay in seconds
    inline static void SetTimeout(std::function<void()> function, float delaySeconds) {
        tasks.push_back({function, delaySeconds, false});
    }

    // Update the scheduler: decrement delays and execute tasks as needed
    inline static void Update(float deltaTime) {
        for (auto& task : tasks) {
            if (!task.toBeExecuted) {
                task.delay -= deltaTime;
                if (task.delay <= 0) {
                    task.toBeExecuted = true; // Mark for execution
                }
            }
        }

        // Execute and remove all tasks marked for execution
        auto it = tasks.begin();
        while (it != tasks.end()) {
            if (it->toBeExecuted) {
                it->function();  // Execute the function
                it = tasks.erase(it);  // Remove the completed task
            } else {
                ++it;
            }
        }
    }

    // Clear all scheduled tasks
    inline static void Clear() {
        tasks.clear();
    }
};


#endif
