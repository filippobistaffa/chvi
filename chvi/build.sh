cmake -DBUILD_CYTHON=OFF . $*
if [ $? -eq 0 ]
then
    make -j
fi
