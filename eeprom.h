/*
 * eeprom.h
 *
 *  Created on: Mar 22, 2021
 *      Author: jorgejuarez
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#define STRSIZE 12 //max size i will allow user to enter

enum STATUS
{
    Error = -1, Found = 1, notFound = 255
};

void initEeprom();
void writeEeprom(uint16_t add, uint32_t data);
uint32_t readEeprom(uint16_t add);
void addInstruction(char *, uint8_t, uint8_t);
void clearEeprom();
void testCommand();
void printDectoBin(uint8_t x);
uint32_t findIndex(char * name);
void infoName(char * );
void eraseName(char *);
//void findInstruction(char *, uint8_t, uint8_t);
void infoIndex(uint16_t index);
void listCommands();
uint16_t getInfo(char * name);

#endif /* EEPROM_H_ */
