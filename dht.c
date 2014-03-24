#include "dht.h"

struct dht_t* t = NULL;

// Função para a inicialização do módulo
// Ainda com código de teste
int __init init_module(){
	char data[256];
	char key[256];
	int myApp = 10;

	//char myDev[10] = "virbr0";
	char myDev[10] = "eth0";

	dev = get_dev(myDev, strlen(myDev));
	if(!dev){
		return -ENXIO;
	}

	dev_add_pack(&dht_pkt_type);

	dht_create();

	/****** User Code *******/

	//char data[6];
	//int myApp = 10;
	strcpy(data, "192.168.0.153");
	strcpy(key, "00:aa:bb:dd:ee:ff");
	dht_insert(myApp, key, strlen(key), data, strlen(data));
	memset(data, '\0', 256);
	memset(key, '\0', 256);
	strcpy(data, "192.168.0.1");
	strcpy(key, "ee:ff:aa:dd:33:22");
	dht_insert(myApp, key, strlen(key), data, strlen(data));

	//strcpy(data, "192.168.0.153");
	//strcpy(key, "00:aa:bb:dd:ee:ff");

	//dht_remove(2, key, strlen(key));
	//dht_print();

	return 0;
}

// Função chamada na desativação do módulo
void cleanup_module(){
	dev_remove_pack(&dht_pkt_type);
	nbt_disassociate();
//	dht_check();
	dht_destroy();
	nbt_destroy();
}

static struct packet_type dht_pkt_type = {
	.type = PROTO_TYPE,
	.func = dht_rcv,
};

int update_task_func(void* data){
	while(!kthread_should_stop()) {
		if (t){
			dht_check();
			printk("############# DHT ##############\n");
			dht_print();
			printk("###############################\n");
		}
		msleep_interruptible(UPDATE_DELAY);
	}
	return 0;
}


void dht_check(void){
	struct dht_node_t* pt = NULL, *ant = NULL, *target = NULL;
	uint8_t* addr = NULL;

	if (!dev){
		return;
	}

	if (!t) {
		return;
	}
	
	if (!t->head){
		return;
	}

	addr = nbt_get_mac(t->head->idx);

	if (addr) {
		if (maccmp(addr, dev->dev_addr)){
			dht_craft_msg_insert(addr, t->head->app, t->head->key, t->head->key_size, t->head->data, t->head->data_size);

			target = t->head;
			t->head = t->head->next;
			
			kfree(target);
			target = NULL;

			pt = t->head;
			/* ant = NULL; */
		} else {
			pt = t->head->next;
			ant = t->head;
		}
	}

	while(pt){
		addr = nbt_get_mac(pt->idx);

		if (addr)
			if (maccmp(addr, dev->dev_addr)){

				dht_craft_msg_insert(addr, pt->app, pt->key, pt->key_size, pt->data, pt->data_size);

				target = pt;

				if (!ant){
					pt = pt->next;
					t->head = pt;
				} else {
					ant->next = pt->next;
					pt = pt->next;
				}

				kfree(target);
				target = NULL;

				continue;
			}

		ant = pt;
		pt = pt->next;
	}
}


// Cria uma nova dht
// Futuramente será possível passar parâmetros para a dht a ser criada
// É necessário fazer alguns estudos sobre a possibilidade de se ter várias DHT's
void dht_create(){
	if (!t){
		t = (struct dht_t*) kmalloc(sizeof(struct dht_t*), GFP_KERNEL);
		t->head = NULL;

		update_task = kthread_run(update_task_func, NULL, "dht_update_task");
	}
}


// Remove a dht criada e desaloca toda a estrutura alocada
void dht_destroy(){
	struct dht_node_t* pt = NULL, *target = NULL;

	if (!t){
		return;
	}

	if (update_task)
		kthread_stop(update_task);

	if (!t->head){
		kfree(t);
		t = NULL;
		return;
	}
	
	pt = t->head;
	
	while(pt){
		target = pt;
		pt = pt->next;
		kfree(target);	
		target = NULL;
	};

	kfree(t);
	t = NULL;
}

