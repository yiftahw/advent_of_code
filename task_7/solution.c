#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

typedef enum
{
    SUCCESS = 0,
    FAILURE,
    MEM_ALLOC_FAILED,
    INVALID_INPUT
} res_status_e;

struct inode; //forward declaration

typedef struct {
    struct inode** files_list; // array of inode pointers
    uint32_t dir_size;
    uint32_t num_files;
} dir_metadata;

typedef struct inode
{
    char* name;
    bool is_dir;
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
    if(!folder->is_dir)
    {
        printf("validate_folder(): folder->is_dir == false!\n");
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

res_status_e validate_file(inode* file)
{
    if(!file)
    {
        printf("validate_file(): file is NULL!\n");
        return INVALID_INPUT;
    }
    if(file->is_dir)
    {
        printf("validate_file(): file->is_dir == true!\n");
        return INVALID_INPUT;
    }
    if(!file->name)
    {
        printf("validate_folder(): file->name == NULL!\n");
        return INVALID_INPUT;
    }

    return SUCCESS;
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
    res = validate_file(file);
    if(res != SUCCESS)
    {
        return res;
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

    uint32_t index = folder->dir_md->num_files;
    folder->dir_md->files_list[index] = file;
    folder->dir_md->num_files++;

    return SUCCESS;
}

res_status_e init_dir(char* name, inode* parent_dir, inode* output_dir)
{
    res_status_e status;
    
    if(!name || !parent_dir)
    {
        printf("init_dir(): input is NULL (name || parent_dir) !\n");
        return INVALID_INPUT;
    }
    if(output_dir)
    {
        printf("init_dir(): output_dir must be NULL!\n");
    }
    status = validate_folder(parent_dir);
    if(status != SUCCESS)
    {
        return status;
    }

    inode* res_dir = (inode*)calloc(1, sizeof(inode));
    dir_metadata* dir_md = (dir_metadata*)calloc(1, sizeof(dir_metadata));
    inode* par_dir = (inode*)calloc(1, sizeof(inode));
    char* par_name = (char*)calloc(1, 3);

    if((!res_dir) || (!dir_md) || (!par_dir) || (!par_name))
    {
        if(res_dir)
            free(res_dir);
        if(dir_md)
            free(dir_md);
        if(par_dir)
            free(par_dir);
        if(par_name)
            free(par_name);
        printf("init_dir(): calloc failed!\n");
        return MEM_ALLOC_FAILED;
    }

    output_dir = res_dir;
    output_dir->name = name;
    output_dir->is_dir = true;
    output_dir->dir_md = dir_md;

    par_dir->name = par_name;
    strcpy(par_dir->name, "..");
    par_dir->is_dir = true;
    par_dir->dir_md = parent_dir->dir_md; // point to parent's metadata

    status = add_file_to_dir(par_dir, output_dir);
    if(status != SUCCESS)
    {
        // TODO: handle error here
        printf("init_root_dir(): add_file_to_dir_failed!\n");
        return status;
    }
    return SUCCESS;
}

res_status_e init_root_dir(inode* output_dir)
{
    res_status_e status;
    if(output_dir)
    {
        printf("init_root_dir(): output_dir must be NULL\n");
        return INVALID_INPUT;
    }
    inode* dir = (inode*)calloc(1, sizeof(inode));
    char* name = (char*)calloc(1,5);
    inode* par_dir = (inode*)calloc(1, sizeof(inode));
    char* par_name = (char*)calloc(1,3);
    dir_metadata* dir_md = (dir_metadata*)calloc(1, sizeof(dir_metadata));

    if((!dir) || (!name) || (!par_dir) || (!par_name) || (!dir_md))
    {
        if(dir)
            free(dir);
        if(name)
            free(name);
        if(par_dir)
            free(par_name);
        if(par_name)
            free(par_name);
        if(dir_md)
            free(dir_md);
        printf("init_root_dir(): calloc failed!\n");
        return MEM_ALLOC_FAILED;
    }

    dir->name = name;
    strcpy(dir->name, "root");
    dir->is_dir = true;
    dir->dir_md = dir_md;

    par_dir->name = par_name;
    strcpy(par_dir->name, "..");
    par_dir->is_dir = true;
    par_dir->dir_md = dir_md; // circular reference to root dir

    status = add_file_to_dir(par_dir, dir);
    if(status != SUCCESS)
    {
        //TODO: handle error here
        printf("init_root_dir(): add_file_to_dir_failed!\n");
        return status;
    }
    return SUCCESS;
}

res_status_e init_file(char* name, uint32_t size, inode* out_file)
{
    if(!name)
    {
        printf("init_file(): name is NULL!\n");
        return INVALID_INPUT;
    }
    if(out_file)
    {
        printf("init_file(): out_file must be NULL!\n");
        return INVALID_INPUT;
    }

    out_file = (inode*)calloc(1, sizeof(inode));
    if(!out_file)
    {
        printf("init_file(): calloc failed!\n");
        return MEM_ALLOC_FAILED;   
    }

    out_file->name = name;
    out_file->is_dir = false;
    out_file->file_size = size;
    return SUCCESS;
}

res_status_e init_fs(file_system* out_fs)
{
    if(out_fs != NULL)
    {
        printf("init_fs(): fs must be NULL!\n");
        return INVALID_INPUT;
    }

    out_fs = (file_system*)malloc(sizeof(file_system));
    if(out_fs == NULL)
    {
        printf("init_fs(): malloc failed!\n");
        return MEM_ALLOC_FAILED;
    }

    inode* root_dir = NULL;
    res_status_e status = init_root_dir(root_dir);
    if(status != SUCCESS)
    {
        return status;        
    }

    out_fs->root_folder = root_dir;
    out_fs->current_folder = root_dir; // start from root;
    return SUCCESS;
}

void print_dir(inode* folder)
{
    if(!validate_folder)
    {
        printf("print_dir(): folder not valid!\n");
        return;
    }
    printf("folder name: %s", folder->name);
    for(uint32_t index = 0; index < folder->dir_md->num_files; index++)
    {
        inode* file_ptr = folder->dir_md->files_list[index];
        if(file_ptr->is_dir)
        {
            printf("dir:  %s\n", file_ptr->name);
        }
        else
        {
            printf("file: %s size: %u\n", file_ptr->name, file_ptr->file_size);
        }
    }
}

int main()
{
    //TODO: names should be handled with alloc and strcpy
    //TODO: init_dir should not add parent pointer to folder
    //TODO: instead, add_file_to_folder should "mount" the folder and add a parent pointer
    
    file_system* fs = NULL;
    res_status_e status = init_fs(fs);

    inode* home_dir = NULL;
    status = init_dir("home", fs->root_folder, home_dir);
    if(status != SUCCESS)
    {
        printf("init dir failed");
        return;
    }


}
