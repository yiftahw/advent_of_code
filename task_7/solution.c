#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#define STR_BUF_LEN(x) (strlen(x) + 1)

typedef enum
{
    SUCCESS = 0,
    FAILURE,
    MEM_ALLOC_FAILED,
    INVALID_INPUT
} res_status_e;

typedef enum
{
    TYPE_FILE = 0,
    TYPE_DIR,
    TYPE_LINK
} file_type_e;

struct inode; //forward declaration

typedef struct {
    struct inode** files_list; // array of inode pointers
    uint32_t dir_size;
    uint32_t num_files;
} dir_metadata;

typedef struct inode
{
    char* name;
    file_type_e file_type;
    union {
        uint32_t file_size;
        dir_metadata* dir_md;
    };
} inode;

typedef struct
{
    inode* root_folder;
    inode* current_folder;
} file_system;

res_status_e validate_folder(inode* folder)
{
    if(!folder)
    {
        printf("validate_folder(): folder is NULL!\n");
        return INVALID_INPUT;
    }
    if(folder->file_type != TYPE_DIR)
    {
        printf("validate_folder(): folder->file_type != TYPE_DIR!\n");
        return INVALID_INPUT;
    }
    if(!folder->name)
    {
        printf("validate_folder(): folder->name == NULL!\n");
        return INVALID_INPUT;
    }
    if(!folder->dir_md)
    {
        printf("validate_folder(): folder->dir_md == NULL!\n");
        return INVALID_INPUT;
    }

    return SUCCESS;
}

void print_dir(inode* folder)
{
    if(!validate_folder)
    {
        printf("print_dir(): folder not valid!\n");
        return;
    }
    printf(" folder name: %s\n", folder->name);
    printf("num of files: %u\n", folder->dir_md->num_files);
    for(uint32_t index = 0; index < folder->dir_md->num_files; index++)
    {
        inode* file_ptr = folder->dir_md->files_list[index];
        switch(file_ptr->file_type)
        {
            case TYPE_DIR:
                printf("\t dir: %s\n", file_ptr->name);
                break;
            case TYPE_LINK:
                printf("\tlink: %s\n", file_ptr->name);
                break;
            default: // TYPE_FILE
                printf("\tfile: %s size: %u\n", file_ptr->name, file_ptr->file_size);
                break;
        }
    }
    printf("\n");
}

/*
return types:
- MEM_ALLOC_FAILED
- SUCCESS
*/
res_status_e expand_folder_mem(inode* folder)
{
    res_status_e res = validate_folder(folder);
    if(res != SUCCESS)
    {
        return res;
    }
    
    inode** old_files_ptr = folder->dir_md->files_list;
    uint32_t old_num_files = folder->dir_md->num_files;
    uint32_t new_dir_size = (old_files_ptr) ? (2 * folder->dir_md->dir_size) : 1;
    inode** new_files_ptr = (inode**)calloc(new_dir_size, sizeof(inode*));
    
    if(!new_files_ptr)
    {
        printf("expand_folder_mem(): calloc failed!\n");
        return MEM_ALLOC_FAILED;
    }

    // for loop will not run if old_files_ptr is NULL (size is zero)
    assert((old_files_ptr != NULL) == (old_num_files > 0));
    for(uint32_t index = 0; index < old_num_files; index++)
    {
        new_files_ptr[index] = old_files_ptr[index];
    }

    folder->dir_md->files_list = new_files_ptr;
    folder->dir_md->dir_size = new_dir_size;

    if(old_files_ptr)
    {
        free(old_files_ptr);
    }
    
    return SUCCESS;
}

