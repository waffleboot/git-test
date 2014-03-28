struct list_head {
	struct list_head *next, *prev;
};

#define container_of(ptr,_struct_,name) \
	(_struct_*)((char*)ptr - (char*)(&((_struct_*)0)->name))

#define LIST_HEAD_INIT(name) {&(name),&(name)}

#define LIST(name) struct list_head name = LIST_HEAD_INIT(name)

#define list_entry(head,_struct_,name) \
	container_of(head,_struct_,name)

#define for_each_entry(pos,head) \
	for (pos = (head)->next; pos != head; pos = pos->next)

static void __list_add(struct list_head *new, struct list_head *prev, struct list_head *next) {
	new->prev=prev;
	new->next=next;
	prev->next=new;
	next->prev=new;
}

void list_add(struct list_head *new, struct list_head *head) {
	__list_add(new,head->prev,head);
}

struct device {
	char *name;
	void (*test) (struct device *dev);
	struct list_head list;
};

LIST(devices);

/*
 * эта штука публикуется всем драйверам для добавления своих устройств
 */
void add_device(struct device *dev) {
	printf("add device '%s'\n", dev->name);
	list_add(&dev->list,&devices);
}

static void test_devices() {
	struct list_head *pos;
	for_each_entry(pos,&devices) {
		struct device *dev = list_entry(pos,struct device,list);
		printf("run test for '%s' device\n", dev->name);
		dev->test(dev);
	}
}

extern void init_devices();

int main() {
	init_devices();
	test_devices();
	return 0;
}

/*
 * это мое устройство
 * могу обернуть struct device в мою структуру
 * а могу вынести ее в указатель
 */

struct mydevice {
	int num;
	struct device dev;
};

static void test_disk(struct device *dev) {
	struct mydevice *mydisk = container_of(dev,struct mydevice,dev);
	printf("'disk %i' is tested\n", mydisk->num);
}

#define PREFIX "disk"

static void init(int num) {
	struct mydevice *d = malloc(sizeof(struct mydevice));
	d->num = num;
	d->dev.name = malloc(sizeof(PREFIX) + sizeof(num) + 2);
	sprintf(d->dev.name, "disk %i", num);
	d->dev.test = test_disk;
	add_device(&d->dev);
}

void init_devices() {
	init(1);
	init(2);
}

