#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <map>
using namespace std;

int main(int argc, char* argv[])
{
std::map<string,string> mymap;
mymap["IDX"]="1234";
mymap["TEMP"]="12";


  string strInput;


std::cout << "mymap contains:\n";
  for (std::map<string,string>::iterator it=mymap.begin(); it!=mymap.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';

  mymap.clear();
  mymap["a"]="1101";
  mymap["b"]="2202";

  std::cout << "mymap contains:\n";
  for (std::map<string,string>::iterator it=mymap.begin(); it!=mymap.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';


  while( getline(cin, strInput))
  {
	cout << strInput.substr(0,3) << endl;
	if ( strInput.at(0) == '>' ) {
	cout << "DECODE FRAME:"<<strInput<<endl;;

	} else if (strInput.substr(0,3) == "20;" ) {
	cout << "DECODE RFLINK:"<<strInput<<endl;;

} else {
	cout << "PARSE:"<<strInput<<endl;;

	}
  }
 return 0;

}
