#!/bin/bash
sed 's/double toDouble\(\)/\
	explicit operator double() const { return toDouble(); }\
	explicit operator float() const { return toDouble(); }\
	&/' softposit_cpp.h > a
