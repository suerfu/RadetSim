#include "CommandlineHandler.h"

#include <iostream>

using namespace std;

CommandlineHandler::CommandlineHandler( int argc, char* argv[] ){
    Parse( argc, argv);
}


CommandlineHandler::~CommandlineHandler(){}


void CommandlineHandler::Parse( int argc, char* argv[]){
    map.clear();

    string key;
    string tmp;

    for( unsigned int i=0; i<argc; i++){

        tmp = string( argv[i] );

        if( IsOpt(tmp) || i==0 ){
            key = tmp;
            map[key] = Parameter();
        }
        else{
            map[key].push_back(tmp);
        }
    }
}


void CommandlineHandler::Print(){

    ParameterMap::iterator itr;
    for( itr=map.begin(); itr!=map.end(); itr++ ){

        cout << itr->first << ":";
        for( Parameter::iterator itr2 = itr->second.begin(); itr2!=itr->second.end(); itr2++ ){
            cout << " " << *itr2;
        }
        cout << endl;
    }
}


bool IsOpt( string tmp ){
    if( tmp.size()>1 ){
        if( tmp[0]=='-'){
            if(tmp[1]=='-' || isalpha(tmp[1])>0 )
                return true;
        }
    }
    return false;
}


