
//判断文件是否存在
int file_exists(char* filename, size_t* filesize)
{
    if (filename) {
        struct stat sbuf;
        int rc = stat(filename, &sbuf);
        if (rc == 0 && filesize)
            *filesize = sbuf.st_size; 
        return rc == 0;
    }
    return 0;
}

//判断路径是否存在。如果不存在则创建
void dir_create(char* dir_name){
	int rc;
	struct stat sbuf;
	rc = stat(dir_name, &sbuf);
    if (rc != 0 || !S_ISDIR(sbuf.st_mode)) {
        unlink(dir_name); 
		mkdir(dir_name, 0777);
    }
}