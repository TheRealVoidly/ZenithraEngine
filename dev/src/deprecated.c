#include"zenithra_core.h"

void zenithra_free_list(void **head){
    if(!*head){
        zenithra_log_msg("Attempt to free null list");
        return;
    }

    struct TempList{
        struct TempList *next;
        char *temp_arr_fill;
    };

    void *temp;
    while(((void*)(((struct TempList*)*head)->next))){
        temp = *head;
        *head = ((void*)(((struct TempList*)*head)->next));
        zenithra_free(&temp);
        if(!((void*)(((struct TempList*)*head)->next))){
            zenithra_free(head);
            break;
        }
    }
}

void* zenithra_create_node(void **node, bool is_head, int struct_type_size){
    struct TempList{
        struct TempList *next;
        char temp_arr_fill[struct_type_size-8];
    };
    void *new_node;

    if(is_head == true){
        new_node = (void*)malloc(sizeof(struct TempList));
        ((struct TempList*)new_node)->next = NULL;
    }else{
        if(((void*)(((struct TempList*)*node)->next))){
            zenithra_log_msg("Attempt to create node with non-NULL pointer");
            return new_node;
        }
        ((struct TempList*)*node)->next = (void*)malloc(sizeof(struct TempList));
        new_node = ((void*)(((struct TempList*)*node)->next));
        ((struct TempList*)new_node)->next = NULL;
    }
    
    return (struct TempList*)new_node;
}