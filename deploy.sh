make clean

make web

rm -rf ../ld_release/*
cp -r dist/* ../ld_release

cd ../ld_release
git add .
git commit -m "Deploy"
git push