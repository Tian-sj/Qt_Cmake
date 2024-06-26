#ifndef TIAN_LOCUFREEQUEUE_H
#define TIAN_LOCUFREEQUEUE_H

#include <atomic>
#include <memory>

template <typename T>
class LockFreeQueue
{
public:
    LockFreeQueue() : head_(new Node), tail_(head_.load()) {}
    ~LockFreeQueue()
    {
        while (Node *old_head = head_.load())
        {
            head_.store(old_head->next);
            delete old_head;
        }
    }

    LockFreeQueue(const LockFreeQueue &other) = delete;
    LockFreeQueue &operator=(const LockFreeQueue &other) = delete;

    bool is_empty() const { return head_.load() == tail_.load(); }

    void push(const T &data)
    {
        auto new_data = std::make_shared<T>(data);
        Node *p = new Node;            // 3
        Node *old_tail = tail_.load(); // 4
        old_tail->data.swap(new_data); // 5
        old_tail->next = p;            // 6
        tail_.store(p);                // 7
    }

    std::shared_ptr<T> pop()
    {
        Node *old_head = pop_head();
        if (old_head == nullptr)
        {
            return std::shared_ptr<T>();
        }

        const std::shared_ptr<T> res(old_head->data); // 2
        delete old_head;
        return res;
    }

private:
    // If the struct definition of Node is placed in the private data member
    // field where 'head_' is defined, the following compilation error will occur:
    //
    // error: 'Node' has not been declared ...
    //
    // It should be a bug of the compiler. The struct definition of Node is put in
    // front of the private member function `DeleteNodes` to eliminate this error.
    struct Node
    {
        // std::make_shared does not throw an exception.
        Node() : data(nullptr), next(nullptr) {}

        std::shared_ptr<T> data;
        Node *next;
    };

private:
    Node *pop_head()
    {
        Node *old_head = head_.load();
        if (old_head == tail_.load())
        { // 1
            return nullptr;
        }
        head_.store(old_head->next);
        return old_head;
    }

private:
    std::atomic<Node *> head_;
    std::atomic<Node *> tail_;
};

#endif // TIAN_LOCUFREEQUEUE_H