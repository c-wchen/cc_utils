//
// Created by 忘尘 on 2022/7/23.
// Reference: http://troydhanson.github.io/uthash/userguide.html
//

#include "uthash.h"
#include "utlist.h"
#include "utarray.h"
#include "utstring.h"
#include "utstack.h"
#include "gtest/gtest.h"
#include "stdint.h"

// ---------------------------ut hash-------------------------------------- //
typedef struct {
    int64_t id;
    char *data;
    UT_hash_handle hh;
} user;

user *g_users = NULL;

user *find_user(int64_t key) {
    user *u1 = NULL;
    HASH_FIND_INT(g_users, &key, u1);
    return u1;
}

void add_user(user *u1) {
    user *u2 = NULL;
    HASH_FIND_INT(g_users, &(u1->id), u2);
    if (u2 != NULL) {
        return;
    } else {
        HASH_ADD_INT(g_users, id, u1);
    }
}

void delete_user(int64_t key) {
    user *u1 = NULL;
    HASH_FIND_INT(g_users, &key, u1);
    if (u1 != NULL) {
        HASH_DEL(g_users, u1);
        free(u1);
    }
}

void delete_all() {
    user *current_user, *tmp;
    HASH_ITER(hh, g_users, current_user, tmp) {
        HASH_DEL(g_users, current_user);
        free(current_user);
    }
}

void print_all() {
    user *current_user, *tmp;
    HASH_ITER(hh, g_users, current_user, tmp) {
        printf("id: %lld, data: %s\n", current_user->id, current_user->data);
    }
}

void clear_all() {
    user *current_user, *tmp;
    HASH_ITER(hh, g_users, current_user, tmp) {
        HASH_DEL(g_users, current_user);
        free(current_user);
    }
}

TEST(TEST_UT, test_ut_hash) {
    user *u1 = (user *) malloc(sizeof(user));
    user *u2 = (user *) malloc(sizeof(user));
    u1->id = (int64_t) u1;
    u2->id = (int64_t) u2;
    u1->data = "u1";
    u2->data = "u2";
    add_user(u1);
    add_user(u2);
    user *u3 = find_user(1);
    if (u3 != NULL) {
        printf("%ld %s\n", u3->id, u3->data);
    }
    print_all();
    delete_user((int64_t) u1);
    clear_all();
    print_all();
}

// ---------------------------ut list-------------------------------------- //
typedef struct student {
    uint64_t user_id;
    char *user_name;
    struct student *prev; /* needed for a doubly-linked list only */
    struct student *next; /* needed for singly- or doubly-linked lists */
} student;

student *head = NULL;

int32_t cmp_from_stu(const void *s1, const void *s2) {
    return ((student *) s1)->user_id - ((student *) s2)->user_id;
}

TEST(TEST_UT, test_ut_singly_linked) {
    student s1, s2, s3, s4, s5, *tmp, tmp2;
    s1.user_id = 20;
    s1.user_name = "s1";
    s2.user_id = 19;
    s2.user_name = "s2";
    s3.user_id = 40;
    s3.user_name = "s3";
    s4.user_id = 10;
    s4.user_name = "s4";
    s5.user_id = 100;
    s5.user_name = "s5";
    LL_APPEND(head, &s1); // 表尾插入
    LL_APPEND(head, &s2);
    LL_APPEND(head, &s3);
    LL_APPEND(head, &s4);
    LL_PREPEND(head, &s5); // 表头插入
    LL_FOREACH(head, tmp) {
        printf("user id: %ld, user name: %s\n", tmp->user_id, tmp->user_name);
    }
    LL_SORT(head, cmp_from_stu); // 排序
    printf("=========================\n");
    LL_FOREACH(head, tmp) {
        printf("user id: %ld, user name: %s\n", tmp->user_id, tmp->user_name);
    }
    tmp = NULL;
    tmp2.user_id = 100;
    LL_SEARCH(head, tmp, &tmp2, cmp_from_stu); // 搜索
    printf("search id: 100, user id: %ld, user name: %s\n", tmp->user_id, tmp->user_name);
}


TEST(TEST_UT, test_ut_double_linked) {
    student s1, s2, s3, s4, *tmp;
    s1.user_id = 20;
    s1.user_name = "s1";
    s2.user_id = 19;
    s2.user_name = "s2";
    s3.user_id = 40;
    s3.user_name = "s3";
    s4.user_id = 10;
    s4.user_name = "s4";

    DL_APPEND(head, &s1);
    DL_APPEND(head, &s2);
    DL_APPEND(head, &s3);
    DL_APPEND(head, &s4);
    DL_FOREACH(head, tmp) {
        printf("user id: %ld, user name: %s\n", tmp->user_id, tmp->user_name);
    }
    DL_SORT(head, cmp_from_stu);
    printf("=========================\n");
    DL_FOREACH(head, tmp) {
        printf("user id: %ld, user name: %s\n", tmp->user_id, tmp->user_name);
    }
}
// ---------------------------ut array-------------------------------------- //
TEST(TEST_UT, test_ut_array) {
    UT_array *num_arr;
    int *p;
    utarray_new(num_arr, &ut_int_icd);
    for (int i = 0; i < 100; ++i) {
        utarray_push_back(num_arr, &i);
    }
    for (
            p = (int *) utarray_front(num_arr);
            p != NULL;
            p = (int *) utarray_next(num_arr, p)) {
        printf("%d\n", *p);
    }
    utarray_free(num_arr);
}

typedef struct {
    int32_t a;
    int32_t b;
} arr_item;

// TODO: 深复制
UT_icd my_icd = {sizeof(arr_item), NULL, NULL, NULL};

TEST(TEST_UT, test_ut_array_from_arr_item) {
    UT_array *num_arr;
    arr_item p;
    arr_item *tmp;
    utarray_new(num_arr, &my_icd);
    for (int i = 0; i < 100; ++i) {
        p.a = i;
        p.b = i * i;
        utarray_push_back(num_arr, &p);
    }

    for (
            tmp = (arr_item *) utarray_front(num_arr);
            tmp != NULL;
            tmp = (arr_item *) utarray_next(num_arr, tmp)) {
        printf("%d %d\n", tmp->a, tmp->b);
    }

    utarray_free(num_arr);
}

// ---------------------------ut string-------------------------------------- //
TEST(TEST_UT, test_ut_string) {
    UT_string *str;
    utstring_new(str);
    utstring_printf(str, "hello world;");
    utstring_printf(str, "hello world2;");
    printf("%s\n", utstring_body(str));
    utstring_free(str);
}

// ---------------------------ut stack-------------------------------------- //
TEST(TEST_UT, test_ut_stack) {
    student *top = NULL;
    student s1, s2, s3, s4, *tmp;
    s1.user_id = 20;
    s1.user_name = "s1";
    s2.user_id = 19;
    s2.user_name = "s2";
    s3.user_id = 40;
    s3.user_name = "s3";
    s4.user_id = 10;
    s4.user_name = "s4";
    STACK_PUSH(top, &s1);
    STACK_PUSH(top, &s2);
    STACK_PUSH(top, &s3);
    STACK_PUSH(top, &s4);
    while (!STACK_EMPTY(top)) {
        tmp = STACK_TOP(top);
        printf("user id: %ld, user name: %s\n", tmp->user_id, tmp->user_name);
        STACK_POP(top, tmp);
    }
}