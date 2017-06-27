# Homegear IPC Example

This example project demonstrates how to access Homegear's IPC (= Inter-process Communication) interface. It registers two RPC methods in Homegear (`exampleTest1()` and `exampleTest2()`). `exampleTest1()` prints a line to the terminal and returns a string containing the current timestamp. `exampleTest2()` expects a string as parameter, also prints a line to the terminal and returns the length of the string.

To compile, just execute:

```
./makeDebug.sh
```

To test, you can call the newly registerd RPC methods from the shell:

```
homegear -e rc 'print_v($hg->exampleTest1());'
homegear -e rc 'print_v($hg->exampleTest2("Hello"));'
```

All RPC methods known by Homegear can be called by executing `invoke()`. The signature is:

```
Ipc::PVariable invoke(std::string methodName, Ipc::PArray parameters);
```

The first parameter is the name of the RPC method (e. g. `getValue()`). The second parameter is a vector of `Ipc::PVariable` (= a `std::shared_ptr` to `Ipc::Variable`). `Ipc::Variable` is a class which can contain values of any type. The usage should be self-explaining.

For example to create the parameters array for `getValue(12, 1, "STATE")`, call:

```
Ipc::PArray parameters = std::make_shared<Ipc::Array>();
parameters->reserve(3);
```

To avoid copy operations, we reserve space for 3 elements.

Now we add the parameters (peer ID, channel and variable name):

```
parameters->push_back(std::make_shared<Ipc::Variable>(12));
parameters->push_back(std::make_shared<Ipc::Variable>(1));
parameters->push_back(std::make_shared<Ipc::Variable>("HUMIDITY"));
```

Call `invoke()` and print out the result:

```
Ipc::PVariable result = invoke("getValue", parameters);
std::cout << "Result: " << result->integerValue << std::endl;
```
