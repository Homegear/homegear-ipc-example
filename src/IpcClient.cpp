/* Copyright 2013-2017 Sathya Laufer
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

IpcClient::IpcClient(std::string socketPath) : IIpcClient(socketPath)
{
	//Bind local RPC methods
	_localRpcMethods.emplace("exampleTest1", std::bind(&IpcClient::test1, this, std::placeholders::_1));
	_localRpcMethods.emplace("exampleTest2", std::bind(&IpcClient::test2, this, std::placeholders::_1));
}

void IpcClient::onConnect()
{
	try
	{
		bool error = false;

		//Register RPC methods
		{
			//Create the parameter array
			Ipc::PArray parameters = std::make_shared<Ipc::Array>();
			parameters->reserve(2);

			//First parameter is of type "string" and the method name
			parameters->push_back(std::make_shared<Ipc::Variable>("exampleTest1"));

			//Second parameter is an array containing the method's signature. In this case the array is empty, meaning there are no parameters and the method returns "void".
			parameters->push_back(std::make_shared<Ipc::Variable>(Ipc::VariableType::tArray));

			//Call the RPC method "registerRpcMethod"
			Ipc::PVariable result = invoke("registerRpcMethod", parameters);
			if (result->errorStruct)
			{
				error = true;
				Ipc::Output::printCritical("Critical: Could not register RPC method test1: " + result->structValue->at("faultString")->stringValue);
			}
		}

		{
			//Create the parameter array
			Ipc::PArray parameters = std::make_shared<Ipc::Array>();
			parameters->reserve(2);

			//First parameter is of type "string" and the method name
			parameters->push_back(std::make_shared<Ipc::Variable>("exampleTest2"));

			//Second parameter is a nested array containing the method's signature. In this case the method returns "integer" and has one parameter of type "string".
			//The signature is encoded by sending empty variables. Each element of the outer array contains one signature. The inner array (= signature array) contains
			//the return parameter first followed by the arguments.
			parameters->push_back(std::make_shared<Ipc::Variable>(Ipc::VariableType::tArray));
			parameters->back()->arrayValue->push_back(std::make_shared<Ipc::Variable>(Ipc::VariableType::tInteger));
			parameters->back()->arrayValue->push_back(std::make_shared<Ipc::Variable>(Ipc::VariableType::tString));

			//Call the RPC method "registerRpcMethod". All RPC methods known by Homegear can be called this way.
			Ipc::PVariable result = invoke("registerRpcMethod", parameters);
			if (result->errorStruct)
			{
				error = true;
				Ipc::Output::printCritical("Critical: Could not register RPC method test2: " + result->structValue->at("faultString")->stringValue);
			}
		}

		if (!error) Ipc::Output::printInfo("Info: RPC methods successfully registered.");
	}
	catch (const std::exception& ex)
	{
		Ipc::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch (Ipc::IpcException& ex)
	{
		Ipc::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch (...)
	{
		Ipc::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
	}
}

// {{{ RPC methods
Ipc::PVariable IpcClient::test1(Ipc::PArray& parameters)
{
	try
	{
		Ipc::Output::printInfo("Test1 called");

		return Ipc::PVariable(new Ipc::Variable(std::string("Test1 ") + std::to_string(Ipc::HelperFunctions::getTimeSeconds())));
	}
	catch (const std::exception& ex)
	{
		Ipc::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch (Ipc::IpcException& ex)
	{
		Ipc::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch (...)
	{
		Ipc::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
	}
	return Ipc::Variable::createError(-32500, "Unknown application error.");
}

Ipc::PVariable IpcClient::test2(Ipc::PArray& parameters)
{
	try
	{
		if(parameters->size() != 1) return Ipc::Variable::createError(-1, "Method expects exactly one parameter. " + std::to_string(parameters->size()) + " given.");
		if(parameters->at(0)->type != Ipc::VariableType::tString) return Ipc::Variable::createError(-1, "Parameter is not of type string.");

		Ipc::Output::printInfo("Test2 called");

		//Return the string length
		return std::make_shared<Ipc::Variable>((int32_t)parameters->at(0)->stringValue.size());
	}
	catch (const std::exception& ex)
	{
		Ipc::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch (Ipc::IpcException& ex)
	{
		Ipc::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch (...)
	{
		Ipc::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
	}
	return Ipc::Variable::createError(-32500, "Unknown application error.");
}
// }}}
