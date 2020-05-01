#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/timer.h>
#include <linux/mutex.h>
#include <linux/skbuff.h>


#include <net/inet_sock.h>
#include <net/sock.h>

const static char *b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// maps A=>0,B=>1..
const static unsigned char unb64[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //10
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //20
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //30
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //40
    0,
    0,
    0,
    62,
    0,
    0,
    0,
    63,
    52,
    53, //50
    54,
    55,
    56,
    57,
    58,
    59,
    60,
    61,
    0,
    0, //60
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    2,
    3,
    4, //70
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    14, //80
    15,
    16,
    17,
    18,
    19,
    20,
    21,
    22,
    23,
    24, //90
    25,
    0,
    0,
    0,
    0,
    0,
    0,
    26,
    27,
    28, //100
    29,
    30,
    31,
    32,
    33,
    34,
    35,
    36,
    37,
    38, //110
    39,
    40,
    41,
    42,
    43,
    44,
    45,
    46,
    47,
    48, //120
    49,
    50,
    51,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //130
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //140
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //150
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //160
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //170
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //180
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //190
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //200
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //210
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //220
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //230
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //240
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //250
    0,
    0,
    0,
    0,
    0,
    0,
}; // This array has 256 elements

#define PHY_PACKET_DATA_SIZE 48 // must be > 48, preferably just 48
#define PHY_PACKET_CONTROL_SIZE 15
#define IPOA_MTU 576

MODULE_AUTHOR("Bolaji Bankole (mbankole)");
MODULE_LICENSE("GPL");

typedef struct phy_packet_t phy_packet_t;
typedef struct phy_packet_buff_t phy_packet_buff_t;

// struct for each element of the send buffer
struct phy_packet_t
{ // can send 64 bytes at a time?
    // split into control and data
    char data[PHY_PACKET_DATA_SIZE];
    union control
    {
        char raw[PHY_PACKET_CONTROL_SIZE];
        struct fields {
            bool is_first;
            short data_len;
            short seq_num;
            short total_packets;
            short sk_len;
            short sk_data_len;
            short sk_mac_len;
            short sk_hdr_len;
            __u16 sk_transport_header;
            __u16 sk_network_header;
            __u16 sk_mac_header;
            __be16 sk_protocol;
        } fields;
    } control;
    
    phy_packet_t *next;
};

// struct for maintaining the buffer
struct phy_packet_buff_t
{ // can send 64 bits at a time?
    phy_packet_t *head;
    phy_packet_t *tail;
    unsigned int len;
    spinlock_t lock;
};

struct net_device *ipoa;
static dev_t ipoa_char_device_no;

static struct device *ipoa_char_device;

static struct class *ipoa_char_class;

phy_packet_buff_t *phy_packet_buff;
phy_packet_buff_t *rx_phy_packet_buff;

static int timer_delay = 10; // check the buffers at 100hz, 
static struct timer_list ipoa_timer;
static long int timer_data = 40;

phy_packet_buff_t *new_buff(void)
{
    phy_packet_buff_t *buff = kmalloc(sizeof(phy_packet_buff_t), GFP_KERNEL);
    buff->head = NULL;
    buff->tail = NULL;
    buff->len = 0;
    spin_lock_init(&(buff->lock));
    return buff;
}

phy_packet_t *new_data_packet(void)
{
    phy_packet_t *packet = kmalloc(sizeof(phy_packet_t), GFP_KERNEL);
    packet->next = NULL;
    memset(packet->data, 0, PHY_PACKET_CONTROL_SIZE);
    return packet;
}

// Converts binary data of length=len to base64 characters.
// Length of the resultant string is stored in flen
// (you must pass pointer flen).
char *base64(const void *binaryData, int len, int *flen)
{
    const unsigned char *bin = (const unsigned char *)binaryData;
    char *res;

    int rc = 0; // result counter
    int byteNo; // I need this after the loop

    int modulusLen = len % 3;
    int pad = ((modulusLen & 1) << 1) + ((modulusLen & 2) >> 1); // 2 gives 1 and 1 gives 2, but 0 gives 0.

    *flen = 4 * (len + pad) / 3;
    res = (char *)kmalloc(*flen + 1, GFP_KERNEL); // and one for the null
    if (!res)
    {
        return 0;
    }

    for (byteNo = 0; byteNo <= len - 3; byteNo += 3)
    {
        unsigned char BYTE0 = bin[byteNo];
        unsigned char BYTE1 = bin[byteNo + 1];
        unsigned char BYTE2 = bin[byteNo + 2];
        res[rc++] = b64[BYTE0 >> 2];
        res[rc++] = b64[((0x3 & BYTE0) << 4) + (BYTE1 >> 4)];
        res[rc++] = b64[((0x0f & BYTE1) << 2) + (BYTE2 >> 6)];
        res[rc++] = b64[0x3f & BYTE2];
    }

    if (pad == 2)
    {
        res[rc++] = b64[bin[byteNo] >> 2];
        res[rc++] = b64[(0x3 & bin[byteNo]) << 4];
        res[rc++] = '=';
        res[rc++] = '=';
    }
    else if (pad == 1)
    {
        res[rc++] = b64[bin[byteNo] >> 2];
        res[rc++] = b64[((0x3 & bin[byteNo]) << 4) + (bin[byteNo + 1] >> 4)];
        res[rc++] = b64[(0x0f & bin[byteNo + 1]) << 2];
        res[rc++] = '=';
    }

    res[rc] = 0; // NULL TERMINATOR! ;)
    return res;
}

