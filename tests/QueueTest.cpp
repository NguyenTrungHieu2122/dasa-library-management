#include "../src/dsa_core/structures/Queue.h"
#include <iostream>
#include <string>
using namespace std;

void testEnqueue(){
  cout<<"=====Test Enqueue===="<<endl;
  Queue<string> q;
  q.enqueue("M01");
  q.enqueue("M03");
  q.enqueue("M24");
  q.enqueue("M19");
  cout<<"size"<<q.size()<<endl;
  cout<<"phần tử đầu tiên"<<q.front()<<endl;
  cout<<"phần tử cuối cùng"<<q.back()<<endl;
  if(q.size()==4 and q.front()=="M01" and q.back()=="M19"){
    cout<<"PASS"<<endl;
  }
  else{
    cout<<"FAIL"<<endl;
  }
}

void testDequeue(){
  cout<<"=====Test Dequeue====="<<endl;
  Queue<string> q;
  q.enqueue("M01");
  q.enqueue("M03");
  q.enqueue("M24");
  q.enqueue("M19");
	string member=q.dequeue();
	cout<<"member được lấy ra"<<member<<endl;
	cout<<"size"<<q.size()<<endl;
	cout<<"Đứng đầu hàng đợi"<<q.front()<<endl;
	if(member=="M01" and q.size==3 and q.front()=="M03"){
		cout<<"PASS"<<endl;
	}
	else{
		cout<<"FAIL"<<endl;
	}
}

void testFrontBack()
{
    cout << "\n===== Test front/back =====\n";
    Queue<string> q;
    q.enqueue("M01");
    q.enqueue("M05");
    q.enqueue("M08");
    if (q.front() == "M01" &&
        q.back() == "M08")
    {
        cout << "front/back PASS\n";
    }
    else
    {
        cout << "front/back FAIL\n";
    }
}

void testEmpty()
{
    cout << "\n===== Test empty =====\n";
    Queue<string> q;
    if (q.isEmpty())
    {
        cout << "Empty PASS\n";
    }
    else
    {
        cout << "Empty FAIL\n";
    }
    q.enqueue("M01");
    if (!q.isEmpty())
    {
        cout << "After enqueue PASS\n";
    }
    else
    {
        cout << "After enqueue FAIL\n";
    }
}

void testDequeueEmpty()
{
    cout << "\n===== Test dequeue empty =====\n";
    Queue<string> q;
    try
    {
        q.dequeue();
        cout << "dequeue empty FAIL\n";
    }
    catch (runtime_error& e)
    {
        cout << "Exception caught: "
             << e.what()
             << endl;
        cout << "dequeue empty PASS\n";
    }
}
int main()
{
    testEnqueue();
    testDequeue();
    testFrontBack();
    testEmpty();
    testDequeueEmpty();
}
