#include "stdafx.h"
#include "work.h"
#include <filesystem>
#include <regex>
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
		CQ_addLog(ac, CQLOG_ERROR, "keywords words �����ļ�����ʧ��", e.what());
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
		CQ_addLog(ac, CQLOG_ERROR, "keywords group �����ļ�����ʧ��", e.what());
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
		CQ_addLog(ac, CQLOG_ERROR, "keywords auth �����ļ�����ʧ��", e.what());
	}
	
	CQ_addLog(ac, CQLOG_INFO, "keywords", "�����ļ���ɣ�");
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
			CQ_addLog(ac, CQLOG_INFO, "keywords", "û���ҵ������ļ���ʹ�ó�ʼ���ã�");
		}
	}
	catch (const std::exception& e)
	{
		CQ_addLog(ac, CQLOG_WARNING, "keywords words �����ļ�����ʧ��", e.what());
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
		CQ_addLog(ac, CQLOG_WARNING, "keywords group �����ļ�����ʧ��", e.what());
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
		CQ_addLog(ac, CQLOG_WARNING, "keywords auth �����ļ�����ʧ��", e.what());
	}
	file.close();
}

void work::checkDir()
{
	if (!std::tr2::sys::exists(_dir)) {
		std::tr2::sys::create_directories(_dir);
	}
}

bool work::_cmd(string)
{
	return false;
}

bool work::_gupCmd(string)
{
	return false;
}

bool work::cmd(msgFrom from)
{
	if (authIds.find(from.fromQQ) == authIds.end()) {
		return false;
	}
	string respone;
	if (from.msg.find("��ӹؼ���")) {
		int end = from.msg.find(" ����");
		if (end < 0) {
			respone = "��ӹؼ��� ����ʧ�ܣ���ȷ�������Ƿ���ȷ��";
		}
		else
		{
			string key = from.msg.substr(5, end - 6);
			string content = from.msg.substr(end + 3);
			vector<keyword>::iterator it;
			for (it = words.begin(); it != words.end();)
			{
				if (it->key == key) {
					respone = "�˹ؼ����Ѵ�����������Ϊ"+it->content+"�������Ҫ�滻����ɾ����";
					break;
				}
			}
			if (respone.empty()) {
				keyword w;
				w.key = key;
				w.content = content;
				words.push_back(w);
				respone.append("��ӹؼ��� ").append(key).append(" �ɹ�����");
			}
		}
	}
	else if (from.msg.find("ɾ���ؼ���")) {
		string key= from.msg.substr(5);
		bool isSucc = false;
		vector<keyword>::iterator it;
		for (it=words.begin(); it !=words.end();)
		{
			if (it->key == key) {
				it = words.erase(it);
				isSucc = true;
				break;
			}
		}
		respone = isSucc ? "ɾ���ɹ���~" : "û���ҵ�������";
	}else if (from.msg.find("�����Ȩ")) {
		string key = from.msg.substr(4);
		int64_t toAdd= stoll(key);
		authIds.insert(toAdd);
		respone = "�����Ȩ�ɹ���~";
	}
	else if (from.msg.find("ɾ����Ȩ")) {
		string key = from.msg.substr(4);
		int64_t toAdd = stoll(key);
		if (toAdd == MINE_QQ) {
			respone = "���Ǹ��������������ǲ����������˵�����";
		}
		else
		{
			int i = authIds.erase(toAdd);
			respone = i > 0 ? "ɾ����Ȩ�ɹ���~" : "û���ҵ�������";
		}
	}
	if (respone.empty()) {
		return false;
	}
	else
	{
		CQ_sendPrivateMsg(ac,from.fromQQ,respone.c_str());
		return true;
	}
}

bool work::gupMsg(msgFrom from)
{
	return false;
}