#include <iostream>
#include <deque>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <climits>
#include "thread.h"
#include <algorithm>

using namespace std;

struct disk_struct{
    int requester;
    int track;
};

mutex mutex1, mutex2;
cv cv1, cv2;
int max_size;
int total_pushed = 0;
int num_tracks = 0;
int tracks_in_q = 0;
int last_track = 0;
int active_requester;
vector<deque<int>> deque_request;
vector<disk_struct>work_queue;
vector<int>vec_int;
vector<int>vec_active;
vector<cv *>vec_cv;
vector<bool>vec_done;


void disk_request(void *a){
    int *arg = (int*)a;
    int i = 0;
    mutex1.lock();
    while(i < (int) deque_request[*arg].size()){
        while (tracks_in_q >= max_size || !vec_done[*arg]){
            vec_cv[*arg]->wait(mutex1);
        }        
        disk_struct d1;
        d1.requester = *arg;
        d1.track = deque_request[*arg][i];
        work_queue.push_back(d1);
        ++total_pushed;
        ++tracks_in_q;
        vec_done[*arg] = false;
        cv1.signal();
        ++i;
    }
    mutex1.unlock();
}

void disk_service(void *a){
    int request_done = 0;
    mutex1.lock();
    while (request_done != num_tracks){
        while (tracks_in_q != min(max_size, active_requester)){
            cv1.wait(mutex1); 
        }        
        int lowest = INT_MAX;
        int position = 0;
        for (int i = 0; i < (int) work_queue.size(); ++i){
            if (abs(last_track - work_queue[i].track) < lowest){
                lowest = abs(last_track - work_queue[i].track);
                position = i;
            }
        }
        --vec_active[work_queue[position].requester];
        vec_done[work_queue[position].requester] = true;
        if (vec_active[work_queue[position].requester] == 0){
            --active_requester;
        }
        last_track = work_queue[position].track;
        vec_cv[work_queue[position].requester]->signal();

        work_queue.erase(work_queue.begin() + position);
        ++request_done;
        --tracks_in_q;
        for (int i = 0; i < (int)vec_cv.size(); ++i){
            vec_cv[i]->signal();
        }
    }
    mutex1.unlock();
}

void disk_main(void *a){
    int i = 0;
    while(i < (int) deque_request.size()){
        thread t1 ((thread_startfunc_t) disk_request, (void *) &vec_int[i]);
        ++i;
    }
    thread t1 ((thread_startfunc_t) disk_service, (void *) 100);
}

int main(){
    max_size = 3;
    for (int i = 2; i < 6; ++i){
        deque<int>temp;
        vec_active.push_back(0);
        for(int j = 0; j < 5; ++j){
            ++vec_active[i - 2];
            temp.push_back(j);
            ++num_tracks;
        }
        deque_request.push_back(temp);
        vec_int.push_back(i - 2);
        vec_done.push_back(true);
        vec_cv.push_back(new cv());
    }
    active_requester = 4;
    int num = 10;
    cpu::boot(3, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 0);
}