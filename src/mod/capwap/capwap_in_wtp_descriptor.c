/*
    This file is part of libcapwap.

    libcapwap is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libcapwap is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

*/


#include "cw/sock.h"
#include "cw/cw.h"

#include "mod_capwap.h"

int capwap_in_wtp_descriptor(struct conn *conn, struct cw_ElemHandler *eh, uint8_t * data,
			 int len, struct sockaddr *from)
{
	int rc;
	printf("WTP Descriptor reader\n");
/*
//	mbag_t mbag = conn->incomming;
*/
	rc =cw_read_wtp_descriptor(conn->remote_cfg, conn, eh, data, len, NULL);

	return rc;
}



