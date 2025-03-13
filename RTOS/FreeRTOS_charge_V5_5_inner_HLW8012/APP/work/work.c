
#include "work.h"

extern void sock_ctrl_work( void );

void work_entry(void)
{
	sock_ctrl_work();
}

/*
(1)上电后读取数据库，得到2个插座最后一个订单情况，

(2)联网，登录服务器，获取日期
(3)获取日期成功后，处理未结束订单。
(4)进入轮询。


一、底层部分HLW8012
1、定期6s发送电能消耗消息，并更新总电能
2、检测是否发生拔插座的信号，并发送消息


二、网络部分
1、等待服务器下发的消息
(1)下单消息，
	直接返回 LOG all 01序号消息，告诉服务器收到消息了。
	把订单信息发送出去。
(2)结束消息

2、交互池

3、看门狗


三、插座工作部分
1、处理订单
(1) 获取到充电订单消息后，先判断当前插座是否存在工作中的订单
	如果没有工作中的订单，则直接使用订单信息，开启订单。并写入到FDB
	如果  有工作中的订单，则屏蔽非当前订单号的消息。
		(同订单号说明是修改时长，重新得出结束日期时间，并写入到FDB)。
(2) 获取到结束订单消息后，先判断当前插座是否存在工作中的订单
	如果没有，则丢弃
	如果存在工作中的订单，则屏蔽非当前订单号的消息。
	 (同订单号说明是真的结束，重新得出结束日期时间，并写入到FDB)
	
2、处理工作中订单
	判断当前时间是否已经超过订单结束时间，超过，则结束订单
	处理平均功率时，判断当前时间，是否进入下一个分时时段。
	
	
*/







