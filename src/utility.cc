
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
            key = input;
            newKey = false;
        }
        else{
            cmdl[key] = input;
        }
    }
}



CommandlineArguments::~CommandlineArguments(){
    cmdl.clear();
}



void CommandlineArguments::Print(){

    ArgumentMap::iterator itr;
    for( itr = cmdl.begin(); itr!=cmdl.end(); itr++ ){
        cout << '\t' << itr->first << " : " << itr->second << endl;
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
        cerr << "Commandline arguments does not have Key = " << key << endl;
        return -1;
    }
}



float CommandlineArguments::GetFloat( string key ){
    if( Find(key) ){
        return stof( cmdl[key] );
    }
    else{
        cerr << "Commandline arguments does not have Key = " << key << endl;
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
        cerr << "Key " << s << " already exists. Not inserted.\n";
    }
}
