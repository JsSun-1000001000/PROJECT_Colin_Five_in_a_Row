#pragma once

#include<memory.h>

#define _DEF_BUFFER         (4096)
#define _DEF_CONTENT_SIZE	(1024)
#define _MAX_SIZE           (40)
#define _MAX_PATH           (260)

//自定义协议   先写协议头 再写协议结构
//登录 注册 获取好友信息 添加好友 聊天 发文件 下线请求
#define _DEF_PACK_BASE	(10000)
#define _DEF_PACK_COUNT (100)

//注册
#define _DEF_PACK_REGISTER_RQ	(_DEF_PACK_BASE + 0 )
#define _DEF_PACK_REGISTER_RS	(_DEF_PACK_BASE + 1 )
//登录
#define _DEF_PACK_LOGIN_RQ	(_DEF_PACK_BASE + 2 )
#define _DEF_PACK_LOGIN_RS	(_DEF_PACK_BASE + 3 )

//p.s. 客户端是聊天的协议 服务端是音视频的协议 2025.12.26
//游戏头文件在下面

//返回的结果
//注册请求的结果 注册带则会昵称
#define tel_is_exist		(0)
#define register_success	(1)
#define name_is_exist        (2)
//登录请求的结果
#define user_not_exist		(0)
#define password_error		(1)
#define login_success		(2)
//添加好友
#define no_this_user        (0)
#define user_refuse         (1)
#define user_offline        (2)
#define add_success         (3)

typedef int PackType;

//协议结构
//注册
typedef struct STRU_REGISTER_RQ
{
	STRU_REGISTER_RQ():type(_DEF_PACK_REGISTER_RQ)
	{
		memset( tel  , 0, sizeof(tel));
		memset( name  , 0, sizeof(name));
		memset( password , 0, sizeof(password) );
	}
	//需要手机号码 , 密码, 昵称
	PackType type;
	char tel[_MAX_SIZE];
	char name[_MAX_SIZE];
	char password[_MAX_SIZE];

}STRU_REGISTER_RQ;

typedef struct STRU_REGISTER_RS
{
	//回复结果
	STRU_REGISTER_RS(): type(_DEF_PACK_REGISTER_RS) , result(register_success)
	{
	}
	PackType type;
	int result;

}STRU_REGISTER_RS;

//登录
typedef struct STRU_LOGIN_RQ
{
	//登录需要: 手机号 密码 
	STRU_LOGIN_RQ():type(_DEF_PACK_LOGIN_RQ)
	{
		memset( tel , 0, sizeof(tel) );
		memset( password , 0, sizeof(password) );
	}
	PackType type;
	char tel[_MAX_SIZE];
	char password[_MAX_SIZE];

}STRU_LOGIN_RQ;

typedef struct STRU_LOGIN_RS
{
	// 需要 结果 , 用户的id
	STRU_LOGIN_RS(): type(_DEF_PACK_LOGIN_RS) , result(login_success),userid(0)
	{
        memset( name , 0, sizeof(name) );
	}
	PackType type;
	int result;
	int userid;
    char name[_MAX_SIZE];

}STRU_LOGIN_RS;

/*------------------------------------游戏相关------------------------------------*/

#define DEF_PACK_JOIN_ZONE      (_DEF_PACK_BASE + 4 )
#define DEF_PACK_LEAVE_ZONE     (_DEF_PACK_BASE + 5 )

enum ENUM_PLAY_ZONE{Five_in_Line = 0x10, E_L_S, D_D_Z};
//加入专区
struct STRU_JOIN_ZONE{
    //解决这是什么包 谁加入哪个专区
    STRU_JOIN_ZONE():type(DEF_PACK_JOIN_ZONE),userid(0),zoneid(0){

    }
    PackType type;
    int userid;
    int zoneid;
};
//退出专区
struct STRU_LEAVE_ZONE{
    //解决这是什么包 谁退出哪个区
    STRU_LEAVE_ZONE():type(DEF_PACK_LEAVE_ZONE),userid(0){

    }
    PackType type;
    int userid;
};

//专区内每个房间人数————和视频里的不一样 注意
#define DEF_ZONE_ROOM_INFO ( _DEF_PACK_BASE + 10 )
#define DEF_ZONE_INFO_RQ   ( _DEF_PACK_BASE + 11 )

#define DEF_ZONE_ROOM_COUNT 121
//请求 专区每个房间人数
struct STRU_ZONE_INFO_RQ{

    STRU_ZONE_INFO_RQ():type( DEF_ZONE_INFO_RQ )
        ,zoneid(0)
    {

    }
    PackType type;
    int zoneid;
};

struct STRU_ZONE_ROOM_INFO{
    //解决这是什么包 谁退出哪个区
    STRU_ZONE_ROOM_INFO():type( DEF_ZONE_ROOM_INFO )
        ,zoneid(0)
    {
        memset( roomInfo, 0, sizeof( roomInfo ));
    }
    PackType type;
    int zoneid;
    int roomInfo[ DEF_ZONE_ROOM_COUNT ];
};

#define DEF_JOIN_ROOM_RQ    (_DEF_PACK_BASE + 6 )

//加入房间
struct STRU_JOIN_ROOM_RQ{
    //解决这是什么包 谁加入哪个房间
    STRU_JOIN_ROOM_RQ():type(DEF_JOIN_ROOM_RQ),userid(0),roomid(0){

    }
    PackType type;
    int userid;
    int roomid;
};//发给服务器 服务器会同步房间成员信息

//房间 为了避免0 出现歧义 房间号是0 还是没有初始化 把0让出来 1-120

#define DEF_JOIN_ROOM_RS    (_DEF_PACK_BASE + 7 )
enum ENUM_ROOM_STATUS{ _host, _player, _watcher };//房主 玩家 观战者
//加入房间回复
struct STRU_JOIN_ROOM_RS{
    //解决这是什么包 谁 哪个房间 叫什么名字
    STRU_JOIN_ROOM_RS():type( DEF_JOIN_ROOM_RS ),
        userid(0),
        roomid(0),
        status(_host),
        result(1){

    }
    PackType type;
    int userid;
    int roomid;
    int status;
    int result;// 0 fail 1 success
};

#define DEF_ROOM_MEMBER     ( _DEF_PACK_BASE + 8 )
//房间成员
struct STRU_ROOM_MEMBER{
    //解决这是什么包 谁 哪个房间 叫什么名字
    STRU_ROOM_MEMBER():type( DEF_ROOM_MEMBER ), userid(0), status(_player){
        memset( name, 0, sizeof(name) );
    }
    PackType type;
    int userid;
    //加上身份
    int status;
    char name[_MAX_SIZE];
};


#define DEF_LEAVE_ROOM_RQ   ( _DEF_PACK_BASE + 9 )
//退出房间
struct STRU_LEAVE_ROOM_RQ{
    //解决这是什么包 谁 退出了房间
    STRU_LEAVE_ROOM_RQ():type( DEF_LEAVE_ROOM_RQ ),
        userid(0),status(_player),roomid(0){

    }
    PackType type;
    int userid;
    int status;
    int roomid;
};//会被转发 如果自己不是房主 房主退出 自己也跟着退出