unsigned char *unbase64(const char *ascii, int len, int *flen)
{
    const unsigned char *safeAsciiPtr = (const unsigned char *)ascii;
    unsigned char *bin;
    int cb = 0;
    int charNo;
    int pad = 0;

    if (len < 2)
    { // 2 accesses below would be OOB.
        // catch empty string, return NULL as result.
        *flen = 0;
        return 0;
    }
    if (safeAsciiPtr[len - 1] == '=')
        ++pad;
    if (safeAsciiPtr[len - 2] == '=')
        ++pad;

    *flen = 3 * len / 4 - pad;

    bin = (unsigned char *)kmalloc(*flen, GFP_KERNEL);
    if (!bin)
    {
        return 0;
    }

    for (charNo = 0; charNo <= len - 4 - pad; charNo += 4)
    {
        int A = unb64[safeAsciiPtr[charNo]];
        int B = unb64[safeAsciiPtr[charNo + 1]];
        int C = unb64[safeAsciiPtr[charNo + 2]];
        int D = unb64[safeAsciiPtr[charNo + 3]];

        bin[cb++] = (A << 2) | (B >> 4);
        bin[cb++] = (B << 4) | (C >> 2);
        bin[cb++] = (C << 6) | (D);
    }

    if (pad == 1)
    {
        int A = unb64[safeAsciiPtr[charNo]];
        int B = unb64[safeAsciiPtr[charNo + 1]];
        int C = unb64[safeAsciiPtr[charNo + 2]];

        bin[cb++] = (A << 2) | (B >> 4);
        bin[cb++] = (B << 4) | (C >> 2);
    }
    else if (pad == 2)
    {
        int A = unb64[safeAsciiPtr[charNo]];
        int B = unb64[safeAsciiPtr[charNo + 1]];

        bin[cb++] = (A << 2) | (B >> 4);
    }

    return bin;
}

// adds things to the back of the buffer
void buff_add(phy_packet_t *new, phy_packet_buff_t *buff)
{
    spin_lock(&(buff->lock));
    if (buff->head == NULL || buff->tail == NULL) // empty case
    {
        buff->head = new;
        buff->tail = new;
    }
    else // otherwise, should be able to put it in
    {
        buff->tail->next = new;
        buff->tail = new;
    }
    buff->len++;
    spin_unlock(&(buff->lock));
}

phy_packet_t *buff_pop(phy_packet_buff_t *buff)
{
    spin_lock(&(buff->lock));
    phy_packet_t *ret = buff->head;
    // if empty, none of these will happen and ret will remain null
    if (buff->len == 1) // if one item, null out the buffer pointers
    {
        buff->head = NULL;
        buff->tail = NULL;
        buff->len--;
    }
    else if (buff->len > 1)
    { // more than one, shift it over
        buff->head = buff->head->next;
        buff->len--;
    }
    spin_unlock(&(buff->lock));
    return ret;
}

unsigned int buff_len(phy_packet_buff_t *buff)
{
    return buff->len;
}

int ipoa_open(struct net_device *dev)
{
    printk("ipoa_open called\n");
    netif_start_queue(dev);
    return 0;
}

int ipoa_release(struct net_device *dev)
{
    printk("ipoa_release called\n");
    netif_stop_queue(dev);
    return 0;
}

