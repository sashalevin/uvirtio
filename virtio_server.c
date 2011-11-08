#include "virtio_server.h"

#include <stdio.h>
#include <linux/list.h>

struct virtqueue *vring_new_virtqueue(unsigned int num,
				      unsigned int vring_align,
				      struct virtio_device *vdev,
				      void *vq_addr,
				      void *pages,
				      void (*notify)(struct virtqueue *),
				      void (*callback)(struct virtqueue *),
				      const char *name)
{
	struct vring_virtqueue *vq = vq_addr;
	unsigned int i;

	/* We assume num is a power of 2. */
	if (num & (num - 1)) {
		printf("Bad virtqueue length %u\n", num);
		return NULL;
	}

	vring_init(&vq->vring, num, pages, vring_align);
	vq->vq.callback = callback;
	vq->vq.vdev = vdev;
	vq->vq.name = name;
	vq->notify = notify;
	vq->broken = false;
	vq->last_used_idx = 0;
	vq->num_added = 0;
	list_add_tail(&vq->vq.list, &vdev->vqs);
#ifdef DEBUG
	vq->in_use = false;
#endif

//	vq->indirect = virtio_has_feature(vdev, VIRTIO_RING_F_INDIRECT_DESC);
//	vq->event = virtio_has_feature(vdev, VIRTIO_RING_F_EVENT_IDX);

	/* No callback?  Tell other side not to bother us. */
	if (!callback)
		vq->vring.avail->flags |= VRING_AVAIL_F_NO_INTERRUPT;

	/* Put everything in free lists. */
	vq->num_free = num;
	vq->free_head = 0;
	for (i = 0; i < num-1; i++) {
		vq->vring.desc[i].next = i+1;
		vq->data[i] = NULL;
	}
	vq->data[i] = NULL;

	return &vq->vq;
}