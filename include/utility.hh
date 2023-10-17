/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

#ifndef UTILITY_H
#define UTILITY_H 1

#include <iostream>
#include <ostream>
#include <string>
#include <map>


using namespace std;


typedef std::map<string, string > ArgumentMap; 


/// This function judges if the input string is a key of commandline argument.
/// If yes, it will return the key with the preceding - or -- removed.
/// If it is instead a value (implied by the presence of preceding - or the preceding - is interpreted as a minus sign), it returns empty string.
string IsKey( string s);


string FormatArgument( bool& isKey, string s );


class CommandlineArguments{

public:

    CommandlineArguments( int argc, char** argv );

    ~CommandlineArguments();

    void Print( ostream& os = cout );

    bool Find( string );

    int GetInt( string );

    float GetFloat( string );

    string Get( string );

    void Insert( string, string);

private:

    ArgumentMap cmdl;

};



#endif
