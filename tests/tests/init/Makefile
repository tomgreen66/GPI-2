BIN =  proc_init.bin proc_init_timeout.bin cmd_line_args.bin \
	kill_procs.bin cl.bin hello_world.bin

CFLAGS+=-I../

build: $(BIN)

%.bin: %.o ../test_utils.o  
	$(CC) $(CFLAGS) $(LIB_PATH) -o $(TOPROOT)/bin/$@ $^ $(LIBS)
clean:
	rm -rf *~ \#_* *.o *.bin