int ipoa_receive(struct sk_buff *skb, struct net_device *dev)
{
    // https://ufal.mff.cuni.cz/~jernej/2018/docs/predavanja15.pdf
    // will need to have some background thing listening, and then interrupt into this somehow? maybe add a file handler and it just writes in
    // netif_receive_skb(struct sk_buff* skb); // NEED TO BE FROM INTERRUPT CONTEXT!?!?!

    unsigned char *data = skb->data;
    unsigned int datalen = skb->data_len;
    unsigned int len = skb->len;

    unsigned short protocol = skb->protocol;

    dev->stats.rx_packets++;
    dev->stats.rx_bytes += len;

    printk("IPOA receive function called, protocol = %d, datalen = %u, len = %u\n", protocol, datalen, len);

    //netif_rx(skb);
    return 0;
}

// take in the skb, get the mutex for the buffer (should probably have that)
// split the data in the thing into chunks of 64 bits, put them in the buffer
// then transmit everything in the buffer freeing each chunk, then return
netdev_tx_t ipoa_xmit(struct sk_buff *skb,
                      struct net_device *dev)
{
    // skb also has a LOT of other stuff
    // https://elixir.bootlin.com/linux/v4.4/source/include/linux/skbuff.h#L545

    // should transmit cb (control block?)

    // so for each skb, we need to transmit the cb (48 bytes) plus all the data

    unsigned char *data = skb->data;
    unsigned int datalen = skb->data_len;
    unsigned int len = skb->len;
    unsigned int handled_bytes = 0;
    unsigned int packet_data_len = 0;
    phy_packet_t *first_packet = NULL;
    phy_packet_t *data_packet = NULL;

    unsigned short protocol = skb->protocol;

    int i = 1;

    dev->stats.tx_packets++;
    dev->stats.tx_bytes += len;

    printk("IPOA xmit function called, protocol = %d, datalen = %u, len = %u\n", protocol, datalen, len);

    // length of cb plus the actual data
    unsigned int total_bytes = 48 + len + skb->hdr_len;

    unsigned int packets = (total_bytes / PHY_PACKET_DATA_SIZE) + (total_bytes % PHY_PACKET_DATA_SIZE == 0 ? 0 : 1);

    printk("Adding %d bytes to send buffer in %d packets\n", total_bytes, packets);

    // will always send at least one packet with the cb
    first_packet = new_data_packet();
    first_packet->control.fields.is_first = 1;
    first_packet->control.fields.data_len = 48;      // control 0 is the length of valid data
    first_packet->control.fields.seq_num = 1;       // one here means it is the first packet in this sequence
    first_packet->control.fields.total_packets = packets; // total number of packets to expect in this sequence
    first_packet->control.fields.sk_len = skb->len;
    first_packet->control.fields.sk_data_len = skb->data_len;
    first_packet->control.fields.sk_mac_len = skb->mac_len;
    first_packet->control.fields.sk_hdr_len = skb->hdr_len;
    first_packet->control.fields.sk_transport_header = skb->transport_header;
    first_packet->control.fields.sk_network_header = skb->network_header;
    first_packet->control.fields.sk_mac_header = skb->mac_header;
    first_packet->control.fields.sk_protocol = protocol; // just pack more things in here i guess


    memcpy(first_packet->data, skb->cb, 48);
    handled_bytes += 48;
    buff_add(first_packet, phy_packet_buff);

    total_bytes -= 48; // we have dealt with the first 48 bytes now

    // now, add more data as necessary?
    for (i = 1; i < packets; i++)
    {
        data_packet = new_data_packet();
        packet_data_len = total_bytes > PHY_PACKET_DATA_SIZE ? PHY_PACKET_DATA_SIZE : total_bytes;

        data_packet->control.fields.is_first = 0;
        data_packet->control.fields.data_len = packet_data_len;
        data_packet->control.fields.seq_num = i + 1;
        data_packet->control.fields.total_packets = packets;

        memcpy(data_packet->data, skb->head + handled_bytes, packet_data_len);

        handled_bytes += packet_data_len;
        total_bytes -= packet_data_len;

        buff_add(data_packet, phy_packet_buff);
    }

    printk("IPOA internal buffer length is %d, added %d bytes of data\n", buff_len(phy_packet_buff), handled_bytes);

    // ipoa_receive(skb, dev);

    dev_kfree_skb(skb);

    return NETDEV_TX_OK; // https://elixir.bootlin.com/linux/v4.4/source/include/linux/netdevice.h#L110 things say you should never do anything other than ok?
}

int ipoa_inner_init(struct net_device *dev)
{
    printk("IPOA device inner initialized\n");
    return 0;
}

int ipoa_set_mac_address(struct net_device *dev, void *addr)
{
    char *char_addr = (char *)addr;
    printk("IPOA set mac address (meaningless for us) to %s\n", char_addr);
    return 0;
}

void ipoa_uninit(struct net_device *dev)
{
    printk("IPOA device uninitialized\n");
}

