#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contact.h"

#ifdef _WIN32
#include <windows.h>
#endif

#define DATA_FILE "contacts.txt"

/* 读一行字符串（比 scanf("%s") 安全，能读空格，不会残留换行符）
   返回值：1 = 读到内容，0 = 输入结束（EOF） */
static int read_line(char *buf, int size) {
    if (fgets(buf, size, stdin) == NULL) {
        buf[0] = '\0';
        return 0;
    }
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}

/* 读一个整数
   返回值：>=0 = 读到的数字；-1 = 输入不是数字；-2 = 输入结束（EOF） */
static int read_int(void) {
    char buf[32];
    int value;
    if (!read_line(buf, sizeof(buf))) {
        return -2;
    }
    if (sscanf(buf, "%d", &value) != 1) {
        return -1;
    }
    return value;
}

static void show_menu(void) {
    printf("\n========== 通讯录 ==========\n");
    printf(" 1. 添加联系人\n");
    printf(" 2. 删除联系人\n");
    printf(" 3. 查找联系人\n");
    printf(" 4. 修改联系人\n");
    printf(" 5. 显示全部\n");
    printf(" 0. 保存并退出\n");
    printf("============================\n");
    printf("请选择：");
}

static void do_add(ContactBook *book) {
    Contact c;
    int result;

    printf("姓名：");
    read_line(c.name, NAME_LEN);
    printf("电话：");
    read_line(c.phone, PHONE_LEN);
    printf("邮箱：");
    read_line(c.email, EMAIL_LEN);

    result = add_contact(book, &c);
    if (result == 0) {
        printf("添加成功。\n");
        save_book(book, DATA_FILE);
    } else if (result == -1) {
        printf("通讯录已满，添加失败。\n");
    } else {
        printf("已经有叫这个名字的联系人了。\n");
    }
}

static void do_delete(ContactBook *book) {
    char name[NAME_LEN];

    printf("要删除的姓名：");
    read_line(name, NAME_LEN);

    if (delete_contact(book, name) == 0) {
        printf("删除成功。\n");
        save_book(book, DATA_FILE);
    } else {
        printf("没有找到「%s」。\n", name);
    }
}

static void do_find(const ContactBook *book) {
    char name[NAME_LEN];
    int idx;

    printf("要查找的姓名：");
    read_line(name, NAME_LEN);

    idx = find_contact(book, name);
    if (idx < 0) {
        printf("没有找到「%s」。\n", name);
        return;
    }
    printf("姓名：%s\n电话：%s\n邮箱：%s\n",
           book->items[idx].name,
           book->items[idx].phone,
           book->items[idx].email);
}

static void do_modify(ContactBook *book) {
    char name[NAME_LEN];
    char phone[PHONE_LEN];
    char email[EMAIL_LEN];

    printf("要修改的姓名：");
    read_line(name, NAME_LEN);

    if (find_contact(book, name) < 0) {
        printf("没有找到「%s」。\n", name);
        return;
    }

    printf("新电话（直接回车表示不改）：");
    read_line(phone, PHONE_LEN);
    printf("新邮箱（直接回车表示不改）：");
    read_line(email, EMAIL_LEN);

    if (modify_contact(book, name, phone, email) == 0) {
        printf("修改成功。\n");
        save_book(book, DATA_FILE);
    }
}

int main(void) {
    ContactBook book;
    int choice;
    int loaded;

    /* 让 Windows 终端按 UTF-8 显示，避免中文乱码
       （用 API 设置，比 system("chcp") 干净，也不会影响标准输入） */
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    init_book(&book);
    loaded = load_book(&book, DATA_FILE);
    if (loaded > 0) {
        printf("已从 %s 载入 %d 位联系人。\n", DATA_FILE, loaded);
    }

    while (1) {
        show_menu();
        choice = read_int();

        if (choice == -2) {              /* 输入结束了（比如重定向的文件读完） */
            printf("\n输入结束，程序退出。\n");
            save_book(&book, DATA_FILE);
            return 0;
        }

        switch (choice) {
            case 1:
                do_add(&book);
                break;
            case 2:
                do_delete(&book);
                break;
            case 3:
                do_find(&book);
                break;
            case 4:
                do_modify(&book);
                break;
            case 5:
                show_all(&book);
                break;
            case 0:
                save_book(&book, DATA_FILE);
                printf("已保存到 %s，再见！\n", DATA_FILE);
                return 0;
            default:
                printf("无效选择，请输入 0~5 的数字。\n");
                break;
        }
    }
}
