#include "ruby.h"
#include <sys/types.h>
#include <sys/shm.h>
#include <dlfcn.h>
#include "bartlby.h"



struct bartlby_ruby_cfg {

	
	void * bartlby_address;
	char * cfgfile;
	void * SOHandle;
	

		
};



static int id_push;
static char * dlmsg;


void * bartlby_get_shm();
char * getConfigValue(char * key, char * fname);


void * bartlby_get_sohandle(char * cfgfile) {
	char * data_lib;
	void * SOHandle;
	
	
	data_lib=getConfigValue("data_library", cfgfile);
	if(data_lib == NULL) {
			//php_error(E_WARNING, "cannot find data_lib key in %s", cfgfile);	
			return NULL;
	}
	SOHandle=dlopen(data_lib, RTLD_LAZY);
	if((dlmsg=dlerror()) != NULL) {
					//php_error(E_ERROR, "DL Error: %s", dlmsg);
        	return NULL;
    	}	
    	free(data_lib);
    	return SOHandle;
} 





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




void * bartlby_get_shm(char * cfgfile) {
	char * shmtok;
	void * bartlby_address;
	int shm_id;

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
		return NULL;
	}

	
}


void bartlby_res_free() {
	
}

static VALUE t_initialize(VALUE self, VALUE obj) {



//	cfgfile=RSTRING_PTR(obj);
	
	
	struct bartlby_ruby_cfg * bartlby_ruby;
	Data_Get_Struct(self, struct bartlby_ruby_cfg, bartlby_ruby);
	
	
	bartlby_ruby->cfgfile=RSTRING_PTR(obj);
	bartlby_ruby->bartlby_address=bartlby_get_shm(bartlby_ruby->cfgfile);
	bartlby_ruby->SOHandle=bartlby_get_sohandle(bartlby_ruby->cfgfile);

	
	
	
	
	return Qnil;
	//return self;
}


static VALUE t_lib_info(VALUE self) {
	

	VALUE result;
	
	char * (*GetAutor)();
	char * (*GetVersion)();
	char * (*GetName)();
	char * GetAutorStr;
	char * GetVersionStr;
	char * GetNameStr;
	

	struct bartlby_ruby_cfg * bartlby_ruby;
	Data_Get_Struct(self, struct bartlby_ruby_cfg, bartlby_ruby);



	
	LOAD_SYMBOL(GetAutor,bartlby_ruby->SOHandle, "GetAutor");
  LOAD_SYMBOL(GetVersion,bartlby_ruby->SOHandle, "GetVersion");
  LOAD_SYMBOL(GetName,bartlby_ruby->SOHandle, "GetName");
    	
	GetAutorStr=GetAutor();
  GetVersionStr=GetVersion();
  GetNameStr=GetName();
    	
 
 	
 	result = rb_hash_new();
 	
 	rb_hash_aset(result, ID2SYM(rb_intern("autor")),rb_str_new2(GetAutorStr));
 	rb_hash_aset(result, ID2SYM(rb_intern("version")),rb_str_new2(GetVersionStr));
 	rb_hash_aset(result, ID2SYM(rb_intern("name")),rb_str_new2(GetNameStr));
 	
 	
 	free(GetAutorStr);
	free(GetVersionStr);
	free(GetNameStr);
	
	return result;
 	

 	
	/*
	add_assoc_string(return_value, "Autor", GetAutorStr, 1);
	add_assoc_string(return_value, "Version", GetVersionStr, 1);
	add_assoc_string(return_value, "Name", GetNameStr, 1);
	
	
	dlclose(SOHandle);
	*/
    		
}
static VALUE t_close(VALUE self) {
	/*
	
	*/
	struct bartlby_ruby_cfg * bartlby_ruby;
	Data_Get_Struct(self, struct bartlby_ruby_cfg, bartlby_ruby);
	
	shmdt(bartlby_ruby->bartlby_address);	
	if(bartlby_ruby->SOHandle != NULL) dlclose(bartlby_ruby->SOHandle); 
	
	
	return Qnil;
}
static VALUE t_info(VALUE self) {



	
	VALUE result, result1;
	struct shm_header * shm_hdr;
	
	struct bartlby_ruby_cfg * bartlby_ruby;
	Data_Get_Struct(self, struct bartlby_ruby_cfg, bartlby_ruby);

	
	if(bartlby_ruby->bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_ruby->bartlby_address;
		
		result = rb_hash_new();
		result1 = rb_hash_new();
		
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


		rb_hash_aset(result1, ID2SYM(rb_intern("checks_performed_time")),INT2FIX(shm_hdr->checks_performed_time));


		rb_hash_aset(result, ID2SYM(rb_intern("hash2")),result1);

		
		
		
		
		
		return result;	
	} else {
		
		return Qnil;
	}
	
	
	

}

VALUE  t_alloc(VALUE self){
	
	VALUE instance = Qnil;
  struct bartlby_ruby_cfg * bartlby_ruby;


   
	bartlby_ruby=malloc(sizeof(struct bartlby_ruby_cfg));
	instance = Data_Wrap_Struct(self, 0, free, bartlby_ruby);
	return instance;
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
  
  rb_define_alloc_func(cBartlby, t_alloc);
  
  rb_define_method(cBartlby, "initialize", t_initialize, 1);
  rb_define_method(cBartlby, "close", t_close, 0);
  rb_define_method(cBartlby, "lib_info", t_lib_info, 0);
  rb_define_method(cBartlby, "info", t_info, 0);
  rb_define_method(cBartlby, "add", t_add, 1);
  id_push = rb_intern("push");
  
  
  
}