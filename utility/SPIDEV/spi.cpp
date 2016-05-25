/* 
 * File:   spi.cpp
 * Author: Purinda Gunasekara <purinda@gmail.com>
 * 
 * Created on 24 June 2012, 11:00 AM
 * 
 * Inspired from spidev test in linux kernel documentation
 * www.kernel.org/doc/Documentation/spi/spidev_test.c 
 */

#include "spi.h"

#include <pthread.h>
static pthread_mutex_t spiMutex;

struct spi_ioc_transfer_ {
        __u64           tx_buf;
        __u64           rx_buf;

        __u32           len;
        __u32           speed_hz;


        __u16           delay_usecs;
        __u16           interbyte_usecs;
        __u8            bits_per_word;
        __u8            cs_change;
        __u32           pad;
};


#define SPI_MSGSIZE_(N) \
        ((((N)*(sizeof (struct spi_ioc_transfer_))) < (1 << _IOC_SIZEBITS)) \
                 ? ((N)*(sizeof (struct spi_ioc_transfer_))) : 0)
#define SPI_IOC_MESSAGE_(N) _IOW(SPI_IOC_MAGIC, 0, char[SPI_MSGSIZE_(N)])

SPI::SPI():fd(-1) {
}

void SPI::begin(int busNo){

	this->device = "/dev/spidev0.0";
    /* set spidev accordingly to busNo like:
     * busNo = 23 -> /dev/spidev2.3
     *
     * a bit messy but simple
     * */
    this->device[11] += (busNo / 10) % 10;
    this->device[13] += busNo % 10;
	this->bits = 8;
	this->speed = RF24_SPIDEV_SPEED;
	this->mode=0;
    //this->mode |= SPI_NO_CS;
	this->init();
}

void SPI::init()
{
	int ret;
    
    if (this->fd < 0)  // check whether spi is already open
    {
	  this->fd = open(this->device.c_str(), O_RDWR);

      if (this->fd < 0)
      {
        perror("can't open device");
        abort();
      }
    }

	/*
	 * spi mode
	 */
	ret = ioctl(this->fd, SPI_IOC_WR_MODE, &this->mode);
	if (ret == -1)
	{
		perror("can't set spi mode");
		abort();
	}

	ret = ioctl(this->fd, SPI_IOC_RD_MODE, &this->mode);
	if (ret == -1)
	{
		perror("can't set spi mode");
		abort();				
	}
	
	/*
	 * bits per word
	 */
	ret = ioctl(this->fd, SPI_IOC_WR_BITS_PER_WORD, &this->bits);
	if (ret == -1)
	{
		perror("can't set bits per word");
		abort();				
	}

	ret = ioctl(this->fd, SPI_IOC_RD_BITS_PER_WORD, &this->bits);
	if (ret == -1)
	{
		perror("can't set bits per word");
		abort();						
	}
	/*
	 * max speed hz
	 */
	ret = ioctl(this->fd, SPI_IOC_WR_MAX_SPEED_HZ, &this->speed);
	if (ret == -1)
	{
		perror("can't set max speed hz");
		abort();						
	}

	ret = ioctl(this->fd, SPI_IOC_RD_MAX_SPEED_HZ, &this->speed);
	if (ret == -1)
	{
		perror("can't set max speed hz");
		abort();						
	}
}

uint8_t SPI::transfer(uint8_t tx_)
{
    pthread_mutex_lock (&spiMutex);
	int ret;
  	uint8_t tx[1] = {tx_};
	uint8_t rx[1];
     
    this->init();
    struct spi_ioc_transfer_ tr;

	tr.tx_buf = (unsigned long)&tx[0];
	tr.rx_buf = (unsigned long)&rx[0];
	tr.len = 1;
	tr.cs_change = 1;
	tr.delay_usecs = 0;
	tr.bits_per_word = bits;
        tr.speed_hz = speed;

	ret = ioctl(this->fd, SPI_IOC_MESSAGE_(1), &tr);
	if (ret < 1)
	{
        pthread_mutex_unlock (&spiMutex);
		perror("can't send spi message 1");
		abort();		
	}

    pthread_mutex_unlock (&spiMutex);
	return rx[0];
}

//void bcm2835_spi_transfernb(char* tbuf, char* rbuf, uint32_t len)
void SPI::transfernb(char* tbuf, char* rbuf, uint32_t len)
{
	
	pthread_mutex_lock (&spiMutex);
	int ret;

	this->init();
	struct spi_ioc_transfer_ tr;

	tr.tx_buf = (unsigned long)tbuf;
	tr.rx_buf = (unsigned long)rbuf;
	tr.len = len;
	tr.cs_change = 1;
	tr.delay_usecs = 0;
	tr.bits_per_word = bits;
        tr.speed_hz = speed;

//        printf("[Writing %i, %i, %i]\n", (unsigned int)(tr.tx_buf) ,(unsigned int)(tr.rx_buf) , tr. len);
//        printf("[Writing %i, %i, %i]\n",  tr.cs_change , tr.bits_per_word,  tr.speed_hz);
//        printf("[Writing %i, %i, %i]\n",  fd , tr.cs_change,  tr.delay_usecs);


	ret = ioctl(this->fd, SPI_IOC_MESSAGE_(1), &tr);
	if (ret < 1)
	{
            pthread_mutex_unlock (&spiMutex);
             perror("can't send spi message 2");
	  abort();
	}
    pthread_mutex_unlock (&spiMutex);
	//return rx[0];
}

void SPI::transfern(char* buf, uint32_t len)
{
    transfernb(buf, buf, len);
}


SPI::~SPI() {
    if (!(this->fd < 0))
	    close(this->fd);
}