void dht_craft_msg_insert(uint8_t* dst, uint8_t app, void* key, uint8_t key_size, void* data, uint8_t data_size){
	struct sk_buff *skb = NULL;
	uint8_t hlen, tlen, *p = NULL, ret;
	struct dht_msg_insert* msg = NULL;

	hlen = LL_RESERVED_SPACE(dev);
	tlen = dev->needed_tailroom;
	skb = alloc_skb(dev->hard_header_len + sizeof(struct dht_msg_insert) + hlen + tlen, GFP_ATOMIC);

	skb->protocol = htons(PROTO_TYPE);
	skb->dev = dev;
	skb->priority = TC_PRIO_CONTROL;
	skb_reset_network_header(skb);
	skb_put(skb, dev->hard_header_len + sizeof(struct dht_msg_insert));

	p = skb->data;
	
	memcpy(p, dst, ETH_ALEN);
	p += ETH_ALEN;
	memcpy(p, dev->dev_addr, ETH_ALEN);
	p += ETH_ALEN;
	memcpy(p, &skb->protocol, 2);
	p += 2;
	

	msg = (struct dht_msg_insert*) kmalloc(sizeof(struct dht_msg_insert), GFP_KERNEL);
	msg->type = DHT_INSERT_ID;
	msg->app = app;
	msg->idx = nbt_hash_func(key, key_size);
	memset(msg->key, '\0', sizeof(msg->key));
	memcpy(msg->key, key, key_size);
	msg->key_size = key_size;

	memset(msg->data, '\0', sizeof(msg->data));
	memcpy(msg->data, data, data_size);
	msg->data_size = data_size;

	memcpy(p, msg, sizeof(struct dht_msg_insert));

	ret = dev_queue_xmit(skb);
}

void dht_craft_msg_remove(uint8_t* dst, uint8_t app, void* key, uint8_t key_size){
	struct sk_buff *skb = NULL;
	uint8_t hlen, tlen, *p = NULL, ret;
	struct dht_msg_remove* msg = NULL;

	hlen = LL_RESERVED_SPACE(dev);
	tlen = dev->needed_tailroom;
	skb = alloc_skb(dev->hard_header_len + sizeof(struct dht_msg_remove) + hlen + tlen, GFP_ATOMIC);
	skb->protocol = htons(PROTO_TYPE);
	skb->dev = dev;
	skb->priority = TC_PRIO_CONTROL;
	skb_reset_network_header(skb);
	skb_put(skb, dev->hard_header_len + sizeof(struct dht_msg_remove));
	p = skb->data;
	memcpy(p, dst, ETH_ALEN);
	p += ETH_ALEN;
	memcpy(p, dev->dev_addr, ETH_ALEN);
	p += ETH_ALEN;
	memcpy(p, &skb->protocol, 2);
	p += 2;
	
	msg = (struct dht_msg_remove*) kmalloc(sizeof(struct dht_msg_remove), GFP_KERNEL);
	msg->type = DHT_REMOVE_ID;
	msg->app = app;
	msg->idx = nbt_hash_func(key, key_size);
	memset(msg->key, '\0', key_size);
	memcpy(msg->key, key, key_size);
	msg->key_size = key_size;

	memcpy(p, msg, sizeof(struct dht_msg_remove));
	
	ret = dev_queue_xmit(skb);
}

void dht_craft_msg_retrive(uint8_t* dst, uint32_t key){
	// TODO
}

void dht_craft_msg_response(uint8_t* dst, uint16_t rSeq){
	// TODO
}

void dht_craft_msg_retrive_response(uint8_t* dst, uint16_t rSeq, uint8_t data_size, void* data){
	// TODO
}

void dht_craft_msg_confirm(uint8_t *dst, uint16_t rSeq){
	// TODO
}


