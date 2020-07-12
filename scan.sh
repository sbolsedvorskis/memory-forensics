DIRECTORY=out_$1
KERNEL_PROFILE=LinuxUbuntu_5_3_0-51-generic_profilex64

rm -rf $DIRECTORY
mkdir $DIRECTORY

volatility --plugins=. --profile=$KERNEL_PROFILE --filename=$1 linux_banner > $DIRECTORY/linux_banner.txt
volatility --plugins=. --profile=$KERNEL_PROFILE --filename=$1 linux_check_creds > $DIRECTORY/linux_check_creds.txt
volatility --plugins=. --profile=$KERNEL_PROFILE --filename=$1 linux_psscan > $DIRECTORY/linux_psscan.txt
volatility --plugins=. --profile=$KERNEL_PROFILE --filename=$1 linux_pstree > $DIRECTORY/linux_pstree.txt
volatility --plugins=. --profile=$KERNEL_PROFILE --filename=$1 linux_strings > $DIRECTORY/linux_strings.txt
volatility --plugins=. --profile=$KERNEL_PROFILE --filename=$1 linux_truecrypt_passphrase > $DIRECTORY/linux_truecrypt_passphrase.txt
volatility --plugins=. --profile=$KERNEL_PROFILE --filename=$1 imageinfo > $DIRECTORY/imageinfo.txt

bulk_extractor -x all -e aes -e elf -e wordlist -o $DIRECTORY/bulk_extractor $1 

tar -czvf $DIRECTORY.zip $DIRECTORY
rm -rf $DIRECTORY
