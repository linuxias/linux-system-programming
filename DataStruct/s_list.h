#ifndef __S_LIST_H__
#define __S_LIST_H__

#define ERR(msg) do { \
		printf("%s", msg); \
} while (0);

typedef void* spointer;
typedef struct _SList SList;

typedef void (*s_func) (spointer data, void *user_data);
typedef void (*s_del_func) (spointer data);
typedef int (*s_comp_func) (spointer a, spointer b);

struct _SList {
	SList *pre;
	SList *next;
	spointer data;
};

SList *s_list_append(SList *list, spointer data);

void s_list_free(SList *list);

void s_list_free_full(SList *list, s_del_func func);

SList *s_list_remove(SList *list, spointer data);

SList *s_list_remove_link(SList *list, SList *link);

SList *s_list_delete_link(SList *list, SList *link);

void s_list_foreach(SList *list, s_func func, void *user_data);

SList *s_list_last(SList *list);

SList *s_list_first(SList *list);

spointer s_list_get_data(SList *list);

SList *s_list_find(SList *list, spointer data);

SList *s_list_find_custom(SList *list, spointer data, s_comp_func func);
#endif /* __S_LIST_H__ */

