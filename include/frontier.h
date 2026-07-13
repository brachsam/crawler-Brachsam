#pragma once
#include <stdexcept>
#include <string>
#include "Linkedlist.h"
#include "common.h"

// Frontier hides the LinkedList's internal exceptions.
// If the Frontier is empty, it throws its own exception instead,
// so users only deal with Frontier-specific errors.

class FrontierEmptyException : public std::runtime_error{
    public:
        explicit FrontierEmptyException(const std::string& msg)
            : std::runtime_error(msg){}
};

// frontier
// Frontier is a FIFO (First In, First Out) queue used for BFS crawling.
// It stores URL and depth pairs using a SinglyLinkedList internally.
// The LinkedList is hidden from other classes, so they only use Frontier's methods.
// This file contains only the class declaration. The implementation is in Frontier.cpp.

class Frontier {
    private: 
        SinglyLinkedList<URLDepth> queue;

    public:
        Frontier();
        ~Frontier();

        // One Frontier object is used for the whole crawl.
        // Copying is not allowed to avoid bugs.
        Frontier(const Frontier&) = delete;
        Frontier& operator=(const Frontier&) = delete;

        //Adds a URL at the given depth to the back of the queue O(1)
        void enqueue(const URLDepth& item);

        // Removes and returns the URL at the front of the queue
        // Throes FrontierEmptyException if the frontier has nothing left.
        URLDepth dequeue();

        bool isEmpty() const;

        int size() const;
        
    };
