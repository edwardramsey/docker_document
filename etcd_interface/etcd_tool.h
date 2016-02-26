#ifndef ETCD_INTERFACE__H
#define ETCD_INTERFACE__H

#include <iostream>
#include <map>
#include <curl/curl.h>
#include <sstream>
#include <stdexcept>
#include "./rapidjson/include/rapidjson/document.h"
using namespace std;

void OUT(const string& content)
{
#ifdef ODFRAME

#else
	cout << content << endl;
#endif
}

int parseJson(const string& str_json, map<string, string>& mapKv);

int parseJsonObject(const rapidjson::Value& object, map<string, string>& mapKv);

class CCurl
{
public:
	CCurl(const string& ip, const int port);
	~CCurl();

	/*
	 * @brief
	 * curl 请求函数
	 *
	 * @param
	 * key  			请求的key
	 * c_req_type  		请求类型 GET/PUT/DELETE 等
	 * option 			请求需要附加的额外参数
	 * 
	 * @return
	 * 返回http请求获得的字符串，未处理的Json串
	 */
	string curl_func(const string& key, 
			  const char* c_req_type = "GET", 
			  const string& option = "");

private:
	CURL* 					m_handler;
	string 					m_basic_url;
};

class Client
{
public:
	Client(const string& ip = "127.0.0.1", const int port = 2379)
		:m_Curl(ip, port)
	{}

	~Client()
	{}

	/*
	 * @brief
	 * Etcd Set 操作
	 * 
	 * @param
	 * ttl 若为0，则为永久节点，若不为0则为临时节点，存在时间为ttl秒
	 * 
	 * @return
	 * 设置成功返回0， 不成功返回-1
	 */
	int Set(const string& key, const string& value, int ttl = 0);

	/*
	 * @brief
	 * Etcd Get 操作
	 * 
	 * @return 
	 * 返回key对应的值，若为目录，则返回空
	 */
	string Get(const string& key);

	/*
	 * @brief
	 * Etcd 递归地列出目录下所有key，key可以不是目录
	 * 
	 * @param
	 * mapKv 结果保存在mapKv中
	 *
	 * @return
	 * 成功返回0，失败返回-1
	 */
	int ListDir(const string& key, map<string, string>& mapKv);

	/*
	 * @brief
	 * Etcd 删除节点
	 * 
	 * @param
	 * isDir 默认为false，此时无法删除目录，只能删除节点,
	 * 为true时会将目录下的都删除，但无法删除单一节点
	 * 
	 * @return
	 * 成功返回0，失败返回-1
	 */
	int Delete(const string& key);

private:
	CCurl			m_Curl;

};

class CurlException : public runtime_error 
{
public:
    CurlException(CURLcode errorCode, const string& msg)
       : runtime_error("curl exception")
	   , error_code(errorCode)
       , msg(msg)
	{}

	virtual ~CurlException() throw() {}

    virtual const char* what() const throw() {
        ostringstream estr;
        estr << msg << " [code: " << error_code << "] ";
        estr << curl_easy_strerror(error_code);
        return estr.str().c_str();
    }

private:
    CURLcode 		error_code; 
    string 			msg;
};


class ParseException : public runtime_error
{
public:
	ParseException(int code, const string& msg)
		: runtime_error("parse exception")
		, code(code)
		, msg(msg)
	{}

	virtual ~ParseException() throw() {}

	virtual const char* what() const throw()
	{
		return msg.c_str();
	}

	int 			code;
private:
	string 			msg;
};

#endif
