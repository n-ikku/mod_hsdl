#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include <unistd.h>

#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"
#include "apr_strings.h"
#include "apr_hash.h"
#include "apr_general.h"
#include "apr_pools.h"
#include "http_log.h"

#define HS_BUF_SIZE 1024
#define APR_BUF_SIZE_R 32768
#define APR_BUF_SIZE_W "2097152"
#define QUERY_SIZE 50

extern "C" module AP_MODULE_DECLARE_DATA hsdl_module;
int tcp_conn(char *, int,request_rec *);
void error_out(char *,request_rec *);
void set_body(char *,int,request_rec *);

typedef struct module_server_conf{
  char * hsdl_dbhost;         // MySQL Host
  char * hsdl_hsport;         // HandlerSocket Port(default 9998)
  char * hsdl_dbname;         // MySQL DB Name
  char * hsdl_dbtable;        // MySQL DB Table
  int    hsdl_bufsize;        // write buffer size
}module_server_conf;

int hs(module_server_conf *cf, request_rec *r)
{
  int sock;
  char sbuf[HS_BUF_SIZE];
  char rbuf2[APR_BUF_SIZE_R];
  char mbuf[HS_BUF_SIZE]={};
  char qbuf[QUERY_SIZE]={};
  char vbuf[cf->hsdl_bufsize];
  int  i,j,mc=0,mp=0,nlen = 0,loop=0;
  unsigned int vc = 0;
  int  tabcount = 0;
  int  next_add_flg=0;
  int  nl = 0;

  if(strlen(r->path_info) > QUERY_SIZE){
    //指定文字が長すぎるとしてエラー(便宜上)
    return HTTP_NOT_FOUND;
  }
  j=0;

  for(i=0;i<(int) strlen(r->path_info);i++){
    switch(r->path_info[i]){
      case '/'  : 
      case 0x0a : 
      case 0x09 : 
      case '\'' : 
      case '"'  : 
      case ' '  : break;
      default :
        qbuf[j] = r->path_info[i];
        j++;
      break;
    }
  }

  if(j == 0){
    return HTTP_NOT_FOUND;
  }
  

  sock = tcp_conn( cf->hsdl_dbhost, atoi(cf->hsdl_hsport),r );
  if( sock<0 )
  {
    return HTTP_INTERNAL_SERVER_ERROR;
  }
  

  // 問合せ文字列を作成
  sprintf(sbuf,"P\t0\t%s\t%s\tPRIMARY\tm,v\n",cf->hsdl_dbname,cf->hsdl_dbtable);
  write(sock, sbuf,strlen(sbuf) );
  read(sock, rbuf2, APR_BUF_SIZE_R);

  if(rbuf2[0] == '0'){  // Success
    sprintf(sbuf,"0\t=\t1\t%s\n",qbuf);
    write(sock, sbuf,strlen(sbuf) );

    while (1) {
      char rbuf[APR_BUF_SIZE_R]={};
      nlen = read(sock, rbuf,(size_t) APR_BUF_SIZE_R);
      if(nlen < 0){
        break;
      }
      for(i=0;i<nlen;i++)
      {
        switch(rbuf[i]){
          case 0x0a : nl=1; break;
          case 0x09 : tabcount++; break;
          case 0x01 : next_add_flg = 1; break;
          default:
            switch(tabcount)
            {
              case 2: // mデータ
                if(next_add_flg == 1){
                  mbuf[mc] = (char)rbuf[i] - 0x40;
                }else{
                  mbuf[mc] = (char)rbuf[i];
                }
                mc ++;
                next_add_flg = 0;
                break;
              case 3: // vデータ
                  // mime-type
                  if(mp == 0){
                    mp = 1;
                    ap_set_content_type(r, mbuf);
                  }
                  if(next_add_flg == 1){
                    vbuf[vc] = (char)rbuf[i] - 0x40;
                  }else{
                    vbuf[vc] = (char)rbuf[i];
                  }
                  vc ++;
                  if(sizeof(vbuf) < vc ){
                    set_body(vbuf, vc, r);
                    loop++;
                    vc = 0;
                  }
                  next_add_flg = 0;
                break;
              default:
                break;
            }
          break;
        }
      }
      if(nl == 1){
        // New Lineを見つけたときはレコードの終了
        break;
      }
    }
    if(tabcount == 3 && vc != 0){
      if(loop==0){
        ap_set_content_length  ( r,  vc) ;
      }
      set_body(vbuf, vc, r);
    }else if(tabcount < 3){
      // データがない
      return HTTP_NOT_FOUND;
    }
  }else{
    return HTTP_NOT_FOUND;
  }
  close( sock );
  return OK;
}

