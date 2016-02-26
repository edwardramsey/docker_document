#include "etcd_tool.h"

#define    ETCD_DIR    	1
#define    ETCD_VAL		2

// curl的回调函数
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

CCurl::CCurl(const string& ip, const int port)
{
	curl_global_init(CURL_GLOBAL_ALL);
	m_handler = curl_easy_init();
	if(!m_handler)
	{
		cout << "curl handle init error" << endl;
	}

	ostringstream url;
	url << "http://" << ip << ":" << port << "/v2/keys"; 
	m_basic_url = url.str();
}

CCurl::~CCurl()
{
	if(m_handler)
	{
		curl_easy_cleanup(m_handler);
	}
}


string CCurl::curl_func(const string& key, 
					const char* c_req_type, 
					const string& option)
{
	/*
	 * curl_easy_setopt 参数:
	 *		CURLOPT_WRITEFUNCTION 	需要的回调函数
	 *		CURLOPT_WRITEDATA  		获取的结果是第四个参数中的内容
	*/
    string result;
	CURLcode code;

	if(0 == strcmp(c_req_type, "PUT"))
	{
		curl_easy_setopt(m_handler, CURLOPT_POST, 1);
		curl_easy_setopt(m_handler, CURLOPT_COPYPOSTFIELDS, option.c_str());
	}

	code = curl_easy_setopt(m_handler, CURLOPT_CUSTOMREQUEST, c_req_type);
	_checkError(code, "set request type");

    code = curl_easy_setopt(m_handler, CURLOPT_URL, (m_basic_url + key).c_str());
	_checkError(code, "set url");
    
	code = curl_easy_setopt(m_handler, CURLOPT_WRITEFUNCTION, &callback_write_func);
	_checkError(code, "set write callback function");

    code = curl_easy_setopt(m_handler, CURLOPT_WRITEDATA, &result);
	_checkError(code, "set result target");

    code = curl_easy_perform(m_handler);
	_checkError(code, "curl perform");

	return result;
}


string Client::Get(const string& key)
{
	try
	{
		string str_json = m_Curl.curl_func(key, "GET");	
		map<string, string> mapKv;
		int ret = parseJson(str_json, mapKv);
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
		string op_cmd = "value=" + value;	
		if(ttl != 0)
		{
			ostringstream s;
			s << "&ttl=" << ttl;
			op_cmd += s.str(); 
		}

		m_Curl.curl_func(key, "PUT", op_cmd);
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
		string str_json = m_Curl.curl_func(key + "?recursive=true", "GET");	
		parseJson(str_json, mapKv);
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
		string str_json = m_Curl.curl_func(key + "?recursive=true", "DELETE");
		cout << str_json << endl;
		map<string, string> mapKv;
		parseJson(str_json, mapKv);
	}
	catch(const exception &e)
	{
		cout << e.what() << endl;
		return -1;
	}

	return 0;
}



/*
 * 解第一遍json，将值从返回的json串中解析出来
 */
int parseJson(const string& str_json, map<string, string>& mapKv)
{
	/* 
	 * 错误的格式
	 * {"errorCode":100,"message":"Key not found","cause":"/status/vm/31","index":564}
	 * 正确的格式
	 * {"action":"get","node":{"key":"/status/vm/6","value":"2","modifiedIndex":67,"createdIndex":67}}
	 */

	rapidjson::Document dc;	
	dc.Parse(str_json.c_str());

	if(dc.IsObject())
	{
		if(dc.HasMember("errorCode"))
		{
			throw ParseException(dc["errorCode"].GetInt(),
					"request error: message[" + string(dc["message"].GetString()) 
					+ string("] cause:") + dc["cause"].GetString());
		}
		else
		{
			if(dc.HasMember("node") && 0 == strcmp(dc["action"].GetString(), "get"))
			{
				return parseJsonObject(dc["node"], mapKv);
			}
		}
	}
	else
	{
		throw ParseException(1, "usefuless json, content: " + str_json);
	}

}


/*
 * 根据获得的object判断是目录dir还是值value
 * 如果是目录，递归获得下面的值
 */
int parseJsonObject(const rapidjson::Value& object, map<string, string>& mapKv)
{
	int dir_flag = ETCD_VAL;
	if(object.HasMember("dir") && true == object["dir"].GetBool())
	{
		if(object.HasMember("nodes"))
		{
			for(size_t i = 0; i < object["nodes"].Size(); ++i)
			{
				parseJsonObject(object["nodes"][i], mapKv);
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
