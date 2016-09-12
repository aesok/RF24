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

#include <span>
#include <scope>

using namespace std;

namespace spidev {

constexpr uint32_t default_speed = 8000000;


static auto
make_spi_handle (const std::string& name)
{
  using namespace std::experimental;

  return make_unique_resource_checked(::open(name.c_str(), O_RDWR),
                                      -1, &::close);
}

using spi_handle = decltype (make_spi_handle (std::string ()));

}

class SPI final {
public:
	
	/**
	* SPI constructor
	* @param device Device name
	* @param mode Device mode
	*/	 
	SPI (const std::string & device,
             uint32_t speed_hz = spidev::default_speed,
             uint32_t mode = SPI_MODE_0);
	
	~SPI();

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

private:

  static constexpr uint8_t bits_per_word = 8;

  spidev::spi_handle h;
  uint32_t speed;

  std::mutex mtx;
};

/**
 * \endcond
 */
/*@}*/
#endif	/* SPI_H */