int tcp_conn( char *server, int portno, request_rec *r )
{
  struct sockaddr_in addr ; 
  int s ;

  if( (s = socket(PF_INET, SOCK_STREAM, 0)) < 0 )
  {
    ap_rputs("socket error<br/>\n", r);
    return( -1 );
  }

  bzero( (char *) &addr, sizeof(&addr) );
  addr.sin_family = PF_INET;
  addr.sin_port = htons( portno );

  if( connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
  {
    close( s );
    return( -1 );
  }
  return( s );
}

void set_body(char *org,int n,request_rec * r)
{
  ap_rwrite(org, n ,r);
}

void error_out(char *v,request_rec * r)
{
  ap_set_content_type(r, "text/plain");
  ap_rputs(v, r);
}
static void * create_per_dir_conf(apr_pool_t* p, char *arg)
{
  module_server_conf *conf;
  conf = (module_server_conf * )apr_pcalloc(p, sizeof(module_server_conf));
  // デフォルト値
  conf->hsdl_dbhost        = apr_pstrdup(p, "localhost");
  conf->hsdl_hsport        = apr_pstrdup(p, "9998");
  conf->hsdl_dbname        = apr_pstrdup(p, "");
  conf->hsdl_dbtable       = apr_pstrdup(p, "");
  conf->hsdl_bufsize       = atoi(apr_pstrdup(p, APR_BUF_SIZE_W));
  return (void *) conf;
}

static const command_rec modulename_cmds[] = {
    {"hsdl_dbhost"  , (cmd_func)ap_set_string_slot, (void *)APR_OFFSETOF(module_server_conf, hsdl_dbhost) , OR_ALL, TAKE1, "MySQL Host (default:localhost)"},
    {"hsdl_hsport"  , (cmd_func)ap_set_string_slot, (void *)APR_OFFSETOF(module_server_conf, hsdl_hsport) , OR_ALL, TAKE1, "HandlerSocket Port (default:9998)"},
    {"hsdl_dbname"  , (cmd_func)ap_set_string_slot, (void *)APR_OFFSETOF(module_server_conf, hsdl_dbname) , OR_ALL, TAKE1, "DB NAME"},
    {"hsdl_dbtable" , (cmd_func)ap_set_string_slot, (void *)APR_OFFSETOF(module_server_conf, hsdl_dbtable), OR_ALL, TAKE1, "DB Table"},
    {"hsdl_bufsize" , (cmd_func)ap_set_int_slot   , (void *)APR_OFFSETOF(module_server_conf, hsdl_bufsize), OR_ALL, TAKE1, "Write Buffer Size (default:2097152)"},
  {NULL},
};

extern "C" {
  static int hsdl_handler(request_rec *r)
  {
    if (strcmp(r->handler, "hsdl")) {
      return DECLINED;
    }
    struct module_server_conf *conf = (struct module_server_conf *)ap_get_module_config(r->per_dir_config, &hsdl_module);
    if (!r->header_only){
        return hs(conf,r);
    }
    return OK;
  }
};

static void hsdl_register_hooks(apr_pool_t *p)
{
  ap_hook_handler(hsdl_handler, NULL, NULL, APR_HOOK_MIDDLE);
}

/* Dispatch list for API hooks */

module AP_MODULE_DECLARE_DATA hsdl_module = {
  STANDARD20_MODULE_STUFF, 
  create_per_dir_conf,         /* create per-dir  config structures */
  NULL,                        /* merge  per-dir  config structures */
  NULL,                        /* create per-server config structures */
  NULL,                        /* merge  per-server config structures */
  modulename_cmds,             /* table of config file commands     */
  hsdl_register_hooks          /* register hooks            */
};