/*
return types:
- INVALID_INPUT
- MEM_ALLOC_FAILED
- SUCCESS
*/
res_status_e add_file_to_dir(inode* file, inode* folder)
{
    res_status_e res;
    res = validate_folder(folder);
    if(res != SUCCESS)
    {
        return res;
    }
    if(!file)
    {
        printf("add_file_to_dir(): file is NULL!");
        return INVALID_INPUT;
    }

    // if first time or no more space
    if((!folder->dir_md->files_list) || (folder->dir_md->dir_size == folder->dir_md->num_files))
    {
        res = expand_folder_mem(folder);
        if(res != SUCCESS)
        {
            return res;
        }
    }

    // if the file is a folder, add a parent link in the new folder (..)
    // if the file is a TYPE_LINK, will not enter condition, thus not causing infinite recursions.
    if(file->file_type == TYPE_DIR)
    {
        inode* parent_link = (inode*)calloc(1, sizeof(inode));
        char* parent_static_name = "..";
        char* parent_dynamic_name = (char*)calloc(1, STR_BUF_LEN(parent_static_name));
        if(!parent_link || !parent_dynamic_name)
        {
            if(parent_link)
                free(parent_link);
            if(parent_dynamic_name)
                free(parent_dynamic_name);
            printf("add_file_to_dir(): calloc failed!\n");
            return MEM_ALLOC_FAILED;
        }
        strcpy(parent_dynamic_name, parent_static_name);
        parent_link->name = parent_dynamic_name;
        parent_link->file_type = TYPE_LINK;
        parent_link->dir_md = folder->dir_md; // reference the parent folder's metadata
        res = add_file_to_dir(parent_link, file); // recursive call
        if(res != SUCCESS)
        {
            if(parent_link)
                free(parent_link);
            if(parent_dynamic_name)
                free(parent_dynamic_name);
            printf("add_file_to_folder(): add parent link reference failed!\n");
            return res;
        }
    }

    // add file to parent folder
    uint32_t index = folder->dir_md->num_files;
    folder->dir_md->files_list[index] = file;
    folder->dir_md->num_files++;

    return SUCCESS;
}

inode* init_dir(char* name)
{    
    if(!name)
    {
        printf("init_dir(): name is NULL!\n");
        return NULL;
    }

    inode* res_dir = (inode*)calloc(1, sizeof(inode));
    dir_metadata* dir_md = (dir_metadata*)calloc(1, sizeof(dir_metadata));
    char* dir_name = (char*)calloc(1,STR_BUF_LEN(name));

    if((!res_dir) || (!dir_md) || (!dir_name))
    {
        if(res_dir)
            free(res_dir);
        if(dir_md)
            free(dir_md);
        if(dir_name)
            free(dir_name);
        printf("init_dir(): calloc failed!\n");
        return NULL;
    }

    strcpy(dir_name, name);
    res_dir->name = dir_name;
    res_dir->file_type = TYPE_DIR;
    res_dir->dir_md = dir_md;

    return res_dir;
}

inode* init_file(char* name, uint32_t size)
{
    if(!name)
    {
        printf("init_file(): name is NULL!\n");
        return NULL;
    }

    inode* out_file = (inode*)calloc(1, sizeof(inode));
    char* file_name = (char*)calloc(1,STR_BUF_LEN(name)); // null terminator
    if((!out_file) || (!file_name))
    {
        if(out_file)
            free(out_file);
        if(file_name)
            free(file_name);
        printf("init_file(): calloc failed!\n");
        return NULL;   
    }

    out_file->name = file_name;
    strcpy(out_file->name, name);
    out_file->file_type = TYPE_FILE;
    out_file->file_size = size;
    return out_file;
}

file_system* init_fs()
{
    file_system* fs = (file_system*)malloc(sizeof(file_system));
    inode* root_dir = init_dir("root");
    
    if(!fs || !root_dir)
    {
        if(fs)
            free(fs);
        if(root_dir)
            free(root_dir);
        printf("init_fs(): memory allocation failed!\n");
        return NULL;
    }

    fs->root_folder = root_dir;
    fs->current_folder = root_dir; // start from root;

    return fs;
}



int main()
{
    /*
    TODO: make add_file_to_dir idempotent or crash if called twice?
    */
    
    res_status_e status = SUCCESS;

    file_system* fs = init_fs();

    inode* home_dir = init_dir("home");
    assert(home_dir != NULL);

    inode* ywaisman_dir = init_dir("ywaisman");
    assert(ywaisman_dir != NULL);

    inode* some_file = init_file("grades.txt", 512);
    assert(some_file != NULL);

    assert(add_file_to_dir(home_dir, fs->root_folder) == SUCCESS);
    assert(add_file_to_dir(ywaisman_dir, home_dir) == SUCCESS);
    assert(add_file_to_dir(some_file, home_dir) == SUCCESS);

    print_dir(fs->root_folder);

    print_dir(home_dir);

    return 0;
}
