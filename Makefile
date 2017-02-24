SWOOLE_SRC = "/root/swoole"
all: m_aiengine.c
	cc -DHAVE_CONFIG_H -g -o m_aiengine.so -O0 -fPIC -shared m_aiengine.c -ldl -I${SWOOLE_SRC}/include -I${SWOOLE_SRC} -laiengine 

