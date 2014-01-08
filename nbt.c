#include "nbt.h"

struct nbt_t* t = 0;


/*
 * Função de inicialização do módulo
 */ 
int __init init_module(void){
	/* init module */
	printk(KERN_INFO "NBT module initializing\n");
	nbt_create();

	dev_add_pack(&nbt_pkt_type);
	
	nbt_associate();
			
	return 0;
}

/*
 * Função de remoção do módulo
 */
void cleanup_module(void){
	printk(KERN_INFO "NBT module exiting\n");
	dev_remove_pack(&nbt_pkt_type);
	nbt_disassociate();
	nbt_destroy();
}

struct nbt_msg_associate {
	uint8_t type;
} *nbt_msg_associate;

struct nbt_msg_disassociate {
	uint8_t type;
} *nbt_msg_disassociate;

struct nbt_msg_update{
	uint8_t type;
	uint8_t seq;
	uint8_t idx;
} *nbt_msg_update;


void nbt_associate(void){
	nbt_craft_msg_associate();
}

void nbt_disassociate(void){
	nbt_craft_msg_disassociate();
}

void nbt_update(void){
	nbt_craft_msg_update();
}

void nbt_craft_msg_associate(void){
	struct sk_buff* skb;
	struct net_device* dev;
	uint8_t hlen, tlen, *p;

	dev = get_dev();
        if (!dev)
                return;

        hlen = LL_RESERVED_SPACE(dev);
        tlen = dev->needed_tailroom;
        
        skb = alloc_skb(dev->hard_header_len + sizeof(struct nbt_msg_associate) + hlen + tlen, GFP_ATOMIC);

        skb->protocol = htons(NBT_PROTO_TYPE);
        skb->dev = dev;
        skb->priority = TC_PRIO_CONTROL;
        skb_reset_network_header(skb);
        skb_put(skb, dev->hard_header_len + sizeof(struct nbt_msg_associate));
        p = skb->data;
        memcpy(p, dev->broadcast, ETH_ALEN);
        p += ETH_ALEN;
        memcpy(p, dev->dev_addr, ETH_ALEN);
        p += ETH_ALEN;
        memcpy(p, &skb->protocol, 2);
        p += 2;

	*p = NBT_ASSOCIATE_ID;
	if (dev_queue_xmit(skb) == NET_XMIT_SUCCESS){
		printk("craft_msg_associate(): Ok!\n");
	} else {
		printk("craft_msg_associate(): Fail!\n");
	}
}


void nbt_craft_msg_disassociate(void){
	struct sk_buff* skb;
	struct net_device* dev;
	uint8_t hlen, tlen, *p;

	dev = get_dev();
        if (!dev)
                return;

        hlen = LL_RESERVED_SPACE(dev);
        tlen = dev->needed_tailroom;
        
        skb = alloc_skb(dev->hard_header_len + sizeof(struct nbt_msg_disassociate) + hlen + tlen, GFP_ATOMIC);

        skb->protocol = htons(NBT_PROTO_TYPE);
        skb->dev = dev;
        skb->priority = TC_PRIO_CONTROL;
        skb_reset_network_header(skb);
        skb_put(skb, dev->hard_header_len + sizeof(struct nbt_msg_disassociate));
        p = skb->data;
        memcpy(p, dev->broadcast, ETH_ALEN);
        p += ETH_ALEN;
        memcpy(p, dev->dev_addr, ETH_ALEN);
        p += ETH_ALEN;
        memcpy(p, &skb->protocol, 2);
        p += 2;

	*p = NBT_DISASSOCIATE_ID;
	if (dev_queue_xmit(skb) == NET_XMIT_SUCCESS){
		printk("craft_msg_disassociate(): Ok!\n");
	} else {
		printk("craft_msg_disassociate(): Fail!\n");
	}
}

void nbt_craft_msg_update(void){

}


static struct packet_type nbt_pkt_type = {
	.type = NBT_PROTO_TYPE,
	.func = nbt_rcv,	
};

