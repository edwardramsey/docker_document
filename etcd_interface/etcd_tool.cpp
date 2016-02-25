#include "etcd_tool.h"
#include "./rapidjson/include/rapidjson/document.h"

#define    ETCD_DIR    	1
#define    ETCD_VAL		2

size_t callback_write_func(void *ptr, size_t size, size_t nmemb, void *stream) 
{
    string data((const char*) ptr, (size_t)size * nmemb);
    *(string*)stream = data;
    return size * nmemb;
}

void _checkError(CURLcode& code, const string& str_err)
{
	if(CURLE_OK != code)		
	{
		throw CurlException(code, "Failed " + str_err);
	}
}

Client::Client(const string& ip, const int port)
{
	ostringstream url;
	url << "http://" << ip << ":" << port << "/v2/keys"; 
	m_url_key = url.str();

	init_handler();
}

int Client::init()
{
	curl_global_init(CURL_GLOBAL_ALL);
	m_curlHandler = curl_easy_init();
	if(!m_curlHandler)
	{
		cout << "curl handle init error" << endl;
		return -1;
	}

	return 0;
}

Client::~Client()
{
	if(!m_curlHandler)
	{
		curl_easy_cleanup(m_curlHandler);
	}
}

string Client::Get(const string& key)
{
	try
	{
		string str_jason = curl_func(m_url_key + key, "GET");	
		map<string, string> mapKv;
		int ret = parseJason(str_jason, mapKv);
		if(ret == ETCD_VAL)
		{
			return mapKv[key];
		}
	}
	catch(const exception &e)
	{
		cout << "Get exception:" << e.what() << endl;	
	}

	return string("");
}

int Client::Set(const string& key, const string& value, int ttl)
{
	try
	{
		string buffer = "value=" + value;	
		if(ttl != 0)
		{
			ostringstream s;
			s << "&ttl=" << ttl;
			buffer += s.str(); 
		}

		curl_easy_setopt(m_curlHandler, CURLOPT_POST, 1);
		curl_easy_setopt(m_curlHandler, CURLOPT_COPYPOSTFIELDS, buffer.c_str());
		curl_func(m_url_key + key, "PUT");
	}	
	catch(const exception &e)
	{
		cout << e.what() << endl;	
		return -1;
	}

	return 0;
}

int Client::ListDir(const string& key, map<string, string>& mapKv)
{
	try
	{
		string str_jason = curl_func(m_url_key + key + "?recursive=true", "GET");	
		parseJason(str_jason, mapKv);
	}
	catch(const exception &e)
	{
		cout << e.what() << endl;	
		return -1;
	}

	return 0;
}

int Client::Delete(const string& key)
{
	try
	{
		string str_jason = curl_func(m_url_key + key, "DELETE");
	}
	catch(const exception &e)
	{
		cout << e.what() << endl;
		return -1;
	}

	return 0;
}

string Client::curl_func(const string& str_url, const char* c_type)
{
	/*
	 * curl_easy_setopt 参数:
	 *		CURLOPT_WRITEFUNCTION 	需要的回调函数
	 *		CURLOPT_WRITEDATA  		获取的结果是第四个参数中的内容
	*/
    string result;
	CURLcode code;

	code = curl_easy_setopt(m_curlHandler, CURLOPT_CUSTOMREQUEST, c_type);
	_checkError(code, "set request type");

    code = curl_easy_setopt(m_curlHandler, CURLOPT_URL, str_url.c_str());
	_checkError(code, "set url");
    
	code = curl_easy_setopt(m_curlHandler, CURLOPT_WRITEFUNCTION, &callback_write_func);
	_checkError(code, "set write callback function");

    code = curl_easy_setopt(m_curlHandler, CURLOPT_WRITEDATA, &result);
	_checkError(code, "set result target");

    code = curl_easy_perform(m_curlHandler);
	_checkError(code, "curl perform");

	return result;
}

/*
 * 解第一遍jason，将值从返回的jason串中解析出来
 */
int parseJason(const string& str_json, map<string, string>& mapKv)
{
	/* 
	 * 错误的格式
	 * {"errorCode":100,"message":"Key not found","cause":"/status/vm/31","index":564}
	 * 正确的格式
	 * {"action":"get","node":{"key":"/status/vm/6","value":"2","modifiedIndex":67,"createdIndex":67}}
	 */

	rapidjson::Document dc;	
	dc.Parse(str_json.c_str());
	if(dc.HasMember("errorCode"))
	{
		throw ParseException("request error: message" + 
				string(dc["message"].GetString()) + dc["cause"].GetString());
	}
	else
	{
		if(dc.HasMember("node"))
		{
			return parseJasonObject(dc["node"], mapKv);
		}
		else
		{
			throw ParseException("unknown exception");
		}
	}

}


/*
 * 根据获得的object判断是目录dir还是值value
 * 如果是目录，递归获得下面的值
 */
int parseJasonObject(const rapidjson::Value& object, map<string, string>& mapKv)
{
	int dir_flag = ETCD_VAL;
	if(object.HasMember("dir") && true == object["dir"].GetBool())
	{
		if(object.HasMember("nodes"))
		{
			// Value::ConstMemberIterator itObj = object.MemberBegin();
			// for ( #<{(| ;itObj != object.MemberEnd(); ++itOb |)}>#j)
			for(size_t i = 0; i < object["nodes"].Size(); ++i)
			{
				parseJasonObject(object["nodes"][i], mapKv);
			}
		}

		dir_flag = ETCD_DIR;
	}
	else
	{
		if(object.HasMember("key"))	
		{
			mapKv[object["key"].GetString()] = object["value"].GetString();
		}
		else
		{
			mapKv[object["key"].GetString()] = "";
		}
	}

	return dir_flag;
}