static int dht_rcv(struct sk_buff* skb, struct net_device* dev, struct packet_type *dht_pkt_type, struct net_device* orig){
	uint8_t rcv_type;
	uint8_t *p = NULL;
	void* msg = NULL;

	skb = skb_share_check(skb, GFP_ATOMIC);
	if (!skb)
		return -1;

	p = skb->data;
	msg = p;
	rcv_type = *p;
	
	switch(rcv_type){
		case DHT_INSERT_ID:
			dht_handle_insert(msg);
			break;	
		case DHT_REMOVE_ID:
			dht_handle_remove(msg);
			break;
		case DHT_RETRIVE_ID:
			//TODO
			break;
		case DHT_RESPONSE_ID:
			//TODO
			break;
		case DHT_NONE_ID:
			break;
		default:
			break;
	}	
	return 0;
}

int dht_handle_insert(void* msg){
	struct dht_node_t* pt = NULL, *ant = NULL, *newbie = NULL;
	struct dht_msg_insert* ins_msg = NULL;

	ins_msg = msg;
	//ins_msg = (struct dht_msg_insert * ) kmalloc(sizeof(dht_msg_insert), GFP_KERNEL);
	//memcpy(ins_msg, msg, sizeof(dht_msg_insert));
	
	newbie = (struct dht_node_t*) kmalloc(sizeof(struct dht_node_t), GFP_KERNEL);
	newbie->app = ins_msg->app;
	newbie->idx = ins_msg->idx;
	newbie->key = kmalloc(sizeof(uint8_t) * ins_msg->key_size, GFP_KERNEL);
	memcpy(newbie->key, ins_msg->key, ins_msg->key_size);
	newbie->key_size = ins_msg->key_size;
	newbie->data = kmalloc(sizeof(uint8_t) * ins_msg->data_size, GFP_KERNEL);
	memcpy(newbie->data, ins_msg->data, GFP_KERNEL);
	newbie->data_size = ins_msg->data_size;
	newbie->next = NULL;

	if (!t){
		printk("dht_handle_insert(): No DHT.\n");
		return 0;
	}

	if (!t->head){
		t->head = newbie;
		return 0;
	}

	if (t->head->idx > newbie->idx){
		newbie->next = t->head;
		t->head = newbie;
		return 0;
	}


	ant = t->head;
	pt = t->head->next;
	

	while (pt){
		if (pt->idx > newbie->idx){
			newbie->next = pt;
			ant->next = newbie;	
			return 0;
		}
		ant = pt;
		pt = pt->next;
	}	
	ant->next = newbie;
	return 0;
}

int dht_handle_remove(void* msg){
	struct dht_node_t* pt = NULL, *ant = NULL, *target = NULL;
	struct dht_msg_remove* rmv_msg = NULL;
	uint32_t idx;

	rmv_msg = msg;

	idx = nbt_hash_func(rmv_msg->key, rmv_msg->key_size);

	pt = t->head;
	
	if (!pt) {
		return -2;
	}

	if (pt->app == rmv_msg->app && 
		!memcmp(pt->key, rmv_msg->key, (pt->key_size < rmv_msg->key_size ? pt->key_size : rmv_msg->key_size)) &&
			pt->key_size == rmv_msg->key_size)
	{
		target = pt;
		t->head = pt->next;
		kfree(target);
		return 0;
	}

	ant = pt;
	pt = pt->next;
	
	while (pt){
		if (pt->app == rmv_msg->app && 
		memcmp(pt->key, rmv_msg->key, (pt->key_size < rmv_msg->key_size ? pt->key_size : rmv_msg->key_size)) == 0 &&
			pt->key_size == rmv_msg->key_size)
		{
			target = pt;
			ant->next = pt->next;
			kfree(target);
			return 0;
		}
		ant = pt;
		pt = pt->next;
	}
	return -1;
}

int dht_handle_retrive(void* data){
	// TODO
	return 0;
}

