/*
 * ispi.hpp
 *
 *  Created on: Feb 28, 2019
 *      Author: rev
 */

#ifndef ISPI_HPP_
#define ISPI_HPP_


#include "iport.hpp"

class ISPI : public IPort
{
public:

	virtual ~ISPI();
	virtual void init() =0;
	virtual void sendByte(uint16_t data) =0;
	virtual uint16_t receiveData() =0;
};




#endif /* ISPI_HPP_ */
