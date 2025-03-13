echo "[>] packing"

./exec.sh

SUBMIT_PATH=../release/23127065

mkdir -p $SUBMIT_PATH

echo "+ copy inp/out files..."
cp -r ../23127065-Code $SUBMIT_PATH
mv ../23127065.exe $SUBMIT_PATH/23127065.exe
cp ../data/inp/input0.txt $SUBMIT_PATH/input.txt
cp ../data/out/output0.txt $SUBMIT_PATH/output.txt

echo "+ zipping..."
cd $SUBMIT_PATH
zip -9 -r ../23127065.zip ../23127065
cd -
