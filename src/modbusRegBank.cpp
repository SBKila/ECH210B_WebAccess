#include <modbusRegBank.h>
#include <stdlib.h>

modbusRegBank::modbusRegBank(void)
{
	_anaRegs = 0;
}

void modbusRegBank::add(word addr)
{
	modbusAnaReg *temp;

	temp = (modbusAnaReg *)malloc(sizeof(modbusAnaReg));
	temp->address = addr;
	temp->value = 0;
	temp->next = _anaRegs;
	_anaRegs = temp;
}

boolean modbusRegBank::get(word addr, word &value)
{
	modbusAnaReg *regPtr;
	regPtr = (modbusAnaReg *)this->search(addr);
	if (regPtr)
	{
		value = regPtr->value;
		return true;
	}
	else
		return false;
}
void modbusRegBank::remove(word addr)
{
	modbusAnaReg *prevPtr = 0;
	modbusAnaReg *regPtr = _anaRegs;

	//if there is no register configured, bail
	if (regPtr == 0)
		return;

	//scan through the linked list until the end of the list or the register is found.
	//return the pointer.
	do
	{
		if (regPtr->address == addr)
		{
			if (prevPtr)
			{
				prevPtr->next = regPtr->next;
			}
			else
			{
				_anaRegs = regPtr->next;
			}
			free(regPtr);
			return;
		}
		prevPtr = regPtr;
		regPtr = regPtr->next;
	} while (regPtr);
}
void modbusRegBank::set(word addr, word value)
{
	modbusAnaReg *regPtr;
	//search for the register address
	regPtr = (modbusAnaReg *)this->search(addr);
	//if found then assign the register value to the new value.
	if (regPtr)
	{
		regPtr->value = value;
	}
	else
	{
		add(addr);
		set(addr, value);
	}
}

void *modbusRegBank::search(word addr)
{
	//if the requested address is 0-19999
	//use a digital register pointer assigned to the first digital register
	//else use a analog register pointer assigned the first analog register

	modbusAnaReg *regPtr = _anaRegs;

	//if there is no register configured, bail
	if (regPtr == 0)
		return (0);

	//scan through the linked list until the end of the list or the register is found.
	//return the pointer.
	do
	{
		if (regPtr->address == addr)
			return (regPtr);
		regPtr = regPtr->next;
	} while (regPtr);

	return (0);
}
