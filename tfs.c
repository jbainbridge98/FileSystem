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

bitmap_t ibitmap; // in memory inode bitmap
bitmap_t dbitmap; // in memory data bitmap
struct superblock sb; // in memory superblock
struct inode *inodes[MAX_INUM+1]; // in memory inode array; +1 for 0th inode (invalid)

// Declare your in-memory data structures here

/*
 * Get available inode number from bitmap
 */
int get_avail_ino() {

 	// Step 1: Read inode bitmap from disk

 	bio_read(sb->i_bitmap_blk, ibitmap);

 	// Step 2: Traverse inode bitmap to find an available slot

 	int stat = -1;

 	for (int i = 0; i < MAX_INUM; i++){
 		if (get_bitmap(i_bitmap_blk, i) == 0){
 			stat = i;
 			break;
 		}
 	}

 	// Step 3: Update inode bitmap and write to disk

 	if (stat != -1){
 		set_bitmap(i_bitmap_blk, i);
 		bio_write(sb->i_bitmap_blk, ibitmap);
 	}

 	return stat;
 }

 /*
  * Get available data block number from bitmap
  */
int get_avail_blkno() {

 	// Step 1: Read data bitmap from disk

 	bio_read(sb->d_bitmap_blk, dbitmap);

 	// Step 2: Traverse data bitmap to find an available slot

 	int stat = -1;

 	for (int i = 0; i < MAX_DNUM; i++){
 		if (get_bitmap(d_bitmap_blk, i) == 0){
 			stat = i;
 			break;
 		}
 	}

 	// Step 3: Update inode bitmap and write to disk

 	if (stat != -1){
 		set_bitmap(d_bitmap_blk, i);
 		bio_write(sb->d_bitmap_blk, dbitmap);
 	}

 	return stat;
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
int readi(uint16_t ino, struct inode *inode) {

  // Step 1: Get the inode's on-disk block number

	int block = ino / (BLOCK_SIZE/sizeof(struct inode));

  // Step 2: Get offset of the inode in the inode on-disk block

	int offset = ino % (BLOCK_SIZE/sizeof(struct inode));

  // Step 3: Read the block from disk and then copy into inode structure

	struct inode *temp = malloc(BLOCK_SIZE);

	bio_read(block, temp);

	memcpy(inode, temp[offset], sizeof(struct inode));

	free(temp);

	return 0;
}

int writei(uint16_t ino, struct inode *inode) {

	// Step 1: Get the block number where this inode resides on disk

	int block = ino / (BLOCK_SIZE/sizeof(struct inode));

	// Step 2: Get the offset in the block where this inode resides on disk

	int offset = ino % (BLOCK_SIZE/sizeof(struct inode));

	// Step 3: Write inode to disk

	struct inode *temp = malloc(BLOCK_SIZE);

	bio_write(block, temp);

	memcpy(inode, temp[offset], sizeof(struct inode));

	free(temp);

	return 0;
}


/*
 * directory operations
 */
 //later
int dir_find(uint16_t ino, const char *fname, size_t name_len, struct dirent *dirent) {

	char *fp = strdup(fname);
	char *bp = basename(fp);
  // Step 1: Call readi() to get the inode using ino (inode number of current directory)
	uint16_t validity = searchFile(inodes[0], inodes[0]->ino, fp);

	if(validity > 0){
		readi(validity, inodes[ino - 1]);
		dirent->ino = validity;
		dirent->valid = 1;
		strcpy(dirent->name, bp);
		return 0;
	}else{
		return -1;
	}

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

 	sb = (struct superblock *)calloc(1, BLOCK_SIZE);
 	sb->magic_num = MAGIC_NUM;
 	sb->max_inum = MAX_INUM;
 	sb->max_dnum = MAX_DNUM;
 	sb->i_bitmap_blk = 1;
 	sb->d_bitmap_blk = 2;
 	sb->i_start_blk = 3;
 	sb->d_start_blk = sb->i_start_blk + (sizeof(struct inode)*MAX_INUM / BLOCK_SIZE);

 	// initialize in memory inodes

 	for (int i = 0; i < MAX_INUM; i++){
 		inodes[i] = (struct inode)calloc(1, sizeof(struct inode));
 		for (int j = 0; j < DIR_PTR; j++){ // invalidate pointers
 			inodes[i]->direct_ptr[j] = -1;
 			if (j < INDIR_PTR) inodes[i]->indirect_ptr[j] = -1;
 		}
 	}

 	// initialize inode bitmap

 	ibitmap = (bitmap_t)calloc(1, BLOCK_SIZE);
 	get_avail_ino(); // sets 0th inode to in use

 	// initialize data block bitmap

 	dbitmap = (bitmap_t)calloc(1, BLOCK_SIZE);

 	for (i = 0; i < sb->d_start_blk; i++){
 		set_bitmap(dbitmap, i); // sets the blocks for sb, bitmaps, inodes, to in use
 	}

 	// update bitmap information for root directory

 	int root_node = get_avail_ino(); // 1st inode now in use

 	// update inode for root directory

	inodes[root_node]->root_node;
	inodes[root_node]->valid = 1;
	inodes[root_node]->size = BLOCK_SIZE;
	inodes[root_node]->type = DIR;
	inodes[root_node]->link = 2;
	inodes[root_node]->direct_ptr[0] = get_avail_blkno();
	inodes[root_node]->vstat.st_atime = time(NULL);
	inodes[root_node]->vstat.st_mtime = time(NULL);

	struct dirent *dir = (struct dirent *)calloc(1, BLOCK_SIZE);
	int maxEntries = BLOCK_SIZE/sizeof(struct dirent);z

	dir[0].ino = 1;
	dir[1].ino = 1;
	dir[2].ino = 1;
	dir[0].valid = 1;
	dir[1].valid = 1;
	dir[2].valid = 1;
	strcpy(dir[0].name, "/");
	strcpy(dir[1].name, ".");
	strcpy(dir[2].name, "..");

	for (int i = 3; i < maxEntries; i++){
		dir[i].valid = 0;
	}

	bio_write(inode[root_node]->direct_ptr[0]);

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
	bio_read(1, i_bitmap);
	bio_read(2, d_bitmap);

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
	int icur = get_node_by_path(path, //more later);
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

char **split(char pn[]){

	int filenameLen = strlen(pn);
	int slash = 0;
	int index = 1;
	int start = -1;

	for(int i = 0; i <= filenameLen; i++){
		if(pn[i] == '/'){
			slash++;
		}
	}

	char **splitName = (char **)malloc((slash + 2) * sizeof(char *));

	if(strcmp(pn, "/") == 0){
		splitName[index] = (char *)malloc(2 * sizeof(char));
		strcpy(splitName[index], "/");
	}else{
		for(int j = 0; j <= filenameLen; j++){
			if(pn[j] == '/'){
				if(i == 0){
					splitName[index] = (char *)malloc(2 * sizeof(char));
					strcpy(splitName[index], "/");
				}else{
					int sublen = j - start - 1;
					 splitName[index] = (char *)malloc((sublen + 1) * sizeof(char));
					 memcpy(splitName[index], &pn[start + 1], sublen);
					 splitName[index][sublen] = '\0';
				}
				start = j;
				index++;
			}
		}
	}

	splitName[0] = malloc(11 * sizeof(char));
	sublen = filenameLen - start;
	splitName[index] = (char *)malloc((sublen + 1) * sizeof(char));
	memcpy(splitName[index], &pn[start + 1], sublen);
	splitName[index][sublen] = '\0';

	return splitname;

}

uint16_t searchFile(struct inode *in, uint16_t ino, char pn[]){
	double numentries = BLOCK_SIZE/(sizeof(struct dirent));
	struct dirent *subFandD = (struct dirent *)malloc(BLOCK_SIZE);

	char** splitPath = split(pn);
	int len = strtod(splitPath[0], NULL);
	int index = 0;
	int foundDisk = -1;
	struct inode found;

	readi(1, &found);

	int DirOff = sb->d_start_blk + found.direct_ptr[0];

	bio_read(DirOff, subFandD);

	if(strcmp(splitPath[len], "/") == 0){
		return 1;
	}

	for(;;){

		for(int i = 0; i < DIR_PTR; i++){
			if(index == len && foundDisk == 0){
				i = 0;
				DirOff = sb->d_start_blk + found.direct_ptr[i];
				bio_read(DirOff, subFandD);
			}
			if(found.direct_ptr[i] == -1){
				continue;
			}
			DirOff = sb->d_start_blk + found.direct_ptr[i];
			bio_read(DirOff, subFandD);
			foundDisk = -1;

			for(int j = 0; j < numentries; j++){
				if(subFandD[j].valid == 1){
					if(strcmp(subFandD[j].name, splitPath[index] == 0){
						readi(subFandD[j].ino, &found);
						foundDisk = 0;
						found.ino = subFandD[j].ino;
						index++;
						break;
					}
				}
			}
			if(foundDisk == -1){
				continue;
			}
			if(index > len){
				free(subFandD);
				subFandD == NULL;
				return found.ino;
			}
		}
		if(foundDisk == -1){
			break;
		}

	}
	free(subFandD);
	subFandD == NULL;
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
