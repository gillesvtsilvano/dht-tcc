#include "dht.h"

struct dht_t* t = 0;

// Função para a inicialização do módulo
// Ainda com código de teste
int __init init_module(){
	uint8_t dst[6];
	char data[6];

	dev = first_net_device(&init_net);
	while (dev){
		if (memcmp(dev->name, "wlp2s0", 6) == 0)
			break;
		dev = next_net_device(dev);
	}
	
	if (!dev)
		return;
	
	printk(KERN_INFO "DHT module initializing\n");

	memcpy(dst, "\x52\x54\x00\x55\x3a\x88", 6);
	memcpy(data, "Hello", 5);

	/*printk("dst: %02x:%02x:%02x:%02x:%02x:%02x\tdata: %s\n",
	 dst[0], dst[1], dst[2], dst[3], dst[4], dst[5], data);*/
	
	seq = 9;	
	dht_create();

	dev_add_pack(&dht_pkt_type);

	dht_craft_msg_insert(dst, 6, data);

	dht_craft_msg_remove(dst, nbt_hash_func(data, 6));
	
	return 0;
}

// Função chamada na desativação do módulo
void cleanup_module(){
	printk(KERN_INFO "DHT module exiting\n");
	dev_remove_pack(&dht_pkt_type);
}

static struct packet_type dht_pkt_type = {
	.type = PROTO_TYPE,
	.func = dht_rcv,
};


// Cria uma nova dht
// Futuramente será possível passar parâmetros para a dht a ser criada
// É necessário fazer alguns estudos sobre a possibilidade de se ter várias DHT's
void dht_create(){
	t = (struct dht_t*) kmalloc(sizeof(struct dht_t*), GFP_KERNEL);
	t->head = 0;
}


// Remove a dht criada e desaloca toda a estrutura alocada
void dht_destroy(){
	struct dht_node_t* pt, *target;
	if (t == 0)
		return;
	if (t->head == 0){
		kfree(t);
		return;
	}
	
	pt = t->head;
	
	do{
		target = pt;
		pt = pt->next;
		kfree(target);	
	}while (pt != 0);	

	kfree(t);
}


// Cria uma função para inserção de dados em uma dht
void dht_craft_msg_insert(uint8_t* dst, uint8_t data_size, void* data){
	struct sk_buff *skb;
	uint8_t hlen, tlen, *p, ret;
	struct dht_msg_insert* msg;
	
	// Alocação de um cabeçalho para a tecnologia correta obtida através do dispositivo
	hlen = LL_RESERVED_SPACE(dev);
	// Alocação de um 'tail' caso seja necessário
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
	
	// Criando
	msg = (struct dht_msg_insert*) kmalloc(sizeof(struct dht_msg_insert), GFP_KERNEL);
	msg->seq = seq++;
	//msg->msg_type = DHT_INSERT_ID;
	msg->family = DHT_INSERT_ID;
	msg->key = nbt_hash_func(data, data_size);
	msg->data_size = data_size;
	memcpy(msg->data, data, data_size);

	memcpy(p, msg, sizeof(struct dht_msg_insert));
	
	ret = dev_queue_xmit(skb);
	if (ret == NET_XMIT_SUCCESS)
		printk("craft_msg_insert(): Ok!\n");
	else
		printk("craft_msg_insert(): Fail!\n");
}

void dht_craft_msg_remove(uint8_t* dst, uint32_t key){
	struct sk_buff *skb;
	uint8_t hlen, tlen, *p, ret;
	struct dht_msg_remove* msg;

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
	
	// Creating data structure
	msg = (struct dht_msg_remove*) kmalloc(sizeof(struct dht_msg_remove), GFP_KERNEL);
	msg->seq = seq++;
	//msg->msg_type = DHT_REMOVE_ID;
	msg->family = DHT_REMOVE_ID;
	msg->key = key;

	memcpy(p, msg, sizeof(struct dht_msg_remove));
	
	ret = dev_queue_xmit(skb);
	if (ret == NET_XMIT_SUCCESS)
		printk("craft_msg_insert(): Ok!\n");
	else
		printk("craft_msg_insert(): Fail!\n");
}

