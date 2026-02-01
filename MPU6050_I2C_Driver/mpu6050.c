#include<linux/module.h>
#include<linux/i2c.h>
#include<linux/kernel.h>
#include<linux/of.h>
#include<linux/device.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/delay.h>
#include<linux/uaccess.h>

#define MPU6050_ADDR   (0x68 << 1)   // 7-bit address 0x68 shifted for HAL (8-bit)
#define WHO_AM_I_REG   0x75
#define PWR_MGMT_1     0x6B
#define ACCEL_XOUT_H   0x3B



// functions declarations

static int mpu6050_probe(struct i2c_client *client, const struct i2c_device_id *dev_id);
static int mpu6050_remove(struct i2c_client *client);
static int m_open(struct inode *m_inode, struct file *m_file);
static int m_close(struct inode *m_inode, struct file *m_file);
static ssize_t m_write(struct file *m_file, const char *m_buf, size_t m_size, loff_t *m_offset);
static ssize_t m_read(struct file *m_file, char *m_buf, size_t m_size, loff_t *m_offset);


// i2c_device_id for matching the device with the module
const struct i2c_device_id mpu6050_id[] = {
	{"mpu6050",0},
	{},

};

// add it it with the kernel
MODULE_DEVICE_TABLE(i2c,mpu6050_id);

// higher precedence of_match table of = open firmware

static const struct of_device_id mpu6050_table[] = {
	{.compatible = "invensense,mpu6050"},
	{},
};

MODULE_DEVICE_TABLE(of,mpu6050_table);



// i2c driver info and matching

static struct i2c_driver driver_info={
	.driver={
		.name = "mpu6050",
	},

	.probe = mpu6050_probe,
	.remove = mpu6050_remove,
	.id_table = mpu6050_id,

};

// register the driver

//module_i2c_driver(driver_info);

static dev_t devNo;
static struct class *mclass;
struct ref_struct *ref;
//static struct cdev mcdev;

static struct file_operations f_ops={
	.open = m_open,
	.release = m_close,
	.read = m_read,
	.write = m_write,
};


typedef struct ref_struct{
	struct cdev m_cdev;
	struct i2c_client *client; 
}ref_struct;



static int __init i2c_init(void){

	int ret;
	static struct device *mdevice;
	

	
	pr_info("MPU6050 init is called for %s\n",THIS_MODULE->name);
	ref = (struct ref_struct*)kmalloc(1 * sizeof(struct ref_struct),GFP_KERNEL);

	if(ref == NULL){
		ret = -ENOMEM;
		pr_err("kmalloc failed\n");
		goto kmalloc_failed;
	}
	ret = alloc_chrdev_region(&devNo,0,1,"mpu6050");
	if(ret < 0){
		pr_err(" %s alloc_chrdev_region failed \n",THIS_MODULE->name);
		goto alloc_chrdev_region_failed;
	}

	mclass = class_create(THIS_MODULE,"mpu_class");
	if(IS_ERR(mclass)){
		pr_err("class create failed\n");
		ret = -1;
		goto class_create_failed;
	}

	mdevice = device_create(mclass,NULL,devNo,NULL,"mpu6050%d",0);
	if(IS_ERR(mdevice)){
		pr_err("device create failed\n");
		ret = -1;
		goto device_create_failed;
	}

	cdev_init(&ref->m_cdev,&f_ops);
	ret = cdev_add(&ref->m_cdev,devNo,1);
	if(ret < 0){
		pr_err("cdev add failed\n");
		goto cdev_add_failed;
	}


	
	ret = i2c_add_driver(&driver_info);
	if(ret < 0){
		pr_err("i2c_add_driver failed\n");
		goto i2c_add_driver_failed;
	}

	return 0;

i2c_add_driver_failed:
	cdev_del(&ref->m_cdev);
cdev_add_failed:
	device_destroy(mclass,devNo);
device_create_failed:
	class_destroy(mclass);
class_create_failed:
	unregister_chrdev_region(devNo,1);
alloc_chrdev_region_failed:
	kfree(ref);
kmalloc_failed:
	return ret;
}


static void __exit i2c_exit(void){
	
	pr_info("MPU6050 exit is called for %s\n",THIS_MODULE->name);

	i2c_del_driver(&driver_info);
	cdev_del(&ref->m_cdev);
	device_destroy(mclass,devNo);
	class_destroy(mclass);
	unregister_chrdev_region(devNo,1);
	kfree(ref);
}

