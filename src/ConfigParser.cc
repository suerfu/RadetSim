#include "ConfigParser.hh"

#include <algorithm>
#include <cctype>
#include <queue>

#include <string>
#include <sstream>

#include "globals.hh"

using std::endl;
using std::string;
using std::stringstream;


// Default constructor, only initialize mutex.
ConfigParser::ConfigParser(){
    Initialize();
}


// Constructor with filename as argument. The parameters in the file specified is loaded into the file.
ConfigParser::ConfigParser( const char* filename){
    Initialize();
    LoadFile( filename);
}



// Constructor with commandline arguments. Commandlines are also treated as configuration parameters.
// The argument specified with --cfg is loaded as config file. Others loaded with directory /cmdl/
//ConfigParser::ConfigParser( int argc, char* argv[]){
//    Initialize();
//}


// Initialize function so that one does not repeat code in overloaded constructor.
void ConfigParser::Initialize(){
    directory.push_back( "/" ); // add root directory
}


// Destructor.
ConfigParser::~ConfigParser(){}


// Copy constructor
ConfigParser::ConfigParser( const ConfigParser& rhs){
    Initialize();
    parameters = rhs.parameters;
    file_loaded = rhs.file_loaded;
}


// Assignment operator
ConfigParser& ConfigParser::operator= ( const ConfigParser& rhs){
    Initialize();
    parameters = rhs.parameters;
    file_loaded = rhs.file_loaded;
    return *this;
}


// Load configuration file.
int ConfigParser::LoadFile( const string& filename ){

    // check if file opened successfully
    ifstream file( filename.c_str(), std::ios_base::in );
    if( !file.good() ){
        G4cerr << "Error: opening file "+filename << G4endl;
        parameters.clear();
    	return -1;      // signal error if file cannot be opened.
    }

    // check if loading the same file
    if( std::find( file_loaded.begin(), file_loaded.end(), filename)!= file_loaded.end() ){
        G4cerr << "Warning: attempting to load the same configuration file "+filename << G4endl;
    }
    else
        file_loaded.push_back( filename );

    string key, val;        // to hold parameter name and value

    string cur_dir = GetCurrentDir( directory );  // vector to keep track of current directory

    while(file.good()){

        // config file format is:
        //  dir{
        //      par1 : val1, par2 : val2, par3 : val3-1 val3-2 val3-3,
        //  }

	   	RmComment( file );      // remove any preceeding white lines, spaces and comments.
	    file >> key;            // key should always hold current directory.
        file >> std::ws;    // eat up white space in case format is dir + space + bracket

        // New directory begin with style " directory{ " :

        if( *key.rbegin()=='{' ){   // input starts a new directory
            key.erase( key.end()-1 );
            directory.push_back( key + "/" );
            cur_dir = GetCurrentDir( directory );
            file >> std::ws;
        }

        // New directory begin with style " directory { " :

        else if( file.peek()== '{' ){
            directory.push_back( key + "/" );
            cur_dir = GetCurrentDir( directory );
            file >> std::ws;
            file.ignore( 10, '{' );    // eat up the { character and prepare for next.
            file >> std::ws;
        }

        // New directory end:
    	else if( key=="}" ){       // if string read is }, current directory is closed, and working dir readjust to parent directory
    		directory.pop_back();
            cur_dir = GetCurrentDir( directory );
        }

        else if( key=="$include" ){     // $include can be used to add parameters of another file under current directory.
            val = GetQuotedString( file);
            // file >> val;
            LoadFile(val);
        }

        // Starting a parameter

    	else{
            if( !file )
                break;

            // input key is parameter parameter : value, parameter: value, parameter :value

            if( *key.rbegin()!=':' && *key.rbegin()!='=' ){  // if input key does not end with : or =
                char check_format;
                check_format = file.peek();

                if( check_format!= ':' && check_format!='=' ){
                    G4cerr << "Error: key " + key + " is not specified with : or = delimiter." << G4endl;
                    Clear();
                    return -1;
                }

                else{
                    file.ignore(1, check_format);
                }
            }

            while( file.good() ){
                //file >> val;    // keep loading new values.
                val = GetQuotedString( file); 
                if( val=="," )   // standalone comma, exit loop
                    break;
                else if( val=="}" ){
                    directory.pop_back();
                    break;
                }
                else{

                    bool eol = false;
                    if( *val.rbegin()==',' ){  // end with comma, remove comma
                        val.erase( val.end()-1 );
                        eol = true;
                    }

                    AddParameter( cur_dir+key, val);

                    if( eol )
                        break;
                }
            }
    	}   // at this point one line of parameter should have been added to config parser map.
    }

    if(cur_dir!="/"){
        G4cerr << "Error: parameter directory didn't close." << G4endl;
        Clear();
        return -1;
    }
    
    return parameters.size();   // return total number of directories registered.
}