void dht_craft_msg_retrive(uint8_t* dst, uint32_t key){
	struct sk_buff *skb;
	uint8_t hlen, tlen, *p, ret;
	struct dht_msg_remove* msg;

	hlen = LL_RESERVED_SPACE(dev);
	tlen = dev->needed_tailroom;
	
	skb = alloc_skb(dev->hard_header_len + sizeof(struct dht_msg_retrive) + hlen + tlen, GFP_ATOMIC);

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
	
	// Creating data structure
	msg = (struct dht_msg_remove*) kmalloc(sizeof(struct dht_msg_remove), GFP_KERNEL);
	msg->seq = seq++;
	//msg->msg_type = DHT_REMOVE_ID;
	msg->family = DHT_REMOVE_ID;
	msg->key = key;

	memcpy(p, msg, sizeof(struct dht_msg_remove));
	
	ret = dev_queue_xmit(skb);
	if (ret == NET_XMIT_SUCCESS)
		printk("craft_msg_insert(): Ok!\n");
	else
		printk("craft_msg_insert(): Fail!\n");
}

void dht_craft_msg_response(uint8_t* dst, uint16_t rSeq){

}

void dht_craft_msg_retrive_response(uint8_t* dst, uint16_t rSeq, uint8_t data_size, void* data){

}

void dht_craft_msg_confirm(uint8_t dst, uint16_t rSeq){
	struct sk_buff *skb;
	uint8_t hlen, tlen, *p, ret;
	struct dht_msg_remove* msg;

	hlen = LL_RESERVED_SPACE(dev);
	tlen = dev->needed_tailroom;
	
	skb = alloc_skb(dev->hard_header_len + sizeof(struct dht_msg_confirm) + hlen + tlen, GFP_ATOMIC);

	skb->protocol = htons(PROTO_TYPE);
	skb->dev = dev;
	skb->priority = TC_PRIO_CONTROL;
	skb_reset_network_header(skb);
	skb_put(skb, dev->hard_header_len + sizeof(struct dht_msg_confirm));
	p = skb->data;
	memcpy(p, dst, ETH_ALEN);
	p += ETH_ALEN;
	memcpy(p, dev->dev_addr, ETH_ALEN);
	p += ETH_ALEN;
	memcpy(p, &skb->protocol, 2);
	p += 2;
	
	// Creating data structure
	msg = (struct dht_msg_confirm*) kmalloc(sizeof(struct dht_msg_confirm), GFP_KERNEL);
	msg->seq = rSeq;
	//msg->msg_type = DHT_REMOVE_ID;
	msg->family = DHT_CONFIRM_ID;

	memcpy(p, msg, sizeof(struct dht_msg_confirm));
	
	ret = dev_queue_xmit(skb);
	if (ret == NET_XMIT_SUCCESS)
		printk("craft_msg_insert(): Ok!\n");
	else
		printk("craft_msg_insert(): Fail!\n");	
}


static int dht_rcv(struct sk_buff* skb, struct net_device* dev, struct packet_type *dht_pkt_type, struct net_device* orig){
	uint16_t rSeq;
	uint8_t rType;
	uint8_t *p;
	uint8_t src_addr[6];
	uint8_t dst_addr[6];
	uint8_t data_size_aux;
	void* data;
	bool is_broad_cast = false;

	skb = skb_share_check(skb, GFP_ATOMIC);
	if (!skb)
		return -1;

	printk("Rcv(): Got a packet\n");
	p = skb->data;
	rSeq = *p;
	p += 2;
	rType = *p;
	p += 1;

	printk("Rcv(): seq = %x\ttype = %x\n", rSeq, rType);

	memcpy(src_addr, "\xfe\x54\x00\x55\x3a\x88" , 6);
	
	switch(rType){
		case DHT_INSERT_ID:
			printk("Rcv(): Got an Insert Id\n");
			if (!dht_handle_insert(p)){
				dht_craft_msg_response(src_addr, rSeq);
			} else {
				return -1;
			}
			dht_print();
			break;	
		case DHT_REMOVE_ID:
			printk("Rcv(): Got an Remove Id\n");
			if (!dht_handle_remove(p)){
				dht_craft_msg_response(src_addr, rSeq);
			} else {
				return -1;
			}	
			dht_print();
			break;
		case DHT_RETRIVE_ID:
			data = kmalloc(256, GFP_KERNEL);
			if (dht_handle_retrive(p, &data_size_aux, data)){
				dht_craft_msg_retrive_response(src_addr, rSeq, data_size_aux, data);	
			}
			kfree(data);
			break;
		case DHT_RESPONSE_ID:
			if (!is_broad_cast){
				if (rSeq == seq){
					//Ok
				} else {
					return -1;
				}
			}
			break;
		case DHT_NONE_ID:
			break;
		default:
			break;
	}	
	
	return 0;
}

int dht_handle_insert(uint8_t* data){
	
	return 0;
}

int dht_handle_remove(uint8_t* data){

	return 0;
}

