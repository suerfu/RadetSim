#include "ConfigParser.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <queue>

using std::endl;
using std::string;


// Default constructor, only initialize mutex.
ConfigParser::ConfigParser(){
    Initialize();
    last_print = 0;
}


// Constructor with filename as argument. The parameters in the file specified is loaded into the file.
ConfigParser::ConfigParser( const char* filename){
    Initialize();
    LoadFile( filename);
}



// Constructor with commandline arguments. Commandlines are also treated as configuration parameters.
// The argument specified with --cfg is loaded as config file. Others loaded with directory /cmdl/
ConfigParser::ConfigParser( int argc, char* argv[]){
    Initialize();
    LoadCmdl( argc, argv );
}



void ConfigParser::Serialize( ostream& os){

    uint32_t header = 0xcc1234cc;
        // header to indicate the following is config file and to indicate its endianess
    uint32_t len = 0;
        // length of strings to write.

    map< string, map<string, vector<string> > >::iterator itr1;
    map< string, vector<string> >::iterator itr2;
    vector< string >::iterator itr3;

    // --- calculate how many bytes of data to write ---

    uint32_t bytes = 2*sizeof( header );
    uint32_t version = 0x0;
    bytes += sizeof( version);

    for( itr1=parameters.begin(); itr1!=parameters.end(); ++itr1){

        len = (itr1->first).length();
        bytes += sizeof(len);
        bytes += len;

        len = itr1->second.size();
        bytes += sizeof(len);

        for( itr2=itr1->second.begin(); itr2!=itr1->second.end(); ++itr2){
            
            // write size of parameter string in binary form
            len = (itr2->first).length();
            bytes += sizeof(len);
            bytes += len;

            // write the number of string values to follow
            len = (itr2->second).size();
            bytes += sizeof(len);

            for( itr3=itr2->second.begin(); itr3!=itr2->second.end(); ++itr3 ){
                len = itr3->length();
                bytes += sizeof(len);
                bytes += len;
            }
        }
    }
    bytes += sizeof( header );

    // --- write actual header and the contents of the parameters

    os.write( reinterpret_cast<char*>( &header ), sizeof( header) );
    os.write( reinterpret_cast<char*>( &bytes), sizeof(bytes));
    os.write( reinterpret_cast<char*>( &version), sizeof(version));


    for( itr1=parameters.begin(); itr1!=parameters.end(); ++itr1){

        // write the size of directory string in binary
        len = (itr1->first).length();
        os.write( reinterpret_cast<char*>( &len ), sizeof( len) );

        // write the actual directory in char stream
        os.write( itr1->first.c_str(), len );

        // write number of parameters under the directory
        len = itr1->second.size();
        os.write( reinterpret_cast<char*>( &len), sizeof(len));

        for( itr2=itr1->second.begin(); itr2!=itr1->second.end(); ++itr2){
            
            // write size of parameter string in binary form
            len = (itr2->first).length();
            os.write( reinterpret_cast<char*>( &len ), sizeof( len) );

            // write the parameter name in stream
            os.write( itr2->first.c_str(), len );

            // write the number of string values to follow
            len = (itr2->second).size();
            os.write( reinterpret_cast<char*>( &len ), sizeof( len) );

            for( itr3=itr2->second.begin(); itr3!=itr2->second.end(); ++itr3 ){
                len = itr3->length();
                os.write( reinterpret_cast<char*>( &len ), sizeof( len) );
                os.write( itr3->c_str(), len );
            }
        }
    }

    os.write( reinterpret_cast<char*>( &header ), sizeof( header) );
}



