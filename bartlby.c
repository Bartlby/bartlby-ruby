#include "ruby.h"
#include <sys/types.h>
#include <sys/shm.h>

#include "bartlby.h"


static char * shmtok;
static	int shm_id;
static	void * bartlby_address;
static struct shm_header * shm_hdr;
static char * cfgfile;



static int id_push;

char * getConfigValue(char * key, char * fname) {
	FILE * fp;
	char  str[1024];

	char * tok;
	
	fp=fopen(fname, "r");
	if(!fp) {
	//	php_error(E_WARNING, "Config file '%s' failed", fname);
		return NULL;
	}
	
	while(fgets(str,1024,fp) != NULL) {
		str[strlen(str)-1]='\0';
		tok=strtok(str, "=");
		if(tok != NULL) {
				if(strcmp(tok, key) == 0) {
						tok=strtok(NULL, "=");
						if(tok == NULL) {
								return NULL;
						}
						if(tok[strlen(tok)-1] == '\r') {
							tok[strlen(tok)-1]='\0';
						}
						fclose(fp);
						return strdup(tok);
						
				} else {
					continue;
				}
					
		}
			
	}
	
	
	fclose(fp);
	
	
	return NULL;
}




void * bartlby_get_shm() {

	shmtok = getConfigValue("shm_key",cfgfile );
	if(shmtok == NULL) {
		return NULL;	
	}		
	
	
	shm_id = shmget(ftok(shmtok, 32), 0,  0777);
	free(shmtok);
	if(shm_id != -1) {
		bartlby_address=shmat(shm_id,NULL,0);
		

		return bartlby_address;
	} else {
		//FIXME ERROR ATTACHING
		return Qnil;
	}
}

static VALUE t_initialize(VALUE self, VALUE obj) {
	cfgfile=RSTRING_PTR(obj);
	bartlby_get_shm();
	return 1;
}



static VALUE t_close() {
	shmdt(bartlby_address);	
	return Qnil;
}
static VALUE t_info(VALUE self) {



	
	VALUE result;
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		
		result = rb_hash_new();

		
		rb_hash_aset(result, ID2SYM(rb_intern("services")),INT2FIX(shm_hdr->svccount));
		rb_hash_aset(result, ID2SYM(rb_intern("workers")),INT2FIX(shm_hdr->wrkcount));
		rb_hash_aset(result, ID2SYM(rb_intern("current_running")),INT2FIX(shm_hdr->current_running));
		rb_hash_aset(result, ID2SYM(rb_intern("version")),rb_str_new2(shm_hdr->version));
		rb_hash_aset(result, ID2SYM(rb_intern("do_reload")),INT2FIX(shm_hdr->do_reload));
		rb_hash_aset(result, ID2SYM(rb_intern("last_replication")),INT2FIX(shm_hdr->last_replication));
		rb_hash_aset(result, ID2SYM(rb_intern("startup_time")),INT2FIX(shm_hdr->startup_time));
		rb_hash_aset(result, ID2SYM(rb_intern("downtimes")),INT2FIX(shm_hdr->dtcount));
		rb_hash_aset(result, ID2SYM(rb_intern("sirene_mode")),INT2FIX(shm_hdr->sirene_mode));
		rb_hash_aset(result, ID2SYM(rb_intern("servers")),INT2FIX(shm_hdr->srvcount));
		rb_hash_aset(result, ID2SYM(rb_intern("round_time_sum")),INT2FIX(shm_hdr->pstat.sum));
		
		rb_hash_aset(result, ID2SYM(rb_intern("round_time_count")),INT2FIX(shm_hdr->pstat.counter));
		rb_hash_aset(result, ID2SYM(rb_intern("servicegroups")),INT2FIX(shm_hdr->svcgroupcount));
		rb_hash_aset(result, ID2SYM(rb_intern("servergroups")),INT2FIX(shm_hdr->srvgroupcount));
		rb_hash_aset(result, ID2SYM(rb_intern("checks_performed")),INT2FIX(shm_hdr->checks_performed));
		rb_hash_aset(result, ID2SYM(rb_intern("checks_performed_time")),INT2FIX(shm_hdr->checks_performed_time));

		
		
		
		
		
		return result;	
	} else {
		//return void;
		return Qnil;
	}
	
	
	
	
}

static VALUE t_add(VALUE self, VALUE obj)
{
  VALUE arr;
  arr = rb_iv_get(self, "@arr");
  rb_funcall(arr, id_push, 1, obj);
  return arr;
}

VALUE cBartlby;
void Init_bartlby() {
  cBartlby = rb_define_class("Bartlby", rb_cObject);
  rb_define_method(cBartlby, "initialize", t_initialize, 1);
  rb_define_method(cBartlby, "close", t_close, 0);
  rb_define_method(cBartlby, "info", t_info, 0);
  rb_define_method(cBartlby, "add", t_add, 1);
  id_push = rb_intern("push");
  
  
  
}