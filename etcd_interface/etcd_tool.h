#ifndef ETCD_INTERFACE__H
#define ETCD_INTERFACE__H

#include <iostream>
#include <map>
#include <curl/curl.h>
#include "./rapidjson/include/rapidjson/document.h"
using namespace std;

namespace etcd
{

int parseJason(const string& str_json, map<string, string>& mapKv);

int parseJasonObject(const rapidjson::Value& object, map<string, string>& mapKv);

class Client
{
public:
	Client(const string& ip = "127.0.0.1", const int port = 2379);
	~Client();

	int init();

	/*
	 * Etcd Set 操作
	 * 
	 * @param
	 * ttl 若为0，则为永久节点，若不为0则为临时节点，存在时间为ttl秒
	 */
	int Set(const string& key, const string& value, int ttl = 0);

	/*
	 * Etcd Get 操作
	 * 
	 * @return 返回key对应的值
	 * 若为目录，则返回"this is a dir"
	 */
	string Get(const string& key);

	// int Mkdir();
	
	/*
	 * Etcd 递归地列出目录下所有key
	 *
	 */
	int ListDir(const string& key, map<string, string>& mapKv);

	int Delete(const string& key);

protected:
	string curl_func(const string& strurl, const string& str_type = "");

private:
	CURL*  			m_curlHandler;
	// int 			m_port;
	// string  		m_strIp;
	string 			m_url_key;
};

class CurlException : public runtime_error 
{
public:
    CurlException(CURLcode errorCode, const string& msg)
       : runtime_error("curl exception")
	   , error_code(errorCode)
       , msg(msg)
	{
		
	}

    virtual const char* what() const throw() {
        ostringstream estr;
        estr << msg << " [code: " << error_code << "] ";
        estr << curl_easy_strerror(error_code);
        return estr.str().c_str();
    }

private:
    CURLcode error_code; 
    string msg;
};


class ParseException : public runtime_error
{
public:
	ParseException(const string& msg)
		: runtime_error("parse exception")
		, msg(msg)
	{

	}

	virtual const char* what() const throw()
	{
		return msg.c_str();
	}

private:
	string 			msg;
};

}
#endif
