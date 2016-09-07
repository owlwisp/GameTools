//局部返回值的一种通过bind来实现多态 c11

#include <iostream>
#include <stdio.h>


class A
{
public:

	A(int a ,int b) { 
		_a = a;
		_b = b;
		printf("%s\n", "A");
	}

	~A(){
		printf("%s\n", "~A");
	}
	
	A(const A& b) {
    	printf("A  copy\n");
    	_a = b._a;
    	_b = b._b;
    }

    virtual bool operator()(int  a){
   		if(a != _a){
   			printf("%s\n", "A-->()");
   			return false;
   		}
        return true;
    }

    virtual void log(){
    	printf("A --->%d   %d\n", _a,_b);
    }


	int _a;
	int _b;
};

class B : public A
{
public:
	B(int a, int b):A(a,b)
	{
		printf("%s\n", "B");
	}
	
	~B(){
		printf("%s\n", "~B");
	}

	B(const B& b):A(b._a,b._b) {
    	printf("B  copy\n");
    }

	bool operator()(int  a){
   		if(a != _a){
   			printf("%s\n", "B-->()");
   			return false;
   		}
        return true;
    }

    void log(){
    	printf("B --->%d   %d\n", _a,_b);
    }


};

class C : public A
{
public:


	C(int a, int b):A(a,b){
		printf("%s\n", "C");
	}

	~C(){
		printf("%s\n", "~C");
	}

	bool operator()(int  a){
   		if(a != _a){
   			printf("%s\n", "C-->()");
   			return false;
   		}
        return true;
    }

	void log(){
    	printf("C --->%d   %d\n", _a,_b);
    }

	
};

std::function<bool(int )> getB(int a,int b){
	B t(a,b);
	return std::bind(&B::operator (),t,std::placeholders::_1);
}

std::function<bool(int)> getC(int a,int b){
	C t(a,b);
	return std::bind(&C::operator (),t,std::placeholders::_1);

}

class M{
public:
	typedef std::function<bool(int)> Func;

	Func func;
};


int main(){

	M m;
	m.func = getB(3,2);
	printf("%s\n", "1 -->");
	std::function<bool(int a )> b2 = m.func;
	printf("%s\n", "2 --->");
	b2(2);
	printf("%s\n", "3 -->");

	// m.func = getC(3,2);
	// std::function<bool(int a )> b3 = m.func;
	// b3(4);
	
	return 1;
}