#pragma once
#include "string"
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include "cqp.h"
#pragma comment(lib,"jsoncpp.lib")
#include "json/json.h"
constexpr auto MINE_QQ = 10000;
constexpr auto CQ_SAVE_WORDS = "words.json";
constexpr auto CQ_SAVE_GUP = "group.json";
constexpr auto CQ_SAVE_AUTH = "auth.json";
constexpr auto INTRO = "喵，我是豆豆喵，有关我的指令在这里喵：\n召唤豆豆\\出来吧豆豆 启用\\解除禁言\n豆豆，介绍下自己 查看此信息\n豆豆，禁言X天X小时X分钟 X为数字，不添加时间为永久禁言\n添加关键字XX 内容XX 注意空格\n删除关键字XX\n添加授权1000\n删除授权1000\n注：授权相关和关键字相关命令需要在私聊窗口发的喵，授权后才能让豆豆修改关键字喵！群内命令可以也直接at豆豆来替代称呼喵。";
using namespace std;
enum msgType { CQ_MSG_USR, CQ_MSG_GUP, CQ_MSG_DIS };
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
struct msgFrom
{
	msgType type;
	int64_t fromGup;
	int64_t fromQQ;
	string msg;
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

	bool cmd(msgFrom);
	bool gupMsg(msgFrom);
private:
	vector<keyword> words;
	map<int64_t, gupStatus> groupIds;
	set<int64_t> authIds;
	string _dir;
	string _qq;//自己的qq
	void checkDir();
	bool _gupMsg(msgFrom);
	bool _gupCmd(msgFrom);
};