#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/init.h>
#include <linux/etherdevice.h>
#include <linux/if_ether.h>
#include <linux/pkt_sched.h>
#include <linux/kthread.h>

#ifndef _NBT_H_
#define _NBT_H_

#define NBT_PROTO_TYPE 0x0909
#define MAX_SIZE 50

#define NBT_ASSOCIATE_ID 0x01
#define NBT_DISASSOCIATE_ID 0x02
#define NBT_UPDATE_ID 0x03

#define UPDATE_DELAY 5000
static struct task_struct *update_task;

struct nbt_node_t;
struct nbt_t;
struct nbt_node_t* nbt_create_neighbor(uint8_t* mac);

void nbt_print(void);
void print_neighbor(struct nbt_node_t *n);
void print_neighbors(struct nbt_node_t *n);

int maccmp(uint8_t* mac1, uint8_t* mac2);
uint32_t nbt_hash_func(void* info, size_t size);

//struct nbt_t* nbt_create(void);
void nbt_create(void);
uint8_t* nbt_get_mac(uint32_t key);

int nbt_remove_neighbor(uint8_t* mac);

int nbt_insert_neighbor(struct nbt_node_t* n);


int nbt_insert_mac(uint8_t* mac);

int nbt_remove_mac(uint8_t* mac);

struct nbt_node_t* nbt_create_neighbor(uint8_t* mac);
void nbt_destroy(void);


static struct packet_type nbt_pkt_type;

struct nbt_msg_associate;
struct nbt_msg_disassociate;
struct nbt_msg_update;

void nbt_craft_msg_associate(void);
void nbt_craft_msg_disassociate(void);
void nbt_craft_msg_update(void);

void nbt_associate(void);
void nbt_disassociate(void);
void nbt_update(void);


int update_task_func(void *data);


static int nbt_rcv(struct sk_buff* skb, struct net_device* dev, struct packet_type *nbt_pkt_type, struct net_device *orig);


struct net_device* get_dev(void);


/* debug function */
void print_mac(uint8_t* mac);

void print_neighbor(struct nbt_node_t *n);

void print_neighbors(struct nbt_node_t *n);

void nbt_print(void);

#endif /* _NBT_H_ */

