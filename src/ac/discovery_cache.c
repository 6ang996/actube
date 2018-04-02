#include <string.h>

#include "cw/sock.h"
#include "cw/mlist.h"
#include "cw/mavl.h"

#include "discovery_cache.h"

struct cw_DiscoveryCache{
	int len;
	int qpos;
	int qdel;
	mavl_t byaddrp;
	mavl_t byaddr;
	struct cw_DiscoveryCacheElem * queue;
	uint32_t ctrlo, ctrhi;
	
};


void discovery_cache_destroy(struct cw_DiscoveryCache * c)
{
	if (c->byaddr)
		mavl_destroy(c->byaddr);
	
	if (c->byaddrp)
		mavl_destroy(c->byaddrp);
	
	free(c);
}


static int cmpaddrp ( const void * d1, const void *d2 )
{
	struct cw_DiscoveryCacheElem * c1 = * ( void ** ) d1 ;
	struct cw_DiscoveryCacheElem * c2 = * ( void ** ) d2 ;
	return sock_cmpaddr ( ( struct sockaddr* ) &c1->addr, ( struct sockaddr* ) &c2->addr, 1 );
}


static int cmpaddr ( const void * d1, const void *d2 )
{
	struct cw_DiscoveryCacheElem * c1 = * ( void ** ) d1 ;
	struct cw_DiscoveryCacheElem * c2 = * ( void ** ) d2 ;
	int rc;

	rc = sock_cmpaddr ( ( struct sockaddr* ) &c1->addr, ( struct sockaddr* ) &c2->addr, 0 );
	if (rc != 0)
		return rc;
	
	if (c1->ctrhi != c2->ctrhi)
		return c1->ctrhi-c2->ctrhi;
	
	return c1->ctrlo-c2->ctrlo;

}




struct cw_DiscoveryCache * discovery_cache_create(int len)
{
	
	int i;
	struct cw_DiscoveryCache * cache;
	
	cache = malloc (sizeof(struct cw_DiscoveryCache));
	if (cache==NULL)
		return NULL;
		
	memset(cache,0,sizeof(struct cw_DiscoveryCache));
	cache->queue = malloc(sizeof(struct cw_DiscoveryCacheElem)*len);
	if (cache->queue==NULL)
		goto errX;

	for (i=0; i<len; i++){
		cache->queue[i].mod_capwap=NULL;
	}

	cache->byaddr = mavl_create_ptr(cmpaddr,NULL);
	if (cache->byaddr == NULL)
		goto errX;

	
	cache->byaddrp = mavl_create_ptr(cmpaddrp,NULL);
	if (cache->byaddrp == NULL)
		goto errX;

	cache->len=len;
	cache->qpos=0;
	
	
	return cache;
errX:
	discovery_cache_destroy(cache);
	return NULL;
}

void discovery_cache_add(struct cw_DiscoveryCache *cache, 
	struct sockaddr * addr, const char * mod_capwap, const char * mod_bindings)
{
	struct cw_DiscoveryCacheElem * cur = cache->queue+cache->qpos;
	if (cur->mod_capwap==NULL){
		/* delete here */
		mavl_del(cache->byaddr,cur);
		mavl_del(cache->byaddr,cur);

	}
	
	cur->mod_capwap=mod_capwap;
	cur->mod_bindings=mod_bindings;
	sock_copyaddr(&cur->addr,addr);
	
	cur->ctrhi=cache->ctrhi;
	cur->ctrlo=cache->ctrlo;
	
	if ((++cache->ctrlo)==0){
		cache->ctrhi++;
	}
	
	mavl_add_ptr(cache->byaddr,cur);
	mavl_add_ptr(cache->byaddrp,cur);
	
	cache->qpos++;
	if (cache->qpos>=cache->len)
		cache->qpos=0;
	
}


struct cw_DiscoveryCacheElem * discovery_cache_get(struct cw_DiscoveryCache * cache,struct sockaddr *addr)
{
	struct cw_DiscoveryCacheElem * elem, ** result, *search_ptr, search;
	sock_copyaddr(&search.addr,addr);
	search.ctrhi=search.ctrlo=0;
	

	elem = mavl_get_ptr(cache->byaddrp ,&search);
	if (elem != NULL)
		return elem;

	search.mod_capwap="hallo";
	search_ptr = &search;
	/*elem = *((struct cw_DiscoveryCacheElem **)
*/
	result = mavl_get_first(cache->byaddr, &search_ptr);
	if (result == NULL)
		return NULL;
	
	elem = *result;
	if (elem != NULL){
		if (sock_cmpaddr((struct sockaddr*)&elem->addr,addr,0)!=0)
			return NULL;
			
		mavl_del(cache->byaddr,result);
		mavl_del(cache->byaddrp,result);
	}
	
	return NULL;
	
}