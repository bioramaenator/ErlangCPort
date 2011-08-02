all : erlang-part c-part

erlang-part : src/porttest.erl
	erlc -o ebin +warning_all +debug_info $^

c-part : src/cport.c
	gcc -o ./cport src/cport.c

clean :
	@rm -rf ebin/*.beam ./cport
