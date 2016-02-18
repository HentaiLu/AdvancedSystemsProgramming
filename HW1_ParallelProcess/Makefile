all: mapper reducer combiner

mapper: 
	gcc Mapper.c -o Mapper

reducer:  
	gcc Reducer.c -o Reducer

combiner: 
	gcc Combiner.c -o Combiner

clean: 
	echo "Deleting old executables"
	rm -f Mapper Reducer Combiner
    