// Read in a string inside a pair of quotation mark.
string ConfigParser::GetQuotedString( istream& file ){
    string val, temp;
    if( file.good() )
        file >> val;    // get initial input
    else
        return "";

    while( 1 ){         // first test for a number of termination conditions
        if( *val.begin()!='"'){      // if the word does not start with ", simply return it.
            return val;
        }

        if( *val.rbegin()=='}' )    // end of directory
            break;

        if( val.find( "\",", val.length()-2)!=string::npos && val.find( "\\\",", val.length()-3)==string::npos )
            // string ends with ", but not with \",
            break;

        if( val.find( '"', val.length()-1) != string::npos && val.find("\\\"", val.length()-2)==string::npos)
            // as long as current value does not terminate with ", keep loading words
            break;

        file >> temp;
        val += " "+temp;
    }
    return val;
}


//! Return the current directory in config file.
string ConfigParser::GetCurrentDir( vector<string> s ){
    string dir;
    for(std::vector<string>::iterator itr = s.begin(); itr!=s.end(); itr++)
        dir += *itr;
    if( *dir.begin()!='/' ) dir = "/"+dir;
    if( *dir.rbegin()!='/') dir = dir + "/";
    while( dir.find("//")!=string::npos ){
        dir.replace( dir.find("//"), 2, "/");
    }
    return dir;
}


//! Add parameter to the config parser.
int ConfigParser::AddParameter( const string& s, string val){
    string dir = ExtractDirectory(s);
    string key = ExtractParameter(s);
    (parameters[dir])[key].push_back( val);
    return 0;
}



int ConfigParser::RemoveParameter( const string& s){
    string dir = ExtractDirectory(s);
    string key = ExtractParameter(s);
    parameters[dir].erase(key);
    return 0;
}



int ConfigParser::RemoveParameter( const string& s, const string& rm){
    string dir = ExtractDirectory(s);
    string key = ExtractParameter(s);
    if( parameters[dir][key].size()==1 ){
        parameters[dir].erase(key);
    }
    else{
        vector<string>::iterator itr = (parameters[dir])[key].begin();
        for( ; itr!= (parameters[dir])[key].end(); itr++ ){
            if( *itr==rm )
                parameters[dir][key].erase( itr );
        }
    }
    return 0;
}



void ConfigParser::Clear(){
    parameters.clear();
}



vector< string > ConfigParser::GetStrArray( const string& name) const{
    string dir = ExtractDirectory(name);
    string key = ExtractParameter(name);

    vector<string> vec_str;   // vector for holding string value before casting

    auto temp = parameters.find(dir);
    if( temp != parameters.end() ){
        map< string, vector<string> >::const_iterator itr;
        for( itr = temp->second.begin(); itr!=temp->second.end(); ++itr){
            if( itr->first==key )
            vec_str = itr->second;
        }
    }

    return vec_str;
}



string ConfigParser::GetString( const string& name ) const{
    vector< string > ret = GetStrArray( name);
    if(ret.size()==0)
        return "";
    else{
        size_t first = ret[0].find_first_not_of('"');
        size_t last = ret[0].find_last_not_of('"');
        return ret[0].substr( first, last-first+1 );
    }
}



string ConfigParser::GetString( const string& name, string def ) const{
    string ret = GetString( name );
    if( ret!="" )
        return ret;
    else{
        G4cerr << "ConfigParser : cannot find "+name+", using default value "+def << G4endl;
        return def;
    }
}



vector< int > ConfigParser::GetIntArray( const string& name){
    vector<string> str = GetStrArray( name );
    vector<int> int_array;
    for( vector<string>::iterator itr = str.begin(); itr!=str.end(); ++itr){
        int_array.push_back( std::stol( *itr, 0, 0) );
    }
    return int_array;
}



vector< float > ConfigParser::GetFloatArray( const string& name){
    vector<string> str = GetStrArray( name );
    vector<float> float_array;
    for( vector<string>::iterator itr = str.begin(); itr!=str.end(); ++itr){
        float_array.push_back( std::stof(*itr) );
    }
    return float_array;
}


vector< double > ConfigParser::GetDoubleArray( const string& name){
    vector<string> str = GetStrArray( name );
    vector<double> double_array;
    for( vector<string>::iterator itr = str.begin(); itr!=str.end(); ++itr){
        double_array.push_back( std::stod(*itr) );
    }
    return double_array;
}



