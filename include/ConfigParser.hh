#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H 1

/*
 * Author: Suerfu Burkhant
 * Contact: suerfu@post.kek.jp
 * Date: 2023 Oct. 16
 * Configuration Parser
 * Used to pass parameters into the program
 * Adapted from polaris.git
*/ 

#include <string>
#include <vector>
#include <map>

#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <iomanip>

#include <ctime>
#include <sstream>

using namespace std;

//! ConfigParser reads configuration file and other commandline arguments and makes them available for each modules.
//! Internally it uses a directory-like file structure to store key-value pairs, where key is parameter name and value parameter value. This directory structure is implemented using std::map.
//!< It is possible to specify multiple parameters under the same name. 
//!< In such cases parameters are stored and accessed as vector.

class ConfigParser {

public:

    ConfigParser();
        //!< Default constructor.

    ConfigParser( const char* p);
        //!< Constructor with filename as argument. Upon creation, it reads the config file and loads parameters from file.

    ConfigParser( int argc, char* argv[]);
        //!< Constructure with commandline arguments. It parses commandline arguments and reads the config file specified with --cgf keyword. All other keywords are stored under directory "/cmdl/".

    ~ConfigParser();
        //!< Destructor.

    ConfigParser( const ConfigParser& rhs);
        //!< Copy constructor.

    ConfigParser& operator=( const ConfigParser& rhs);
        //!< Assignment operator.

    void Initialize();
        //!< Initialize function member variables.
        //!< It is created as an independent function to avoid repeating same code in different constructors.

    //======= Operations on entire structure ======================================

    //void LoadCmdl( int argc, char* argv[]);
        //!< Loads commandline parameters under directory /cmdl.

        //!< If it finds option --cfg, it will read the corresponding config file.

    int LoadFile( const string& s);
        //!< load parameters in the config file.

    void Clear();
        //!< Clear all parameters from internal storage.

    string GetQuotedString( istream& );
        //!< Get \" delimited strings from input.


    //======= Operation on individual parameters ==============================

    int AddParameter( const string& s, string t);
        //!< Add parameter. First argument string should be in the form dir/parameter.

    int RemoveParameter( const string& s);
        //!< Removes parameter in the argument from map.

    int RemoveParameter( const string& s, const string& rm);
        //!< Searches the value and remove only matched value.

    bool Find( const string& s) const;
        //!< Searches for parameter.
        //!< Returns true if found.


    // access methods for arrays.
    // string array
    vector<string> GetStrArray( const string& name ) const;
        //!< Returns parameters as vector of strings.
        //!< Empty vector if not found.

    // int
    vector<int> GetIntArray( const string& name );
        //!< Returns parameters as vector of integers.
        
        //!< Empty vector if not found.

    // float
    vector<float> GetFloatArray( const string& name );
        //!< Return multiple parameters as vector of floats.

        //!< Empty vector is returned if not found.
    
    // double
    vector<double> GetDoubleArray( const string& name );
        //!< Return multiple parameters as vector of doubles.

        //!< Empty vector is returned if not found.

    // bool
    vector<bool> GetBoolArray( const string& name ); 
        //!< Return multiple parameters as vector of boolean variables.


    // returns 0th member in the vector.
    string GetString( const string& name) const;
        //!< Returns the first element of the string vector. Emprty string if not found.

    string GetString( const string& name, string def) const;
        //!< Returns the first element of the string vector. With default value and warning message.

    int GetInt( const string& name, bool* found) const;
        //!< Returns the first element of the vector. Set second argument to false if not found.

    int GetInt( const string& name, int def) const;
        //!< Returns the first element of the vector. Set second argument to false if not found.

    float GetFloat( const string& name, bool* found); 
        //!< Returns the parameter with specified name as float. Will set found true if the key exists. Otherwise it is set as false.

    float GetFloat( const string& name, float def); 
        //!< Returns the parameter with specified name as float. If parameter is not found, default value is returned instead.

    double GetDouble( const string& name, bool* found) const; 
        //!< Returns the parameter with specified name as double. found is set true if key exists. Otherwise it is set as false.

    double GetDouble( const string& name, double def) const; 
        //!< Returns the parameter with specified name as float. If parameter is not found, default value is returned instead.

    bool GetBool( const string& name, bool* found); 
        //!< Returns the parameter with specified name as boolean variable. Will set found true if the key exists. Otherwise it is set as false.

    bool GetBool( const string& name, bool def); 
        //!< Returns the parameter with specified name as boolean variable. If parameter is not found, default value is returned instead.

    map< string, vector<string> > GetListOfParameters( const string& s);
        //!< Returns a subset of parameter trees as a map with string as key and vector of strings as value.

        //!< Function returns only parameters in the directories matching the search criteria, which compares up to the total characters in the input.

    ConfigParser GetSubParameters( const string& s);

    int size();
        //!< return size of the underlying map

    bool empty();
        //!< Returns true if the underlying storage map is empty.

    map< string, map<string, vector<string> > >::iterator begin();
        //!< Returns beginning element of parameter tree as iterator.

    map< string, map<string, vector<string> > >::iterator end();
        //!< Returns the end of parameter tree as iterator.

    map< string, map<string, vector<string> > >::reverse_iterator rbegin();
        //!< Returns beginning element of parameter tree as reverse iterator.

    map< string, map<string, vector<string> > >::reverse_iterator rend();
        //!< Returns the end of parameter tree as reverse iterator.

    map< string, map<string, vector<string> > >::const_iterator cbegin();
        //!< Returns beginning element of parameter tree as constant iterator.

    map< string, map<string, vector<string> > >::const_iterator cend();
        //!< Returns the end of parameter tree as const iterator.

    map< string, map<string, vector<string> > >::const_reverse_iterator crbegin();
        //!< Returns beginning element of parameter tree as reverse constant iterator.
    map< string, map<string, vector<string> > >::const_reverse_iterator crend();
        //!< Returns the end of parameter tree as reverse const iterator.

    void Print( ostream& os, string prefix = "");
        //!< Print contents in the parameter tree to output stream.

    string PrintToString();

private:

    vector<string> directory;
        //!< Variable to hold working directory of parameters.

    pthread_mutex_t mutex_map;
        //!< mutex to guard data structure for multithreading.

    map< string, map<string, vector<string> > > parameters;
        //!< data structure to hold the parameters.
        //!< Key of the first map is the directory name, and key of second is the parameter name.
        //!< Parameter type supports vector.

    vector< string > file_loaded;
        //!< Prevent loading same file multiple times via the @include feature.

    void RmComment(  ifstream& input, char marker = '#' );
        //!< Removes white space, lines starting with comment marker and any following white lines.

    void RmLine(  ifstream& input );
        //!< Remove one line (up to \n) and any succeeding white space characters.

    string ExtractDirectory( const string& s) const;
        //!< Given a string, it will strip the previous /../ and returns the directory

    string ExtractParameter( const string& s) const;
        //!< Given a string, it returns the parameter with no preceding directory.

    string GetCurrentDir(vector<string> s);
        //!< It will sum up all strings in the vector to form a directory

    bool str_to_bool( string s);
    
};


#endif
