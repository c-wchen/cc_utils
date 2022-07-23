//
// Created by 忘尘 on 2022/7/23.
// Reference: http://troydhanson.github.io/uthash/userguide.html
//

#include "uthash.h"
#include "gtest/gtest.h"
#include "stdint.h"

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

TEST(TEST_UT_HASH, test_hash_struct) {
    user *u1 = (user *) malloc(sizeof(user));
    user *u2 = (user *) malloc(sizeof(user));
    u1->id = (int64_t)u1;
    u2->id = (int64_t)u2;
    u1->data = "u1";
    u2->data = "u2";
    add_user(u1);
    add_user(u2);
    user *u3 = find_user(1);
    if (u3 != NULL) {
        printf("%ld %s\n", u3->id, u3->data);
    }
    print_all();
    delete_user((int64_t)u1);
    clear_all();
    print_all();
}

