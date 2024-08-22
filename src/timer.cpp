#include <timer.hpp>

#include <cstdio>
#include <glad/glad.h>

#include <unordered_set>
#include <unordered_map>

static std::unordered_set<int> TimersStarted;
static std::unordered_map<int, std::pair<unsigned int, bool>> GPUQueries; 
static std::unordered_map<int, std::pair<double, double>> lastTimes;

static bool ShouldDisplay = false;

Timer::Timer(const char* name)
{
    m_ID = Hash(name);
    m_Name = name;
    
    m_StartTime = std::chrono::high_resolution_clock::now();
    if(TimersStarted.find(m_ID) == TimersStarted.end()){
        glGenQueries(1, &GPUQueries[m_ID].first);
        glBeginQuery(GL_TIME_ELAPSED, GPUQueries[m_ID].first);
        TimersStarted.insert(m_ID);
        GPUQueries[m_ID].second = false;
    }
}

Timer::~Timer()
{
}

double Timer::GetMillisecondsElapsed()
{
    auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(endTime - m_StartTime).count();
}

void Timer::PrintTime()
{
    if(!GPUQueries[m_ID].second){
        glEndQuery(GL_TIME_ELAPSED);
    }

    GPUQueries[m_ID].second = true;

    GLint gpuTimeAvailable = 0;
    glGetQueryObjectiv(GPUQueries[m_ID].first, GL_QUERY_RESULT_AVAILABLE, &gpuTimeAvailable);

    GLuint64 gpuTime;
    double cpuTime;

    if(gpuTimeAvailable && ShouldDisplay){
        glGetQueryObjectui64v(GPUQueries[m_ID].first, GL_QUERY_RESULT, &gpuTime);
        cpuTime = GetMillisecondsElapsed();
        glDeleteQueries(1, &GPUQueries[m_ID].first);
        TimersStarted.erase(m_ID);

        lastTimes[m_ID] = {cpuTime, gpuTime / 1000000.0};
    }

    if(ShouldDisplay){
        if(gpuTimeAvailable){
            printf("CPU: %s took %f ms ID: %d\n", m_Name, cpuTime, m_ID);
            printf("GPU: %s took %f ms ID: %d\n", m_Name, gpuTime / 1000000.0, m_ID);
        }else{
            printf("CPU: %s took %f ms ID: %d\n", m_Name, lastTimes[m_ID].first, m_ID);
            printf("GPU: %s took %f ms ID: %d\n", m_Name, lastTimes[m_ID].second, m_ID);
        }
    }
}

void ShouldDisplayTimers(bool shouldDisplay)
{
    ShouldDisplay = shouldDisplay;
}

int Timer::Hash(const char* str)
{
    int h = 0;

    for(int i = 0; str[i] != '\0'; i++){
        h = 31 * h + str[i];
    }

    return h;
}