void ipoa_init(struct net_device *dev)
{
    // https://docs.oracle.com/en/operating-systems/oracle-linux/6/porting/ch06s07.html

    char *mac = "deadbeefdead";

    // gotta allocate this in kernel space
    struct net_device_ops *net_dev_ops = kmalloc(sizeof(struct net_device_ops), GFP_KERNEL);
    memset(net_dev_ops, 0, sizeof(struct net_device_ops));

    net_dev_ops->ndo_init = ipoa_inner_init;
    net_dev_ops->ndo_uninit = ipoa_uninit;
    net_dev_ops->ndo_open = ipoa_open;
    net_dev_ops->ndo_stop = ipoa_release;
    net_dev_ops->ndo_start_xmit = ipoa_xmit;
    net_dev_ops->ndo_set_mac_address = ipoa_set_mac_address;

    dev->netdev_ops = net_dev_ops;

    memcpy(dev->perm_addr, mac, strlen(mac));
    memcpy(dev->dev_addr, mac, strlen(mac));

    dev->hw_features = 0;
    dev->mtu = IPOA_MTU;
    dev->features = 0;
    dev->ifindex = 0;                        // from the kernel code on bootlin, seems this has to be init to 0 to get hte kernel to generate one
    dev->addr_assign_type = NET_ADDR_RANDOM; // idk just setting it to SOMETHING
    dev->flags = IFF_BROADCAST | IFF_NOARP;  // no idea for this one

    printk("IPOA device initialized\n");
}

void ipoa_cleanup(void)
{
    printk("IPOA Cleaning Up the Module\n");

    device_destroy(ipoa_char_class, MKDEV(ipoa_char_device_no, 0));

    class_unregister(ipoa_char_class);

    class_destroy(ipoa_char_class);

    unregister_chrdev(ipoa_char_device_no, "IPOA");

    del_timer(&ipoa_timer);

    unregister_netdev(ipoa);
    free_netdev(ipoa);
    kvfree(phy_packet_buff);
    kvfree(rx_phy_packet_buff);

    printk("IPOA Clean up done!\n");

    return;
}

static int ipoa_char_open(struct inode *inodep, struct file *filep)
{
    printk("IPOA chardev opened\n");
    return 0;
}

// 
static ssize_t ipoa_char_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    ssize_t bytes;
    char text[128];
    char *textCursor;
    int i;

    bytes = sprintf(text, "%d, %d, %d, %d, %d\n", 1, 2, 3, 4, 5);
    textCursor = (char *)text;

    // copy the data into the buffer wowow
    for (i = 0; i < bytes && i < len; i++)
    {
        put_user(*(textCursor++), buffer++);
    }

    return bytes;
}

// take the line, alloc it to a buffer element thing, add it to the received list
static ssize_t ipoa_char_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    int base, parse_retval;
    long parsed;
    char s[len];
    if (len == 0)
    {
        return -1;
    }
    memcpy(s, (void *)buffer, len);
    s[len] = '\0';
    base = 10;
    parse_retval = kstrtol(s, base, &parsed);
    if (parse_retval == -ERANGE || parse_retval == -EINVAL ||
        parsed < -100 || parsed > 100)
    {
        printk(KERN_INFO "Invalid PWM setting: %li%%\n", parsed);
        return (ssize_t)len;
    }
    printk(KERN_INFO "Setting PWM to %li%%\n", parsed);

    return (ssize_t)len;
}

static int ipoa_char_release(struct inode *inodep, struct file *filep)
{
    printk("IPOA char device released\n");
    return 0;
}

static const struct file_operations fops =
    {
        .open = ipoa_char_open,
        .read = ipoa_char_read,
        .write = ipoa_char_write,
        .release = ipoa_char_release,
};

