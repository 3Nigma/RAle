PROJ_NAME := pale

CXX := g++
CXXFLAGS := -g -std=c++0x
LDFLAGS := `pkg-config gtkmm-3.0 --cflags --libs` #`pkg-config gtksourceviewmm-3.0 --cflags --libs`
CXXOBJECTS := fereastraprincipala.o fereastracod.o

%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@ $(LDFLAGS)

all : $(CXXOBJECTS)
	$(CXX) $(CXXFLAGS) main.cpp $(CXXOBJECTS) -o $(PROJ_NAME) $(LDFLAGS)

clean :
	rm -f $(PROJ_NAME) $(CXXOBJECTS)