vector< bool > ConfigParser::GetBoolArray( const string& name){
    vector<string> str = GetStrArray( name );
    vector<bool> bool_array;
    for( vector<string>::iterator itr = str.begin(); itr!=str.end(); ++itr){
        bool_array.push_back( str_to_bool(*itr) );
    }
    return bool_array;
}



int ConfigParser::GetInt( const string& name, bool* found) const{
    string s = GetString( name);
    if( s=="" ){
        *found = false;
        return 0;
    }
    else{
        *found = true;
    }
    return std::stol( s, 0, 0 );
}



int ConfigParser::GetInt( const string& name, int def) const{
    bool found = false;
    int a = GetInt( name, &found);
    if( found )
        return a;
    else{
        return def;
    }
}



float ConfigParser::GetFloat( const string& name, bool* found){
    string s = GetString( name);
    if( s=="" ){
        *found = false;
        return 0;
    }
    else{
        *found = true;
        return stof( s );
    }
}



float ConfigParser::GetFloat( const string& name, float def){
    bool found = false;
    float a = GetFloat( name, &found);
    if( found )
        return a;
    else{
        return def;
    }
}


double ConfigParser::GetDouble( const string& name, bool* found) const{
    string s = GetString( name );
    //G4cout << "GetDouble called with " << name << ", returning " << s << G4endl;
    if( s=="" ){
        *found = false;
        return 0;
    }
    else{
        //G4cout << "GetDouble called with " << name << ", returning " << s << " " << stod(s) << G4endl;
        *found = true;
        return stod( s );
    }
}



double ConfigParser::GetDouble( const string& name, double def) const{
    bool found = false;
    double a = GetDouble( name, &found);
    if( found ){
        return a;
    }
    else{
        return def;
    }
}



bool ConfigParser::str_to_bool( string s ){
    for( string::iterator itr = s.begin(); itr!=s.end(); ++itr)
        *itr = tolower( *itr );
    if( s=="true" )
        return true;
    else
        return false;
}



bool ConfigParser::GetBool( const string& name, bool* found){
    string s = GetString( name);
    if( s=="" ){
        *found = false;
        return false;
    }
    *found = true;
    for( string::iterator itr = s.begin(); itr!=s.end(); ++itr)
        *itr = tolower( *itr );
    if( s=="true" )
        return true;
    else if( s=="false")
        return false;
    else
        G4cerr << "Warning: parameter "+name+" has invalid bool type "+s << G4endl;
    return false;
}



bool ConfigParser::GetBool( const string& name, bool def){
    bool found = false;
    bool a = GetBool( name, &found);
    if( found )
        return a;
    else{
        return def;
    }
}



bool ConfigParser::Find( const string& name) const {
    bool ret = false;
    string dir = ExtractDirectory(name);    // extracted directory as /dir/
    string key = ExtractParameter(name);    // parameter name

    auto temp = parameters.find(dir);
    if( temp==parameters.end())
        ret = false;   // didn't find the directory.
    else if( key=="" )
        ret = true;    // did not specify key, so searching for directory.
    else{
        map<string, vector<string> >::const_iterator itr;
        for( itr = temp->second.begin(); itr!=temp->second.end(); ++itr){
            if(itr->first==key)
               ret = true;
        }
    }   
    
    return ret;
}



void ConfigParser::Print( ostream& os, string prefix){
    map< string, map< string, vector<string> > >::iterator itr;
    for( itr=parameters.begin(); itr!=parameters.end(); ++itr){
        os << prefix << itr->first << endl;

        map<string, vector<string> >::iterator itr2;
        for( itr2=(itr->second).begin(); itr2!=(itr->second).end(); ++itr2){
            os << prefix << "  |- " << itr2->first << "    ";
            vector<string>::iterator itr3;
            for( itr3=itr2->second.begin(); itr3!= itr2->second.end(); ++itr3){
                os << *itr3 << ", ";
            }
            os << endl;
        }
    }
    os << endl;
    return;
}



string ConfigParser::PrintToString(){

    map< string, map< string, vector<string> > >::iterator itr;
    stringstream ss;
    for( itr=parameters.begin(); itr!=parameters.end(); ++itr){
        ss << "  "+itr->first+"\n";

        map<string, vector<string> >::iterator itr2;
        for( itr2=(itr->second).begin(); itr2!=(itr->second).end(); ++itr2){
            string params = "    |-" + itr2->first + " : ";
            vector<string>::iterator itr3;
            for( itr3=itr2->second.begin(); itr3!= itr2->second.end(); ++itr3){
                params += *itr3 + ", ";
            }
            ss << params+"\n";
        }
    }
    ss << "\n";
    return ss.str();
}