void ipoa_timer_callback(unsigned long data)
{
    // for now, just pop from the outbound buffer and call receive with it???

    phy_packet_t* packet = buff_pop(phy_packet_buff);
    unsigned int copied_data_bytes = 0;

    // this one is local to here, copy to another when passing to the receive function
    static struct sk_buff* r_skb = NULL;

    struct sk_buff* local_skb = NULL;

    // will be building a local skb struct, reset on first packets, finish it on last packets

    // always assume its just one phy packet worth for now
    if (packet != NULL) {
        if (packet->control.fields.seq_num == 1) {
            
            if (r_skb == NULL) { // if we dont have one yet, get one
                r_skb = alloc_skb(IPOA_MTU, GFP_KERNEL);
            } else {
                dev_kfree_skb(r_skb); // just fucking reset it damn fuck this shit
                r_skb = alloc_skb(IPOA_MTU, GFP_KERNEL);
            }
            
            printk("detected first packet\n");
            skb_reserve(r_skb, 64);
            copied_data_bytes = 0;
            memcpy(r_skb->cb, packet->data, 48);
            r_skb->data_len = packet->control.fields.sk_data_len;
            r_skb->mac_len = packet->control.fields.sk_mac_len;
            r_skb->hdr_len = packet->control.fields.sk_hdr_len;
            r_skb->transport_header = packet->control.fields.sk_transport_header;
            r_skb->network_header = packet->control.fields.sk_network_header;
            r_skb->mac_header = packet->control.fields.sk_mac_header;
            r_skb->protocol = packet->control.fields.sk_protocol;
            r_skb->dev = ipoa;
            printk("started skb with length %d\n", r_skb->len);
            kvfree(packet);
        } else if (packet->control.fields.seq_num == packet->control.fields.total_packets) {
            printk("detected last packet\n");
            memcpy(r_skb->head + copied_data_bytes, packet->data, packet->control.fields.data_len);
            skb_put(r_skb, packet->control.fields.data_len);
            copied_data_bytes += packet->control.fields.data_len;
            kvfree(packet);
            skb_put(r_skb, packet->control.fields.sk_hdr_len); // i dont fucking know
            printk("finished up skb with length %d\n", r_skb->len);
            local_skb = skb_copy(r_skb, GFP_KERNEL);
            ipoa_receive(local_skb, ipoa);
            // eventually, copy and call the other thing
        } else {
            printk("detected general data packet\n");
            memcpy(r_skb->head + copied_data_bytes, packet->data, packet->control.fields.data_len);
            skb_put(r_skb, packet->control.fields.data_len);
            copied_data_bytes += packet->control.fields.data_len;
            printk("added to skb with length %d\n", r_skb->len);
            kvfree(packet);
        }
        // struct sk_buff* skb =
    
        // skb_reserve(skb, 64);//sk->sk_prot->max_header);
        
        // memcpy(skb->cb, packet->data, 48);
        // skb->data = skb_put(skb, packet->control.fields.sk_data_len);
        // skb->len = packet->control.fields.sk_len;
        // skb->data_len = packet->control.fields.sk_data_len;
        // skb->mac_len = packet->control.fields.sk_mac_len;
        // skb->hdr_len = packet->control.fields.sk_hdr_len;
        // skb->transport_header = packet->control.fields.sk_transport_header;
        // skb->network_header = packet->control.fields.sk_network_header;
        // skb->mac_header = packet->control.fields.sk_mac_header;
        // skb->protocol = packet->control.fields.sk_protocol;
        // skb->dev = ipoa;
        // ipoa_receive(skb, ipoa);
    }



    volatile int ret_val;
    /* print log */
    // printk("IPOA kernel timer callback executing, data is %ld\n", data);
    /* setup timer interval to msecs */
    ret_val = mod_timer(&ipoa_timer, jiffies + msecs_to_jiffies(timer_delay));
    if (ret_val)
    {
        printk("Cant set timer, error occurred\n");
    }
}

int ipoa_init_module(void)
{
    int result;
    // register the network stuff
    phy_packet_buff = new_buff();
    rx_phy_packet_buff = new_buff();
    ipoa = alloc_netdev(4096,
                        "IPOA",
                        NET_NAME_ENUM, // kernel will enumerate it??
                        ipoa_init);    // need to use alloc_netdev?

    printk("IPOA register start\n");
    if ((result = register_netdev(ipoa)))
    {
        printk("ipoa: Error %d  initializing card ipoa", result);
        return result;
    }
    printk("IPOA register done!\n");

    // register the char device stuff
    ipoa_char_class = class_create(THIS_MODULE, "ipoa_char_class");

    printk("IPOA chardev register\n");
    ipoa_char_device_no = register_chrdev(0, "IPOA", &fops);

    if (ipoa_char_device_no < 0)
    {
        printk("Failed to register IPOA chardev\n");
        return ipoa_char_device_no;
    }

    ipoa_char_device = device_create(ipoa_char_class,
                                     NULL,
                                     MKDEV(ipoa_char_device_no, 0),
                                     NULL,
                                     "IPOA");

    setup_timer(&ipoa_timer, ipoa_timer_callback, timer_data);
    /* setup timer interval to msecs */

    result = mod_timer(&ipoa_timer, jiffies + msecs_to_jiffies(timer_delay));

    if (result)
    {
        printk("Failed to init timer!\n");
    }

    printk("IPOA init done!\n");
    return 0;
}

module_init(ipoa_init_module);
module_exit(ipoa_cleanup);