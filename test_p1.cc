#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "thread.h"
#include "mutex.h"


size_t maxRequests;
size_t activeThreads;
std::vector<std::pair<int, int>> requests;
std::vector<cv> requestCVs;
std::vector<bool> requestIsBeingServiced;
mutex requestsMutex;
cv servicerCV;

struct commandLineArgs {
    int argc;
    const char **argv;
};

struct requesterArgs {
    int requesterNum;
    const char *inputFile;
};

std::vector<requesterArgs> requestArguments;

void service(void *maxReqs) {
    int lastTrack = 0;
    
    while (true) {
        requestsMutex.lock();
        if (activeThreads == 0) {break;}
        
        while (requests.size() != std::min(activeThreads, maxRequests)) {
            servicerCV.wait(requestsMutex);
        }
        
        int distOfClosest = 1000, indexOfClosest = 0;
        for (int i = 0; i < int(requests.size()); ++i) {
            if (std::abs(lastTrack - requests[i].first) < distOfClosest) {
                distOfClosest = std::abs(lastTrack - requests[i].first);
                indexOfClosest = i;
            }
        }
        
        int requestValue = requests[indexOfClosest].first;
        int requestRequesterNum = requests[indexOfClosest].second;
        lastTrack = requestValue;
        requests.erase(requests.begin() + indexOfClosest);
        requestIsBeingServiced[requestRequesterNum] = false;
        requestCVs[requestRequesterNum].signal();
        requestsMutex.unlock();
        
        if (activeThreads == 0) {break;}
    }
}

void request(void *args) {
    requesterArgs requestArgs = *(requesterArgs *) args;
    std::string diskName(requestArgs.inputFile);
    std::ifstream inputData(diskName);
    int track;
    
    while (inputData >> track) {
        std::pair<int, int> trackAndRequester = std::make_pair(track, requestArgs.requesterNum);
        requestsMutex.lock();
        while (requests.size() >= std::min(activeThreads, maxRequests)) {
            requestCVs[requestArgs.requesterNum].wait(requestsMutex);
        }
        requests.push_back(trackAndRequester);
        requestIsBeingServiced[requestArgs.requesterNum] = true;
        
        if (requests.size() == std::min(activeThreads, maxRequests)) {
            servicerCV.signal();
        }
        
        while (requestIsBeingServiced[requestArgs.requesterNum] == true) {
            requestCVs[requestArgs.requesterNum].wait(requestsMutex);
        }
        
        requestsMutex.unlock();
    }
    
    requestsMutex.lock();
    activeThreads -= 1;
    if (requests.size() == std::min(activeThreads, maxRequests) and activeThreads != 0) {
        servicerCV.signal();
    }
    else if (activeThreads != 0) {
        for (int i = 0; i < int(requestCVs.size()); ++i) {
            requestCVs[i].signal();
        }
    }
    requestsMutex.unlock();
}


void diskScheduler(void *arguments) {
    commandLineArgs args = *(commandLineArgs *) arguments;
    activeThreads = args.argc - 2;
    
    for (int i = 2; i < args.argc; ++i) {
        requesterArgs requestArgs = {i-2, args.argv[i]};
        requestArguments.push_back(requestArgs);
        requestCVs.push_back(cv());
        requestIsBeingServiced.push_back(false);
    }
    
    for (int i = 2; i < args.argc; ++i) {
        thread requester (request, &requestArguments[i-2]);
    }
    
    thread servicer (service, &args.argc);
}

int main(int argc, const char **argv) {
    commandLineArgs arguments = {argc, argv};
    maxRequests = atoi(argv[1]);
    cpu::boot(4, diskScheduler, &arguments, 0, 1, 0);
    return 0;
}

