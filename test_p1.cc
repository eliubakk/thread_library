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
thread* servicer_t;
int joined = 0;
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
    if(*arg % 2 == 1)
        joined++;
    while(i < (int) deque_request[*arg].size()){
        while (tracks_in_q >= max_size || !vec_done[*arg]){
            cv2.wait(mutex1);
        }        
        thread::yield();
        disk_struct d1;
        d1.requester = *arg;
        d1.track = deque_request[*arg][i];
        thread::yield();
        work_queue.push_back(d1);
        thread::yield();
        mutex2.lock();
        thread::yield();
        cout << "requester " << *arg << " track " << deque_request[*arg][i] << endl;
        mutex2.unlock();
        ++total_pushed;
        thread::yield();
        ++tracks_in_q;
        cv1.signal();
        vec_done[*arg] = false;
        vec_cv[*arg]->wait(mutex1);
        ++i;
    }
    mutex1.unlock();
    cv1.signal();
    if(*arg % 2 == 1){
        servicer_t->join();
        mutex2.lock();
        cout << "joined" << endl;
        mutex2.unlock();
        mutex1.lock();
        joined--;
        mutex1.unlock();
        if(joined == 0)
            delete servicer_t;
    }
}

void disk_service(void *a){
    int request_done = 0;
    mutex1.lock();
    while (request_done != num_tracks){
        while (tracks_in_q != min(max_size, active_requester)){
            cv1.wait(mutex1); 
        }        
        thread::yield();
        int lowest = INT_MAX;
        int position = 0;
        for (int i = 0; i < (int) work_queue.size(); ++i){
            if (abs(last_track - work_queue[i].track) < lowest){
                lowest = abs(last_track - work_queue[i].track);
                position = i;
            }
        }
        thread::yield();
        disk_struct req = work_queue[position];
        work_queue.erase(work_queue.begin() + position);
        --tracks_in_q;
        cv2.signal();
        --vec_active[req.requester];
        if (vec_active[req.requester] == 0){
            --active_requester;
        }
        last_track = req.track;
        mutex2.lock();
        cout << "service requester " << req.requester << " track " << last_track << endl;
        mutex2.unlock();
        thread::yield();
        vec_done[req.requester] = true;
        vec_cv[req.requester]->signal();
        ++request_done;
    }
    mutex1.unlock();
}

void disk_main(void *a){
    int i = 0;
    while(i < (int) deque_request.size()){
        thread t1 ((thread_startfunc_t) disk_request, (void *) &vec_int[i]);
        ++i;
    }
    servicer_t = new thread((thread_startfunc_t) disk_service, (void *) 100);
}

int main(){
    max_size = 3;
    for (int i = 2; i < 6; ++i){
        deque<int>temp;
        vec_active.push_back(0);
        for(int j = 0; j < (10 - i); ++j){
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
    cpu::boot(1, (thread_startfunc_t)disk_main ,(void *)&num, 0, 0, 0);
}