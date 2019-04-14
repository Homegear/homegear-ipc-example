/* Copyright 2013-2019 Homegear GmbH
 *
 * Homegear is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * Homegear is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with Homegear.  If not, see
 * <http://www.gnu.org/licenses/>.
 * 
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU Lesser General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
*/

#include "IpcClient.h"

#include <signal.h>

std::unique_ptr<IpcClient> _ipcClient;

void terminate(int32_t signalNumber)
{
	if(signalNumber == SIGTERM || signalNumber == SIGINT)
	{
		Ipc::Output::printMessage("Shutting down (Signal: " + std::to_string(signalNumber) + ")");
		_ipcClient->stop();
		Ipc::Output::printMessage("Shutdown complete.");
		exit(0);
	}
}

int main(int argc, char* argv[])
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = terminate;

	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);

	//Set debug level to "info"
	Ipc::Output::setLogLevel(4);

	//Change the path to the correct location of "homegearIPC.sock"
	_ipcClient.reset(new IpcClient("/var/run/homegear/homegearIPC.sock"));
	_ipcClient->start();

	Ipc::Output::printMessage("Startup complete.");

	while(true) std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	return 0;
}
