#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/init.h>
#include <linux/etherdevice.h>
#include <linux/if_ether.h>
#include <linux/pkt_sched.h>

#define PROTO_TYPE 0x0808
#define DHT_INSERT_ID 0x01
#define DHT_REMOVE_ID 0x02
#define DHT_RETRIVE_ID 0x03
#define DHT_ACK_ID 0x04
#define DHT_RESPONSE_ID 0x05
#define DHT_RETRIVE_RESPONSE_ID 0x06
#define DHT_CONFIRM_ID	0x07
#define DHT_NONE_ID 0x09


uint16_t seq;

static struct packet_type dht_pkt_type;

struct dht_t {
	struct dht_node_t* head;
} dht_t;

struct dht_node_t{
	uint8_t family;
	uint32_t key;
	uint8_t data_size;
	void* data;
	struct dht_node_t* next;
} dht_node_t;

struct dht_msg_insert{
	uint8_t family;
	uint32_t key;
	uint16_t seq;
	uint8_t data_size;
	uint8_t data[256];
} dht_msg_insert;


struct dht_msg_remove {
	uint8_t family;
	uint32_t key;
	uint16_t seq;
} dht_msg_remove;

struct dht_msg_retrive{
	uint16_t seq;
	uint8_t family;
	uint32_t key;
} dht_msg_retrive;

struct dht_msg_confirm {
	uint8_t family;
	uint16_t seq;
} dht_msg_response;


struct dht_msg_retrive_response {
	uint16_t seq;
	uint8_t family;
	uint32_t key;
	uint8_t data_size;
	uint8_t data[256];
};


int dht_handle_insert(uint8_t* data);
int dht_handle_remove(uint8_t* data);
int dht_handle_retrive(uint8_t* data, uint8_t* data_size, void* ret_data);
int dht_handle_response(uint8_t* data);
int dht_handle_retrive_response(uint8_t* data);

void dht_craft_msg_insert(uint8_t* dst, uint8_t data_size, void* data);
void dht_craft_msg_remove(uint8_t* dst, uint32_t key);
void dht_craft_msg_retrive(uint8_t* dst, uint32_t key);
void dht_craft_msg_response(uint8_t* dst, uint16_t rSeq);		// Usado para ?????
void dht_craft_msg_retrive_response(uint8_t* dst, uint16_t rSeq, uint8_t data_size, void* data);

struct net_device* dev = NULL;

void dht_create(void);
void dht_destroy(void);
void dht_insert(uint8_t app, size_t dsize, void* d);
void dht_remove(uint32_t _key);
void* dht_get_data(uint32_t _key);
void* dht_get_key(uint32_t _key);

static struct packet_type dht_pkt_type;

static int dht_rcv(struct sk_buff* skb, struct net_device* dev, struct packet_type *dht_pkt_type, struct net_device* orig);

// Auxiliar functions
char* strsub(const char* s, size_t n);

// Debug functions
void dht_print(void);
void dht_print_node(struct dht_node_t*);

extern int nbt_insert_mac(uint8_t*);
extern void nbt_print(void);
extern void print_mac(uint8_t*);
extern int nbt_remove_mac(uint8_t*);
extern uint8_t* nbt_get_mac(uint32_t);
extern uint32_t nbt_hash_func(void* info, unsigned long size);