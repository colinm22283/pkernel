INCLUDE_DIRS+=$(CURDIR)/include

MODULE_OBJS+=$(MODULE_OD_64)/pkfs.o
MODULE_OBJS+=$(MODULE_OD_64)/mount.o
MODULE_OBJS+=$(MODULE_OD_64)/superblock_ops.o
MODULE_OBJS+=$(MODULE_OD_64)/disc_operations.o

