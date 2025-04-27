struct list_temp_vec3{
    struct list_temp_vec3 *next;
    float data[3];
};

struct list_temp_vec2{
    struct list_temp_vec2 *next;
    float data[2];
};

struct list_temp_mat3{
    struct list_temp_mat3 *next;
    int data[3][3];
};