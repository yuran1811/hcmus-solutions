echo "[>] test scripting"

echo "+ executing..."
mkdir -p ../data/test/out

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
	echo ">> executing test case $i..."
	../test ../data/test/inp/input$i.txt ../data/test/out/output$i.txt
done