// will be called when device is detected on the i2c bus2 
static int mpu6050_probe(struct i2c_client *client, const struct i2c_device_id *dev_id){
	int is_mpu,ret;
	pr_info("Probe is called for %s\n",THIS_MODULE->name);
	ref->client = client;
	// check if the device is mpu6050 only = 0x68
	is_mpu = i2c_smbus_read_byte_data(client,WHO_AM_I_REG);

	if(is_mpu < 0){
		pr_err("mpu6050 is not detected wrong address get for %s\n",THIS_MODULE->name);
		return -1;
	}
	if(is_mpu != 0x68){
		pr_err("the addresse mismatch 0x%x\n",is_mpu);
		return -ENODEV;
	}
	pr_info("The mpu6050 successfuly identified at address 0x%x for %s\n",is_mpu,THIS_MODULE->name);

	// wake up the mpu6050

	ret = i2c_smbus_write_byte_data(client,PWR_MGMT_1,0x00);

	if(ret < 0){
		pr_err("wake up failed for mpu6050\n");
		return ret;
	}

	msleep(100);
	pr_info("mpu6050 wakeup successfully\n");
	// configure the gyro output rate   reg = 0x19 , val = 0x07

	ret = i2c_smbus_write_byte_data(client,0x19,0x07);

	if(ret < 0){
		pr_err("gyro configure failed\n");
		return ret;
	}

	pr_info("gyro configured successfully\n");
	// configure the DLPF reg = 0x1A , val = 0x00

	ret = i2c_smbus_write_byte_data(client,0x1A,0x00);
	if(ret < 0){
		pr_err("DLPF configuration failed\n");
		return ret;
	}

	pr_info("DLPF configured\n");
	// configure gyro range +250g reg = 0x1B , val = 0x00
	ret = i2c_smbus_write_byte_data(client,0x1B,0x00);
	if(ret < 0){
		pr_err("gyro range confugarion failed\n");
		return ret;
	}


	pr_info("gyro configured\n");
	// configure the accelometer range +2g  reg = 0x1C, val = 0x00
	ret = i2c_smbus_write_byte_data(client,0x1C,0x00);

	if(ret < 0){
		pr_err("accelerometer range configuration failed\n");
		return ret;
	}

	pr_info("accelerometer configured\n");



	return 0;
}


// will be called when the device is removed from the i2c bus2
static int mpu6050_remove(struct i2c_client *client){
	pr_info("remove is called for %s\n",THIS_MODULE->name);

	return 0;
}


static int m_open(struct inode *m_inode, struct file *m_file){

	m_file->private_data = container_of(m_inode->i_cdev,struct ref_struct,m_cdev);
	pr_info("%s open is called\n",THIS_MODULE->name);


	return 0;
}
static int m_close(struct inode *m_inode, struct file *m_file){

	pr_info("%s close is called\n",THIS_MODULE->name);

	return 0;
}
static ssize_t m_write(struct file *m_file, const char *m_buf, size_t m_size, loff_t *m_offset){
	pr_info("%s write is called\n",THIS_MODULE->name);

	return 0;
}


static ssize_t m_read(struct file *m_file, char __user *m_buf,
                      size_t m_size, loff_t *m_offset)
{
    u8 buffer[14];
    u8 reg = 0x3B;
    int ret;

    struct ref_struct *ref = m_file->private_data;
    struct i2c_client *client = ref->client;
    struct i2c_msg msgs[2];

    pr_info("%s read is called\n", THIS_MODULE->name);

    if (*m_offset > 0)
        return 0;   // EOF after first read

    if (m_size < 14)
        return -EINVAL;

    msgs[0].addr  = client->addr;
    msgs[0].flags = 0;
    msgs[0].len   = 1;
    msgs[0].buf   = &reg;

    msgs[1].addr  = client->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len   = 14;
    msgs[1].buf   = buffer;

    ret = i2c_transfer(client->adapter, msgs, 2);
    if (ret != 2) {
        pr_err("%s i2c_transfer failed\n", THIS_MODULE->name);
        return -EIO;
    }

    if (copy_to_user(m_buf, buffer, 14)) {
        pr_err("%s copy_to_user failed\n", THIS_MODULE->name);
        return -EFAULT;
    }

    *m_offset += 14;
    return 14;
}



module_init(i2c_init);
module_exit(i2c_exit);


//MODULE_LICENCE("GPL");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("NIVO");
MODULE_DESCRIPTION("this is the i2c driver for the device mpu6050");
