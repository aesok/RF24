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

SPI::SPI (const std::string & device, uint32_t speed_hz, uint32_t mode)
  : h (make_spi_handle (device)), speed (speed_hz)
{
  int ret;

  if (h.get () < 0)
  {
    perror("can't open device");
    abort();
  }

  // Set spi mode
  ret = ::ioctl (h.get (), SPI_IOC_WR_MODE, &mode);
  if (ret == -1)
  {
    perror ("can't set spi mode");
    abort ();
  }

  // Set max speed hz
  ret = ::ioctl (h.get (), SPI_IOC_WR_MAX_SPEED_HZ, &speed);
  if (ret == -1)
  {
    perror ("can't set max speed hz");
    abort ();
  }
}

SPI::~SPI()
{
}

void SPI::begin()
{
}

void
SPI::transfer (gsl::span<uint8_t> const & data)
{
  std::lock_guard <std::mutex> guard (mtx);

  struct spi_ioc_transfer_ tr;
  tr.tx_buf = reinterpret_cast <typeof (tr.tx_buf)> (data.data ());
  tr.rx_buf = reinterpret_cast <typeof (tr.rx_buf)> (data.data ());
  tr.len = data.size ();
  tr.cs_change = 1;
  tr.delay_usecs = 0;
  tr.bits_per_word = bits_per_word;
  tr.speed_hz = speed;

  int ret = ::ioctl (h.get (), SPI_IOC_MESSAGE_(1), &tr);
  if (ret < 1)
  {
    perror("can't send spi message 2");
    abort();
  }
}

//void bcm2835_spi_transfernb(char* tbuf, char* rbuf, uint32_t len)
void SPI::transfernb(char* tbuf, char* rbuf, uint32_t len)
{
  std::lock_guard <std::mutex> guard (mtx);

	int ret;

	struct spi_ioc_transfer_ tr;

	tr.tx_buf = (unsigned long)tbuf;
	tr.rx_buf = (unsigned long)rbuf;
	tr.len = len;
	tr.cs_change = 1;
	tr.delay_usecs = 0;
	tr.bits_per_word = bits_per_word;
        tr.speed_hz = speed;

//        printf("[Writing %i, %i, %i]\n", (unsigned int)(tr.tx_buf) ,(unsigned int)(tr.rx_buf) , tr. len);
//        printf("[Writing %i, %i, %i]\n",  tr.cs_change , tr.bits_per_word,  tr.speed_hz);
//        printf("[Writing %i, %i, %i]\n",  fd , tr.cs_change,  tr.delay_usecs);


	ret = ::ioctl (h.get (), SPI_IOC_MESSAGE_(1), &tr);
	if (ret < 1)
	{
             perror("can't send spi message 2");
	  abort();
	}
	//return rx[0];
}

void SPI::transfern(char* buf, uint32_t len)
{
    transfernb(buf, buf, len);
}
