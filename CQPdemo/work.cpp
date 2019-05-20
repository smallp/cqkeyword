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
		CQ_addLog(ac, CQLOG_WARNING, "keywords group 配置文件加载失败", e.what());
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

bool work::_gupMsg(msgFrom from)
{
	string respone;
	if (from.msg.find("?") != string::npos || from.msg.find("？") != string::npos) {
		//问号一般代表有正常事务，不作处理
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
	if (from.msg.find("介绍") != string::npos && from.msg.find("自己") != string::npos) {
		respone = INTRO;
	}
	else if (from.msg.find("禁言") != string::npos) {
		string toParse = from.msg.substr(from.msg.find("禁言")+4);
		regex reg1("\\d+天");
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
		reg1 = regex("\\d+小时");
		regex_search(toParse, r1, reg1);
		target = r1.str();
		if (target.empty()) {
			hour = 0;
		}
		else {
			target = target.substr(0, target.length() - 4);
			hour = stoi(target);
		}
		reg1 = regex("\\d+分");
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
			respone = "哔——";
		}
		else
		{
			int t = time(NULL);
			respone = "哔——(持续";
			if (day > 0) {
				respone.append(to_string(day)).append("天");
				t += day * 86400;
			}
			if (hour > 0) {
				respone.append(to_string(hour)).append("小时");
				t += day * 3600;
			}
			if (min > 0) {
				respone.append(to_string(min)).append("分钟");
				t += day * 60;
			}
			respone.append(")");
			groupIds[from.fromGup].time = t;
		}
	}
	else
	{
		CQ_addLog(ac, CQLOG_DEBUG, "???", from.msg.c_str());
		respone = "喵？";
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
	if (from.msg.find("添加关键字")!= string::npos) {
		int end = from.msg.find(" 内容");
		if (end == string::npos) {
			respone = "添加关键字 解析失败，请确认命令是否正确喵";
		}
		else
		{
			string key = from.msg.substr(10, end - 10);
			string content = from.msg.substr(end + 5);
			vector<keyword>::iterator it;
			for (it = words.begin(); it != words.end();it++)
			{
				if (it->key == key) {
					respone = "此关键字已存在喵，内容为"+it->content+"，如果需要替换请先删除喵";
					break;
				}
			}
			if (respone.empty()) {
				keyword w;
				w.key = key;
				w.content = content;
				words.push_back(w);
				respone.append("添加关键字 ").append(key).append(" 成功了喵");
			}
		}
	}
	else if (from.msg.find("删除关键字") != string::npos) {
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
		respone = isSucc ? "删除成功喵~" : "没有找到喵……";
	}else if (from.msg.find("添加授权") != string::npos) {
		string key = from.msg.substr(8);
		try
		{
			int64_t toAdd = stoll(key);
			authIds.insert(toAdd);
			respone = "添加授权成功喵~";
		}
		catch (const std::exception&)
		{
			CQ_addLog(ac, CQLOG_ERROR,"添加授权失败", key.c_str());
			respone = "添加授权失败喵，确认一下输入是否正确喵~";
		}
	}
	else if (from.msg.find("删除授权") != string::npos) {
		string key = from.msg.substr(8);
		try
		{
			int64_t toAdd = stoll(key);
			if (toAdd == MINE_QQ) {
				respone = "你是个坏人喵！豆豆是不会抛弃主人的喵！";
			}
			else
			{
				int i = authIds.erase(toAdd);
				respone = i > 0 ? "删除授权成功喵~" : "没有找到喵……";
			}
		}
		catch (const std::exception&)
		{
			CQ_addLog(ac, CQLOG_ERROR, "删除授权失败", key.c_str());
			respone = "删除授权失败喵，确认一下输入是否正确喵~";
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
	if (from.msg.find("召唤豆豆") == 0 || from.msg.find("出来吧豆豆") == 0) {
		status.status = true;
		status.time = 0;
		groupIds[from.fromGup]=status;
		if (from.type == CQ_MSG_GUP) {
			CQ_sendGroupMsg(ac, from.fromGup, "喵~");
		}
		else
		{
			CQ_sendDiscussMsg(ac, from.fromGup, "喵~");
		}
		return true;
	}
	//验证需要处理不
	time_t now = time(NULL);
	if (groupIds.find(from.fromGup) == groupIds.end()) {
		return false;
	}
	status = groupIds[from.fromGup];
	if (!status.status || status.time > now) {
		return false;
	}

	//验证通过，开始处理
	if (from.msg.find("豆豆") != string::npos || from.msg.find(_qq) != string::npos) {
		//at自己或者开始叫自己，识别成命令
		_gupCmd(from);
		return true;
	}
	return _gupMsg(from);
}