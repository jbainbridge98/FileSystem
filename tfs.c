/*
 *  Copyright (C) 2021 CS416 Rutgers CS
 *	Tiny File System
 *	File:	tfs.c
 *
 */

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/time.h>
#include <libgen.h>
#include <limits.h>

#include "block.h"
#include "tfs.h"

char diskfile_path[PATH_MAX];

// Declare your in-memory data structures here

/*
 * Get available inode number from bitmap
 */
int get_avail_ino() {

	// Step 1: Read inode bitmap from disk

	bitmap_t *bitmap;
	bio_read(sb->i_bitmap_blk, bitmap);
	int index = -1;

	// Step 2: Traverse inode bitmap to find an available slot

	for (int i = 0; i < MAX_INUM; i++){
		if (get_bitmap(bitmap, i) == 0){
			index = i;
			break;
		}
	}

	// Step 3: Update inode bitmap and write to disk

	if (index == -1){
		return -1;
	} else {
		set_bitmap(bitmap, index);
		bio_write(sb->i_bitmap_blk, bitmap);
	}

	return index;
}

/*
 * Get available data block number from bitmap
 */
int get_avail_blkno() {

	// Step 1: Read data block bitmap from disk

	bitmap_t *bitmap;
	bio_read(sb->d_bitmap_blk, bitmap);

	// Step 2: Traverse data block bitmap to find an available slot

	for(int i = 0; i < MAX_DNUM; i++){
			if(get_bitmap(bitmap, i) == 0){
				int index = i;
				break;
			}
	}

	// Step 3: Update data block bitmap and write to disk

	if(index = -1){
		return -1;
	}else{
		set_bitmap(bitmap, index);
		bio_write(sb->d_bitmap_blk, bitmap);
	}

	return index;
}

/*
 * inode operations
 */
 //later
int readi(uint16_t ino, struct inode *inode) {

  // Step 1: Get the inode's on-disk block number

	int size = sizeof(struct inode) * ino;
	int block;

	if (size % BLOCK_SIZE == 0){
		block = size/BLOCKSIZE;
	} else {
		block = size/BLOCKSIZE + 1;
	}

	block += 3;

  // Step 2: Get offset of the inode in the inode on-disk block

	int offset = size%BLOCKSIZE;

  // Step 3: Read the block from disk and then copy into inode structure

	struct inode *blk = malloc(BLOCKSIZE);
	bio_read(block, blk);

	//blk[offset] // inode

	memcpy(blk[offset], inode, sizeof(stuct inode));

	return 0;
}
//later
int writei(uint16_t ino, struct inode *inode) {

	// Step 1: Get the block number where this inode resides on disk

	int size = sizeof(struct inode) * ino;
	int block;

	if (size % BLOCK_SIZE == 0){
		block = size/BLOCKSIZE;
	} else {
		block = size/BLOCKSIZE + 1;
	}

	block += 3;

	// Step 2: Get the offset in the block where this inode resides on disk

	int offset = size%BLOCKSIZE;

	// Step 3: Write inode to disk

	bio_write(block, inode);

	return 0;
}


/*
 * directory operations
 */
 //later
int dir_find(uint16_t ino, const char *fname, size_t name_len, struct dirent *dirent) {

  // Step 1: Call readi() to get the inode using ino (inode number of current directory)

  // Step 2: Get data block of current directory from inode

  // Step 3: Read directory's data block and check each directory entry.
  //If the name matches, then copy directory entry to dirent structure

	return 0;
}
//later
int dir_add(struct inode dir_inode, uint16_t f_ino, const char *fname, size_t name_len) {

	// Step 1: Read dir_inode's data block and check each directory entry of dir_inode

	// Step 2: Check if fname (directory name) is already used in other entries

	// Step 3: Add directory entry in dir_inode's data block and write to disk

	// Allocate a new data block for this directory if it does not exist

	// Update directory inode

	// Write directory entry

	return 0;
}
//later
int dir_remove(struct inode dir_inode, const char *fname, size_t name_len) {

	// Step 1: Read dir_inode's data block and checks each directory entry of dir_inode

	// Step 2: Check if fname exist

	// Step 3: If exist, then remove it from dir_inode's data block and write to disk

	return 0;
}

