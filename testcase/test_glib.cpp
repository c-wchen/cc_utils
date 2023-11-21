//
// Created by 忘尘 on 2022/7/24.
//
#include "glib.h"
#include "gtest/gtest.h"

TEST(TEST_GLIB, test_glib_list)
{
    GList *list = NULL;
    guint a = 2;
    list = g_list_append(list, (gpointer)"Hello world!");
    list = g_list_append(list, (gpointer)"made by pcat");
    list = g_list_append(list, (gpointer) "http://pcat.cnblogs.com");
    printf("The first item is %s\n", g_list_nth(list, a)->data);
}
