#pragma once
#include<queue>
template <typename T>
class Queue {
private:
	std::queue<T> container;
public:
	Queue() = default;
	void enqueue(const T& item);//thêm phần tử vào cuối hàng đợi
	void dequeue();// lấy và xóa phần tử đầu của hàng đợi
	T& front();//lấy phần tử đầu
	T& back();//lấy phần tử cuối
	bool isEmpty() const;//kiểm tra queue có trống không
	int size() const;//lấy số phần tử trong queue
};