void ConfigParser::Deserialize( istream& is){

    uint32_t header = 0;
        // header to indicate the following is config file
    uint32_t len = 0;
        // length of strings to read.
    uint32_t bytes = 0;
        // total number of bytes in header
    uint32_t bytes_read = 0;
        // total number of bytes read
    uint32_t version = 0;
        // version control

    is.read( reinterpret_cast<char*>( &header), sizeof(header));
    bytes_read += sizeof(header);

    if( header!=0xcc1234cc ){
        Print( "Wrong endianess\n", ERR);
        return;
    }
        //change_endian = false;

    is.read( reinterpret_cast<char*>( &bytes), sizeof(bytes));
        // total number of bytes to be extracted
    bytes_read += sizeof(bytes);

    is.read( reinterpret_cast<char*>( &version), sizeof(version));
        // total number of bytes to be extracted
    bytes_read += sizeof(version);

    uint32_t nparam, nval;

    string dir;
    while( bytes_read < bytes-sizeof(header) ){

        is.read( reinterpret_cast<char*>( &len), sizeof(len));
        bytes_read += sizeof(len);
        dir = string( len, ' ');
        is.read( &dir[0], len);
        bytes_read += len;

        is.read( reinterpret_cast<char*>( &nparam), sizeof(nparam));
        bytes_read += sizeof(nparam);

        map< string, vector< string > > subparam;
            // map to store parameter name, value-array pairs

        string param;
            // string to store parameter name
        for( unsigned int i=0; i<nparam; i++){
            is.read( reinterpret_cast<char*>( &len), sizeof(len));
            bytes_read += sizeof(len);
            param = string( len, ' ');
            is.read( &param[0], len);
            bytes_read += len;
            
            is.read( reinterpret_cast<char*>( &nval), sizeof(nval));
            bytes_read += sizeof(nval);


            vector< string > val;
                // vector to store values
            for( unsigned int j=0; j<nval; j++){
                string temp;
                is.read( reinterpret_cast<char*>( &len), sizeof(len));
                bytes_read += sizeof(len);
                temp = string( len, ' ');
                is.read( &temp[0], len);
                bytes_read += len;
                val.push_back( temp );
            }

            subparam[param] = val;
            parameters[dir] = subparam;
        }
    }

    is.read( reinterpret_cast<char*>( &header), sizeof(len));
}


// Initialize function so that one does not repeat code in overloaded constructor.
void ConfigParser::Initialize(){
    pthread_mutex_init( &mutex_map, 0);
    pthread_mutex_init( &mutex_cout, 0);
                                // initialize all mutex object
    SetVerbosity( INFO);        // default verbosity level

    directory.push_back( "/" ); // add root directory
}


// Destructor.
ConfigParser::~ConfigParser(){
    pthread_mutex_destroy( &mutex_map);
    log.close();
}


// Copy constructor
ConfigParser::ConfigParser( const ConfigParser& rhs){
    Initialize();
    verb = rhs.verb;
    parameters = rhs.parameters;
    file_loaded = rhs.file_loaded;
}


// Assignment operator
ConfigParser& ConfigParser::operator= ( const ConfigParser& rhs){
    Initialize();
    parameters = rhs.parameters;
    file_loaded = rhs.file_loaded;
    verb = rhs.verb;
    return *this;
}




