all:
	${MAKE} -f util/lion.mk
clean:
	${MAKE} -f util/lion.mk clean
	
.PHONY:all clean
