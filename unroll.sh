for x in $(seq 0 $1)
do
    echo "         if( strstr( line[k], word[i + $x] ) != NULL ) count[i + $x]++;"
done
