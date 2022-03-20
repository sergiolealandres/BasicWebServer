
NRep=10000
for ((J= 0; J < NRep ; J+=1)); do

	curl -X GET http://localhost:8080/IMPORTANTE.txt
	
done


