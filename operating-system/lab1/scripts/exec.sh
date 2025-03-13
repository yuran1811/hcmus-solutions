echo "[>] main scripting"

echo "+ compiling..."
g++ ../23127065-Code/\*.cpp -o ../23127065

echo "+ executing..."
mkdir -p ../data/out

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
	../23127065 ../data/inp/input$i.txt ../data/out/output$i.txt
done
