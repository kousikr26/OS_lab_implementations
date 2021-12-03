## Installing necessary packages

zfs filesystem

`sudo apt install zfsutils-linux`

gparted to format partitions

`sudo apt install gparted` 

## Preparing partition

- Format a partition of the hard drive 
- Create a zfs pool with the partition

`sudo zpool create zfs_pool /dev/sdb`

## Using vdbench

In a separate terminal open the folder containing vdbench

Create a parameter file with the desired attributes

Run `sudo ./vdbench -f ./parameter_file_name -o output_folder`


## Running experiments

a) Dedup

Set dedup off with

`sudo zfs set dedup=off zfs_pool`
