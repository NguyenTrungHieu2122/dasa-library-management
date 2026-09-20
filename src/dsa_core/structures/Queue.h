
#pragma once
#include<queue>
#include<stdexcept>
template <typename T>
class Queue {
private:
	std::queue<T> q;
public:
	Queue() = default;
	void enqueue(const T& item)//thêm phần tử vào cuối hàng đợi
	{
		q.push(item);
	}
	T dequeue()// lấy và xóa phần tử đầu của hàng đợi
	{
		if (q.empty()) {
			throw std::runtime_error("Queue is empty");
		}
		T mem = q.front();
		q.pop();
		return mem;
	}
	T& front()//lấy phần tử đầu
	{
		if(q.empty()){
			throw std::runtime_error("Queue is empty");
		}
		return q.front();
	}
	T& back()//lấy phần tử cuối
	{
		if(q.empty()){
			throw std::runtime_error("Queue is empty");
		}
		return q.back();
	}
	bool isEmpty() const//kiểm tra queue có trống không
	{
		return q.empty();
	}
	int size() const//lấy số phần tử trong queue
	{
		return q.size();
	}
};

