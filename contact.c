#include <stdio.h>
#include <string.h>
#include "contact.h"

/* 安全复制字符串：最多复制 size-1 个字符，并保证结尾有 '\0' */
static void copy_str(char *dst, int size, const char *src) {
    strncpy(dst, src, (size_t)size - 1);
    dst[size - 1] = '\0';
}

void init_book(ContactBook *book) {
    book->count = 0;
}

int add_contact(ContactBook *book, const Contact *c) {
    if (book->count >= MAX_CONTACTS) {
        return -1;                         /* 满了 */
    }
    if (find_contact(book, c->name) >= 0) {
        return -2;                         /* 重名 */
    }
    book->items[book->count] = *c;         /* 结构体可以直接整体赋值 */
    book->count++;
    return 0;
}

int find_contact(const ContactBook *book, const char *name) {
    int i;
    for (i = 0; i < book->count; i++) {
        if (strcmp(book->items[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int delete_contact(ContactBook *book, const char *name) {
    int i;
    int idx = find_contact(book, name);
    if (idx < 0) {
        return -1;
    }
    /* 把后面的元素整体往前挪一位 */
    for (i = idx; i < book->count - 1; i++) {
        book->items[i] = book->items[i + 1];
    }
    book->count--;
    return 0;
}

int modify_contact(ContactBook *book, const char *name,
                   const char *new_phone, const char *new_email) {
    int idx = find_contact(book, name);
    if (idx < 0) {
        return -1;
    }
    if (new_phone != NULL && new_phone[0] != '\0') {
        copy_str(book->items[idx].phone, PHONE_LEN, new_phone);
    }
    if (new_email != NULL && new_email[0] != '\0') {
        copy_str(book->items[idx].email, EMAIL_LEN, new_email);
    }
    return 0;
}

void show_all(const ContactBook *book) {
    int i;
    if (book->count == 0) {
        printf("（通讯录是空的）\n");
        return;
    }
    printf("共 %d 位联系人：\n", book->count);
    for (i = 0; i < book->count; i++) {
        printf("%d. 姓名：%s | 电话：%s | 邮箱：%s\n",
               i + 1,
               book->items[i].name,
               book->items[i].phone,
               book->items[i].email);
    }
}

/* ---------------------------------------------------------------
   文件格式（每行一个联系人，用 | 分隔）：
   张三|13800138000|zhangsan@example.com
   --------------------------------------------------------------- */
int load_book(ContactBook *book, const char *path) {
    FILE *fp = fopen(path, "r");
    char line[256];
    int loaded = 0;

    if (fp == NULL) {
        return 0;                 /* 第一次运行，还没有文件，不算错误 */
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        char name[NAME_LEN] = "";
        char phone[PHONE_LEN] = "";
        char email[EMAIL_LEN] = "";
        int n;

        line[strcspn(line, "\r\n")] = '\0';   /* 去掉行尾换行符 */
        if (line[0] == '\0') {
            continue;                          /* 跳过空行 */
        }
        if (book->count >= MAX_CONTACTS) {
            break;                             /* 存不下了 */
        }

        /* 按 | 切分三个字段；%31[^|] 表示最多读 31 个非 | 字符 */
        n = sscanf(line, "%31[^|]|%19[^|]|%63[^\n]", name, phone, email);
        if (n < 1) {
            continue;                          /* 格式不对，跳过这一行 */
        }

        copy_str(book->items[book->count].name,  NAME_LEN,  name);
        copy_str(book->items[book->count].phone, PHONE_LEN, phone);
        copy_str(book->items[book->count].email, EMAIL_LEN, email);
        book->count++;
        loaded++;
    }

    fclose(fp);
    return loaded;
}

int save_book(const ContactBook *book, const char *path) {
    FILE *fp = fopen(path, "w");
    int i;

    if (fp == NULL) {
        return -1;
    }

    for (i = 0; i < book->count; i++) {
        fprintf(fp, "%s|%s|%s\n",
                book->items[i].name,
                book->items[i].phone,
                book->items[i].email);
    }

    fclose(fp);
    return 0;
}