void ConfigParser::LoadCmdl( int argc, char* argv[]){

    // Check for the verbosity level specified through commandline and logging.
    for( int i=1; i<argc; i++){
        if( string( argv[i] ) == "-v" || string( argv[i] ) == "--verbose" || string( argv[i] ) == "--detail" )
            SetVerbosity( DETAIL );
        if( string( argv[i] ) == "-q" || string( argv[i] ) == "--quiet" )
            SetVerbosity( ERR );
        if( string( argv[i] ) == "-g" || string( argv[i] ) == "--debug" )
            SetVerbosity( DEBUG );
        if( (i+1<argc) && ( string( argv[i] ) == "--log") ){
            Print( "Adding log file "+string(argv[i+1])+"\n", DEBUG);
            AddLog( argv[i+1] );
        }
    }

    Print( "Loading commandline arguments...\n", DEBUG);

    string cml = "/cmdl/";  // directory name to append to all commandline parameter names
    string key, value;      // used to store directory name later

    for( int i=1; i<argc; ){    // loop over all commandline arguments, excluding program name.
        key = string( argv[i]);

        // if it's a key, remove preceeding '-'
        if( *key.begin()=='-' )
            while( *key.begin()=='-' ) key.erase( key.begin() );   // remove preceeding - sign in parameter

        ++i;
        if( i< argc && argv[i][0]!='-' ){   // at least one parameter
            do{
                value = string( argv[i] );
                // if the string does not begin with ", it is a simple word.
                if( *value.begin()!='"'){
                        ++i;
                }
                else{
                    string temp = string( argv[i] );
                    ++i;
                    if( i>= argc )
                        break;
                    // keep appending string until another string ends with " but not with \".
                    while( i<argc && ( temp.find('"', temp.length())==string::npos || temp.find( "\\\"", temp.length()-1)!=string::npos ) ){
                        // did not find " or found \" at the end of string
                        value += " "+temp;
                        ++i;
                    }
                }
                Print( "Adding key-value pair: key - "+cml+key+", value - "+value+"\n", DEBUG);
                AddParameter( cml+key, value);
            } while( i< argc && argv[i][0]!='-' );
        }
        else{
            AddParameter( cml+key, "true");
            Print( "Enabling parameter "+cml+key+"\n", DEBUG);
        }

        if( (key=="cfg") || (key=="config") ){
            LoadFile( value );              // if the parameter is a config file, then load all parameters from the file too.
        }
    }
}