string ConfigParser::ExtractDirectory( const string& s) const{
    size_t pos = s.rfind("/");
    if(pos==string::npos)
        return "/";
            // if didn't find "/", then it is by default parameter under root.
    else
        return s.substr(0,pos+1);
}



string ConfigParser::ExtractParameter(const string& s) const{
    size_t pos = s.rfind("/");
    if(pos==string::npos)
        return s;
            // didn't find /, everything is parameter name
    else if(pos==string::npos-1)
        return "";
            // no parameter is specified, it is only directory name.
    else
        return s.substr(pos+1,string::npos);
}



//! Search the existing config parser, and return a submap that contains parameters matching the search conditions.
//! It supports only searching for a pattern from beginning.
map<string, vector<string> > ConfigParser::GetListOfParameters(const string& s){

    map<string, vector<string> > ret;
        // final map to be returned.

    map< string, map< string, vector<string> > >::iterator itr;
        // iterator to go through the current config parser.

    for( itr=parameters.begin(); itr!=parameters.end(); ++itr){
        // iterate through the first level, i.e. /module/ etc.

        map< string, vector<string> >::iterator itr2;
        for( itr2=(itr->second).begin(); itr2!=(itr->second).end(); ++itr2){
            // iterate through the second leve, i.e. /module/A, /module/B, ...

            string dir_par = itr->first + itr2->first;

            if( dir_par.compare( 0, s.size(), s)==0 ){

                size_t pos = dir_par.find("/", s.size());

                // if no more '/' is found after the matching part, directory is fully specified
                // will retrieve the corresponding parameters
                // otherwise, will return a list of matching directories with minimum depth.
                if( pos==string::npos){
                    ret[dir_par] = itr2->second;

                    //vector<string>::iterator vitr;
                    //for( vitr=itr2->second.begin(); vitr!=itr2->second.end(); ++vitr)
                    //    ret.AddParameter( dir_par, *vitr);
                }
                else{
                    //ret.AddParameter( dir_par.substr( 0, pos)+"/", "dir");
                    ret[dir_par.substr( 0, pos)+"/"].push_back("dir");
                }
            }
        }
    }

    return ret;
}



map< string, map< string, vector<string> > >::iterator ConfigParser::begin(){
    map< string, map< string, vector<string> > >::iterator itr;
    itr = parameters.begin();
    return itr;
}



map< string, map< string, vector<string> > >::iterator ConfigParser::end(){
    map< string, map< string, vector<string> > >::iterator itr;
    itr = parameters.end();
    return itr;
}



map< string, map< string, vector<string> > >::const_iterator ConfigParser::cbegin(){
    map< string, map< string, vector<string> > >::const_iterator itr;
    itr = parameters.cbegin();
    return itr;
}



map< string, map< string, vector<string> > >::const_iterator ConfigParser::cend(){
    map< string, map< string, vector<string> > >::const_iterator itr;
    itr = parameters.cend();
    return itr;
}



map< string, map< string, vector<string> > >::reverse_iterator ConfigParser::rbegin(){
    map< string, map< string, vector<string> > >::reverse_iterator itr;
    itr = parameters.rbegin();
    return itr;
}



map< string, map< string, vector<string> > >::reverse_iterator ConfigParser::rend(){
    map< string, map< string, vector<string> > >::reverse_iterator itr;
    itr = parameters.rend();
    return itr;
}



map< string, map< string, vector<string> > >::const_reverse_iterator ConfigParser::crbegin(){
    map< string, map< string, vector<string> > >::const_reverse_iterator itr;
    itr = parameters.crbegin();
    return itr;
}



map< string, map< string, vector<string> > >::const_reverse_iterator ConfigParser::crend(){
    map< string, map< string, vector<string> > >::const_reverse_iterator itr;
    itr = parameters.crend();
    return itr;
}



void ConfigParser::RmComment(  ifstream& input, char marker ){
	input>>std::ws;
        // Eat up preceeding white spaces.
	while( input.peek()==marker ){
		RmLine(input);
            // If next character is marker, then remove entire line.
	}
}



int ConfigParser::size(){
    return parameters.size();
}



bool ConfigParser::empty(){
    return parameters.empty();
}



void ConfigParser::RmLine( ifstream& input ){
	while(input.get()!='\n');
	input>>std::ws;
}


