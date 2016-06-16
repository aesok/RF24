/* 
 * File:   spi.h
 * Author: Purinda Gunasekara <purinda@gmail.com>
 * 
 * Created on 24 June 2012, 11:00 AM
 */

#ifndef SPI_H
#define	SPI_H

/**
 * @file spi.h
 * \cond HIDDEN_SYMBOLS
 * Class declaration for SPI helper files
 */
 
 /**
 * Example GPIO.h file
 *
 * @defgroup SPI SPI Example
 *
 * See RF24_arch_config.h for additional information
 * @{
 */
 
#include <string>
#include <mutex>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <span.h>
#include <scope>

#ifndef RF24_SPIDEV_SPEED
/* 8MHz as default */
#define RF24_SPIDEV_SPEED 8000000
#endif

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

using namespace std;

static auto
make_spi_handle (const std::string& name)
{
  using namespace std::experimental;

  return make_unique_resource_checked(::open(name.c_str(), O_RDWR),
                                      -1, &::close);
}

using spi_handle = decltype (make_spi_handle (std::string ()));


class SPI {
public:
	
	/**
	* SPI constructor
	*/	 
	SPI(const std::string & device_);
	
	/**
	* Start SPI
	*/
	void begin ();

	/**
	* Transfer a buffer of data
	* @param buf buffer of data
	*/
	void transfer (gsl::span<uint8_t> const & data);
	
	/**
	* Transfer a buffer of data
	* @param tbuf Transmit buffer
	* @param rbuf Receive buffer
	* @param len Length of the data
	*/
	void transfernb(char* tbuf, char* rbuf, uint32_t len);

	/**
	* Transfer a buffer of data without an rx buffer
	* @param buf Pointer to a buffer of data
	* @param len Length of the data
	*/	
	void transfern(char* buf, uint32_t len);
	
	virtual ~SPI();

private:

  static constexpr uint8_t bits_per_word = 8;

	/** Default SPI device */
	string device;
	/** SPI Mode set */
	uint8_t mode;
	/** Set SPI speed*/
	uint32_t speed;

  spi_handle h;
  std::mutex mtx;
};

/**
 * \endcond
 */
/*@}*/
#endif	/* SPI_H */

