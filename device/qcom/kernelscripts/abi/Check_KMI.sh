#!/bin/bash
set -e

[[ $# -ne 1 ]] && echo "need an input param" && exit 0

OEM_TARGET_PRODUCT=$1
ROOT_DIR=$(readlink -f $(dirname $0)/../../../..)
ANDROID_PRODUCT_OUT=$ROOT_DIR/out/target/product/$OEM_TARGET_PRODUCT

case $OEM_TARGET_PRODUCT in
	"holi" | "lahaina")
	CRC_XML=$ROOT_DIR/kernel/msm-5.4/android/abi_gki_aarch64.xml
	SYMVERS=$ANDROID_PRODUCT_OUT/obj/kernel-gki/msm-5.4/Module.symvers
	;;

	*) SYMVERS=""
	;;
esac

[[ -z $CRC_XML ]] && echo "CRC_XML is not defined" && exit 0

[[ -z $SYMVERS ]] && echo "SYMVERS is not defined" && exit 0
[[ ! -f "$SYMVERS" ]] && echo "Could not find $SYMVERS" && exit 0

OUTPUT=$ANDROID_PRODUCT_OUT/obj/kernel-gki/msm-5.4/CRC_mismatch.txt
echo "[CRC mismatch of the following symbols]" > $OUTPUT

standard=$(
grep -w "elf-symbol name" $CRC_XML |
awk -F " " '{print $2, $NF}' |
awk -F "'" '{print $4, $2}' |
sed -e 's/^0x[0]*//' |
sort -t " " -k 1
)

build=$(
grep -w "vmlinux" $SYMVERS |
grep "EXPORT" |
awk -F " " '{print $1, $2}' |
sed -e 's/^0x[0]*//' |
sort -t " " -k 1
)

standard_file="$(mktemp)"
build_file="$(mktemp)"
echo $standard | xargs -n 2 > $standard_file
echo $build | xargs -n 2 > $build_file

standard_symbol="$(mktemp)"
build_symbol="$(mktemp)"
cat $standard_file | awk -F " " '{print $2}' > $standard_symbol
cat $build_file | awk -F " " '{print $2}' > $build_symbol

merged_file="$(mktemp)"
sort $standard_file $build_file -o $merged_file

merged_file_common=$(
uniq -d $merged_file
)

merged_file_common_symbol="$(mktemp)"
echo $merged_file_common |
xargs -n 2 |
awk -F " " '{print $2}' > $merged_file_common_symbol

merged_symbol_file="$(mktemp)"
sort $standard_symbol $build_symbol -o $merged_symbol_file

merged_symbol_file_common="$(mktemp)"
uniq -d $merged_symbol_file | xargs -n 1 > $merged_symbol_file_common

sort $merged_file_common_symbol -o $merged_file_common_symbol
CRC_mismatch=$(comm -13 $merged_file_common_symbol $merged_symbol_file_common)
echo $CRC_mismatch | xargs -n 1 >> $OUTPUT

cat $OUTPUT | xargs -n 1

rm -f $standard_file $build_file $standard_symbol $build_symbol
rm -f $merged_file $merged_file_common_symbol $merged_symbol_file $merged_symbol_file_common

if [ -n "$CRC_mismatch" ] ; then
	echo "error: GKI crc mismatch!"
	exit 1
fi

exit 0
