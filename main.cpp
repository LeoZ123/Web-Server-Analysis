#include <stdio.h>
#include <string.h>
#include <cmath>
#include "ece150-proj3-provided.h"

using namespace std;

struct queueEntry {
	float arriveTime;
    float size;
	unsigned priority;
    queueEntry *next;
};

class uriQueue {
    public:
	queueEntry *front;
	unsigned sz;

	uriQueue();
	void enqueue(unsigned,unsigned,float);
	queueEntry* dequeue();
	unsigned size();
};

uriQueue::uriQueue() {
    this->front = 0;
    this->sz = 0;
}

void uriQueue::enqueue(unsigned s, unsigned p, float at) {
	queueEntry *newqentry = new queueEntry;
	newqentry -> size = s;
	newqentry -> priority = p;
	newqentry -> arriveTime = at;
	
	if(!front || newqentry -> priority > front -> priority){
		newqentry -> next = front;
		front = newqentry;
	}
	else {
		queueEntry *i = front;
		for(; i->next; i = i->next) {
			queueEntry *j = i->next;
			if(newqentry -> priority > front -> priority){
				break;
			}
		}
		newqentry -> next = i->next;
		i -> next = newqentry;
	}
    sz++;
}

queueEntry* uriQueue::dequeue() {
	queueEntry *result;
	result = front;
	front = front -> next;
	sz--;
	return result;
}

unsigned uriQueue::size() {
    return this->sz;
}

float timeStampToFloat(timestamp a){
	float result=a.hh*60*60+a.mm*60+a.ss;
	return result;
}

timestamp floatToTimeStamp(float a){
	timestamp result;
	result.hh=a/3600;
	a-=result.hh*3600;
	result.mm=a/60;
	a-=result.mm*60;
	result.ss=ceil(a);
	return result;
}

timestamp maxClientDelay(uri *uriFront, request *requestFront){
	float maxDelay = 0;
	float currentTime = 0;
	uriQueue myQueue = uriQueue();
	while (currentTime < 86400 || myQueue.front->next) {
		request *i = requestFront;
		while (i) {
			if (timeStampToFloat(i->t)<=currentTime){
				for (uri *j = uriFront; j; j = j -> next) {
					if (strcmp(i->name,j->name)==0){
						myQueue.enqueue(j->size,j->priority,timeStampToFloat(i->t));
						break;
					}
				}
				requestFront = i -> next;
			}
			i = i -> next;
		}
		
		queueEntry *tmp;
		if (myQueue.size() >= 1){
			tmp = myQueue.dequeue();
			if (maxDelay < currentTime - tmp -> arriveTime){
				maxDelay = currentTime - tmp -> arriveTime;
			}
			currentTime += tmp -> size * 0.8;
		}
		else if (requestFront){
			currentTime = timeStampToFloat(requestFront -> t);
		}
		else{
			break;
		}
    }
    return floatToTimeStamp(maxDelay);
}