/*
 * namei operation
 */
 //later
int get_node_by_path(const char *path, uint16_t ino, struct inode *inode) {

	// Step 1: Resolve the path name, walk through path, and finally, find its inode.

	// Note: You could either implement it in a iterative way or recursive way

	return 0;
}

/*
 * Make file system
 */
int tfs_mkfs() {

	// Call dev_init() to initialize (Create) Diskfile
	dev_init(diskfile_path);
	// write superblock information
	sb = (struct superblock*)malloc(sizeof(struct superblock));
	sb->magic_num = MAGIC_NUM;
	sb->max_inum = MAX_INUM;
	sb->max_dnum = MAX_DNUM;



	// initialize inode bitmap

	i_bitmap = calloc(MAX_INUM/8, MAX_INUM/8);  // needs to be edited; diskfile doesnt point to region in memory; logic is correct

	// initialize data block bitmap

	d_bitmap = calloc(MAX_DNUM/8, MAX_DNUM/8);
	set_bitmap(d_bitmap, 0); // setting bitmap for superblock

	int iblk_size;
	int dblk_size;

	if ((MAX_INUM/8) % BLOCK_SIZE == 0){
		iblk_size = (MAX_INUM/8) / BLOCK_SIZE;
	} else {
		iblk_size = 1 + (MAX_INUM/8) / BLOCK_SIZE;
	}

	if ((MAX_DNUM/8) % BLOCK_SIZE == 0){
		dblk_size = (MAX_DNUM/8) / BLOCK_SIZE;
	} else {
		dblk_size = 1 + (MAX_DNUM/8) / BLOCK_SIZE;
	}

	sb->i_bitmap_blk = ;
	sb->d_bitmap_blk = ;
	sb->i_start_blk = ;
	sb->d_start_blk = ;

	bio_write(0, sb); // write superblock to filesystem; 0th block

	// update bitmap information for root directory

	for (int i = 0; i < MAX_INUM; i++){
		unset_bitmap(i_bitmap, i);
	}
	for (int i = 0; i < MAX_DNUM; i++){
		unset_bitmap(d_bitmap, i);
	}

	// still need to set the first inode in bitmap

	// update inode for root directory



	return 0;
}


/*
 * FUSE file operations
 */
static void *tfs_init(struct fuse_conn_info *conn) {

	// Step 1a: If disk file is not found, call mkfs
	if(dev_open(diskfile) == -1){
		tfs_mkfs();
	}
  // Step 1b: If disk file is found, just initialize in-memory data structures
  // and read superblock from disk

	bio_read(0, sb);


	return NULL;
}

static void tfs_destroy(void *userdata) {

	// Step 1: De-allocate in-memory data structures

	free(sb);
	free(i_bitmap);
	free(d_bitmap);

	// Step 2: Close diskfile

	close(diskfile_path);

}

static int tfs_getattr(const char *path, struct stat *stbuf) {

	// Step 1: call get_node_by_path() to get inode from path

	int icur = get_node_by_path(path);
	if (icur == 0){
		return -1;
	}

	// Step 2: fill attribute of file into stbuf from inode

		struct inode *ncur;

		if (readi(icur, ncur)){
			return -1;
		}

		memcpy(stbuf, &ncur->vstat, sizeof(struct stat));

		stbuf->st_mode   = S_IFDIR | 0755;
		stbuf->st_nlink  = 2;
		time(&stbuf->st_mtime);

	return 0;
}

static int tfs_opendir(const char *path, struct fuse_file_info *fi) {

	// Step 1: Call get_node_by_path() to get inode from path
	int icur = get_node_by_path(path);
	if (icur == 0){
		return -1;
	}
	// Step 2: If not find, return -1

    return 0;
}

static int tfs_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {

	// Step 1: Call get_node_by_path() to get inode from path
	int icur = get_node_by_path(path);
	if (icur == 0){
		return -1;
	}
	// Step 2: Read directory entries from its data blocks, and copy them to filler


	return 0;
}

