
#include <math.h>

#include "list.h"

LIST_ELEMENT * allocate_list_element()
{
	LIST_ELEMENT * new_list_element;

	new_list_element = (LIST_ELEMENT *)malloc(sizeof(LIST_ELEMENT));

	return new_list_element;
}

LIST * create_list()
{	
	LIST * list_head;

	list_head = allocate_list_element();

	list_head->prev_element = list_head;
	list_head->next_element = list_head;

	return list_head;
}

void append_list(LIST * target_list, void * content)
{
	LIST_ELEMENT * last_list_element;
	LIST_ELEMENT * new_list_element;

	new_list_element = allocate_list_element();

	last_list_element = target_list->prev_element;

	last_list_element->next_element = new_list_element;
	target_list->prev_element = new_list_element;

	new_list_element->prev_element = last_list_element;
	new_list_element->next_element = target_list;

	new_list_element->content = content;
}

void * get_list_at(LIST * source_list, int index)
{
	int i;

	void * content;

	LIST_ELEMENT * current_element;

	current_element = source_list;

	for(i = -1; i < abs(index); i++)
	{
		if(index >= 0)
		{	
			if(current_element->next_element == source_list)
			{
				break;
			}
	
			current_element = current_element->next_element;
			content = current_element->content;
		}
		else
		{
			content = current_element->content;

			if(current_element->prev_element == source_list)
			{
				break;
			}	

			current_element = current_element->prev_element;
		}
	}

	return content;
}

int len_list(LIST * source_list)
{
	int i = -1;

	LIST_ELEMENT * current_element;

	current_element = source_list;

	while(current_element->next_element != source_list)
	{
		current_element = current_element->next_element;
		i++;
	}

	return i+1;
}

void destroy_list(LIST * target_list)
{
	LIST_ELEMENT * current_element;

	current_element = target_list;

	while(current_element->next_element != target_list)
	{
		current_element = current_element->next_element;
		free(current_element->prev_element);
	}

	free(current_element);
}
