#ifndef CONFIG_H_
#define CONFIG_H_

#define LIMIT_BROADCAST 0x01	//���޹㲥��ֻ�򱾵����ι㲥
#define DIRECT_BROADCAST 0x02	//ֱ�ӹ㲥��ָ���㲥��ַ�����й㲥


#ifdef __cplusplus
extern "C" {
#endif

int cfg_load( const char* filename );

int cfg_bind_port();
int cfg_broadcast_mode();
int cfg_net_device();			//����������
const char* cfg_nick_name();
const char* cfg_group_name();

void cfg_set_broadcast_mode(int mode);

#ifdef __cplusplus
};
#endif

#endif
                                                  