int dht_handle_response(void* data){
	// TODO
	return 0;
}

int dht_insert(uint8_t app, void* key, uint8_t key_size, void* data, uint8_t data_size){
	struct dht_node_t* pt = NULL, *ant = NULL, *newbie = NULL;

	uint8_t* addr = NULL;
	uint32_t idx;

	idx = nbt_hash_func(key, key_size);
	addr = nbt_get_mac(idx);

	if (!addr){
		return -1;
	}

	if(!dev){
		return -ENXIO;
	}

	if (maccmp(addr, dev->dev_addr)){
		dht_craft_msg_insert(addr, app, key, key_size, data, data_size);
		return 0;
	}

	newbie = (struct dht_node_t*) kmalloc(sizeof(struct dht_node_t), GFP_KERNEL);
	newbie->app = app;
	newbie->idx = idx;
	newbie->key = kmalloc(sizeof(uint8_t) * key_size, GFP_KERNEL);
	memcpy(newbie->key, key, key_size);
	newbie->key_size = key_size;
	newbie->data = kmalloc(sizeof(uint8_t) * data_size, GFP_KERNEL);
	memcpy(newbie->data, data, data_size);
	newbie->data_size = data_size;
	newbie->next = NULL;

	if (!t->head){
		t->head = newbie;
		return 0;
	}
	
	if (t->head->idx > newbie->idx){
		newbie->next = t->head;
		t->head = newbie;
		return 0;
	}

	ant = t->head;
	pt = pt->next;
	
	while (pt){
		if (pt->idx > newbie->idx){
			newbie->next = pt;
			ant->next = newbie;	
			return 0;
		}
		ant = pt;
		pt = pt->next;
	}
	ant->next = newbie;
	return 0;
}
EXPORT_SYMBOL(dht_insert);

int dht_remove(uint8_t app, void* key, uint8_t key_size){
	struct dht_node_t *pt = NULL, *ant = NULL, *target = NULL;
	uint32_t idx;
	uint8_t* addr = NULL;

	if (!t)
		return -3;

	idx = nbt_hash_func(key, key_size);
	addr = nbt_get_mac(idx);

	if (maccmp(addr, dev->dev_addr)){
		dht_craft_msg_remove(addr, app, key, key_size);
		return 0;
	}

	pt = t->head;

	if (pt->app == app && 
		!memcmp(pt->key, key, pt->key_size < key_size ? pt->key_size : key_size) &&
		pt->key_size == key_size)
	{
		target = pt;
		t->head = pt->next;
		kfree(target);
		return 0;
	}

	ant = pt;
	pt = pt->next;
	
	while (pt){
		if (pt->app == app && 
			!memcmp(pt->key, key, pt->key_size < key_size ? pt->key_size : key_size) &&
			pt->key_size == key_size)
		{
			target = pt;
			ant->next = pt->next;
			kfree(target);
			return 0;
		}
		ant = pt;
		pt = pt->next;
	}
	return -1;
}
EXPORT_SYMBOL(dht_remove);


/*
 * Debug functions
 */

void dht_print(){
	struct dht_node_t* pt = NULL;
	
	if (!t){
		printk("dht_print(): There is no table to print.\n");
		return;
	}

	pt = t->head;

	if (!pt){
		printk("dht_print(): DHT is empty\n");
		return;	
	}

	while (pt){
		dht_print_node(pt);
		pt = pt->next;
	}
}
EXPORT_SYMBOL(dht_print);


void dht_print_node(struct dht_node_t* n){
	printk("%s%u\t%s%u\t%s%s\t%s%u\t%s%s\t%s%u\t%s%s\n",
			"app: ", n->app,
			"idx: ", n->idx,  
			"key: ", (char*) n->key, 
			"key_size: ", n->key_size,
			"data : ", (char*) n->data,
			"data_size: ", n->data_size,
			"next? ", n->next ? "yes" : "no");
}