static int nbt_rcv(struct sk_buff* skb, struct net_device* dev, struct packet_type *nbt_pkt_type, struct net_device *orig) {
	struct h {
		uint8_t dst[6];
		uint8_t src[6];
		uint16_t eth_t;
		uint8_t dht_t;
	} *h = (struct h*) kmalloc(sizeof(struct h), GFP_ATOMIC);

	skb = skb_share_check(skb, GFP_ATOMIC);
	if (!skb)
		return -1;

	if (!skb_push(skb, ETH_ALEN*2 + 2))
		return -1;
		
	memcpy(h, skb->data, sizeof(struct h));
//	p += ETH_ALEN * 2 + 2;
 

	printk("dst: %x:%x:%x:%x:%x:%x\n", 
		h->dst[0], h->dst[1], h->dst[2], h->dst[3], h->dst[4], h->dst[5]); 

	printk("dst: %x:%x:%x:%x:%x:%x\n", 
		h->src[0], h->src[1], h->src[2], h->src[3], h->src[4], h->src[5]); 


	printk("eth type: 0x%02x\n", h->eth_t);
	printk("dht type: 0x%02x\n", h->dht_t);
	
	switch(h->dht_t){
		case NBT_ASSOCIATE_ID:
			printk("Rcv(): Got Associate Id\n");
			nbt_insert_mac(h->src);
			//nbt_print();
			break;
		case NBT_DISASSOCIATE_ID:
			printk("Rcv(): Got a Disassociate Id\n");
			nbt_remove_mac(h->src);
			break;
		case NBT_UPDATE_ID:
			break;
		default:
			break;
	}
	
	return 0;	
}

int maccmp(uint8_t* mac1, uint8_t* mac2){
	int i = 0;
	for (; i < 6; ++i, ++mac1, ++mac2){
		if (*mac1 > *mac2 || *mac1 < *mac2){
		       	return *mac1 - *mac2;
		}
	}
	/* equals */
	return 0;
}

uint32_t nbt_hash_func(void* info, size_t size){
	int i = 0;
	uint32_t sum = 0;
	uint8_t* c = info;
	if (info == 0){
		return 0;
	}
	for(; i < size; ++i, ++c){
		//printk(KERN_INFO "nbt.ko: nbt_key(): %u + %u\n", *c, sum);
		sum += *c;
	}
	return sum;
}
EXPORT_SYMBOL(nbt_hash_func);

struct nbt_node_t {
	uint32_t idx;
	uint32_t key;
	uint8_t mac[6];
	struct nbt_node_t *next;
} nbt_node_t;
EXPORT_SYMBOL(nbt_node_t);

struct nbt_t{
	struct nbt_node_t* head;
} nbt_t;
EXPORT_SYMBOL(nbt_t);

//struct nbt_t* nbt_create(void){
void nbt_create(void){
	t = (struct nbt_t*) kmalloc(sizeof(struct nbt_t), GFP_KERNEL);
	t->head = 0;
//	return t;
}
EXPORT_SYMBOL(nbt_create);

uint8_t* nbt_get_mac(uint32_t key){
	struct nbt_node_t* pt = t->head;
	struct nbt_node_t* ant = 0;	

	if (t == 0)
		return 0;

	if (t->head->key == pt->key){
		return pt->mac;
	}
	
	ant = pt;
	pt = pt->next;

	while (pt != 0){
		if (pt->key > key){
			return ant->mac;
		}
		ant = pt;
		pt = pt->next;
	}
	return 0;
}
EXPORT_SYMBOL(nbt_get_mac);

int nbt_remove_neighbor(uint8_t* mac){
	struct nbt_node_t* pt;
	struct nbt_node_t* ant;
	
	if (t == 0 || mac == 0)
		return -2;
	
	printk(KERN_INFO "Remove:\tRemoving neighbor.\n");

	pt = t->head;

	if (!maccmp(t->head->mac, mac)){
		t->head = t->head->next;
		kfree(pt);
		return 0;
	}
	ant = pt;
	pt = pt->next;
	while (pt != 0){
		if (!maccmp(pt->mac, mac)){
			ant->next = pt->next;
			kfree(pt);
			return 0;
		}
	}
	return -1;
}

