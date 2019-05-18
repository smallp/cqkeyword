#include "stdafx.h"
#include "work.h"
#include <filesystem>
work::work()
{
	words = vector<keyword>();
	groupIds= map<int64_t, gupStatus>();
	authIds= set<int64_t>();
	ac = -1;
}

work::~work()
{
}
void work::init()
{
	_dir = CQ_getAppDirectory(ac);
	loadConf();
}

void work::saveConf()
{
	checkDir();
	try
	{
		ofstream file(_dir + CQ_SAVE_WORDS);
		if (file.is_open())
		{
			Json::FastWriter write;
			Json::Value value;
			for each (keyword item in words)
			{
				Json::Value obj;
				obj["key"] = item.key;
				obj["content"] = item.content;
				value.append(obj);
			}
			file << write.write(value);
		}
		file.close();
	}
	catch (const std::exception& e)
	{
		CQ_addLog(ac, CQLOG_ERROR, "keywords words 配置文件保存失败", e.what());
	}
	try
	{
		ofstream file(_dir + CQ_SAVE_GUP);
		if (file.is_open())
		{
			Json::FastWriter write;
			Json::Value value;
			map<int64_t, gupStatus>::iterator it;
			for (it=groupIds.begin();it!= groupIds.end();++it)
			{
				Json::Value obj;
				obj["status"] = it->second.status;
				obj["time"] = it->second.time;
				value[to_string(it->first)] = obj;
			}
			file << write.write(value);
		}
		file.close();
	}
	catch (const std::exception& e)
	{
		CQ_addLog(ac, CQLOG_ERROR, "keywords group 配置文件保存失败", e.what());
	}
	try
	{
		ofstream file(_dir + CQ_SAVE_AUTH);
		if (file.is_open())
		{
			Json::FastWriter write;
			Json::Value value;
			for each (int64_t item in authIds)
			{
				Json::Value obj;
				obj = item;
				value.append(obj);
			}
			file << write.write(value);
		}
		file.close();
	}
	catch (const std::exception& e)
	{
		CQ_addLog(ac, CQLOG_ERROR, "keywords auth 配置文件保存失败", e.what());
	}
	
	CQ_addLog(ac, CQLOG_INFO, "keywords", "保存文件完成！");
}

void work::loadConf()
{
	ifstream file(_dir + CQ_SAVE_WORDS);
	int i = 0, size = 0;
	try
	{
		if (file.is_open())
		{
			string res;
			file >> res;
			Json::Value obj;
			Json::Reader reader;
			reader.parse(file,obj);
			for (i = 0, size = obj.size(); i < size; i++)
			{
				keyword word;
				word.content = obj[i]["content"].asString;
				word.key = obj[i]["key"].asString;
				words.push_back(word);
			}
		}
		else
		{
			CQ_addLog(ac, CQLOG_INFO, "keywords", "没有找到配置文件，使用初始配置！");
		}
	}
	catch (const std::exception& e)
	{
		CQ_addLog(ac, CQLOG_WARNING, "keywords words 配置文件加载失败", e.what());
	}
	file.close();
	file = ifstream(_dir + CQ_SAVE_GUP);
	try
	{
		if (file.is_open())
		{
			string res;
			file >> res;
			Json::Value obj;
			Json::Reader reader;
			reader.parse(file, obj);
			vector<string> members= obj.getMemberNames();
			for each (string member in members)
			{
				gupStatus group;
				group.status = obj[member]["status"].asBool;
				group.time = obj[member]["time"].asInt64;
				int64_t t = stoll(member);
				groupIds[t]=group;
			}
		}
	}
	catch (const std::exception& e)
	{
		CQ_addLog(ac, CQLOG_WARNING, "keywords group 配置文件加载失败", e.what());
	}
	file.close();
	file = ifstream(_dir + CQ_SAVE_AUTH);
	try
	{
		if (file.is_open())
		{
			string res;
			file >> res;
			Json::Value obj;
			Json::Reader reader;
			reader.parse(file, obj);
			for (i = 0, size = obj.size(); i < size; i++)
			{
				authIds.insert(obj[i].asInt64());
			}
		}
	}
	catch (const std::exception & e)
	{
		CQ_addLog(ac, CQLOG_WARNING, "keywords auth 配置文件加载失败", e.what());
	}
	file.close();
}

void work::checkDir()
{
	if (!std::tr2::sys::exists(_dir)) {
		std::tr2::sys::create_directories(_dir);
	}
}