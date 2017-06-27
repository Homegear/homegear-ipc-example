AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -Wall -std=c++11 -DFORTIFY_SOURCE=2 -DGCRYPT_NO_DEPRECATED

bin_PROGRAMS = homegear-ipc-example
homegear_ipc_example_SOURCES = main.cpp IpcClient.cpp
homegear_ipc_example_LDADD = -lpthread -lhomegear-ipc -lgnutls -lgcrypt -lgpg-error

if BSDSYSTEM
else
homegear_ipc_example_LDADD += -ldl
endif
