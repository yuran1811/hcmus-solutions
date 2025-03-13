./exec.sh $1 $2
./exec_test.sh $1 $2

echo "[>] verdict tests"

if [ "$#" -eq 1 ]; then
    START=$1
    END=$1
elif [ "$#" -ge 2 ]; then
    START=$1
    END=$2
else
	START=0
	END=0
fi
for ((i=START; i<=END; i++))
do
	if cmp -s "../data/out/output$i.txt" "../data/test/out/output$i.txt"; then
		echo "+ Test $i: OK!"
	else
		echo "+ Test $i: Fail!"
	fi
done
