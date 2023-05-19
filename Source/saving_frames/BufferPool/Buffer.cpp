//
// Created by Daniel Kowalski on 07/03/2023.
//

#include "Buffer.hpp"
unsigned long Buffer::size() const
{
	return _size;
}
unsigned char *Buffer::data()
{
	return _data;
}
Buffer::Buffer(const unsigned long size)
{
	_size = size;
	_data = new unsigned char[_size];
	_counter = new ReferenceCounter();
}
Buffer& Buffer::operator=(const Buffer &buffer) {
	this->_size = buffer._size;
	this->_data = buffer._data;
	this->_counter = buffer._counter;
	_counter->increase();

	return *this;
}
Buffer::Buffer(const Buffer &buffer)
{
	this->_size = buffer._size;
	this->_data = buffer._data;
	this->_counter = buffer._counter;

	_counter->increase();
}
Buffer::~Buffer()
{
	if (_counter->decrease()) {
		delete[] _data;
		delete _counter;
	}
}
