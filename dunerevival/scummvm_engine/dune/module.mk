MODULE := engines/dune
 
MODULE_OBJS := \
	console.o \
	detection.o \
	dune.o \
	music.o \
	resource.o \
	sentences.o \
	sprite.o
	
MODULE_DIRS += \
	engines/dune
 
# This module can be built as a plugin
ifeq ($(ENABLE_DUNE), DYNAMIC_PLUGIN)
PLUGIN := 1
endif
 
# Include common rules 
include $(srcdir)/rules.mk