/*
  +----------------------------------------------------------------------+
  | Swoole                                                               |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  | If you did not receive a copy of the Apache2.0 license and are unable|
  | to obtain it through the world-wide-web, please send a note to       |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Tianfeng Han  <mikan.tenny@gmail.com>                        |
  +----------------------------------------------------------------------+
*/
#include "aiengine.h"
#include "swoole.h"
#include "module.h"
#include "m_aiengine.h"
#include "semaphore.h"

int swModule_init(swModule *);

struct skegn_easy_usrdata
{
    sem_t *sem;
    int type;
    char *message;
    int size;
};



struct skegn_easy_usrdata *easy_usrdata;

_easy_callback(const void *usrdata, const char *id, int type, const void *message, int size)
{
	
	
    if (easy_usrdata->message) {
        free(easy_usrdata->message);
    }

    easy_usrdata->message = (char *)malloc(size);
    memcpy(easy_usrdata->message, message, size);
    easy_usrdata->size = size;
    easy_usrdata->type = type;
	
	

    sem_post(easy_usrdata->sem);

    return 0;
}


int swModule_init(swModule *module)
{
    module->name = (char *) "module_aiengine";

    swModule_register_function(module, (char *) "m_aiengine", m_aiengine);

    return SW_OK;
}


swVal* m_aiengine(swModule *module, int argc)
{
	int length;
	struct aiengine *engine;
	int   bytes;
    char  id[64];
	char buf[102400]={0};
	char *result;
    FILE *file;
	
	char *cfg = swArgs_pop_string(&length);
	char *params = swArgs_pop_string(&length);
	char *filepath = swArgs_pop_string(&length);
	char *audiotype = swArgs_pop_string(&length);

	engine = aiengine_new(cfg);

	easy_usrdata = (struct skegn_easy_usrdata*)calloc(1, sizeof(*easy_usrdata));
    easy_usrdata->sem = (sem_t *)malloc(sizeof(sem_t));
	sem_init(easy_usrdata->sem, 0, 0);

    aiengine_start(engine, params, id,  (aiengine_callback)_easy_callback, easy_usrdata);

	file = fopen(filepath, "rb");
	if(strcmp(audiotype,"wav") == 0){
		fseek(file,44,SEEK_SET);
	}
	while ((bytes = (int)fread(buf, 1, 1024, file))) {
        aiengine_feed(engine, buf, bytes);
    
	}
	fclose(file);

	aiengine_stop(engine);
    sem_wait(easy_usrdata->sem);

	memcpy(buf, easy_usrdata->message, easy_usrdata->size);

	sem_destroy(easy_usrdata->sem);
	free(easy_usrdata->sem);
    free(easy_usrdata); 
	easy_usrdata = NULL;
	
	aiengine_delete(engine);

	return swReturnValue_string(buf, strlen(buf));
}


