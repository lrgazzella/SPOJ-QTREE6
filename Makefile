qtree6: qtree6.o
	g++ qtree6.o -o qtree6

qtree6.o: qtree6.cpp
	g++ qtree6.cpp -c 

clean:
	rm -f ./qtree6.o ./qtree6

run:
	make clean 
	make 