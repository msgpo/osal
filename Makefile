TGT=libosal.a
CFLAGS=-c -I$(PWD) -pthread
LDFLAGS= -pthread
DIS=linux/

OBJS=$(DIS)mq.o $(DIS)sem.o $(DIS)thread.o $(DIS)event.o $(DIS)mutex.o $(DIS)osal.o

ifeq ($(V),1)
Q_CC=
Q_LD=
Q_AR=
Q_RM=
else
Q_CC=@echo "    CC $@";
Q_LD=@echo "    LD $@";
Q_AR=@echo "    AR $@";
Q_RM=@echo "    RM";
endif

.PHONY: all test clean

%.o: %.c
	$(Q_CC)$(CC) $(CFLAGS) -o $@ $^

all: $(TGT)

test: $(TGT)

$(TGT) : $(OBJS)
	$(Q_AR)ar -r $@ $^

clean:
	$(Q_RM)rm -f $(TGT) $(OBJS)
