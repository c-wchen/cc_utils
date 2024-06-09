
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <assert.h>
#include <endian.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <infiniband/verbs.h>
#include <rdma/rdma_cma.h>

// gcc -o rdma_verbs rdma_verbs.c -libverbs -lrdmacm -lpthread -g -O0
// https://blog.csdn.net/winux/article/details/51772742

#define ERR_SUCCESS 0
#define LOG_INFO(fmt, ...) printf("[INFO]: "fmt "\n", ##__VA_ARGS__)
#define LOG_ERR(fmt, ...)  printf("[ERROR]: "fmt "\n", ##__VA_ARGS__)
#define LOG_DBG(fmt, ...)  printf("[DBG]: "fmt "\n", ##__VA_ARGS__)

#define LOG_ASSERT(cond)                                                        \
	do {                                                                        \
		if (!(cond)) {                                                          \
			printf("LOG_ASSERT: [%s:%d] (" #cond ")\n", __func__, __LINE__);    \
			assert(cond);                                                       \
		}                                                                       \
	} while(0)

#define LOG_ASSERT_F(cond, fmt, ...)                                                                           \
	do {                                                                                                       \
		if (!(cond)) {                                                                                         \
			printf("LOG_ASSERT: [%s:%d] " fmt "(" #cond ")\n", __func__, __LINE__, ##__VA_ARGS__);             \
			assert(cond);                                                                                      \
		}                                                                                                      \
	} while(0)

struct ibv_qp *qp_create(struct ibv_pd *pd, struct ibv_cq *cq) {
	struct ibv_qp_init_attr qp_init_attr = {
		.send_cq = cq,
		.recv_cq = cq,
		.qp_type = IBV_QPT_RC,
		.cap = {
			.max_send_wr = 2,
			.max_recv_wr = 2,
			.max_send_sge = 1,
			.max_recv_sge = 1
		}
	};

	struct ibv_qp *qp = ibv_create_qp(pd, &qp_init_attr);

	LOG_ASSERT(qp != NULL);
	return qp;
}

void qp_destroy(struct ibv_qp *qp) {
	LOG_ASSERT(qp != NULL);
	int ret = ibv_destroy_qp(qp);
	LOG_ASSERT(ret == 0);
}

void dev_show(struct ibv_device **devs, int dev_num) {
    printf("    %-16s\t   node GUID\n", "device");
	printf("    %-16s\t----------------\n", "------");
    for (int i = 0; i < dev_num; ++i) {
		printf("    %-16s\t%016llx\n",
            ibv_get_device_name(devs[i]),
            (unsigned long long) be64toh(ibv_get_device_guid(devs[i])));
	}
}

typedef struct {
    struct rdma_cm_id *cm_id;
    struct ibv_pd *pd;
    struct ibv_comp_channel *channel;
    struct ibv_cq* cq;
} wverbs_req_t;

static int verbs_create_qp(wverbs_req_t *req, struct rdma_cm_id *cm_id) {
    struct ibv_context* rdma_context = cm_id->verbs;
    req->cm_id = cm_id;
     /* 创建内存protection domain, 看作内存保护单位，和内存区域和队列建立关联关系，防止未授权访问 */
	req->pd = ibv_alloc_pd(rdma_context);
    if (req->pd == NULL) {
        LOG_ERR("alloc pd faild.");
        return -EFAULT;
    }
    /* 与event_channel类似，用来报告完成队列事件，当完成队列中有新任务完成时，通过channel向应用程序报告 */
	req->channel = ibv_create_comp_channel(rdma_context);
    if (req->channel == NULL) {
        LOG_ERR("create complete channel faild.");
        return -EFAULT;
    }

    /* 创建完成队列 */
	req->cq = ibv_create_cq(rdma_context, 512, NULL, req->channel, 0);
    if (req->cq == NULL) {
        LOG_ERR("create complete queue faild.");
        return -EFAULT;
    }
    int rc = ibv_req_notify_cq(req->cq, 0);
    if (rc != ERR_SUCCESS) {
        LOG_ERR("request notify CQ faild.");
        return rc;
    }

    struct ibv_qp_init_attr qp_attr= {
        .send_cq = req->cq,
        .recv_cq = req->cq,
        .qp_type = IBV_QPT_RC,
        .cap = {
            .max_send_wr = 512,
            .max_recv_wr = 512,
            .max_send_sge = 1,
            .max_recv_sge = 1
        }
    };
    rc = rdma_create_qp(req->cm_id, req->pd, &qp_attr);

    if (rc != ERR_SUCCESS) {
        LOG_ERR("create queue pair faild.");
        return rc;
    }
    return ERR_SUCCESS;
}

static int verbs_mem_alloc(struct rdma_cm_id *cm_id, struct ibv_sge *sge, uint32_t size) {
    void *buf = aligned_alloc(4096, size);
    memset(buf, 0, size);

     /* ibv_reg_mr注册内存区域，RDMA使用的内存必先注册，这个可以理解DMA的内存边界对齐 */
    struct ibv_mr *buf_mr = ibv_reg_mr(cm_id->pd, buf, 4096, IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ);
    if (buf_mr == NULL) {
        free(buf);
        return -EFAULT;
    }

    sge->addr = (uint64_t)buf;
    sge->length = 4096;
    sge->lkey = buf_mr->lkey;

    return ERR_SUCCESS;
}

static void verbs_show_peer(struct rdma_cm_id *id) {
	struct sockaddr *peer_addr =  rdma_get_peer_addr(id);
	char ip_str[64];
	inet_ntop(AF_INET, &(((struct sockaddr_in *)peer_addr)->sin_addr), ip_str, sizeof(ip_str));
	LOG_INFO("cm from peer:%s .", ip_str);
    return;
}

int handle_client_cm_request(struct rdma_cm_event* event)
{
    int rc = ERR_SUCCESS;

    /* 客户端CM ID */
    struct rdma_cm_id* cm_id = event->id;
   
	
    wverbs_req_t req;
    rc = verbs_create_qp(&req, cm_id);
    LOG_ASSERT_F(rc == ERR_SUCCESS, "create qp faild.");

    struct ibv_recv_wr client_recv_wr, *bad_client_recv_wr = NULL;
    struct ibv_sge client_recv_sge;

    rc = verbs_mem_alloc(cm_id, &client_recv_sge, 4096);
    LOG_ASSERT_F(rc == ERR_SUCCESS, "mem alloc faild.");

    client_recv_wr.wr_id = (uint64_t)1;
    client_recv_wr.next = NULL;
    client_recv_wr.sg_list = &client_recv_sge;
    client_recv_wr.num_sge = 1;

    rc = ibv_post_recv(cm_id->qp, &client_recv_wr, &bad_client_recv_wr);
    LOG_ASSERT_F(rc == ERR_SUCCESS, "ibv_post_recv faild.");

	
    struct rdma_conn_param cm_params = {0};
	cm_params.responder_resources = (uint8_t)16;
	cm_params.initiator_depth = (uint8_t)16;
	cm_params.retry_count = 7;
	cm_params.rnr_retry_count = 7;
    // 接受客户端请求
	rc = rdma_accept(cm_id, &cm_params);
    LOG_ASSERT_F(rc == ERR_SUCCESS, "rdma_accept faild.");
	
	verbs_show_peer(cm_id);

    struct ibv_wc wc[8];
    void *cq_ctx = NULL;
    int n = 0;
    ibv_get_cq_event(req.channel, &req.cq, &cq_ctx);
    ibv_ack_cq_events(req.cq, 1);
    ibv_req_notify_cq(req.cq, 0);
	LOG_INFO("begin polling CQ");
    while((n = ibv_poll_cq(req.cq, 8, wc))) {   
        for(int i = 0; i < n; i++) {   
            long user_data = wc[i].wr_id;
			assert(user_data == 1);
            if(wc[i].status != IBV_WC_SUCCESS){
            	LOG_INFO("wc[%d].status != IBV_WC_SUCCESS, wc.status: %s", i, ibv_wc_status_str(wc[i].status));
				return -EIO;
            }
            LOG_INFO("recv:%d bytes, '%.*s'", wc[i].byte_len, wc[i].byte_len, client_recv_sge.addr);
        }
    }
	return rc;
}

static void *verbs_cm_thread(void *arg)
{
    struct rdma_event_channel *event_channel = (struct rdma_event_channel *)(arg);
    struct rdma_cm_event *event = NULL;

    LOG_INFO("start cm thread process.");
    while (1) {
        int rc = rdma_get_cm_event(event_channel, &event);
        if (rc != ERR_SUCCESS) {
            LOG_ERR("rdma get cm event faild, rc = %d.", rc);
            rdma_ack_cm_event(event); 
            continue;
        }

        struct rdma_cm_event new_event = *event;
        rdma_ack_cm_event(event);

        switch (event->event) {
            case RDMA_CM_EVENT_CONNECT_REQUEST: {
                LOG_INFO("handle RDMA_CM_EVENT_CONNECT_REQUEST and enqueue.");
                handle_client_cm_request(&new_event);
                break;
            }
            case RDMA_CM_EVENT_ESTABLISHED:
            case RDMA_CM_EVENT_ADDR_RESOLVED:
            case RDMA_CM_EVENT_ROUTE_RESOLVED:
            case RDMA_CM_EVENT_CONNECT_RESPONSE:
            case RDMA_CM_EVENT_ADDR_ERROR:
            case RDMA_CM_EVENT_ROUTE_ERROR:
            case RDMA_CM_EVENT_CONNECT_ERROR:
            case RDMA_CM_EVENT_UNREACHABLE:
            case RDMA_CM_EVENT_REJECTED:
            case RDMA_CM_EVENT_DEVICE_REMOVAL:
            default: {
                LOG_INFO("ignore event %s.", rdma_event_str(event->event));
            }
        }
    }
    return NULL;
}


static void socket_init(struct sockaddr_in *sin, const char *ipaddr, uint16_t port) {
    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = inet_addr("192.168.2.160");
    sin->sin_port = htons(8080);
}

/* w rdma context */
typedef struct {
    struct rdma_cm_id *cm_id;
    struct rdma_event_channel *cm_channel;
    pthread_t cm_thread;
} wrnet_context_t;

static int verbs_create_ctx(wrnet_context_t *context) {
    /* rdma设备操作完成后，或者连接请求事件发生时，用来通知引用程序的通道，其内部时FD， 可以使用poll操作 */
    context->cm_channel = rdma_create_event_channel();
    if (NULL == context->cm_channel) {
        LOG_ERR("create event channcel faild.");
        return -errno;
    }

    struct sockaddr_in sin = {0};
    socket_init(&sin, "192.168.2.160", 8080);
    
    /* 创建rdma_cm_id, 等皆与socket编程中socket() */
    int rc = rdma_create_id(context->cm_channel, &context->cm_id, NULL, RDMA_PS_TCP);
    if (rc != ERR_SUCCESS) {
        LOG_ERR("create cm id faild.");
        return rc;
    }
    /* 绑定网卡和端口 */
    rc = rdma_bind_addr(context->cm_id, (struct sockaddr *)&sin);
    if (rc != ERR_SUCCESS) {
        return rc;
    }
    LOG_DBG("rdma bind addr %s port %hd success.", "192.168.2.160", ntohs(rdma_get_src_port(context->cm_id)));
    /* 监听客户端请求,非阻塞调用，当客户端连接时，一个新的连接管理（CM)事件生成到RDMA CM chnannel中 */
    rc = rdma_listen(context->cm_id, 10);

    /* 创建处理cm事件线程 */
    rc = pthread_create(&context->cm_thread, NULL, verbs_cm_thread, context->cm_channel);

    if (rc != ERR_SUCCESS) {
        LOG_ERR("create cm thread faild.");
    }

    return ERR_SUCCESS;
}

int main() {
    bool server = true;
	int dev_num = 0;
    int rc = 0;
	struct ibv_device **devs = ibv_get_device_list(&dev_num);
	LOG_INFO("devs = %p, num_dev = %d", devs, dev_num);
    dev_show(devs, dev_num);


    struct rdma_cm_id *cm_id = NULL;
    if (server) {
        wrnet_context_t context = {0};
        int rc = verbs_create_ctx(&context);
        LOG_ASSERT_F(rc == ERR_SUCCESS, "create context faild, rc = %d", rc);
       
        while (1) {
            sleep(1);
        }
    }
	return 0;
}