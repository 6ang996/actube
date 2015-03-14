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

#include <string.h>

#include "capwap.h"
#include "conn.h"
#include "cwmsg.h"

int cwsend_join_request(struct conn *conn, struct radioinfo *radioinfo, struct wtpinfo *wtpinfo)
{
	uint8_t buffer[CWMSG_MAX_SIZE];
	struct cwmsg cwmsg;

	cwmsg_init(&cwmsg, buffer, CWMSG_JOIN_REQUEST, conn_get_next_seqnum(conn), radioinfo);
	cwmsg.capwap_mode = conn->capwap_mode;

	/* Mandatory elements */

	/* location data */
	cwmsg_addelem(&cwmsg, CWMSGELEM_LOCATION_DATA, wtpinfo->location,
		      strlen((char *) wtpinfo->location));

	/* wtp board data */
	cwmsg_addelem_wtp_board_data(&cwmsg, wtpinfo);

	/* wtp descriptor */
	cwmsg_addelem_wtp_descriptor(&cwmsg, wtpinfo);

	/* wtp name */
	cwmsg_addelem(&cwmsg, CWMSGELEM_WTP_NAME, wtpinfo->name, strlen((char *) wtpinfo->name));

	/* session id */
	cwmsg_addelem_session_id(&cwmsg, wtpinfo->session_id);

	/* frame tunnel mode */
	cwmsg_addelem(&cwmsg, CWMSGELEM_WTP_FRAME_TUNNEL_MODE, &wtpinfo->frame_tunnel_mode,
		      sizeof(uint8_t));

	/* WTP MAC type */
	cwmsg_addelem(&cwmsg, CWMSGELEM_WTP_MAC_TYPE, &wtpinfo->mac_type, sizeof(uint8_t));

	/* WTP radio information elements */
	cwmsg_addelem_wtp_radio_infos(&cwmsg, wtpinfo->radioinfo);

	switch (conn->capwap_mode) {
		case CWMODE_CISCO:
			cwmsg_addelem_vendor_cisco_mwar_addr(&cwmsg,conn);
			break;
		default:
			/* ECN support */
			cwmsg_addelem(&cwmsg, CWMSGELEM_ECN_SUPPORT, &wtpinfo->ecn_support,
				      sizeof(uint8_t));
	}

	/* local ip address */
	cwmsg_addelem_cw_local_ip_addr(&cwmsg, conn);


	/* Non-mandatory elements */

	/* maximum message length */
	uint16_t l = htons(wtpinfo->max_msg_len);
	cwmsg_addelem(&cwmsg, CWMSGELEM_MAXIMUM_MESSAGE_LENGTH, (uint8_t *) & l, sizeof(l));

	return conn_send_cwmsg(conn, &cwmsg);
}