int nbt_insert_neighbor(struct nbt_node_t* n){
	struct nbt_node_t* pt = t->head;
	struct nbt_node_t* ant;

	if (t == 0 || n == 0)
		return -1;	/* null pointer */
	print_mac(n->mac);

	/* 
	 * When the list is empty, the new node will be inserted as a new head
	 */	
	if (pt == 0){
		t->head = n;
		printk(KERN_INFO "Insert:\tTable is empty. Creating a new head.\n");
		return 0;
	}


	/* 
	 * If there is at least one node in the list, first we should try to avoid
	 * move the 'ant' pointer, checking the head and the new node
	 *
	 * If the new node has...
	 */
	if (n->idx < pt->idx){
		t->head = n;
		n->next = pt;
		return 0;
	} else if(n->idx == pt->idx) {
		if (n->key < pt->key){
			t->head = n;
			n->next = pt;
			return 0;
		} else if (n->key == pt->key) {
			if (maccmp(n->mac, pt->mac) < 0){
				t->head = n;
				n->next = pt;
				return 0;
			} else if (maccmp(n->mac, pt->mac) > 0){
				/* } else if (maccmp(n->mac, pt->mac) == 0) { */
				/* n->idx += 1 % UINT32_MAX; */
				n->idx += 1 % MAX_SIZE;
				n->next = 0;
				return nbt_insert_neighbor(n);				
			} else {
				printk("Mac are equals\n");
				return 1;
				// Equals
			}
		} else {
			/* n->idx += 1 % UINT32_MAX; */
			n->idx += 1 % MAX_SIZE;
			n->next = 0;
			return nbt_insert_neighbor(n);				
		}
	}

	ant = pt; 
	pt = pt->next;

	while (pt != 0) {
		if (n->idx < pt->idx){
			ant->next = n;
			n->next = pt;
			return 0;
		} else { 
			if(n->idx == pt->idx) {
				if (n->key < pt->key){
					ant->next = n;
					n->next = pt;
					return 0;
				} else {
					if (n->key == pt->key) {
						if (maccmp(n->mac, pt->mac) < 0){
							ant->next = n;
							n->next = pt;
							return 0;
						} else if (maccmp(n->mac, pt->mac) > 0){
							/* } else if (maccmp(n->mac, pt->mac) == 0){ */
							/* n->idx += 1 % UINT32_MAX; */
							n->idx += 1 % MAX_SIZE;
							n->next = 0;
							return nbt_insert_neighbor( n);
						} else {
							printk("Mac are equals\n");
							return 1;
						}	
					} else {
						/* n->idx += 1 % UINT32_MAX; */
						n->idx += 1 % MAX_SIZE;
						n->next = 0;
						return nbt_insert_neighbor( n);				
					}	
				}
			}
		}
		ant = pt; 
		pt = pt->next;	
	}
	ant->next = n;
	return 0;
}

int nbt_insert_mac(uint8_t* mac){
	struct nbt_node_t* n;
	if (t == 0 || mac == 0)
		return -1;	/* null pointer */
	n = nbt_create_neighbor(mac);

	return nbt_insert_neighbor( n);
}

EXPORT_SYMBOL(nbt_insert_mac);

int nbt_remove_mac(uint8_t* mac){
	return nbt_remove_neighbor( mac);
}
EXPORT_SYMBOL(nbt_remove_mac);

struct nbt_node_t* nbt_create_neighbor(uint8_t* mac){
	struct nbt_node_t* newbie = (struct nbt_node_t*) kmalloc(sizeof( struct nbt_node_t), GFP_KERNEL);
	int i = 0;

	newbie->key = newbie->idx = nbt_hash_func(mac, 6);
	for (; i < 6; ++i, ++mac){
		newbie->mac[i] = *mac;
	}
	newbie->next = 0;

	return newbie;
}

void nbt_destroy(){
	struct nbt_node_t* pt = t->head;
	struct nbt_node_t* target = 0;

	if (t == 0)
		return;

	while (pt != 0){
		target = pt;
		pt = pt->next;
		kfree(target);
	}

	kfree(t);
}
EXPORT_SYMBOL(nbt_destroy);

struct net_device* get_dev(void){
	struct net_device* dev = first_net_device(&init_net);
	while (dev){
		if (memcmp(dev->name, "virbr0", 6) == 0)
			break;
		dev = next_net_device(dev);
	}
	return dev;
}
EXPORT_SYMBOL(get_dev);

/* debug functions */
void print_mac(uint8_t* mac){
	uint8_t *aux = mac;
	if (mac == 0){
		printk(KERN_INFO "Print_mac: null\n");
		return;
	}
	printk("Print_mac(): %x:%x:%x:%x:%x:%x\n",
			*aux, *(aux + 1), *(aux + 2), *(aux + 3), *(aux + 4), *(aux + 5));
}
EXPORT_SYMBOL(print_mac);

void print_neighbor(struct nbt_node_t *n){
	char s = ':';	//mac separator
	printk(KERN_INFO "%s%010u\t%s%010u\t%s%02x%c%02x%c%02x%c%02x%c%02x%c%02x\t%s%s\n",
			"IDX: ", n->idx, 
			"KEY: ", n->key,
			"MAC: ", n->mac[0], s, n->mac[1], s, n->mac[2], s, n->mac[3], s, n->mac[4], s, n->mac[5],
			"NEXT? ", n->next == 0 ? "null" : "yes");
}

void print_neighbors(struct nbt_node_t *n){
	struct nbt_node_t* pt = n;
	
	while (pt != 0){
		print_neighbor(pt);
		pt = pt->next;
	}
}

void nbt_print(){
	printk(KERN_INFO "####################################\n");
	if (t == 0){
		printk ("NULL\n");
		printk(KERN_INFO "####################################\n");
		return;
	}
	print_neighbors(t->head);
	printk(KERN_INFO "####################################\n");
}
EXPORT_SYMBOL(nbt_print);
