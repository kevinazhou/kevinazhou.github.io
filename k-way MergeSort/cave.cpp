#include <iostream>
#include <stack>
#include <string>
#include <cstdlib>

using namespace std;

int main()
{
	string s;
	cin>>s;
	stack <char> st;
	for(size_t i = 0; i<s.size(); i++)
	{
		if(s[i]=='s' || s[i]=='n' || s[i]=='e' || s[i]=='w')
		{
			if(st.empty())
			{
				st.push(s[i]);
			}
			else if(abs(s[i]-st.top())==5)//s and n
			{
				st.pop();
			}
			else if(abs(s[i]-st.top())==18)//e and w
			{
				st.pop();
			}
			else
			{
				st.push(s[i]);
			}
		}
		else
		{
			cout<<"invalid"<<endl;
			return 0;
		}
	}
	if(st.empty())
	{
		cout<<"rolled up"<<endl;
	}
	else
	{
		cout<<"not rolled up"<<endl;
	}
	return 0;
}