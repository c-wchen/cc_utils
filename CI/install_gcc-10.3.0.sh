#! /bin/sh

gcc='gcc-10.3.0.tar.gz'
gmp='gmp-6.1.0.tar.bz2'
mpfr='mpfr-3.1.4.tar.bz2'
mpc='mpc-1.0.3.tar.gz'
isl='isl-0.18.tar.bz2'

cur_dir=${PWD}
gcc_dir=${PWD}/gcc-10.3.0
install_dir=/usr/local/gcc-10.3.0

echo_archives() {
	echo "${gcc}"
    echo "${gmp}"
    echo "${mpfr}"
    echo "${mpc}"
    echo "${isl}"
}

echo_links() {
    echo "${gmp}"
    echo "${mpfr}"
    echo "${mpc}"
    echo "${isl}"
}


for ar in $(echo_archives)
do
    package="${ar%.tar*}"
    [ -e "${cur_dir}/${package}" ]                      \
        || (tar -xvf "${ar}" )                          \
        || die "Cannot extract package from ${ar}"
    unset package
done
unset ar

cd ${gcc_dir}
for ar in $(echo_links)
do
    target="${cur_dir}/${ar%.tar*}/"
    linkname="${ar%-*}"
    [ -e "${linkname}" ]                                                      \
        || ln -s "${target}" "${linkname}"                                    \
        || die "Cannot create symbolic link ${linkname} --> ${target}"
    unset target linkname
done
unset ar

echo "All package decompress successfully."

./configure --prefix=${install_dir} --enable-multilib --enable-languages=c,c++ -disable-multilib
sudo make -j8
sudo make install
echo "gcc install ${install_dir} successfully."


