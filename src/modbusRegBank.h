
#include <stdint.h>
#include <Arduino.h>

#ifndef _MODBUSREGBANK
#define _MODBUSREGBANK

struct modbusAnaReg
{
	word address;
	word value;

	modbusAnaReg *next;
};

class modbusRegBank
{
	public:

		modbusRegBank(void);
		
		boolean get(word,word&);
		void set(word, word);
		void remove(word);				
	private:
		void add(word);
		void * search(word);

		modbusAnaReg	*_anaRegs,
						*_lastAnaReg;
};
#endif
