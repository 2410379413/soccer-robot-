#include<iostream>
using namespace std;class Complex{private:	
	int image,real;
	public:	
				Complex(int a,int b):real(a),image(b)		{		}	
				~Complex()		{		}	
				void print(void)const;
				Complex & operator-=(const Complex &q);	
				Complex & operator*=(const Complex &w);
	};
	Complex &  Complex::operator-=(const Complex &q)
	{real-=q.real;
	image-=q.image;
	return *this;	}	
	Complex & Complex::operator*=(const Complex &w)	
	{real*=w.real;	
	image*=w.image;	
	return *this;		}
	void Complex:: print(void)const	
	{cout<<"e.real is"<<real<<endl;		
	cout<<"e.image is"<<image<<endl;		}	
	int main()	
	{	Complex s(2,5),d(3,7),c(5,2);
	s-=d;
	s.print();
		d*=c;
	d.print ();
	return 0;	}
