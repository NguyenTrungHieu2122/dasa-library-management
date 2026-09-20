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
    cout << "\n=====Test front/back=====\n";
    Queue<string> q;
    q.enqueue("M01");
    q.enqueue("M05");
    q.enqueue("M08");
    if (q.front() == "M01" && q.back() == "M08"){
        cout << "PASS"<<endl;
    }
    else{
        cout << "FAIL"<<endl;
    }
}

void testEmpty()
{
    cout << "=====Test empty====="<<endl;
    Queue<string> q;
    if (q.isEmpty()){
        cout << "PASS"<<endl;
    }
    else{
        cout << "FAIL"<<endl;
    }
    q.enqueue("M01");
    if (!q.isEmpty()){
        cout << "PASS"<<endl;
    }
    else{
        cout << "FAIL"<<endl;
    }
}

void testDequeueEmpty()
{
    cout << "=====Test dequeue empty====="<<endl;
    Queue<string> q;
    try{
        q.dequeue();
        cout << "FAIL"<<endl;
    }
    catch (runtime_error& e){
        cout << "Exception caught: "<< e.what()<< endl;
        cout << "PASS"<<endl;
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
