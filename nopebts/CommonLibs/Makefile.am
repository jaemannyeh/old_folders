
#
# Copyright 2008, 2009 Free Software Foundation, Inc.
# Copyright 2011, 2012, 2014 Range Networks, Inc.
#
# This software is distributed under the terms of the GNU Public License.
# See the COPYING file in the main directory for details.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

#noinst_LTLIBRARIES = libcommon.la
lib_LTLIBRARIES = libcommon.la

libcommon_la_CXXFLAGS = -lpthread -lcoredumper -lsqlite3
libcommon_la_SOURCES = \
	BitVector.cpp \
	Configuration.cpp \
	LinkedLists.cpp \
	Logger.cpp \
	Reporting.cpp \
	SelfDetect.cpp \
	Sockets.cpp \
	sqlite3util.cpp \
	Threads.cpp \
	Timeval.cpp \
	UnixSignal.cpp \
	URLEncode.cpp \
	Utils.cpp \
	Variables.cpp

noinst_PROGRAMS = \
	BitVectorTest \
	ConfigurationTest \
    F16Test \
	LogTest \
	RegexpTest \
	UtilsTest
	
BitVectorTest_SOURCES = BitVectorTest.cpp
BitVectorTest_LDADD = libcommon.la

ConfigurationTest_SOURCES = ConfigurationTest.cpp
ConfigurationTest_LDADD = libcommon.la

F16Test_SOURCES = F16Test.cpp

LogTest_SOURCES = LogTest.cpp
LogTest_LDADD = libcommon.la

RegexpTest_SOURCES = RegexpTest.cpp
RegexpTest_LDADD = libcommon.la

UtilsTest_SOURCES = UtilsTest.cpp
UtilsTest_LDADD = libcommon.la
