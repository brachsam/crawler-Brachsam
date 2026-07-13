#include "frontier.h"

//Default constructor
Frontier::Frontier() = default;
//Default destructor
Frontier::~Frontier() = default;

// Adds a new URLDepth object to the rear of the frontier.
// Since the frontier follows FIFO order, new URL's are always inserted at the back of the queue.
void Frontier::enqueue(const URLDepth& item){
    queue.insertBack(item);
}

// Removes and returns the URLDepth object at the front of the frontier.
// Throws an exception if the frontier is empty.
URLDepth Frontier::dequeue(){
    if (isEmpty()) {
        throw FrontierEmptyException("Frontier::dequeue - frontier is empty");
    }
    URLDepth front = queue.get(0);
    queue.deleteFront();
    return front;
}

// Checks whether the frontier contains any URLs.
// Returns true if the queue is empty, otherwise false.
bool Frontier::isEmpty() const{
    return queue.size()==0;
}

// Returns the total number of URLs currently waiting
// to be crawled in the frontier.
int Frontier::size()const{
    return queue.size();
}