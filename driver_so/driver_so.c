#include <linux/kernel.h>	
#include <linux/module.h>	
#include <linux/fs.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#include "driver_so.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Henrique Scalon/Leonardo Invernizzi");
MODULE_DESCRIPTION("Disciplina SO");

int init_device(void);
void cleanup_device(void);
static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *file, char __user *buffer, size_t length,loff_t * offset);
static ssize_t device_write(struct file *file, const char __user * buffer, size_t length, loff_t * offset);
long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param);

module_init(init_device);
module_exit(cleanup_device)

static int aberto = 0;
static char mensagem[BUF_MSG];
static char * mensagem_ptr;

struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release,	
	.unlocked_ioctl = device_ioctl,
};

int init_device()
{
	int ret;

	ret = register_chrdev(DEVICE, DEVICE_NAME, &fops);

	if (ret < 0)
	{
		printk("Erro ao carregar o dispositivo %d\n.", DEVICE);
		return ret;
	}

	printk("O dispositivo %d foi carregado.\n", DEVICE);

	return 0;
}

void cleanup_device()
{
	unregister_chrdev(DEVICE, DEVICE_NAME);

	printk("O dispositivo %d foi descarregado.\n", DEVICE);
}

static int device_open(struct inode *inode, struct file *file)
{
	if (aberto)
		return -EBUSY;

	aberto++;

	mensagem_ptr = mensagem;

	try_module_get(THIS_MODULE);
	return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
	aberto--;

	module_put(THIS_MODULE);
	return 0;
}

static ssize_t device_read(struct file *file, char __user * buffer, size_t length, loff_t * offset)
{
	int bytes_lidos = 0;

	if (*mensagem_ptr == 0)
		return 0;

	while(length && *mensagem_ptr)
	{
		put_user(*(mensagem_ptr++), buffer++);
		length--;
		bytes_lidos++;
	}

	printk("Leu: %s\n", mensagem);

	return bytes_lidos;
}

static ssize_t device_write(struct file *file, const char __user * buffer, size_t length, loff_t * offset)
{	
	int i;

	printk("device_write: buffer: %s, length: %lu, mensagem: %s\n", buffer, length, mensagem);

	for (i = 0; i < length && i < BUF_MSG; i++)
	{
		get_user(mensagem[i], buffer + i);
	}

	mensagem_ptr = mensagem;
	
	printk("Escreveu: %s\n", mensagem_ptr);

	return i;
}

long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
	int i;
	char * temp = NULL, ch;

	switch (ioctl_num) 
	{
		case IOCTL_SET_POSITION:
			temp = (char *)ioctl_param;
			get_user(ch, temp);
			for (i = 0; ch && i < BUF_MSG; i++, temp++)
				get_user(ch, temp);

			device_write(file, (char *) ioctl_param, i, 0);
			break;
		case IOCTL_GET_POSITION:
			i = device_read(file, (char *)ioctl_param, BUF_MSG-1, 0);
			put_user('\0', (char *) ioctl_param + i);
			break;
		default:
			return FAILURE;
	}
	return SUCCESS;
}