int dht_handle_retrive(uint8_t* data, uint8_t* data_size, void* ret_data){
	return 0;
}

int dht_handle_response(uint8_t* data){

	return 0;
}

void dht_insert(uint8_t app, size_t dsize, void* d) {
	/*
	 * Create a package to send to the correct dht
	 */
	struct dht_node_t* pt = 0, *ant = 0, *newbie = 0;

	//char* k = strsub(d, ksize);
	uint32_t new_hash = nbt_hash_func(d, dsize);
	
	//printk("dht_insert(): hash of %s = %u\n", k, (unsigned int) new_hash); 
	pt = t->head;
	ant = 0;
	
	newbie = (struct dht_node_t*) kmalloc(sizeof(struct dht_node_t), GFP_KERNEL);
	newbie->key = new_hash;
	//newbie->type = app;
	newbie->family = app;
//	newbie->key_size = ksize;
	newbie->data = kmalloc(dsize, GFP_KERNEL);
	memcpy(newbie->data, d, dsize);
	newbie->next = 0;

	if (pt == 0){
		newbie->next = 0;
		t->head = newbie;
		return;
	}
		
//	printk(KERN_INFO "dht_insert(): Searching a place to put the new node\n");
	
	if (pt->key > new_hash){
		newbie->next = pt;
		t->head = newbie;	
		return;
	}

	ant = pt;
	pt = pt->next;
	
	while (pt != 0){
		if (pt->key > new_hash){
			newbie->next = pt;
			ant->next = newbie;	
			return;
		}
		ant = pt;
		pt = pt->next;
	}	

	ant->next = newbie;
	return;
}
EXPORT_SYMBOL(dht_insert);

void dht_remove(uint32_t _key){
	struct dht_node_t* pt = t->head, *ant = 0, *target = 0;
	char* k;
	//char* k = strsub(pt->data, pt->key_size);
	
	uint32_t target_hash = nbt_hash_func(&_key, sizeof(_key));

	//printk("dht_remove(): hash of %s = %u\n", k, (unsigned int) target_hash); 
	if (pt == 0) {
		return;
	}
	
	
	//printk("dht_remove(): compare between %d and %d hashes\n", (int) pt->key, (int) target_hash );
	if (pt->key == target_hash){
		k = strsub(pt->data, sizeof(pt->key));
		//printk("dht_remove(): compare between %s and %s keys\n", _key, k);
		if (strcmp(_key, k) == 0){
			target = pt;	
			t->head = pt->next;
		}		
	}

	ant = pt;
	pt = pt->next;
	
	while (target == 0 && pt != 0){
		//printk("dht_remove(): compare between %d and %d hashes\n", (int) pt->key, (int) target_hash );
		if (pt->key == target_hash){
			k = strsub(pt->data, sizeof(pt->key));
			//printk("dht_remove(): compare between %s and %s keys\n", _key, k);
			if (strcmp(_key, k) == 0){
				ant->next = pt->next;
				target = pt;	
			}	
		}
		ant = pt;
		pt = pt->next;
	}
	kfree(target);
}
EXPORT_SYMBOL(dht_remove);


/*
 * Auxiliar functions
 */
char* strsub(const char* s, size_t n){
        char* dst, *pt;
        int i;
	dst = pt = 0;

	//printk("strsub(): { s  = %s, n = %d }\n", s, (int) n);
        dst = (char*) kmalloc(n + 1, GFP_KERNEL);
	
        pt = dst;
        for ( i = 0; i < n; ++i, ++pt){
                *pt = *(s + i);
        }
	*pt='\0';
	//printk("strsub(): return { dst  = %s(%d), orig = %s }\n", dst, (int) sizeof(dst), s);
        return dst;
}


/*
 * Debug functions
 */

void dht_print(){
	struct dht_node_t* pt = t->head;
	
	printk("dht_print():\n");
	if (pt == 0){
		printk(KERN_INFO "DHT is empy\n");
		return;	
	}
	while (pt != 0){
		dht_print_node(pt);
		pt = pt->next;
	}
}
EXPORT_SYMBOL(dht_print);

void dht_print_node(struct dht_node_t* n){
	char* key_data = strsub(n->data, sizeof(n->key));
	printk(KERN_INFO "%s%010u\t%s%u\t%s%s\t%s%s\t%s%s\n",
			"KEY: ", n->key, 
			"TYPE: ", n->family,//n->type,
			"KEY_DATA: ", key_data,
			"DATA : ", (char*) (n->data + sizeof(n->key)),
			"NEXT? ", n->next == 0 ? "null" : "yes");
	kfree(key_data);
}

