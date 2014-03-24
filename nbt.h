#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/init.h>
#include <linux/etherdevice.h>
#include <linux/if_ether.h>
#include <linux/pkt_sched.h>
#include <linux/kthread.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define NBT_MSG_HEADER	"IDX\tKEY\tMAC\n"

#ifndef _NBT_H_
#define _NBT_H_

#define NBT_MODULE_LICENSE "GPL"
#define NBT_MODULE_AUTHOR "Gilles V. T. Silvano <gillesvtsilvano@gmail.com"
#define NBT_MODULE_DESC   "TODO"

#define NBT_PROTO_TYPE 0x0909
#define MAX_SIZE 50

#define NBT_ASSOCIATE_ID 0x01
#define NBT_DISASSOCIATE_ID 0x02
#define NBT_UPDATE_ID 0x03

#define UPDATE_DELAY 10000			// 10 seconds

#define MSG_BUFFER_SIZE 256

static char msg_buffer[MSG_BUFFER_SIZE];

static struct task_struct *update_task;
static struct proc_dir_entry *nbt_proc_entry;

struct net_device* dev = NULL;

struct nbt_node_t;
struct nbt_t;
struct nbt_node_t* nbt_create_neighbor(uint8_t* mac);
struct nbt_t* nbt_table;

int maccmp(uint8_t* mac1, uint8_t* mac2);
uint32_t nbt_hash_func(void* info, size_t size);
uint8_t* nbt_get_mac(uint32_t key);

//struct nbt_t* nbt_create(void);
void nbt_create(void);
void nbt_destroy(void);

int nbt_remove_neighbor(uint8_t* mac);
int nbt_insert_neighbor(struct nbt_node_t* n);

int nbt_insert_mac(uint8_t* mac);
int nbt_remove_mac(uint8_t* mac);

struct nbt_node_t* nbt_create_neighbor(uint8_t* mac);

static struct packet_type nbt_pkt_type;
static const struct file_operations proc_file_fops;

static ssize_t nbt_write_callback(struct file *fp, const char *buffer, size_t length, loff_t * offset);
static ssize_t nbt_read_callback(struct file *fp, char *buffer, size_t length, loff_t * offset);

struct nbt_msg_associate;
struct nbt_msg_disassociate;
struct nbt_msg_update;

void nbt_craft_msg_associate(uint8_t* mac);
void nbt_craft_msg_disassociate(uint8_t *mac);
void nbt_craft_msg_update(void);

void nbt_associate(void);
void nbt_disassociate(void);
void nbt_update(void);

int update_task_func(void* data);

static int nbt_rcv(struct sk_buff* skb, struct net_device* dev, struct packet_type *nbt_pkt_type, struct net_device *orig);

struct net_device* get_dev(char* d, size_t s);

/* debug function */
char* nbt_print(void);
char* print_mac(uint8_t* mac);
char* print_neighbor(struct nbt_node_t *n);
char* print_neighbors(struct nbt_node_t *n);

MODULE_LICENSE(NBT_MODULE_LICENSE);
MODULE_AUTHOR(NBT_MODULE_AUTHOR);
MODULE_DESCRIPTION(NBT_MODULE_DESC);
#endif /* _NBT_H_ */

