#ifndef CONTACT_H
#define CONTACT_H

/* ===== 常量 ===== */
#define MAX_CONTACTS 100      /* 通讯录最多存多少人 */
#define NAME_LEN     32       /* 姓名最大长度（含结尾的 '\0'） */
#define PHONE_LEN    20       /* 电话最大长度 */
#define EMAIL_LEN    64       /* 邮箱最大长度 */

/* ===== 数据结构 ===== */

/* 一个联系人 */
typedef struct {
    char name[NAME_LEN];
    char phone[PHONE_LEN];
    char email[EMAIL_LEN];
} Contact;

/* 整个通讯录：一个数组 + 当前人数 */
typedef struct {
    Contact items[MAX_CONTACTS];
    int count;
} ContactBook;

/* ===== 函数声明 ===== */

/* 初始化：把人数清零 */
void init_book(ContactBook *book);

/* 添加联系人
   返回值： 0 = 成功
           -1 = 通讯录已满
           -2 = 姓名重复 */
int add_contact(ContactBook *book, const Contact *c);

/* 按姓名查找
   返回值：找到返回下标（0 开始）；找不到返回 -1 */
int find_contact(const ContactBook *book, const char *name);

/* 按姓名删除
   返回值：0 = 成功；-1 = 没有这个人 */
int delete_contact(ContactBook *book, const char *name);

/* 按姓名修改电话和邮箱
   new_phone / new_email 传空字符串 "" 表示这一项不修改
   返回值：0 = 成功；-1 = 没有这个人 */
int modify_contact(ContactBook *book, const char *name,
                   const char *new_phone, const char *new_email);

/* 打印全部联系人 */
void show_all(const ContactBook *book);

/* 从文件加载
   返回值：读到的条数；文件不存在返回 0 */
int load_book(ContactBook *book, const char *path);

/* 保存到文件
   返回值：0 = 成功；-1 = 打开文件失败 */
int save_book(const ContactBook *book, const char *path);

#endif /* CONTACT_H */
