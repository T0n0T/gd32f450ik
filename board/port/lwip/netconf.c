/*!
    \file    netconf.c
    \brief   network connection configuration

    \version 2016-08-15, V1.0.0, demo for GD32F4xx
    \version 2018-12-12, V2.0.0, demo for GD32F4xx
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "netconf.h"
#include "enet_conf.h"
#include "ethernetif.h"
#include "lwip/dhcp.h"
#include "lwip/errno.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "queue.h"
#include "tcpip.h"
#include <stdio.h>
#include "shell.h"
#include "tcp_client.h"

#define MAX_DHCP_TRIES 4
__IO uint32_t g_localtime =
    0; /* for creating a time reference incremented by 10ms */

int errno;

typedef enum {
    DHCP_START = 0,
    DHCP_WAIT_ADDRESS,
    DHCP_ADDRESS_ASSIGNED,
    DHCP_TIMEOUT
} dhcp_state_enum;

#ifdef USE_DHCP
dhcp_state_enum dhcp_state = DHCP_START;
#endif /* USE_DHCP */

struct netif g_mynetif;
ip_addr_t ip_address = {0};

void lwip_netif_status_callback(struct netif *netif)
{
    if (((netif->flags & NETIF_FLAG_UP) != 0) && (0 != netif->ip_addr.addr)) {
        tcp_client_init();
    }
}

/*!
    \brief      initializes the LwIP stack
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lwip_stack_init(void)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    /* create tcp_ip stack thread */
    tcpip_init(NULL, NULL);

    /* IP address setting */
#ifdef USE_DHCP
    ipaddr.addr  = 0;
    netmask.addr = 0;
    gw.addr      = 0;
#else
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2,
             NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

#endif /* USE_DHCP */

    netif_add(&g_mynetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init,
              &tcpip_input);

    /* registers the default network interface */
    netif_set_default(&g_mynetif);
    netif_set_status_callback(&g_mynetif, lwip_netif_status_callback);

    /* when the netif is fully configured this function must be called */
    netif_set_up(&g_mynetif);
}

#ifdef USE_DHCP
/*!
    \brief      dhcp_task
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dhcp_task(void *pvParameters)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
    struct dhcp *dhcp_client;
    printf("come into lwip!\r\n");
    for (;;) {
        printf("current status is %d\r\n", dhcp_state);
        switch (dhcp_state) {
            case DHCP_START:
                dhcp_start(&g_mynetif);
                /* IP address should be set to 0 every time we want to assign a new DHCP
                 * address*/
                dhcp_state = DHCP_WAIT_ADDRESS;
                break;

            case DHCP_WAIT_ADDRESS:
                /* read the new IP address */
                ip_address.addr = g_mynetif.ip_addr.addr;

                if (0 != ip_address.addr) {
                    dhcp_state = DHCP_ADDRESS_ASSIGNED;
                    printf("\r\nDHCP -- eval board ip address: %d.%d.%d.%d \r\n",
                           ip4_addr1_16(&ip_address), ip4_addr2_16(&ip_address),
                           ip4_addr3_16(&ip_address), ip4_addr4_16(&ip_address));
                } else {
                    /* DHCP timeout */
                    dhcp_client = netif_dhcp_data(&g_mynetif);
                    if (dhcp_client->tries > MAX_DHCP_TRIES) {
                        dhcp_state = DHCP_TIMEOUT;
                        /* stop DHCP */
                        dhcp_stop(&g_mynetif);

                        /* static address used */
                        IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
                        IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2,
                                 NETMASK_ADDR3);
                        IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
                        netif_set_addr(&g_mynetif, &ipaddr, &netmask, &gw);
                    }
                }
                break;

            default:
                break;
        }

        /* wait 250 ms */
        vTaskDelay(250);
    }
}
#endif /* USE_DHCP */

int show_ip(void)
{
    ip_address.addr = g_mynetif.ip_addr.addr;

    if (0 != ip_address.addr) {
        printf("\r\nip address: %d.%d.%d.%d \r\n",
               ip4_addr1_16(&ip_address), ip4_addr2_16(&ip_address),
               ip4_addr3_16(&ip_address), ip4_addr4_16(&ip_address));
    }
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN, show_ip, show_ip, show IP);