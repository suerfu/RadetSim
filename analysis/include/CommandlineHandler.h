#ifndef COMMANDLINEHANDLER_H
#define COMMANDLINEHANDLER_H 1

#include <vector>
#include <map>
#include <string>

using namespace std;


// Used to tell if the specified argument is an option or parameter (of option)
//
bool IsOpt( string tmp );


typedef vector<string> Parameter;

typedef map<string, Parameter> ParameterMap;

// Class used to manage commandline arguments;
//
class CommandlineHandler{

public:

    CommandlineHandler( int argc, char* argv[] );

    ~CommandlineHandler();

    void Parse( int argc, char* argv[] );

    bool Find( string key ){ return map.find(key)!=map.end(); }
    
    vector<string> Get( string key ){ return map[key]; }

    void Print();

    int Size(){ return map.size(); }

private:

    ParameterMap map;
};


#endif
