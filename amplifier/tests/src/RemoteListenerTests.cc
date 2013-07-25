/*****************************************************************************/ 
/* RemoteListenerTest.cc for BlasterBox Server Tests                         */
/* Copyright (c) 2013 Tom Hartman (rokstar83@gmail.com)                      */
/*                                                                           */
/* This program is free software; you can redistribute it and/or             */
/* modify it under the terms of the GNU General Public License               */
/* as published by the Free Software Foundation; either version 2            */
/* of the License, or the License, or (at your option) any later             */
/* version.                                                                  */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU General Public License for more details.                              */
/*****************************************************************************/
#include "RemoteListenerTests.hh"
#include "../../src/RemoteListener.hh"
#include "MockCommandQueue.hh"
#include "MockRemoteClient.hh"
#include "StdOutLogger.hh"
#include "FileLogger.hh"
#include "../../src/RemoteCommandList.hh"
#include <thread>
#include <vector>
#include <string>

namespace BlasterBox {

	 class thread_guard {
			std::thread & _t;
			RemoteListener & _r;
	 public:
			explicit thread_guard(std::thread & t, RemoteListener & r) : _t(t), _r(r) {}
			~thread_guard() {if(_t.joinable()) { _r.stopListening(); _t.join(); } }
			thread_guard(thread_guard const &)=delete;
			thread_guard & operator=(thread_guard const &)=delete;
	 };
 
	 void RemoteListenerTests::setUp() { }

	 void RemoteListenerTests::tearDown() { }

	 void RemoteListenerTests::testConn() 
	 {
			MockCommandQueue mockQueue;
			StdOutLogger logger;
			RemoteListener listener(mockQueue, logger);
			MockRemoteClient client;

			std::thread listenerThread(&RemoteListener::listenLoop, 
																 std::ref(listener));
			thread_guard g(listenerThread, listener);

			// Spin until initialized
			while(!listener.isInitialized()) {}

			std::vector<MockRemoteClient> clients(MAX_REMOTE_CONNECTIONS);

			for(int x = 0; x < clients.size(); ++x) {				 
				 CPPUNIT_ASSERT_NO_THROW(clients[x].connect());
				 std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				 CPPUNIT_ASSERT(listener.hasConnections());
				 CPPUNIT_ASSERT(listener.numConnections() == x+1);
			}
	 }

	 void RemoteListenerTests::testProcessSocketBuffer(std::vector<unsigned char> & buf) 
	 {
			MockCommandQueue mockQueue;
			StdOutLogger logger;
			RemoteListener listener(mockQueue, logger);
			std::string str = "HELLO, WORLD";

			std::vector<unsigned char> buffer;

			for_each(str.begin(), str.end(), [] (char c) {
						buffer.push_back((unsigned char)c);
				 });

			/* First test incomplete dataset remains unchanged */
			listener.processSocketBuffer(buffer);
			
			for(int x = 0; x < str.size(); ++x) {
				 CPPUNIT_ASSERT(buffer[x] == str[x]);
			}

			/* Now test a complete message being processed */
			buffer.push_back(MSG_TERM);
			listener.processSocketBuffer(buffer);
			
			/* Buffer should be empty */
			CPPUNIT_ASSERT(buffer.size() == 0);

			/* the command queue should have one command with a message */
			CPPUNIT_ASSERT(mockCommandQueue.numCommands() == 1);
			CPPUNIT_ASSERT(mockCommandQueue.top().to_string() == str);

			/* Now tests a complete message being processed with extra data */
			str = "HELLO, WORLD\0HELLO";
			for_each(str.begin(), str.end(), [] (char c) {
						buffer.push_back((unsigned char)c);
				 });
			listener.processSocketBuffer(buffer);

			/* the buffer should have whats after the first message */
			str = "HELLO";
			for(int x = 0; x < str.size(); ++x) {
				 CPPUNIT_ASSERT(buffer[x] == str[x]);
			}
			
			str = "HELLO, WORLD";
			CPPUNIT_ASSERT(mockCommandQueue.numCommands() == 1);
			CPPUNIT_ASSERT(mockCommandQueue.top().to_string() == str);
	 }
}
