

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "conn.h"
#include "cwrmsg.h"

struct args{
	struct conn * conn;
	struct cwrmsg * cwrmsg;
};

static int pmessage(void *p, struct cwrmsg * cwrmsg)
{
	struct args * args = (struct args*)p;
	struct conn * conn = args->conn;



	memcpy(&conn->cwrmsg,cwrmsg,sizeof(struct cwrmsg));
	memcpy(conn->cwrmsg_buffer,cwrmsg->msgelems,cwrmsg->msgelems_len);
	conn->cwrmsg.msgelems=conn->cwrmsg_buffer;
	args->cwrmsg = &conn->cwrmsg;
	return 0;
}




struct cwrmsg * conn_get_message(struct conn * conn)
{
	struct args args;
	args.cwrmsg=0;
	args.conn=conn;
	uint8_t buf[2024];
	int len=2024;

	int n = conn->read(conn,buf,len);
	if (n>0) 
		conn_process_packet(conn,buf,n,pmessage,&args);

	return args.cwrmsg;
}

