#ifndef STD_DEF_HEADER_SYSPROG
#define STD_DEF_HEADER_SYSPROG

#include <stdint.h>
#include "queue.h"


/**
 * A struct for client specific data.
 *
 * This also includes the tailq entry item so this struct can become a
 * member of a tailq - the linked list of all connected clients.
 */
struct client {
	/* The clients socket. */
	int fd;
	int is_cook;
	/* The bufferedevent for this client. */
	struct bufferevent *buf_ev;
	/* The bufferedevent for assigned client. */
	struct bufferevent *buf_ev_cust;

	/*
	 * This holds the pointers to the next and previous entries in
	 * the tail queue.
	 */
	TAILQ_ENTRY(client) entries;
};

/*
 * register all types of packets here
 */
#define foreach_pkt_type				\
	_(REG_COOK,reg_cook)				\
	_(REG_CUSTOMER,reg_customer)			\
	_(CUST_ORDER,cust_order)			\
	_(COOK_ORDER,cook_order)			\
	_(CUST_UPDATE,cust_update)			\
	_(COOK_UPDATE,cook_update)		

/*
 * register all types of packets handled by server here
 */
#define foreach_server_pkt_type				\
	_(REG_COOK,reg_cook)				\
	_(REG_CUSTOMER,reg_customer)			\
	_(CUST_ORDER,cust_order)			\
	_(COOK_UPDATE,cook_update)		

/*
 * register all types of packets handled by cook here
 */
#define foreach_cook_pkt_type				\
	_(COOK_ORDER,cook_order)			

/*
 * register all types of packets handled by client here
 */
#define foreach_client_pkt_type				\
	_(CUST_UPDATE,cust_update)			

#define foreach_order_type				\
	_(BIRYANI)					\
	_(DOSA)						\
	_(VEG_CURRY)					\
	_(RICE)						\

#undef _
#define _(V,v) V,

/*
 * register all packet types in enum
 */
enum pkt_type
{
	foreach_pkt_type
};

typedef enum pkt_type pkt_type;

#undef _
#define _(V) V,

/*
 * register all order types in enum
 */
enum order_type
{
	NONE=0,
	foreach_order_type
};

typedef enum order_type order_type;

/*
 * define all packet types
 */
/*
 * packet sent to register a cook. from client to server
 */
struct reg_cook_pkt_t
{
	char first_name[32];
	char last_name[32];
	//array of entries suggesting what dishes the cook can make
	uint8_t capability[32];
};

/*
 * packet sent to register a customer. from client to server
 */
struct reg_customer_pkt_t
{
	char first_name[32];
	char last_name[32];
};

/*
 * packet sent by customer to specify his/her order. from client to server
 */
struct cust_order_pkt_t
{
	uint8_t orders[32];
};

/*
 * packet sent to cook by server telling him what all to make. from server to client
 */
struct cook_order_pkt_t
{
	uint8_t orders[32];
};

/*
 * packet sent by cook to server telling what all was made successful and what wasn't. from client to server
 */
struct cook_update_pkt_t
{
	char message[32];
	uint8_t orders_success[32];
	uint8_t orders_failed[32];
};

/*
 * packet sent by server to customer telling what all was made successful and what wasn't. from server to client
 */
struct cust_update_pkt_t
{
	char message[32];
	uint8_t orders_success[32];
	uint8_t orders_failed[32];
};

/*
 * typedef pkt structure types
 */

#undef _
#define _(V,v) typedef struct v##_pkt_t v##_pkt_t;
foreach_pkt_type

#undef _
#define _(V,v) v##_pkt_t v##_pkt;

/*
 * packet structure which will be passed around
 */
struct pkt_t
{
	//type of the packet. not making it pkt_type to keep it portable
	uint16_t type;
	uint16_t size;
	union data {foreach_pkt_type} u;
};
typedef struct pkt_t pkt_t;

/*
 * handler function declarations
 */

#undef _
#define _(V,v) int v##_handler(v##_pkt_t * pkt_data, void * arg);
foreach_pkt_type

#endif
