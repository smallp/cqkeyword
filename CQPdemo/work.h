#pragma once
#include "string"
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include "cqp.h"
#pragma comment(lib,"jsoncpp.lib")
#include "json/json.h"
#define CQ_MSG_GUP 1	//群消息
#define CQ_MSG_DIS 2	//讨论组消息
#define MINE_QQ 10000
#define CQ_SAVE_WORDS "words.json"
#define CQ_SAVE_GUP "group.json"
#define CQ_SAVE_AUTH "auth.json"
using namespace std;
struct keyword
{
	string key;
	string content;
};
struct gupStatus
{
	bool status;
	int64_t time;//禁言恢复时间
};
class work
{
public:
	work();
	~work();
	int ac;
	void loadConf();
	void saveConf();
	void init();
private:
	vector<keyword> words;
	map<int64_t, gupStatus> groupIds;
	set<int64_t> authIds;
	string _dir;
	void checkDir();
};