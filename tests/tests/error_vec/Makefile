BIN =  get_error_vec.bin get_error_vec_fail.bin one_dies.bin

CFLAGS+=-I../

build: $(BIN)

%.bin: %.o ../test_utils.o  
	$(CC) $(CFLAGS) $(LIB_PATH) -o $(TOPROOT)/bin/$@ $^ $(LIBS)
clean:
	rm -rf *~ \#_* *.o *.bin
