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
constexpr auto INTRO = "�������Ƕ��������й��ҵ�ָ������������\n�ٻ�����\\�����ɶ��� ����\\�������\n�������������Լ� �鿴����Ϣ\n����������X��XСʱX���� XΪ���֣������ʱ��Ϊ���ý���\n��ӹؼ���XX ����XX ע��ո�\nɾ���ؼ���XX\n�����Ȩ1000\nɾ����Ȩ1000\nע����Ȩ��غ͹ؼ������������Ҫ��˽�Ĵ��ڷ���������Ȩ������ö����޸Ĺؼ�������Ⱥ���������Ҳֱ��at����������ƺ�����";
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
	int64_t time;//���Իָ�ʱ��
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
	string _qq;//�Լ���qq
	void checkDir();
	bool _gupMsg(msgFrom);
	bool _gupCmd(msgFrom);
};