//later
static int tfs_mkdir(const char *path, mode_t mode) {

	// Step 1: Use dirname() and basename() to separate parent directory path and target directory name

	char *base = basename(path);
	char *dir = dirname(path);

	// Step 2: Call get_node_by_path() to get inode of parent directory

	int icur = get_node_by_path(path, )

	// Step 3: Call get_avail_ino() to get an available inode number

	int inum = get_avail_ino();

	// Step 4: Call dir_add() to add directory entry of target directory to parent directory

	// Step 5: Update inode for target directory

	// Step 6: Call writei() to write inode to disk


	return 0;
}
//later
static int tfs_rmdir(const char *path) {

	// Step 1: Use dirname() and basename() to separate parent directory path and target directory name

	// Step 2: Call get_node_by_path() to get inode of target directory

	// Step 3: Clear data block bitmap of target directory

	// Step 4: Clear inode bitmap and its data block

	// Step 5: Call get_node_by_path() to get inode of parent directory

	// Step 6: Call dir_remove() to remove directory entry of target directory in its parent directory

	return 0;
}

static int tfs_releasedir(const char *path, struct fuse_file_info *fi) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}
//later
static int tfs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {

	// Step 1: Use dirname() and basename() to separate parent directory path and target file name

	// Step 2: Call get_node_by_path() to get inode of parent directory

	// Step 3: Call get_avail_ino() to get an available inode number

	// Step 4: Call dir_add() to add directory entry of target file to parent directory

	// Step 5: Update inode for target file

	// Step 6: Call writei() to write inode to disk

	return 0;
}

static int tfs_open(const char *path, struct fuse_file_info *fi) {

	// Step 1: Call get_node_by_path() to get inode from path
	int icur = get_node_by_path(path);
	if (icur == 0){
		return -1;
	}
	// Step 2: If not find, return -1

	return 0;
}

static int tfs_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {

	// Step 1: You could call get_node_by_path() to get inode from path

	// Step 2: Based on size and offset, read its data blocks from disk

	// Step 3: copy the correct amount of data from offset to buffer

	// Note: this function should return the amount of bytes you copied to buffer
	return 0;
}

static int tfs_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
	// Step 1: You could call get_node_by_path() to get inode from path

	// Step 2: Based on size and offset, read its data blocks from disk

	// Step 3: Write the correct amount of data from offset to disk

	// Step 4: Update the inode info and write it to disk

	// Note: this function should return the amount of bytes you write to disk
	return size;
}
//later
static int tfs_unlink(const char *path) {

	// Step 1: Use dirname() and basename() to separate parent directory path and target file name

	// Step 2: Call get_node_by_path() to get inode of target file

	// Step 3: Clear data block bitmap of target file

	// Step 4: Clear inode bitmap and its data block

	// Step 5: Call get_node_by_path() to get inode of parent directory

	// Step 6: Call dir_remove() to remove directory entry of target file in its parent directory

	return 0;
}

static int tfs_truncate(const char *path, off_t size) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}

static int tfs_release(const char *path, struct fuse_file_info *fi) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
	return 0;
}

static int tfs_flush(const char * path, struct fuse_file_info * fi) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}

static int tfs_utimens(const char *path, const struct timespec tv[2]) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}


static struct fuse_operations tfs_ope = {
	.init		= tfs_init,
	.destroy	= tfs_destroy,

	.getattr	= tfs_getattr,
	.readdir	= tfs_readdir,
	.opendir	= tfs_opendir,
	.releasedir	= tfs_releasedir,
	.mkdir		= tfs_mkdir,
	.rmdir		= tfs_rmdir,

	.create		= tfs_create,
	.open		= tfs_open,
	.read 		= tfs_read,
	.write		= tfs_write,
	.unlink		= tfs_unlink,

	.truncate   = tfs_truncate,
	.flush      = tfs_flush,
	.utimens    = tfs_utimens,
	.release	= tfs_release
};


int main(int argc, char *argv[]) {
	int fuse_stat;

	getcwd(diskfile_path, PATH_MAX);
	strcat(diskfile_path, "/DISKFILE");

	fuse_stat = fuse_main(argc, argv, &tfs_ope, NULL);

	return fuse_stat;
}
