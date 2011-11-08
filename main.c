#include <stdio.h>
#include <stdlib.h>
#include <virtio.h>
#include <virtio_client.h>
#include <virtio_server.h>
#include <linux/list.h>

int main(int argc, char **argv)
{
	void *buffer = malloc(131072);
	void *buffer2 = malloc(131072);
	struct virtio_device vdev = {0};
	struct virtqueue *vq;
	char *data = "my virtio msg";
	struct iovec iov[2], iov2[2] = {0};
	u16 in, out;

	iov[0].iov_base = data;
	iov[0].iov_len = 5;
	iov[1].iov_base = data + 5;
	iov[1].iov_len = 9;

	INIT_LIST_HEAD(&vdev.vqs);
	vq = vring_new_virtqueue(16, 4096, &vdev, buffer, buffer2, NULL, NULL, "test");
	if (virt_queue__available(to_vvq(vq)))
		printf("No buffers available - OK!\n");

	virtqueue_add_buf(vq, iov, 2, 0, NULL);
	virtqueue_kick(vq);
	if (virt_queue__available(to_vvq(vq)))
		printf("Buffers available - OK!\n");

	virt_queue__get_iov(to_vvq(vq), iov2, &in, &out);

	return 0;
}