// Load configuration file.
int ConfigParser::LoadFile( const string& filename ){

    // check if file opened successfully
    ifstream file( filename.c_str(), std::ios_base::in );
    if( !file.good() ){
        Print( "Error: opening file "+filename, ERR);
        parameters.clear();
    	return -1;      // signal error if file cannot be opened.
    }

    // check if loading the same file
    if( std::find( file_loaded.begin(), file_loaded.end(), filename)!= file_loaded.end() ){
        Print( "Warning: attempting to load the same configuration file "+filename, ERR);
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
                    Print( "Error: key " + key + " is not specified with : or = delimiter.\n", ERR);
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
        Print( "Error: parameter directory didn't close.\n", ERR);
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

        string temp;
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
    Print( "Adding parameter "+dir+key+" : "+val+"\n", DEBUG);
    pthread_mutex_lock( &mutex_map);
        (parameters[dir])[key].push_back( val);
    pthread_mutex_unlock( &mutex_map);
    return 0;
}



int ConfigParser::RemoveParameter( const string& s){
    string dir = ExtractDirectory(s);
    string key = ExtractParameter(s);
    
    pthread_mutex_lock( &mutex_map);
        parameters[dir].erase(key);
    pthread_mutex_unlock( &mutex_map);
    return 0;
}



int ConfigParser::RemoveParameter( const string& s, const string& rm){
    string dir = ExtractDirectory(s);
    string key = ExtractParameter(s);
    if( parameters[dir][key].size()==1 ){
        pthread_mutex_lock( &mutex_map);
            parameters[dir].erase(key);
        pthread_mutex_unlock( &mutex_map);
    }
    else{
        pthread_mutex_lock( &mutex_map);
        vector<string>::iterator itr = (parameters[dir])[key].begin();
        for( ; itr!= (parameters[dir])[key].end(); itr++ ){
            if( *itr==rm )
                parameters[dir][key].erase( itr );
        }
        pthread_mutex_unlock( &mutex_map);
    }
    return 0;
}



void ConfigParser::Clear(){
    pthread_mutex_lock( &mutex_map);
    parameters.clear();
    log.close();
    pthread_mutex_unlock( &mutex_map);
}



vector< string > ConfigParser::GetStrArray( const string& name){
    string dir = ExtractDirectory(name);
    string key = ExtractParameter(name);

    vector<string> vec_str;   // vector for holding string value before casting

    pthread_mutex_lock( &mutex_map);
        if( parameters.find( dir ) != parameters.end() ){
            map< string, vector<string> >::iterator itr;
            for( itr = parameters[dir].begin(); itr!=parameters[dir].end(); ++itr){
                if(itr->first==key)
                vec_str = itr->second;
            }
        }
    pthread_mutex_unlock( &mutex_map);

    return vec_str;
}



string ConfigParser::GetString( const string& name ){
    vector< string > ret = GetStrArray( name);
    if(ret.size()==0)
        return "";
    else{
        size_t first = ret[0].find_first_not_of('"');
        size_t last = ret[0].find_last_not_of('"');
        return ret[0].substr( first, last-first+1 );
    }
}



string ConfigParser::GetString( const string& name, string def ){
    string ret = GetString( name );
    if( ret!="" )
        return ret;
    else{
        Print( "ConfigParser : cannot find "+name+", using default value "+def+"\n", ERR);
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



int ConfigParser::GetInt( const string& name, bool* found){
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



int ConfigParser::GetInt( const string& name, int def){
    bool found = false;
    int a = GetInt( name, &found);
    if( found )
        return a;
    else{
        //stringstream ss;    ss << def;
        //Print( "ConfigParser : cannot find "+name+", using default value "+ss.str()+".\n", ERR);
        return def;
    }
}



float ConfigParser::GetFloat( const string& name, bool* found){
    string s = GetString( name);
    if( s=="" ){
        *found = false;
        return 0;
    }
    else
        *found = true;
    return stof( s );
}



float ConfigParser::GetFloat( const string& name, float def){
    bool found = false;
    float a = GetFloat( name, &found);
    if( found )
        return a;
    else{
        //stringstream ss;    ss << def;
        //Print( "ConfigParser : cannot find "+name+", using default value "+ss.str()+".\n", ERR);
        return def;
    }
}


double ConfigParser::GetDouble( const string& name, bool* found){
    string s = GetString( name);
    if( s=="" ){
        *found = false;
        return 0;
    }
    else
        *found = true;
    return stod( s );
}



double ConfigParser::GetDouble( const string& name, double def){
    bool found = false;
    double a = GetDouble( name, &found);
    if( found )
        return a;
    else{
        //stringstream ss;    ss << def;
        //Print( "ConfigParser : cannot find "+name+", using default value "+ss.str()+".\n", ERR);
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
        Print( "Warning: parameter "+name+" has invalid bool type "+s+"\n", ERR);
    return false;
}



bool ConfigParser::GetBool( const string& name, bool def){
    bool found = false;
    bool a = GetBool( name, &found);
    if( found )
        return a;
    else{
        //string ss = def ? "true" : "false";
        //Print( "ConfigParser : cannot find "+name+", using default value "+ss+".\n", ERR);
        return def;
    }
}



bool ConfigParser::Find( const string& name) {
    bool ret = false;
    string dir = ExtractDirectory(name);    // extracted directory as /dir/
    string key = ExtractParameter(name);    // parameter name

    pthread_mutex_lock( &mutex_map);
    if(parameters.find(dir)==parameters.end())
        ret = false;   // didn't find the directory.
    else if( key=="" )
        ret = true;    // did not specify key, so searching for directory.
    else{
        map<string, vector<string> >::iterator itr;
        for( itr = parameters[dir].begin(); itr!=parameters[dir].end(); ++itr){
            if(itr->first==key)
               ret = true;
        }
    }   
    pthread_mutex_unlock( &mutex_map);
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



void ConfigParser::Print(){

    map< string, map< string, vector<string> > >::iterator itr;
    for( itr=parameters.begin(); itr!=parameters.end(); ++itr){
        Print( "  "+itr->first+"\n", INFO);

        map<string, vector<string> >::iterator itr2;
        for( itr2=(itr->second).begin(); itr2!=(itr->second).end(); ++itr2){
            string params = "    |-" + itr2->first + " : ";
            vector<string>::iterator itr3;
            for( itr3=itr2->second.begin(); itr3!= itr2->second.end(); ++itr3){
                params += *itr3 + ", ";
            }
            Print( params+"\n", INFO);
        }
    }
    Print( "\n", INFO);
    return;
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

    pthread_mutex_lock( &mutex_map);

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

    pthread_mutex_unlock( &mutex_map);

    return ret;
}



map< string, map< string, vector<string> > >::iterator ConfigParser::begin(){

    map< string, map< string, vector<string> > >::iterator itr;

    pthread_mutex_lock( &mutex_map);
        itr = parameters.begin();
    pthread_mutex_unlock( &mutex_map);

    return itr;
}



map< string, map< string, vector<string> > >::iterator ConfigParser::end(){

    map< string, map< string, vector<string> > >::iterator itr;

    pthread_mutex_lock( &mutex_map);
        itr = parameters.end();
    pthread_mutex_unlock( &mutex_map);

    return itr;
}



map< string, map< string, vector<string> > >::const_iterator ConfigParser::cbegin(){

    map< string, map< string, vector<string> > >::const_iterator itr;

    pthread_mutex_lock( &mutex_map);
        itr = parameters.cbegin();
    pthread_mutex_unlock( &mutex_map);

    return itr;
}



map< string, map< string, vector<string> > >::const_iterator ConfigParser::cend(){

    map< string, map< string, vector<string> > >::const_iterator itr;

    pthread_mutex_lock( &mutex_map);
        itr = parameters.cend();
    pthread_mutex_unlock( &mutex_map);

    return itr;
}



map< string, map< string, vector<string> > >::reverse_iterator ConfigParser::rbegin(){

    map< string, map< string, vector<string> > >::reverse_iterator itr;

    pthread_mutex_lock( &mutex_map);
        itr = parameters.rbegin();
    pthread_mutex_unlock( &mutex_map);

    return itr;
}



map< string, map< string, vector<string> > >::reverse_iterator ConfigParser::rend(){

    map< string, map< string, vector<string> > >::reverse_iterator itr;

    pthread_mutex_lock( &mutex_map);
        itr = parameters.rend();
    pthread_mutex_unlock( &mutex_map);

    return itr;
}



map< string, map< string, vector<string> > >::const_reverse_iterator ConfigParser::crbegin(){

    map< string, map< string, vector<string> > >::const_reverse_iterator itr;

    pthread_mutex_lock( &mutex_map);
        itr = parameters.crbegin();
    pthread_mutex_unlock( &mutex_map);

    return itr;
}



map< string, map< string, vector<string> > >::const_reverse_iterator ConfigParser::crend(){

    map< string, map< string, vector<string> > >::const_reverse_iterator itr;

    pthread_mutex_lock( &mutex_map);
        itr = parameters.crend();
    pthread_mutex_unlock( &mutex_map);

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



void ConfigParser::AddLog( char* s ){
    if( log )
        log.close();
    Print( "Creating log file "+string(s)+"\n", INFO );
    log.open( s, ios::out );
}



void* GetInput( void* a){
    queue<string>* fifo = reinterpret_cast< queue<string>* >(a);
    string tmp;
    while( cin>>tmp ){
        if( cin.good() )
            fifo->push( tmp );
        else{
            cin.ignore( 1024, '\n');
            cin.clear();
        }
    }
    return 0;
}



string getstr(){
    static bool init = false;
    static queue<string> fifo;
    //static pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER;
    if( init==false ){
        pthread_t thread_root;
        pthread_create( &thread_root, 0, GetInput, &fifo);
        init = true;
    }   
    if( fifo.empty() )
        return ""; 
    else{
        string return_val = fifo.front();
        fifo.pop();
        return return_val;
    }   
}



void ConfigParser::print_timestamp( ostream& os, const time_t* ct ){
    tm* tm_info = localtime( ct );
    os << std::setfill('0') << std::setw(2) << tm_info->tm_hour <<":" 
        << std::setfill('0') << std::setw(2) << tm_info->tm_min << ":"
        << std::setfill('0') << std::setw(2) << tm_info->tm_sec << "  "
        << std::setfill(' ') << std::setw(0);
}

