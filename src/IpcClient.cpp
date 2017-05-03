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

IpcClient::IpcClient(BaseLib::SharedObjects* bl, std::string socketPath) : IIpcClient(bl, socketPath)
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
			BaseLib::PArray parameters = std::make_shared<BaseLib::Array>();
			parameters->reserve(2);

			//First parameter is of type "string" and the method name
			parameters->push_back(std::make_shared<BaseLib::Variable>("exampleTest1"));

			//Second parameter is an array containing the method's signature. In this case the array is empty, meaning there are no parameters and the method returns "void".
			parameters->push_back(std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tArray));

			//Call the RPC method "registerRpcMethod"
			BaseLib::PVariable result = invoke("registerRpcMethod", parameters);
			if (result->errorStruct)
			{
				error = true;
				_out.printCritical("Critical: Could not register RPC method test1: " + result->structValue->at("faultString")->stringValue);
			}
		}

		{
			//Create the parameter array
			BaseLib::PArray parameters = std::make_shared<BaseLib::Array>();
			parameters->reserve(2);

			//First parameter is of type "string" and the method name
			parameters->push_back(std::make_shared<BaseLib::Variable>("exampleTest2"));

			//Second parameter is a nested array containing the method's signature. In this case the method returns "integer" and has one parameter of type "string".
			//The signature is encoded by sending empty variables. Each element of the outer array contains one signature. The inner array (= signature array) contains
			//the return parameter first followed by the arguments.
			parameters->push_back(std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tArray));
			parameters->back()->arrayValue->push_back(std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tInteger));
			parameters->back()->arrayValue->push_back(std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tString));

			//Call the RPC method "registerRpcMethod". All RPC methods known by Homegear can be called this way.
			BaseLib::PVariable result = invoke("registerRpcMethod", parameters);
			if (result->errorStruct)
			{
				error = true;
				_out.printCritical("Critical: Could not register RPC method test2: " + result->structValue->at("faultString")->stringValue);
			}
		}

		if (!error) _out.printInfo("Info: RPC methods successfully registered.");
	}
	catch (const std::exception& ex)
	{
		_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch (BaseLib::Exception& ex)
	{
		_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch (...)
	{
		_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
	}
}

// {{{ RPC methods
BaseLib::PVariable IpcClient::test1(BaseLib::PArray& parameters)
{
	try
	{
		_out.printInfo("Test1 called");

		return BaseLib::PVariable(new BaseLib::Variable(std::string("Test1 ") + std::to_string(BaseLib::HelperFunctions::getTimeSeconds())));
	}
	catch (const std::exception& ex)
	{
		_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch (BaseLib::Exception& ex)
	{
		_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch (...)
	{
		_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
	}
	return BaseLib::Variable::createError(-32500, "Unknown application error.");
}

BaseLib::PVariable IpcClient::test2(BaseLib::PArray& parameters)
{
	try
	{
		if(parameters->size() != 1) return BaseLib::Variable::createError(-1, "Method expects exactly one parameter. " + std::to_string(parameters->size()) + " given.");
		if(parameters->at(0)->type != BaseLib::VariableType::tString) return BaseLib::Variable::createError(-1, "Parameter is not of type string.");

		_out.printInfo("Test2 called");

		//Return the string length
		return std::make_shared<BaseLib::Variable>((int32_t)parameters->at(0)->stringValue.size());
	}
	catch (const std::exception& ex)
	{
		_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch (BaseLib::Exception& ex)
	{
		_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch (...)
	{
		_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
	}
	return BaseLib::Variable::createError(-32500, "Unknown application error.");
}
// }}}
