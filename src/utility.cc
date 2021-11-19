/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
#include "utility.hh"


string IsKey( string s){
    
    int length = s.length();

    if( s[0]!='-' ){
        return "";
    }
    else if( s[1]=='-' ){
        return s.substr(2,length);
    }
    else if( isdigit(s[1]) ){
        return "";
    }
    else{
        return s.substr(1,length);
    }
}



string FormatArgument( bool& isKey, string s ){
    string val = IsKey( s );
    if( val=="" ){
        isKey = false;
        return s;
    }
    else{
        isKey = true;
        return val;
    }
}


CommandlineArguments::CommandlineArguments( int argc, char** argv ){

    string key = argv[0];

    bool newKey = false;

    for ( int i=1; i<argc; i++){
    
        string input = FormatArgument( newKey, argv[i] );

        if( newKey==true){
            //cout << "Adding key " << input << endl;
            key = input;
            cmdl[key] = "";
            newKey = false;
        }
        else{
            cmdl[key] = input;
            //cout << "Setting value " << key << " " << input << endl;
        }
    }
}



CommandlineArguments::~CommandlineArguments(){
    cmdl.clear();
}



void CommandlineArguments::Print(){

    ArgumentMap::iterator itr;
    for( itr = cmdl.begin(); itr!=cmdl.end(); itr++ ){
        std::cout << '\t' << itr->first << " : " << itr->second << std::endl;
    }

}



bool CommandlineArguments::Find( string key){
    return cmdl.find(key)!=cmdl.end();
}



int CommandlineArguments::GetInt( string key ){
    if( Find(key) ){
        return stoi( cmdl[key] );
    }
    else{
        std::cerr << "Commandline arguments does not have Key = " << key << std::endl;
        return -1;
    }
}



float CommandlineArguments::GetFloat( string key ){
    if( Find(key) ){
        return stof( cmdl[key] );
    }
    else{
        std::cerr << "Commandline arguments does not have Key = " << key << std::endl;
        return -1;
    }
}



string CommandlineArguments::Get( string key ){
    if( Find(key) ){
        return cmdl[key];
    }
    else{
        return "";
    }
}

void CommandlineArguments::Insert( string s, string v){
    if( Find(s)==false){
        cmdl[s] = v;
    }
    else{
        std::cerr << "Key " << s << " already exists. Not inserted.\n";
    }
}
