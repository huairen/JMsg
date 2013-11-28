#ifndef CONFIG_H_
#define CONFIG_H_

#define LIMIT_BROADCAST 0x01	//受限广播，只向本地网段广播
#define DIRECT_BROADCAST 0x02	//直接广播，指定广播寺址，进行广播


#ifdef __cplusplus
extern "C" {
#endif

int cfg_load( const char* filename );

int cfg_bind_port();
int cfg_broadcast_mode();
int cfg_net_device();			//绑定网卡索引
const char* cfg_nick_name();
const char* cfg_group_name();

void cfg_set_broadcast_mode(int mode);

#ifdef __cplusplus
};
#endif

#endif
                                                  