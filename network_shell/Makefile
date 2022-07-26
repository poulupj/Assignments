CXXFLAGS = -std=c++17 

# following libgtest.a will be available only after install google gtest
LDFLAGS = /usr/local/lib/libgtest.a /usr/local/lib/libgtest_main.a -lpthread

all: test

-include Makefile.deps

Makefile.deps:
	$(CXX) $(CXXFLAGS) -MM *.[ch]pp > Makefile.deps

%.d: %.cpp
	g++ -MM $(CXXFLAGS) $< -o $@ 

%.o: %.d
	g++ -c $(CXXFLAGS) $< -o $@ 

test: test.o server.o client.o 
	g++ $^ -o $@ $(LDFLAGS)

clean: 
	rm -f *.d *.o test Makefile.deps a.out

