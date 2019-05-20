#include "stdafx.h"
#include "work.h"
#include <filesystem>
#include <regex>
#include <time.h>
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
	_qq = to_string(CQ_getLoginQQ(ac));
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
			Json::Value obj;
			Json::Reader reader;
			reader.parse(file,obj);
			for (i = 0, size = obj.size(); i < size; i++)
			{
				keyword word;
				word.content = obj[i]["content"].asString();
				word.key = obj[i]["key"].asString();
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
			Json::Value obj;
			Json::Reader reader;
			reader.parse(file, obj);
			vector<string> members= obj.getMemberNames();
			for each (string member in members)
			{
				gupStatus group;
				group.status = obj[member]["status"].asBool();
				group.time = obj[member]["time"].asInt64();
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

bool work::_gupMsg(msgFrom from)
{
	string respone;
	if (from.msg.find("?") != string::npos || from.msg.find("��") != string::npos) {
		//�ʺ�һ��������������񣬲�������
		return false;
	}
	for each (keyword item in words)
	{
		if (from.msg.find(item.key) != string::npos) {
			respone = item.content;
			break;
		}
	}
	if (respone.empty()) return false;
	if (from.type == CQ_MSG_GUP) {
		CQ_sendGroupMsg(ac, from.fromGup, respone.c_str());
	}
	else
	{
		CQ_sendDiscussMsg(ac, from.fromGup, respone.c_str());
	}
	return true;
}

bool work::_gupCmd(msgFrom from)
{
	string respone;
	if (from.msg.find("����") != string::npos && from.msg.find("�Լ�") != string::npos) {
		respone = INTRO;
	}
	else if (from.msg.find("����") != string::npos) {
		string toParse = from.msg.substr(from.msg.find("����")+4);
		regex reg1("\\d+��");
		smatch r1;
		int day, hour, min;
		regex_search(toParse, r1, reg1);
		string target = r1.str();
		if (target.empty()) {
			day = 0;
		}
		else {
			target = target.substr(0, target.length() - 2);
			day = stoi(target);
		}
		reg1 = regex("\\d+Сʱ");
		regex_search(toParse, r1, reg1);
		target = r1.str();
		if (target.empty()) {
			hour = 0;
		}
		else {
			target = target.substr(0, target.length() - 4);
			hour = stoi(target);
		}
		reg1 = regex("\\d+��");
		regex_search(toParse, r1, reg1);
		target = r1.str();
		if (target.empty()) {
			min = 0;
		}
		else {
			target = target.substr(0, target.length() - 2);
			min = stoi(target);
		}
		if ((day | hour | min) == 0) {
			groupIds[from.fromGup].status = false;
			respone = "�١���";
		}
		else
		{
			int t = time(NULL);
			respone = "�١���(����";
			if (day > 0) {
				respone.append(to_string(day)).append("��");
				t += day * 86400;
			}
			if (hour > 0) {
				respone.append(to_string(hour)).append("Сʱ");
				t += day * 3600;
			}
			if (min > 0) {
				respone.append(to_string(min)).append("����");
				t += day * 60;
			}
			respone.append(")");
			groupIds[from.fromGup].time = t;
		}
	}
	else
	{
		CQ_addLog(ac, CQLOG_DEBUG, "???", from.msg.c_str());
		respone = "����";
	}
	if (from.type == CQ_MSG_GUP) {
		CQ_sendGroupMsg(ac, from.fromGup, respone.c_str());
	}
	else
	{
		CQ_sendDiscussMsg(ac, from.fromGup, respone.c_str());
	}
	return true;
}

bool work::cmd(msgFrom from)
{
	if (authIds.find(from.fromQQ) == authIds.end() && from.fromQQ!=MINE_QQ) {
		CQ_addLog(ac, CQLOG_DEBUG, "no rights", from.msg.c_str());
		return false;
	}
	string respone;
	if (from.msg.find("��ӹؼ���")!= string::npos) {
		int end = from.msg.find(" ����");
		if (end == string::npos) {
			respone = "��ӹؼ��� ����ʧ�ܣ���ȷ�������Ƿ���ȷ��";
		}
		else
		{
			string key = from.msg.substr(10, end - 10);
			string content = from.msg.substr(end + 5);
			vector<keyword>::iterator it;
			for (it = words.begin(); it != words.end();it++)
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
	else if (from.msg.find("ɾ���ؼ���") != string::npos) {
		string key= from.msg.substr(10);
		bool isSucc = false;
		vector<keyword>::iterator it;
		for (it=words.begin(); it !=words.end();it++)
		{
			if (it->key == key) {
				it = words.erase(it);
				isSucc = true;
				break;
			}
		}
		respone = isSucc ? "ɾ���ɹ���~" : "û���ҵ�������";
	}else if (from.msg.find("�����Ȩ") != string::npos) {
		string key = from.msg.substr(8);
		try
		{
			int64_t toAdd = stoll(key);
			authIds.insert(toAdd);
			respone = "�����Ȩ�ɹ���~";
		}
		catch (const std::exception&)
		{
			CQ_addLog(ac, CQLOG_ERROR,"�����Ȩʧ��", key.c_str());
			respone = "�����Ȩʧ������ȷ��һ�������Ƿ���ȷ��~";
		}
	}
	else if (from.msg.find("ɾ����Ȩ") != string::npos) {
		string key = from.msg.substr(8);
		try
		{
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
		catch (const std::exception&)
		{
			CQ_addLog(ac, CQLOG_ERROR, "ɾ����Ȩʧ��", key.c_str());
			respone = "ɾ����Ȩʧ������ȷ��һ�������Ƿ���ȷ��~";
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
	gupStatus status;
	if (from.msg.find("�ٻ�����") == 0 || from.msg.find("�����ɶ���") == 0) {
		status.status = true;
		status.time = 0;
		groupIds[from.fromGup]=status;
		if (from.type == CQ_MSG_GUP) {
			CQ_sendGroupMsg(ac, from.fromGup, "��~");
		}
		else
		{
			CQ_sendDiscussMsg(ac, from.fromGup, "��~");
		}
		return true;
	}
	//��֤��Ҫ����
	time_t now = time(NULL);
	if (groupIds.find(from.fromGup) == groupIds.end()) {
		return false;
	}
	status = groupIds[from.fromGup];
	if (!status.status || status.time > now) {
		return false;
	}

	//��֤ͨ������ʼ����
	if (from.msg.find("����") != string::npos || from.msg.find(_qq) != string::npos) {
		//at�Լ����߿�ʼ���Լ���ʶ�������
		_gupCmd(from);
		return true;
	}
	return _gupMsg(from);
}